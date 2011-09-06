
/*!
 ******************************************************************************
 * \file me_component.h
 *
 * \brief
 *    - The main header for Motion Estimation component. The applications
 *    should include only this header.
 *
 * \author
 *      - Alexandre Massayuki Okazaki   (alexandre@lisha.ufsc.br)
 *      - Mateus Krepsky Ludwich        (mateus@lisha.ufsc.br)
 *      - Tiago de Albuquerque Reis     (reis@lisha.ufsc.br)
 * 
 *******************************************************************************
 */

#ifndef ME_COMPONENT_H_
#define ME_COMPONENT_H_

#include "block_mode.h"
#include "block_mode_factory.h"
#include "block_mode_type.h"
// not include (internal) #include <dummy_picture_motion_estimator.h"
// not include (internal) #include <h264_modes_partitioner.h"
#include "mec_block.h"
#include "mec_macroblock.h"
#include "me_component_typedefs.h"
#include "mec_picture.h"
#include "mec_plane.h"
#include "mec_sample.h"
#include "modes_partitioner.h"
#include "algorithm_ids.h"

#include "vectorial_pmc.h"

#include "reference_picture_motion_counterpart.h"
#include "search_metric.h"
#include "mec_search_window.h"


/* Este trecho deverá ser substibuído por
 * #include <picture_motion_estimator.h"
 * (sem o uso de compilação condicional), se as interfaces forem
 * re-unificadas. (ver todo_doto...) */
#if 1 /* DISTRIBUTED */
#include "picture_motion_estimator_coordinator.h"
#endif

#endif /* ME_COMPONENT_H_ */

