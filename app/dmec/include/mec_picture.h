
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

#ifndef PICTURE_H_
#define PICTURE_H_

#include "mec_plane.h"
#include "traits.h"

typedef struct {
    MEC_Plane * Y;

#if ENABLE_CHROMA_PLANES
    MEC_Plane * Cb;
    MEC_Plane * Cr;
#endif

    /* MEC_Macroblock */
	void *** macroblocks;

} MEC_Picture;

MEC_Picture * new_Picture(
		unsigned int width, unsigned int height,
		unsigned int chroma_width, unsigned int chroma_height,
		MEC_Sample ** Y_samples,
		MEC_Sample ** Cb_samples,
		MEC_Sample ** Cr_samples);

void delete_Picture(MEC_Picture * _this);

void mec_picture_dumpPicture(MEC_Picture * picture, char * fileName);

#endif /* PICTURE_H_ */

