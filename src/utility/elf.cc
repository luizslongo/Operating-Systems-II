// EPOS-- ELF Utility Implementation

#include <utility/elf.h>
#include <utility/string.h>

__BEGIN_SYS

int ELF::load_segment(int i, void * addr)
{
    if((i > segments()) || (seg(i)->p_type != PT_LOAD))
	return -1;
    
    void * src = ((char *) this) + seg(i)->p_offset;
    void * dst = (addr)? addr : segment_address(i);
    
    memcpy(dst, src, seg(i)->p_filesz);
    memset((char *)dst + seg(i)->p_filesz, 0,
	   seg(i)->p_memsz - seg(i)->p_filesz);
    
    return seg(i)->p_memsz;
}

__END_SYS
