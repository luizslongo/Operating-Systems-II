	.file	"h8_crt1.s"

	.text
	.align 1
	.globl _start
	.type  _start,@function
_start:
	jsr  @__init	
	.align 1
	.globl ___epos_library_app_entry
___epos_library_app_entry:
	jsr  @_main
	; push r0 ; Im not sure if this is really needed because
		  ; r0 is by default return and first parameter of functions
	jsr  @__fini
	jmp  @__exit
