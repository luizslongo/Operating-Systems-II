/*
 Author: Mateus Krepsky Ludwich.
*/
#if 0

#include <dmec_java/abst/quadruple_ulong.h>
#include <dmec_java/abst/map.h>

#include <dmec_java/distributed/mec_observer.h>
#include <dmec_java/distributed/common_constants.h>
#include <dmec_java/distributed/worker_parameters.h>
#include <dmec_java/distributed/flat_h264_pmc_node.h>

#include <dmec_java/master_tracedefs.h>

#include <dmec_java/picture_motion_counterpart.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NUM_MACROBLOCKS 8160 // for pictures 1920 x 1088

#define __MEC_OBSERVER_GENERAL_TRACE (0 && ENABLE_TRACE)
#define __MEC_OBSERVER_DETAILED_TRACE (0 && ENABLE_TRACE)

/* External methods -------------------------------------------------------- */
extern WorkerParameters * __pictureMotionEstimatorWorkerProxy_getWorkerParameters(int workerID);

/* ------------------------------------------------------------------------- */


/* Private type definitions ------------------------------------------------ */
typedef struct {
    int workerID;
    int picturePartitionX;
    int picturePartitionY;
    unsigned int picturePartitionWidth;
    unsigned int picturePartitionHeight;
    int picturePartitionFirstMacroblockID;

} MEC_Observer_WorkerProxyInfo;

typedef struct {
    int _index_macroblocksInWorkerDomain;
    unsigned int numMacroblocksInWorkerDomain;
    int macroblocksInWorkerDomain[MAX_NUM_MACROBLOCKS / NUM_WORKERS];

} MEC_Observer_WorkerInfo;

typedef struct {
    int xInPicture[MAX_NUM_MACROBLOCKS];
    int yInPicture[MAX_NUM_MACROBLOCKS];
    unsigned int numMacroblocks;

} MEC_Observer_PMC_Info;

typedef struct {
    MEC_Observer_WorkerProxyInfo workerProxyInfo[NUM_WORKERS];
    MEC_Observer_PMC_Info pmcInfo;
    MEC_Observer_WorkerInfo workerInfo[NUM_WORKERS];
    bool __enabled;

} MEC_Observer;

/* ------------------------------------------------------------------------- */


/* Singleton --------------------------------------------------------------- */
static MEC_Observer observer;

/* ------------------------------------------------------------------------- */


/* Private methods declaration --------------------------------------------- */
static void check_setUp_PictureMotionEstimatorWorkerProxy_END(int workerID);
static void check_commonMatch_END(PictureMotionCounterpart * pmc);
static void check_fillPMC_WithFlatPMC_BEGIN(int workerID, FlatH264PMC_Node * flatPMC, unsigned int flatPMC_Size);

static inline void Assert(bool expression);
/* ------------------------------------------------------------------------- */


/* Methods implementation -------------------------------------------------- */
void mec_observer_enable() {
	observer.__enabled = true;
}

void mec_observer_disable() {
	observer.__enabled = false;

}


void mec_observer_update(enum ObservableMethod method, int arg0, void * arg1, unsigned int arg2) {
#if __MEC_OBSERVER_GENERAL_TRACE
    printf("mec_observer_update\n");
#endif

    switch (method) {
        case MEC_OBSERVER_setUp_PictureMotionEstimatorWorkerProxy_END:
            check_setUp_PictureMotionEstimatorWorkerProxy_END(arg0);
            break;
        case MEC_OBSERVER_commonMatch_END:
            check_commonMatch_END((PictureMotionCounterpart *) arg1);
            break;
        case MEC_OBSERVER_fillPMC_WithFlatPMC_BEGIN:
            check_fillPMC_WithFlatPMC_BEGIN(arg0, (FlatH264PMC_Node *) arg1, arg2);
            break;
        default:
            abort();
            break;
    }
}

void mec_observer_setExpectedWorkerProxyValues(int workerID,
        int picturePartitionX, int picturePartitionY,
        unsigned int picturePartitionWidth, unsigned int picturePartitionHeight,
        int picturePartitionFirstMacroblockID)
{
    observer.workerProxyInfo[workerID].workerID = workerID;
    observer.workerProxyInfo[workerID].picturePartitionX = picturePartitionX;
    observer.workerProxyInfo[workerID].picturePartitionY = picturePartitionY;
    observer.workerProxyInfo[workerID].picturePartitionWidth = picturePartitionWidth;
    observer.workerProxyInfo[workerID].picturePartitionHeight = picturePartitionHeight;
    observer.workerProxyInfo[workerID].picturePartitionFirstMacroblockID = picturePartitionFirstMacroblockID;
}

