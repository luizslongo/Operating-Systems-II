
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

#include "../include/mec_picture.h"
#include "../include/traits.h"

#if LINUX
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#else
#include <utility/malloc.h>
#endif

MEC_Picture * new_Picture(
		unsigned int width, unsigned int height,
		unsigned int chroma_width, unsigned int chroma_height,
		MEC_Sample ** Y_samples,
		MEC_Sample ** Cb_samples,
		MEC_Sample ** Cr_samples)
{
    MEC_Picture * picture;
    picture = (MEC_Picture *) malloc(sizeof(MEC_Picture));

    picture->Y = new_Plane(width, height, Y_samples);

#if ENABLE_CHROMA_PLANES
    picture->Cb = new_Plane(chroma_width, chroma_height, Cb_samples);
    picture->Cr = new_Plane(chroma_width, chroma_height, Cr_samples);
#endif

    return picture;
}

void delete_Picture(MEC_Picture * _this) {
    delete_Plane(_this->Y);

#if ENABLE_CHROMA_PLANES
    delete_Plane(_this->Cb);
    delete_Plane(_this->Cr);
#endif

    free(_this);
}
