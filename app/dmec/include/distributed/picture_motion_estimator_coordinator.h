
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

#ifndef PICTURE_MOTION_ESTIMATOR_COORDINATOR_H_
#define PICTURE_MOTION_ESTIMATOR_COORDINATOR_H_

#include "../block_mode.h"
#include "../vectorial_pmc.h"
#include "../mec_picture.h"
#include "dmec_picture.h"
#include "../search_metric.h"
#include "../reference_picture_motion_counterpart.h"

#include "../traits.h"
#include "picture_motion_estimator_worker_proxy.h"

/// << singleton >>
class PictureMotionEstimator {
    unsigned int __pictureWidth;
    unsigned int __pictureHeight;

    BlockMode ** __modesAtLevelZero;
    unsigned int __numberOfModesAtLevelZero;
    PictureMotionCounterpart __pmc;

    PictureMotionEstimatorWorkerProxy __workers[NUM_WORKERS];

private:
    PictureMotionEstimator();

    ~PictureMotionEstimator();


    PictureMotionEstimator(unsigned int pictureWidth,
    		unsigned int pictureHeight,
    		unsigned int max_reference_pictures);

private:
    static PictureMotionEstimator * __instance;


public:
    static PictureMotionEstimator * getInstance(unsigned int pictureWidth,
    		unsigned int pictureHeight,
    		unsigned int max_reference_pictures);


    static void deleteInstance();


    PictureMotionCounterpart * match(MEC_Picture * currentPicture,
    		MEC_Picture ** list0ReferencePictures,
    		unsigned int list0ReferencePicturesSize);


    int pme_informMatchAlgorithm();


private:
    unsigned int __computeTotalFlatPMC_Size(unsigned int list0ReferencePictureSize,
    		unsigned int list1ReferencePictureSize);


    PictureMotionCounterpart * __commonMatch(MEC_Picture * currentPicture,
    		MEC_Picture ** list0,
    		unsigned int list0Size,
    		MEC_Picture ** list1,
    		unsigned int list1Size);


    void __fillPMC_WithFlatPMC(PictureMotionCounterpart * pmc,
    		FlatH264PMC_Node * flatPMC,
    		unsigned int flatPMC_Size,
    		int workerID);


    void pp_copyPartitioning(MEC_Picture * picture,
    		DMEC_Picture * partitions,
    		unsigned int numPartitions);


};


#endif /* PICTURE_MOTION_ESTIMATOR_COORDINATOR_H_ */

