/*

    LEON2/3 LIBIO low-level routines 
    Written by Jiri Gaisler.
    Copyright (C) 2004  Gaisler Research AB

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

	.text
	.global _start, main

_start:

	save	%sp, -64, %sp

        /* clear the bss */
 
        sethi %hi(bss_start),%g2
        or    %g2,%lo(bss_start),%g2  ! g2 = start of bss
        sethi %hi(_end),%g3
        or    %g3,%lo(_end),%g3         ! g3 = end of bss
        mov   %g0,%g1                   ! so std has two zeros
	sub   %g3, %g2, %g3
zerobss:
	subcc  %g3, 8, %g3
	bge,a   zerobss
	std    %g0,[%g2+%g3]

        call    main
        nop
	ret
	restore

        .seg    "data"
        .global .bdata
.bdata:
        .align  8
        .global _environ
_environ:
        .word   1
 


