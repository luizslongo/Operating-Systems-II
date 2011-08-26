/*
 * d_trace.h
 *
 *  Created on: Mar 20, 2011
 *      Author: mateus
 */

#ifndef D_TRACE_H_
#define D_TRACE_H_

#include <stdbool.h>

void Trace(bool enabled, char * context, char * message);


void Trace_uint(bool enabled, char * context, char * message, unsigned int value);


void Trace_64addr(bool enabled, char * context, char * message, unsigned long long address);


#endif /* D_TRACE_H_ */
