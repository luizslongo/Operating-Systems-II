	.file	"crtstuff.c"
	.section	".data"
	.align 4
	.type	force_to_data,#object
force_to_data:
	.skip 4
		.section	".ctors",#alloc,#write
	.align 4
	.type	__CTOR_LIST__,#object
__CTOR_LIST__:
	.long	-1
		.section	".dtors",#alloc,#write
	.align 4
	.type	__DTOR_LIST__,#object
__DTOR_LIST__:
	.long	-1
	.section	".eh_frame",#alloc,#write
	.align 4
	.type	__EH_FRAME_BEGIN__,#object
__EH_FRAME_BEGIN__:
	.section	".jcr",#alloc,#write
	.align 4
	.type	__JCR_LIST__,#object
__JCR_LIST__:
	.hidden	__dso_handle
	.global __dso_handle
	.section	".data"
	.align 4
	.type	__dso_handle,#object
__dso_handle:
	.long	0
	.align 4
	.type	p.0,#object
p.0:
	.long	__DTOR_LIST__+4
	.local	completed.1
	.common	completed.1,1,1
	.section	".text"
	.align 4
	.type	__do_global_dtors_aux,#function
	.proc	020
__do_global_dtors_aux:
	!#PROLOGUE# 0
	save	%sp, -104, %sp
	!#PROLOGUE# 1
	sethi	%hi(completed.1), %o0
	ldub	[%o0+%lo(completed.1)], %o1
	cmp	%o1, 0
	bne	.LL1
	sethi	%hi(p.0), %l0
	ld	[%l0+%lo(p.0)], %o0
	ld	[%o0], %o1
	cmp	%o1, 0
	be,a	.LL10
	sethi	%hi(__deregister_frame_info), %o0
	add	%o0, 4, %o0
.LL11:
	call	%o1, 0
	st	%o0, [%l0+%lo(p.0)]
	ld	[%l0+%lo(p.0)], %o0
	ld	[%o0], %o1
	cmp	%o1, 0
	bne	.LL11
	add	%o0, 4, %o0
	sethi	%hi(__deregister_frame_info), %o0
.LL10:
	or	%o0, %lo(__deregister_frame_info), %o0
	cmp	%o0, 0
	be	.LL7
	sethi	%hi(__EH_FRAME_BEGIN__), %o0
	call	__deregister_frame_info, 0
	or	%o0, %lo(__EH_FRAME_BEGIN__), %o0
.LL7:
	mov	1, %o1
	sethi	%hi(completed.1), %o0
	stb	%o1, [%o0+%lo(completed.1)]
.LL1:
	nop
	ret
	restore
	.align 4
	.type	call___do_global_dtors_aux,#function
	.proc	020
call___do_global_dtors_aux:
	!#PROLOGUE# 0
	save	%sp, -104, %sp
	!#PROLOGUE# 1
		.section	".fini"
	call	__do_global_dtors_aux, 0
	 nop
		.section	".text"
	nop
	ret
	restore
	.local	object.2
	.common	object.2,24,4
	.align 4
	.type	frame_dummy,#function
	.proc	020
frame_dummy:
	!#PROLOGUE# 0
	save	%sp, -104, %sp
	!#PROLOGUE# 1
	sethi	%hi(__register_frame_info), %o0
	or	%o0, %lo(__register_frame_info), %o0
	cmp	%o0, 0
	sethi	%hi(__EH_FRAME_BEGIN__), %o0
	sethi	%hi(object.2), %o1
	or	%o0, %lo(__EH_FRAME_BEGIN__), %o0
	be	.LL14
	or	%o1, %lo(object.2), %o1
	call	__register_frame_info, 0
	 nop
.LL14:
	sethi	%hi(__JCR_LIST__), %o0
	ld	[%o0+%lo(__JCR_LIST__)], %o1
	cmp	%o1, 0
	be	.LL13
	or	%o0, %lo(__JCR_LIST__), %i0
	sethi	%hi(_Jv_RegisterClasses), %o0
	or	%o0, %lo(_Jv_RegisterClasses), %o0
	cmp	%o0, 0
	be	.LL13
	nop
	call	_Jv_RegisterClasses, 0
	 restore
.LL13:
	nop
	ret
	restore
	.align 4
	.type	call_frame_dummy,#function
	.proc	020
call_frame_dummy:
	!#PROLOGUE# 0
	save	%sp, -104, %sp
	!#PROLOGUE# 1
		.section	".init"
	call	frame_dummy, 0
	 nop
		.section	".text"
	nop
	ret
	restore
	.weak	_Jv_RegisterClasses
	.weak	__deregister_frame_info
	.weak	__register_frame_info
	.ident	"GCC: (GNU) 3.2.3"
