#ifndef ADHOP_H_
#define ADHOP_H_

#include <system/kmalloc.h>
#include <utility/hash.h>

#define NULL 0
#define MAX_LIST 3
#define MAX_HASH 5

__BEGIN_SYS

template<typename NIC, typename Network>
class ADHOP
{
public:
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned long u32;
    typedef unsigned long long u64;

    typedef typename Network::Address NetAddress;
    typedef typename NIC::Address NicAddress;
    static const unsigned int INITIAL_PHEROMONE = 100;
    static const unsigned int TIMER_CHECK_ROUTING_TABLES = 5000000; // 5s

    ADHOP(): seqNO_ITA(0), seqNO_ETA(0)
    {}

    void init(const NicAddress& pa)
    {
        my_address = pa;
    }

    enum {
        ITA = 0,
        ETA = 1
    };

private:
    class Ant
    {
    public:
        u32 type:1;
        u32 returning:1;
        u32 sequenceNO:14;
        u32 heuristic:16;
        NicAddress neighbor;
    } __attribute__((packed));

    class RTable_Neighbor
    {
    public:
        RTable_Neighbor(const NetAddress& la, const NicAddress& pa): _link(this, INITIAL_PHEROMONE)
        {
            this->restart();
            _destination_node = la;
            _neighbor = pa;
        }

        bool evaporate() //EVAPORATION
        {
            _link.rank((u16) (_link.rank() >> 1));
            if(_link.rank() > 4)
                return false;
            else
                return true;
        }
        void update(u16 heuristic) //UPDATE_PHEROMONE
        {
            if (!heuristic)
                _link.rank( _link.rank() + (u16) (_link.rank() >> 1));
        }

        bool match(const NetAddress& la) { return (la == _destination_node); }
        bool match(const NetAddress& la, const NicAddress& pa)
        {
            return ((la == _destination_node) && (pa == _neighbor));
        }

        bool verify(u16 seq, u8 type)
        {
            switch(type) {
                case ITA:
                    if (seq > this->_seqNO_ITA) {
                        this->_seqNO_ITA = seq;
                        return true;
                    } else {
                        if (seq == 0) {
                            this->restart();
                        }
                        return false;
                    }
                    break;
                case ETA:
                    if (seq > this->_seqNO_ETA) {
                        this->_seqNO_ETA = seq;
                        return true;
                    } else {
                        if (seq == 0) {
                            this->restart();
                        }
                        return false;
                    }
                    break;
            }
            return false;
        }

        typename List_Elements::Singly_Linked_Ordered<RTable_Neighbor, u16> _link;

    private:
        void restart()
        {
            _seqNO_ITA = 0;
            _seqNO_ETA = 0;
        }

        u16 _seqNO_ITA:14;
        u16 _seqNO_ETA:14;
    public:
        NicAddress _neighbor;
        NetAddress _destination_node;
    };

    class RTable_Neighbors:
        Simple_List<RTable_Neighbor, List_Elements::Singly_Linked_Ordered<RTable_Neighbor, u16> >
    {
    private:
        typedef Simple_List<RTable_Neighbor, List_Elements::Singly_Linked_Ordered<RTable_Neighbor, u16> > Base;
        typedef typename Base::Element Element;

    public:
        RTable_Neighbors(const NetAddress& la, const NicAddress& pa): _link(this,la), Base()
        {
            RTable_Neighbor* neighbor = new(kmalloc(sizeof(RTable_Neighbor))) RTable_Neighbor(la, pa);
            this->insert(&neighbor->_link);
        }

        ~RTable_Neighbors()
        {
            while(Base::size()) {
                Element* element = Base::remove();
                if(element) {
                    delete element->object();
                }
            }
        }

        void insert(Element * e)
        {
            if(Base::size() >= MAX_LIST) {
                Element* element = Base::remove_tail();
                if(element) {
                    delete element->object();
                }
            }
            if(Base::empty())
                insert_first(e);
            else {
                Element * next, * prev;
                for(
                        next = Base::head(), prev = NULL;
                        (next->rank() > e->rank()) && next->next();
                        prev = next, next = next->next()
                   );
                if(next->rank() > e->rank()) {
                    insert_tail(e);
                } else if(!prev) {
                    insert_head(e);
                } else {
                    Base::insert(e, prev, next);
                }
            }
        }

        void update()
        {
            for(Element* e = Base::head(); e; e = e->next()) {
                if (e->object()->evaporate()) {
                    Base::remove(e);
                    delete e->object();
                }
            }
        }

        void update(const NetAddress& la, const NicAddress& pa, u16 heuristic)
        {
            Element* element = search_entry(la, pa);
            if(element) {
                Base::remove(element);
                element->object()->update(heuristic);
                this->insert(element);
            } else {
                RTable_Neighbor* neighbor = new(kmalloc(sizeof(RTable_Neighbor))) RTable_Neighbor(la, pa);
                this->insert(&neighbor->_link);
            }
        }

        Element* search_entry(const NetAddress& la, const NicAddress& pa)
        {
            Element * e;
            for(e = Base::head(); e && !e->object()->match(la, pa); e = e->next());
            return e;
        }

        NicAddress search_address(const NetAddress& la)
        {
            Element * e;
            for(e = Base::head(); e && !e->object()->match(la); e = e->next());
            if(e)
                return e->object()->_neighbor;
            else
                return NIC::BROADCAST;
        }

        bool verify(const NetAddress& la, u16 seq, u8 type)
        {
            Element * e;
            for(e = Base::head(); e && !e->object()->match(la); e = e->next());
            if(e)
                return e->object()->verify(seq, type);
            else
                return true;
        }

        typename Minimal_Hash<RTable_Neighbors, MAX_HASH>::Element _link;
    };

