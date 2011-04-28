.file   "crtend.s"

.section    .ctors
__CTOR_END__:
    .long   0

.section    .dtors
__DTOR_END__:
    .long   0

.section    .eh_frame
__FRAME_END__:
    .long   0

.section    .jcr
__JCR_END__:
    .long   0

.text
.globl  __do_global_ctors_aux
.type   __do_global_ctors_aux, function

__do_global_ctors_aux:
    stmfd sp!, {lr}
    ldr r0, =__CTOR_END__ - 4
    ldr r1, =__CTOR_LIST__
  
    ctors_loop:
        cmp r0, r1
        beq ctors_loop_end
        ldr r2, [r0], #-4
        stmfd sp!, {r0-r1}
        blx r2
        ldmfd sp!, {r0-r1}
    b ctors_loop
  
    ctors_loop_end:
    ldmfd sp!, {pc}

.section    .init
    bl __do_global_ctors_aux
