        .file   "arm7_crtend.s"

.section .text
.align 4
.global exit
exit:
    b _exit
    /* TODO: destruct and halt! */
