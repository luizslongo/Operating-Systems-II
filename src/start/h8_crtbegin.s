	.file	"crtstuff.c"
;	GCC For the Hitachi H8/300
;	By Hitachi America Ltd and Cygnus Support
; -O2


	.file	"crtstuff.c"
	.section	.ctors,"aw",@progbits
	.align 1
	.type	___CTOR_LIST__,@object
___CTOR_LIST__:
	.word	-1
	.section	.dtors,"aw",@progbits
	.align 1
	.type	___DTOR_LIST__,@object
___DTOR_LIST__:
	.word	-1
	.section	.eh_frame,"aw",@progbits
	.align 2
	.type	___EH_FRAME_BEGIN__,@object
___EH_FRAME_BEGIN__:
	.section	.jcr,"aw",@progbits
	.align 1
	.type	___JCR_LIST__,@object
___JCR_LIST__:
; #APP
	.hidden	__dso_handle
; #NO_APP
	.global ___dso_handle
	.section .data
	.align 1
	.type	___dso_handle,@object
___dso_handle:
	.word	0
	.align 1
	.type	_p___0,@object
_p___0:
	.word	___DTOR_LIST__+2
	.local	_completed___1
	.comm	_completed___1,1,1
	.section .text
	.align 1
___do_global_dtors_aux:
	push	r6
	mov.w	r7,r6
	mov.b	@_completed___1,r2l
	bne	.L1
	mov.w	@_p___0,r2
	mov.w	@r2,r3
	beq	.L9
.L6:
	adds	#2,r2
	mov.w	r2,@_p___0
	jsr	@r3
	mov.w	@_p___0,r2
	mov.w	@r2,r3
	bne	.L6
.L9:
	mov.w	#___deregister_frame_info,r2
	beq	.L7
	mov.w	#___EH_FRAME_BEGIN__,r0
	jsr	@___deregister_frame_info
.L7:
	mov.b	#1,r2l
	mov.b	r2l,@_completed___1
.L1:
	pop	r6
	rts
	.align 1
_call___do_global_dtors_aux:
	push	r6
	mov.w	r7,r6
; #APP
		.section	.fini
; #NO_APP
	jsr	@___do_global_dtors_aux
; #APP
		.section .text
; #NO_APP
	pop	r6
	rts
	.local	_object___2
	.comm	_object___2,14,2
	.align 1
_frame_dummy:
	push	r6
	mov.w	r7,r6
	mov.w	#___register_frame_info,r2
	beq	.L12
	mov.w	#_object___2,r1
	mov.w	#___EH_FRAME_BEGIN__,r0
	jsr	@___register_frame_info
.L12:
	mov.w	#___JCR_LIST__,r0
	mov.w	@r0,r2
	beq	.L11
	mov.w	#__Jv_RegisterClasses,r2
	beq	.L11
	jsr	@__Jv_RegisterClasses
.L11:
	pop	r6
	rts
	.align 1
_call_frame_dummy:
	push	r6
	mov.w	r7,r6
; #APP
		.section	.init
; #NO_APP
	jsr	@_frame_dummy
; #APP
		.section .text
; #NO_APP
	pop	r6
	rts
	.weak	__Jv_RegisterClasses
	.weak	___deregister_frame_info
	.weak	___register_frame_info
	.end
	.ident	"GCC: (GNU) 3.2.2"
