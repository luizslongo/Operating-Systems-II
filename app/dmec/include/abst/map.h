/*
 * map.h
 *
 *  Created on: Aug 5, 2010
 *      Author: mateus
 */

#ifndef MAP_H_
#define MAP_H_

#include "quadruple_ulong.h"

/* <<singleton>> */

void map_put(unsigned long key, Quadruple_ulong * value); // Copy value data to inside the map

void map_get(Quadruple_ulong * value, unsigned long key);

#endif /* MAP_H_ */