    class RTable: Minimal_Hash<RTable_Neighbors, MAX_HASH>
    {
    private:
        typedef Minimal_Hash<RTable_Neighbors, MAX_HASH> Base;

    public:
        typedef typename Base::Element Element;

        RTable() {}

        ~RTable()
        {
            for(int i = 0; i < MAX_HASH; i++) {
                NetAddress la((u8) i);
                Element* element = Base::remove_key(la);
                if(element)
                    delete element->object();
            }
        }

        void update()
        {
            for(int i = 0; i < MAX_HASH; i++) {
                NetAddress la((u8) i);
                Element* element = Base::search_key(la);
                if(element)
                    element->object()->update();
            }
        }

        void update(const NetAddress& la, const NicAddress& pa, u16 heuristic = 0)
        {
            Element * element = Base::search_key(la);
            RTable_Neighbors * list;
            if(element) {
                list = element->object();
                list->update(la, pa, heuristic);
            } else {
                list = new(kmalloc(sizeof(RTable_Neighbors))) RTable_Neighbors(la, pa);
                Element* e = Base::insert(&list->_link);
                if(e)
                    delete e->object();
            }
        }

        NicAddress search(const NetAddress& la)
        {
            Element * element = Base::search_key(la);
            if(element)
                return element->object()->search_address(la);
            else
                return NIC::BROADCAST;
        }

        bool verify(const NetAddress& la, u16 seq, u8 type)
        {
            Element * element = Base::search_key(la);
            if(element)
                return element->object()->verify(la, seq, type);
            else
                return true;
        }
    };

public:
    bool update(const NetAddress& src, void* header)
    {
        Ant* ant = (Ant*) header;
        if (rtable.verify(src, ant->sequenceNO, ant->type)) {
            rtable.update(src, ant->neighbor, ant->sequenceNO);
        }
        if (ant->type == ETA && ant->returning == 0)
        {
            ant->returning = 1;
            return true;
        }
        return false;
    }

    NicAddress decision(const NetAddress& dst, void* header)
    {
        Ant* ant = (Ant*) header;
        NicAddress nic_addr = rtable.search(dst);
        if (nic_addr == NIC::BROADCAST) {
            ant->type = ETA;
            ant->sequenceNO = seqNO_ETA++;
        } else {
            ant->type = ITA;
            ant->sequenceNO = seqNO_ITA++;
        }
        ant->neighbor = my_address;
        ant->heuristic = 0;
        ant->returning = 0;
        return nic_addr;
    }

    NicAddress decision(const NetAddress& src, const NetAddress& dst, void* header)
    {
        Ant* ant = (Ant*) header;
        if (rtable.verify(src, ant->sequenceNO, ant->type)) {
            rtable.update(src, ant->neighbor, ant->sequenceNO);
            switch ((int) ant->type) {
                case ITA:
                    return receiving_ITA(src, dst, ant);
                case ETA:
                    return receiving_ETA(src, dst, ant);
                default:
                    return NIC::BROADCAST;
            }
        }
        return NIC::BROADCAST;
    }

    static int ant_size() {
        return sizeof(Ant);
    }

private:
    NicAddress receiving_ITA(const NetAddress& src, const NetAddress& dst, Ant* ant)
    {
        ant->neighbor = my_address;
        ant->heuristic = 0;
        NicAddress neighbor = rtable.search(dst);
        if (neighbor == NIC::BROADCAST) {
            ant->type = ETA;
            ant->sequenceNO = seqNO_ETA++;
            return NIC::BROADCAST;
        }
        return neighbor;
    }

    NicAddress receiving_ETA(const NetAddress& src, const NetAddress& dst, Ant* ant)
    {
        ant->neighbor = my_address;
        ant->heuristic = 0;
        return rtable.search(dst);
    }

    NicAddress my_address;
    RTable rtable;
    u16 seqNO_ITA:14;
    u16 seqNO_ETA:14;
};

__END_SYS

#endif /* ADHOP_H_ */
