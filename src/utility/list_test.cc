// EPOS-- List Utility Test Program

#include <utility/ostream.h>
#include <utility/list.h>

__USING_SYS;

struct Integer1 {
    Integer1(int _i) : i(_i), e(this) {}
    
    int i;
    List<Integer1>::Element e;
};

struct Integer2 {
    Integer2(int _i, int _r) : i(_i), e(this, _r) {}
    
    int i;
    Ordered_List<Integer2>::Element e;
};

struct Integer3 {
    Integer3(int _i, int _r) : i(_i), e(this, _r) {}
    
    int i;
    Relative_List<Integer3>::Element e;
};

struct Integer4 {
    Integer4(int _i, int _s) : e((Integer4 *)_i, _s) {}
    
    Grouping_List<Integer4>::Element e;
};

int main()
{
    OStream cout;

    cout << "List Utility Test\n";

    cout << "\nThis is a list of integers:\n";
    Integer1 i1(1), i2(2), i3(3), i4(4);
    List<Integer1> l1;
    cout << "Inserting the integer " << i1.i << "\n";
    l1.insert_tail(&i1.e);
    cout << "Inserting the integer " << i2.i << "\n";
    l1.insert_tail(&i2.e);
    cout << "Inserting the integer " << i3.i << "\n";
    l1.insert_tail(&i3.e);
    cout << "Inserting the integer " << i4.i << "\n";
    l1.insert_tail(&i4.e);
    cout << "The list has now " << l1.size() << " elements.\n";
    cout << "Removing the element whose value is " << i2.i << " => " 
	 << l1.remove(&i2)->object()->i << "\n";
    cout << "Removing the list's head => " << l1.remove_head()->object()->i 
	 << "\n";
    cout << "Removing the element whose value is " << i4.i << " => " 
	 << l1.remove(&i4)->object()->i << "\n";
    cout << "Removing the list's head =>" << l1.remove_head()->object()->i
	 << "\n";
    cout << "The list has now " << l1.size() << " elements.\n";


    cout << "This is an ordered list of integers:\n";
    Integer2 j1(1, 2), j2(2, 3), j3(3, 4), j4(4, 1);
    Ordered_List<Integer2> l2;
    cout << "Inserting the integer " << j1.i 
	 << " with rank " << j1.e.rank() << ".\n";
    l2.insert(&j1.e);
    cout << "Inserting the integer " << j2.i
	 << " with rank " << j2.e.rank() << ".\n";
    l2.insert(&j2.e);
    cout << "Inserting the integer " << j3.i
	 << " with rank " << j3.e.rank() << ".\n";
    l2.insert(&j3.e);
    cout << "Inserting the integer " << j4.i
	 << " with rank " << j4.e.rank() << ".\n";
    l2.insert(&j4.e);
    cout << "The list has now " << l2.size() << " elements.\n";
    cout << "Removing the element whose value is " << j2.i << " => " 
	 << l2.remove(&j2)->object()->i << "\n";
    cout << "Removing the list's head => " << l2.remove()->object()->i 
	 << "\n";
    cout << "Removing the list's head => " << l2.remove()->object()->i
	 << "\n";
    cout << "Removing the list's head => " << l2.remove()->object()->i 
	 << "\n";
    cout << "The list has now " << l2.size() << " elements.\n";


    cout << "This is an list of integers with relative ordering:\n";
    Integer3 k1(1, 2), k2(2, 3), k3(3, 4), k4(4, 1);
    Relative_List<Integer3> l3;
    cout << "Inserting the integer " << k1.i 
	 << " with relative order " << k1.e.rank() << ".\n";
    l3.insert(&k1.e);
    cout << "Inserting the integer " << k2.i 
	 << " with relative order " << k2.e.rank() << ".\n";
    l3.insert(&k2.e);
    cout << "Inserting the integer " << k3.i
	 << " with relative order " << k3.e.rank() << ".\n";
    l3.insert(&k3.e);
    cout << "Inserting the integer " << k4.i
	 << " with relative order " << k4.e.rank() << ".\n";
    l3.insert(&k4.e);
    cout << "The list has now " << l3.size() << " elements.\n";
    cout << "Removing the element whose value is " << j2.i << " => " 
	 << l3.remove(&k2)->object()->i << "\n";
    cout << "Removing the list's head => " << l3.remove()->object()->i
	 << "\n";
    cout << "Removing the list's head => " << l3.remove()->object()->i
	 << "\n";
    cout << "Removing the list's head => " << l3.remove()->object()->i
	 << "\n";
    cout << "The list has now " << l3.size() << " elements.\n";


    cout << "This is a grouping list of integers:\n";
    Grouping_List<int> l4;
    Grouping_List<int>::Element m1((int *)0, 2), m2((int *)16, 2), m3((int *)8, 2), m4((int *)24, 2);
    Grouping_List<int>::Element * d1, * d2;
    cout << "Inserting the integer group beginning with " << m1.object()
	 << " and spanning " << m1.size() << " elements into the list.\n";
    l4.insert_merging(&m1, &d1, &d2);
    if(d1)
	cout << "Element whose valeu was " << d1->object() << " merged\n";
    if(d2)
	cout << "Element whose valeu was " << d2->object() << " merged\n";
    cout << "Inserting the integer group beginning with " << m2.object()
	 << " and spanning " << m2.size() << " elements into the list.\n";
    l4.insert_merging(&m2, &d1, &d2);
    if(d1)
	cout << "Element whose valeu was " << d1->object() << " merged\n";
    if(d2)
	cout << "Element whose valeu was " << d2->object() << " merged\n";
    cout << "Inserting the integer group beginning with " << m3.object()
	 << " and spanning " << m3.size() << " elements into the list.\n";
    l4.insert_merging(&m3, &d1, &d2);
    if(d1)
	cout << "Element whose valeu was " << d1->object() << " merged\n";
    if(d2)
	cout << "Element whose valeu was " << d2->object() << " merged\n";
    cout << "Inserting the integer group beginning with " << m4.object()
	 << " and spanning " << m4.size() << " elements into the list.\n";
    l4.insert_merging(&m4, &d1, &d2);
    if(d1)
	cout << "Element whose valeu was " << d1->object() << " merged\n";
    if(d2)
	cout << "Element whose valeu was " << d2->object() << " merged\n";
    cout << "The list has now " << l4.size() << " elements that group " 
	 << l4.grouped_size() << " elements in total.\n";
    cout << "Removing one element from the list: "; 
    d1 = l4.search_decrementing(1);
    cout << d1->object() + d1->size() << ".\n";
    if(!d1->size())
	cout << "Element whose valeu was " << d1->object() << " deleted\n";
    cout << "Allocating six more elements from the list: "; 
    d1 = l4.search_decrementing(6);
    cout << d1->object() + d1->size() << ".\n";
    if(!d1->size())
	cout << "Element whose valeu was " << d1->object() << " deleted\n";
    cout << "Allocating one element from the list: ";
    d1 = l4.search_decrementing(1);
    cout << d1->object() + d1->size() << ".\n";
    if(!d1->size())
	cout << "Element whose valeu was " << d1->object() << " deleted\n";
    cout << "The list has now " << l4.size() << " elements that group " 
	 << l4.grouped_size() << " elements in total.\n";

    return 0;
}
