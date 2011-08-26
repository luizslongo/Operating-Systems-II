
/*!
 *******************************************************************************
 * \file
 *
 * \brief
 *    -
 *
 * \author
 *      - Alexandre Massayuki Okazaki   (alexandre@lisha.ufsc.br)
 *      - Mateus Krepsky Ludwich        (mateus@lisha.ufsc.br)
 *      - Tiago de Albuqueque Reis      (reis@lisha.ufsc.br)
 * 
 *******************************************************************************
 */

#include "../include/simple_tracer.h"

void simple_tracer_trace(char * msg) {
#if TRACE_ENABLED
    printf("%s\n", msg);
#endif
}

void simple_tracer_traceUint(char * msg, uint value) {
#if TRACE_ENABLED
    printf("%s: %i\n", msg, value);
#endif
}