void mec_observer_setExpectedNumMacroblocks(int numMacroblocks) {
    observer.pmcInfo.numMacroblocks = numMacroblocks;
}

void mec_observer_setExpectedMacroblockXY_Values(int macroblockID, int xInPicture, int yInPicture) {
    Assert(macroblockID < observer.pmcInfo.numMacroblocks);
    observer.pmcInfo.xInPicture[macroblockID] = xInPicture;
    observer.pmcInfo.yInPicture[macroblockID] = yInPicture;
}

void mec_observer_setExpectedNumMacroblocksInWorkerDomain(int workerID, unsigned int n) {
    observer.workerInfo[workerID].numMacroblocksInWorkerDomain = n;
    observer.workerInfo[workerID]._index_macroblocksInWorkerDomain = 0;
}

void mec_observer_putExpectedWorker2MacroblockMapping(int workerID, int macroblockID) {
    int i = observer.workerInfo[workerID]._index_macroblocksInWorkerDomain;

    // observer.workerInfo[workerID].workerIDs[i] = workerID;
    observer.workerInfo[workerID].macroblocksInWorkerDomain[i] = macroblockID;

    observer.workerInfo[workerID]._index_macroblocksInWorkerDomain ++;
}



void mec_observer_registerExpected_ea_s_picture(char * caller,
		unsigned long ea_picture, unsigned long ea_ID,
		unsigned long ea_Y_samples,
		unsigned long ea_Cb_samples, unsigned long ea_Cr_samples)
{
#if __MEC_OBSERVER_DETAILED_TRACE
	printf("mec_observer_registerExpected_ea_s_picture\n");
	printf("caller: %s\n", caller);
	printf("ea_picture: %lx\n", ea_picture);
	printf("ea_ID: %lx\n", ea_ID);
	printf("ea_Y_samples: %lx\n", ea_Y_samples);
	printf("ea_Cb_samples: %lx\n", ea_Cb_samples);
	printf("ea_Cr_samples: %lx\n", ea_Cr_samples);
#endif

	//-- Quadruple_ulong eas;
	//-- map_put(ea_picture, &eas); // copy eas data to inside the map
}

void mec_observer_check_ea_s_picture(char * caller,
		unsigned long ea_picture, unsigned long ea_ID,
		unsigned long ea_Y_samples,
		unsigned long ea_Cb_samples, unsigned long ea_Cr_samples)
{
#if __MEC_OBSERVER_DETAILED_TRACE
	printf("mec_observer_check_ea_s_picture\n");
	printf("caller: %s\n", caller);
	printf("ea_picture: %lx\n", ea_picture);
	printf("ea_ID: %lx\n", ea_ID);
	printf("ea_Y_samples: %lx\n", ea_Y_samples);
	printf("ea_Cb_samples: %lx\n", ea_Cb_samples);
	printf("ea_Cr_samples: %lx\n", ea_Cr_samples);
#endif

	/*
	Quadruple_ulong eas;
	map_get(&eas, ea_picture);
	Assert(eas.e0 == ea_ID);
	Assert(eas.e1 == ea_Y_samples);
	Assert(eas.e2 == ea_Cb_samples);
	Assert(eas.e3 == ea_Cr_samples);
	*/
}

/* ------------------------------------------------------------------------- */


/* Private Methods implementation ------------------------------------------ */
void check_setUp_PictureMotionEstimatorWorkerProxy_END(int workerID) {
#if __MEC_OBSERVER_GENERAL_TRACE
    printf("check_setUp_PictureMotionEstimatorWorkerProxy_END\n");
    printf("...checking if worker proxy: %i is correct\n", workerID);
#endif

    WorkerParameters * currentWorkerProxyInfo;
    currentWorkerProxyInfo = __pictureMotionEstimatorWorkerProxy_getWorkerParameters(workerID);

    MEC_Observer_WorkerProxyInfo * expectedWorkerProxyInfo;
    expectedWorkerProxyInfo = &(observer.workerProxyInfo[workerID]);

#if __MEC_OBSERVER_DETAILED_TRACE
    // incomplete trace...
    if (currentWorkerProxyInfo->picturePartitionX != expectedWorkerProxyInfo->picturePartitionX) {
    	printf("...picturePartitionX, got: %i, expected: %i\n", currentWorkerProxyInfo->picturePartitionX, expectedWorkerProxyInfo->picturePartitionX);
    }

	if (currentWorkerProxyInfo->picturePartitionY != expectedWorkerProxyInfo->picturePartitionY);
	if (currentWorkerProxyInfo->picturePartitionWidth != expectedWorkerProxyInfo->picturePartitionWidth);
	if (currentWorkerProxyInfo->picturePartitionHeight != expectedWorkerProxyInfo->picturePartitionHeight);
	if (currentWorkerProxyInfo->picturePartitionFirstMacroblockID != expectedWorkerProxyInfo->picturePartitionFirstMacroblockID);
#endif

    Assert(currentWorkerProxyInfo->picturePartitionX == expectedWorkerProxyInfo->picturePartitionX);
    Assert(currentWorkerProxyInfo->picturePartitionY == expectedWorkerProxyInfo->picturePartitionY);
    Assert(currentWorkerProxyInfo->picturePartitionWidth == expectedWorkerProxyInfo->picturePartitionWidth);
    Assert(currentWorkerProxyInfo->picturePartitionHeight == expectedWorkerProxyInfo->picturePartitionHeight);
}

