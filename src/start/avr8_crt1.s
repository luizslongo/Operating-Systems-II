        .file   "avr8_crt1.s"

        .text
        .align 1
        .globl __epos_library_app_entry
__epos_library_app_entry:
        rcall  main
	;jmp exit  ;besides doesnt have start, init and fini,
                  ;avr-gcc calls automatically exit from main
