/*#ifdef __H8300H__
	.h8300h
#endif

#ifdef __H8300S__
	.h8300s
#endif*/

	.section .init
	.weak   ___epos_library_app_entry
        .type   ___epos_library_app_entry,@function
___epos_library_app_entry:
        .global  __init
__init:
        .section .fini
        .global  __fini
__fini:  
