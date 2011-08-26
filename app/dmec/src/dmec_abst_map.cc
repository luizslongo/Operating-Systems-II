/*
 * map.c
 *
 *  Created on: Aug 5, 2010
 *      Author: mateus
 */

#if 0

#include <dmec_java/abst/map.h>

#include <stdio.h>
#include <stdbool.h>

static unsigned int __index = 0;

#define MAX_ELEMENTS 20

static unsigned long keys[MAX_ELEMENTS];
static Quadruple_ulong values[MAX_ELEMENTS];


// Copy value data to inside the map
void map_put(unsigned long key, Quadruple_ulong * value) {
	keys[__index] = key;

	values[__index].e0 = value->e0;
	values[__index].e1 = value->e1;
	values[__index].e2 = value->e2;
	values[__index].e3 = value->e3;

	__index ++;
}

void map_get(Quadruple_ulong * value, unsigned long key) {
	int i;
	bool found = false;

	for (i = 0; i < MAX_ELEMENTS; ++ i) {
		if (key == keys[i]) {
			value->e0 = values[i].e0;
			value->e1 = values[i].e1;
			value->e2 = values[i].e2;
			value->e3 = values[i].e3;

			found = true;
			printf("map_get, key: %lx, found!\n", key);
		}
	}

	if (! found) {
		printf("map_get, key: %lx, not found!\n", key);
	}

}

#endif
