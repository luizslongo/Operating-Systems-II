	    .file   "arm7_crt1.S"

        .text
		.align        4
        .global 	  __epos_library_app_entry
	#.set _rom_data_init, 0x108d0

__epos_library_app_entry:

	/* Initialize ROM variables
	 * This stuff should GO to /mach/mc13224v,
	 * but for a while they were sent to crti
	 */
	#ldr r12, =_rom_data_init
	#mov lr, pc
	#mov pc, r12

        b  main
	    # b        _fini
        # bl        _exit


