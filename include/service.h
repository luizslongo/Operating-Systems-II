#ifndef __service_h
#define __service_h

#include <system/meta.h>
#include <adhop.h>
#include <arp.h>

__BEGIN_SYS

template <int SERVICE>
class Service
{
public:
    template <typename Link_Layer, typename Network_Layer>
    class Network_Service:
        public SWITCH<SERVICE,
                     CASE<Traits<Services>::ADHOP,
                         ADHOP<Link_Layer,Network_Layer>,
                     CASE<DEFAULT,
                         ARP<Link_Layer,Network_Layer>
                     > >
               >::Result
    {
    public:
        typedef typename
            SWITCH<SERVICE,
                  CASE<Traits<Services>::ADHOP,
                      ADHOP<Link_Layer,Network_Layer>,
                  CASE<DEFAULT,
                      ARP<Link_Layer,Network_Layer>
                  > >
            >::Result Base;

        Network_Service(Link_Layer* nic, Network_Layer* network):
            Base(nic, network)
        { }
    };
};

__END_SYS

#endif

