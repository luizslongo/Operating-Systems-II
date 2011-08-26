
/*!
 ******************************************************************************
 * \file
 *
 * \brief
 *    -
 *
 * \author
 *      - Alexandre Massayuki Okazaki   (alexandre@lisha.ufsc.br)
 *      - Mateus Krepsky Ludwich        (mateus@lisha.ufsc.br)
 *      - Tiago de Albuquerque Reis     (reis@lisha.ufsc.br)
 * 
 ******************************************************************************
 */

#ifndef SIMPLE_TRACE_H_
#define SIMPLE_TRACE_H_

//#include <stdio.h>
#include "me_component_typedefs.h"

#define TRACE_ENABLED 0

void simple_tracer_trace(char * msg);

void simple_tracer_traceUint(char * msg, uint value);

#endif // SIMPLE_TRACE_H_
