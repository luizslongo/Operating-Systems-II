	.file	"crtstuff.c"
	.section	".data"
	.align 4
	.type	force_to_data,#object
force_to_data:
	.skip 4
		.section	".ctors",#alloc,#write
	.align 4
	.type	__CTOR_END__,#object
__CTOR_END__:
	.long	0
		.section	".dtors",#alloc,#write
	.align 4
	.type	__DTOR_END__,#object
__DTOR_END__:
	.long	0
	.section	".eh_frame",#alloc,#write
	.align 4
	.type	__FRAME_END__,#object
__FRAME_END__:
	.long	0
	.section	".jcr",#alloc,#write
	.align 4
	.type	__JCR_END__,#object
__JCR_END__:
	.long	0
	.section	".text"
	.align 4
	.type	__do_global_ctors_aux,#function
	.proc	020
__do_global_ctors_aux:
	!#PROLOGUE# 0
	save	%sp, -104, %sp
	!#PROLOGUE# 1
	sethi	%hi(__CTOR_END__), %o0
	or	%o0, %lo(__CTOR_END__), %o0
	ld	[%o0-4], %o1
	cmp	%o1, -1
	be	.LL8
	add	%o0, -4, %l0
	mov	%o1, %o0
.LL6:
	call	%o0, 0
	add	%l0, -4, %l0
	ld	[%l0], %o0
	cmp	%o0, -1
	bne	.LL6
	nop
.LL8:
	nop
	ret
	restore
	.align 4
	.type	call___do_global_ctors_aux,#function
	.proc	020
call___do_global_ctors_aux:
	!#PROLOGUE# 0
	save	%sp, -104, %sp
	!#PROLOGUE# 1
		.section	".init"
	call	__do_global_ctors_aux, 0
	 nop
		.section	".text"
	nop
	ret
	restore
	.ident	"GCC: (GNU) 3.2.3"
