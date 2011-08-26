
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
 *      - Tiago de Albuquerque Reis      (reis@lisha.ufsc.br)
 * 
 ******************************************************************************
 */

#include "../include/master_tracedefs.h"

#include "../include/distributed/dmec_picture.h"

#define __DMEC_PICTURE_DETAILED_TRACE (0 && ENABLE_TRACE)

static inline void traceMIMII(char * m1, int i1, char * m2, int i2, int i3) {
#if 0 // __DMEC_PICTURE_DETAILED_TRACE
	printf(m1, i1);
	printf(m2, i2, i3);
	printf("\n");
#endif
}

void copy_MEC_Picture_to_DMEC_Picture(MEC_Picture * source, DMEC_Picture * target) {
	//assert(source);
	//assert(target);

	/* target->ID = source->ID; */
	target->width = source->Y->width;
	target->height = source->Y->height;

#if ENABLE_CHROMA_PLANES
	assert(source->Cb->width == source->Cr->width);
	assert(source->Cb->height == source->Cr->height);
	target->chroma_width = source->Cb->width;
	target->chroma_height = source->Cb->height;
#endif

	unsigned int row, col;
	for (row = 0; row < source->Y->height; ++ row) {
		for (col = 0; col < source->Y->width; ++ col) {
			/* traceMIMII("target->Y_samples[%i] = ", row * target->width + col,
						 "source->Y->samples[%i][%i]", row, col); */

			target->Y_samples[row * target->width + col] = source->Y->samples[row][col];
		}
	}

#if ENABLE_CHROMA_PLANES
	for (row = 0; row < source->Cb->height; ++ row) {
		for (col = 0; col < source->Cb->width; ++ col) {
			target->Cb_samples[row * target->chroma_width + col] = source->Cb->samples[row][col];
			target->Cr_samples[row * target->chroma_width + col] = source->Cr->samples[row][col];
		}
	}
#endif

}

void setUpEAs_DMEC_Picture(DMEC_Picture * picture, EAs_DMEC_Picture * eas) {
	/* unsigned long ea_ID; */
	unsigned long ea_Y_samples;

#if ENABLE_CHROMA_PLANES
	unsigned long ea_Cb_samples;
	unsigned long ea_Cr_samples;
#endif

	/* ea_ID = (unsigned long) &picture->ID; */
	ea_Y_samples = (unsigned long) &picture->Y_samples;

#if ENABLE_CHROMA_PLANES
	ea_Cb_samples = (unsigned long) &picture->Cb_samples;
	ea_Cr_samples = (unsigned long) &picture->Cr_samples;
#endif

	/* eas->ea_ID = ea_ID; */
	eas->ea_Y_samples = ea_Y_samples;

#if ENABLE_CHROMA_PLANES
	eas->ea_Cb_samples = ea_Cb_samples;
	eas->ea_Cr_samples = ea_Cr_samples;
#endif

#if __DMEC_PICTURE_DETAILED_TRACE
	printf("setUpEAs_DMEC_Picture\n");
	printf("...ea_Y_samples: %llx\n", eas->ea_Y_samples);

#if ENABLE_CHROMA_PLANES
	printf("...ea_Cb_samples: %llx\n", eas->ea_Cb_samples);
	printf("...ea_Cr_samples: %llx\n", eas->ea_Cr_samples);
#endif
#endif
}


#if ENABLE_DUMP
static void __dumpSamples(MEC_Sample samples[], char * buffer, int buffer_size, FILE * outputFile, unsigned int width, unsigned int height) {
	unsigned int row, col;

	for (row = 0; row < height; ++ row) {
		d_dumpI(outputFile, row, "=>");

		for(col = 0; col < width; ++ col) {
			d_dumpMEC_Sample(outputFile, samples[row * width + col]);
		}
	}

	fprintf(outputFile, "\n");
}
#endif

#if ENABLE_DUMP
void dmec_picture_dumpPicture(DMEC_Picture * picture, char * fileName) {
	const int buffer_size = 50;
	char buffer[buffer_size];
	char c = '\0';
	memset(buffer, c, buffer_size);
	FILE * outputFile = fopen(fileName, "w" );

	/*
	snprintf(buffer, buffer_size, "Picture: %i\n", picture->ID);
	fwrite(buffer, sizeof(char), buffer_size, outputFile);
	memset(buffer, c, buffer_size);
	*/

	fprintf(outputFile, "Picture\n");
	fprintf(outputFile, "Y plane");

	__dumpSamples(picture->Y_samples, buffer, buffer_size, outputFile, picture->width, picture->height);

#if ENABLE_CHROMA_PLANES
	d_dump(buffer, buffer_size, outputFile, "\nCb plane:");
	__dumpSamples(picture->Cb_samples, buffer, buffer_size, outputFile, picture->chroma_width, picture->chroma_height);

	d_dump(buffer, buffer_size, outputFile, "\nCr plane:");
	__dumpSamples(picture->Cr_samples, buffer, buffer_size, outputFile, picture->chroma_width, picture->chroma_height);
#endif

	fprintf(outputFile, "\n");

	fclose(outputFile);
}
#endif
