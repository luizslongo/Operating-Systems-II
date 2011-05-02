.file "arm7_crt1.s"

.section .text
.align 4
.global _start
_start:
    bl _init

.align  4
.global __epos_library_app_entry
__epos_library_app_entry:
    bl  main
    bl  _fini
    bl  _exit