void check_commonMatch_END(PictureMotionCounterpart * pmc) {
#if __MEC_OBSERVER_GENERAL_TRACE
    printf("check_commonMatch_END\n");
#endif

    Assert(pmc->numberOfMacroblockMotionCounterparts == observer.pmcInfo.numMacroblocks);

    int i;

    int expected_xInPicture;
    int expected_yInPicture;
    int current_xInPicture;
    int current_yInPicture;

    int current_mbID;

    for (i = 0; i < observer.pmcInfo.numMacroblocks; ++ i) {
        MEC_Macroblock * macroblock;
        macroblock = findMacroblockMotionCounterpartByID(pmc, i)->macroblock;

        current_mbID = macroblock->ID;
        Assert(current_mbID == i);

        expected_xInPicture = observer.pmcInfo.xInPicture[i];
        current_xInPicture = macroblock->xInPicture;

        expected_yInPicture = observer.pmcInfo.yInPicture[i];
        current_yInPicture = macroblock->yInPicture;

        if (current_xInPicture != expected_xInPicture) {
#if __MEC_OBSERVER_DETAILED_TRACE
            printf("...MBID:%i, current_xInPicture: %i, expected_xInPicture: %i\n", i, current_xInPicture, expected_xInPicture);
#endif
        }

        Assert(current_xInPicture == expected_xInPicture);

        if (current_yInPicture != expected_yInPicture) {
#if __MEC_OBSERVER_DETAILED_TRACE
            printf("...MBID:%i, current_yInPicture: %i, expected_yInPicture: %i\n", i, current_yInPicture, expected_yInPicture);
#endif
        }

        Assert(current_yInPicture == expected_yInPicture);
    }
}

void check_fillPMC_WithFlatPMC_BEGIN(int workerID, FlatH264PMC_Node * flatPMC, unsigned int flatPMC_Size)
{
    int i, j;
    int mbID_is_in_worker_domain;
    int mbID_is_in_flatPMC;

    for (i = 0; i < flatPMC_Size; ++ i) {
        mbID_is_in_worker_domain = 0;
        // assegura que não tem "lixo" no flat pmc
        for (j = 0; j < observer.workerInfo[workerID].numMacroblocksInWorkerDomain; ++ j) {
            if (flatPMC[i].macroblockID == observer.workerInfo[workerID].macroblocksInWorkerDomain[j]) {
                mbID_is_in_worker_domain = 1;
            }
        }

        if (! mbID_is_in_worker_domain) {
#if __MEC_OBSERVER_DETAILED_TRACE
            printf("flatPMC for worker: %i, cannot contain: %i MBID\n", workerID, flatPMC[i].macroblockID);
#endif
        }
        Assert(mbID_is_in_worker_domain);

    }

    for (i = 0; i < observer.workerInfo[workerID].numMacroblocksInWorkerDomain; ++ i) {
        mbID_is_in_flatPMC = 0;
        int id = observer.workerInfo[workerID].macroblocksInWorkerDomain[i];

        // assegura que flat pmc contém todos os ids de macrobloco que deve conter
        for (j = 0; j < flatPMC_Size; ++j) {
            if (flatPMC[j].macroblockID == id) {
                mbID_is_in_flatPMC = 1;
            }
        }
        Assert(mbID_is_in_flatPMC);
    }

}

void Assert(bool expression) {
	if (observer.__enabled) {
		assert(expression);
	}
}

/* ------------------------------------------------------------------------- */

#endif
