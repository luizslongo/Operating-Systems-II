	.file	"crtstuff.c"
;	GCC For the Hitachi H8/300
;	By Hitachi America Ltd and Cygnus Support
; -O2


	.file	"crtstuff.c"
	.section	.ctors,"aw",@progbits
	.align 1
	.type	___CTOR_END__,@object
___CTOR_END__:
	.word	0
	.section	.dtors,"aw",@progbits
	.align 1
	.type	___DTOR_END__,@object
___DTOR_END__:
	.word	0
	.section	.eh_frame,"aw",@progbits
	.align 2
	.type	___FRAME_END__,@object
___FRAME_END__:
	.long	0
	.section	.jcr,"aw",@progbits
	.align 1
	.type	___JCR_END__,@object
___JCR_END__:
	.word	0
	.section .text
	.align 1
___do_global_ctors_aux:
	push	r6
	mov.w	r7,r6
	push	r4
	push	r5
	mov.w	#___CTOR_END__-2,r4
	mov.w	@r4,r2
	mov.w	#-1,r3
	cmp.w	r3,r2
	beq	.L8
	mov.w	r3,r5
.L6:
	jsr	@r2
	subs	#2,r4
	mov.w	@r4,r2
	cmp.w	r5,r2
	bne	.L6
.L8:
	pop	r5
	pop	r4
	pop	r6
	rts
	.align 1
_call___do_global_ctors_aux:
	push	r6
	mov.w	r7,r6
; #APP
		.section	.init
; #NO_APP
	jsr	@___do_global_ctors_aux
; #APP
		.section .text
; #NO_APP
	pop	r6
	rts
	.end
	.ident	"GCC: (GNU) 3.2.2"
