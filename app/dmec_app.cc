#include "dmec/include/traits.h"

#include "dmec/include/master_tracedefs.h"
#include "dmec/include/abst_random.h"
#include "dmec/include/flat_h264_pmc_node.h"
#include "dmec/include/abst_assert.h"
#include "dmec/include/picture_partitioner.h"
#include "dmec/include/reference_bma.h"


#if ENABLE_MEC_OBSERVER
#include "dmec/include/distributed/mec_observer.h"
#endif

#include "dmec/include/me_component.h"

#if LINUX
#include<iostream>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

#include "dmec/include/dumper.h"

using namespace std;

#else
#include <utility/ostream.h>
#include <utility/malloc.h>

#endif


#define REF_PIC_ID_ONE 43

#define MAX_PEL_SIZE 10
/* #define MAX_PEL_SIZE SHRT_MAX/2 */


void setUpTestSuport(enum ReferenceMatchAlgorithms algorithm, PictureMotionEstimator * pictureMotionEstimator);

MEC_Picture * _createA_Picture(uint width, uint height);
MEC_Picture * _createSpecificPicture(unsigned int width, unsigned int height, unsigned int dataSet);

void testPMC_and_beyond(PictureMotionCounterpart * pmc,
		unsigned int pictureWidth, unsigned int pictureHeight,
		MEC_Picture * currentPicture,
		MEC_Picture ** list0, unsigned int list0Size,
		MEC_Picture ** list1, unsigned int list1Size,
		unsigned int max_reference_pictures);



#define __TEST_SUPORT_GENERAL_TRACE 0 // (1 && ENABLE_TRACE)
#define __TEST_SUPORT_DETAILED_TRACE 0 // (1 && ENABLE_TRACE)
#define __TEST_SUPORT_DUMP 0 && ENABLE_DUMP
#define __TEST_SUPORT_ADVANCED_DUMP 0 && ENABLE_DUMP


#define __MAIN_GENERAL_TRACE 1
#define __MAIN_DETAILED_TRACE 1

// Just sets of test functions.
void testPack10();
void testPack20();

// Test functions declaration.
static void __testRandomicSimplePrediction(unsigned int timesMatch, unsigned int pictureWidth, unsigned int pictureHeight, enum ReferenceMatchAlgorithms referenceBMA);
static void __testFixedSimplePrediction(unsigned int timesMatch, unsigned int pictureWidth, unsigned int pictureHeight, enum ReferenceMatchAlgorithms referenceBMA, unsigned int dataSetCurrentPicture, unsigned int dataSetReferencePicture);

#if !LINUX
System::OStream cout;
#endif

int main()
{
#if LINUX
    random_set_seed(time(NULL));
#else
    random_set_seed(33); // TODO use something else here.
#endif

#if 1 /* QCIF, one match, simple prediction, Full Search algorithm, Fixed Inputs */
    testPack20();
#endif

    cout << "done: OK\n";
    cout << "This ran using: " << NUM_WORKERS << " worker threads\n";
    cout << "bye!\n";
    return 0;
}


void testPack10()
{
	/* QCIF, one match, simple prediction, Full Search algorithm */
	__testRandomicSimplePrediction(1, 176, 144, REFERENCE_FULL_SEARCH_V01);
}


void testPack20()
{
#if __MAIN_GENERAL_TRACE
    cout << "++++++++ testing 176x144 (1 match, fixed set, QCIF, simple prediction)\n";
#endif
    __testFixedSimplePrediction(1, 176, 144, REFERENCE_FULL_SEARCH_V01, 0, 1);
}


void __testRandomicSimplePrediction(unsigned int timesMatch,
		unsigned int pictureWidth,
		unsigned int pictureHeight,
		enum ReferenceMatchAlgorithms referenceBMA)
{
    // creations
    PictureMotionEstimator * pictureMotionEstimator;

    uint list0Size = 1;
    unsigned int max_reference_pictures = list0Size;

    pictureMotionEstimator = PictureMotionEstimator::getInstance(
    		pictureWidth,
    		pictureHeight,
    		list0Size);

	setUpTestSuport(referenceBMA, pictureMotionEstimator);

    MEC_Picture * currentPicture = _createA_Picture(pictureWidth, pictureHeight);

    MEC_Picture ** list0 = (MEC_Picture **) calloc(list0Size, sizeof(MEC_Picture *));

    unsigned int i;
    for (i = 0; i < list0Size; ++ i) {
    	list0[i] = _createA_Picture(pictureWidth, pictureHeight);
    }

    PictureMotionCounterpart * pmc;

    unsigned int im;
    for (im = 0; im < timesMatch; ++ im) {
		// match
#if __MAIN_GENERAL_TRACE && LINUX
    	cout << "...match#: " << im+1 << " (of: " << timesMatch << ")\n";
#endif
		pmc = pictureMotionEstimator->match(currentPicture, list0, list0Size);

		// assertions
		testPMC_and_beyond(pmc,
		        pictureWidth, pictureHeight,
		        currentPicture,
		        list0, list0Size,
		        0, 0,
		        max_reference_pictures);

    }

    // deletions
	PictureMotionEstimator::deleteInstance();
	delete_Picture(currentPicture);

	for (i = 0; i < list0Size; ++ i) {
		delete_Picture(list0[i]);
	}

	free(list0);
}


void __testFixedSimplePrediction(unsigned int timesMatch,
		unsigned int pictureWidth,
		unsigned int pictureHeight,
		enum ReferenceMatchAlgorithms referenceBMA,
		unsigned int dataSetCurrentPicture,
		unsigned int dataSetReferencePicture)
{
    PictureMotionEstimator * pictureMotionEstimator = PictureMotionEstimator::getInstance(pictureWidth, pictureHeight, 1);
    setUpTestSuport(referenceBMA, pictureMotionEstimator);

    MEC_Picture * currentPicture = _createSpecificPicture(pictureWidth, pictureHeight, dataSetCurrentPicture);

    MEC_Picture ** list0 = (MEC_Picture **) calloc(1, sizeof(MEC_Picture *));
	list0[0] = _createSpecificPicture(pictureWidth, pictureHeight, dataSetReferencePicture);

    PictureMotionCounterpart * pmc;

    unsigned int im;
    for (im = 0; im < timesMatch; ++ im) {
#if __MAIN_GENERAL_TRACE
    	cout << "...match#: " << im+1 << " (of: " << timesMatch << ")\n";
#endif
		pmc = pictureMotionEstimator->match(currentPicture, list0, 1);

		testPMC_and_beyond(pmc, pictureWidth, pictureHeight, currentPicture, list0, 1, 0, 0, 1);
    }

	PictureMotionEstimator::deleteInstance();
	delete_Picture(currentPicture);
	delete_Picture(list0[0]);
	free(list0);
}


/* External friend methods -------------------------------------------------------- */
extern WorkerParameters * __pictureMotionEstimatorWorkerProxy_getWorkerParameters(int workerID);

/* ------------------------------------------------------------------------- */


// Trace functions ------------------------------------------------------------
#if __TEST_SUPORT_GENERAL_TRACE
static void trace(char * msg) {

	cout << msg << "\n";
}
#endif

#if __TEST_SUPORT_DETAILED_TRACE
static void traceSSh(char * msg, short i) {
	cout << "..." << msg;
	cout << ": " << i << "\n";
}
#endif


// ----------------------------------------------------------------------------


// Globals --------------------------------------------------------------------
static enum ReferenceMatchAlgorithms reference_match_algorithm;
static FlatH264PMC_Node auxFlatPMC_Node;


static MEC_Sample __samples[2][32][48] = {
		{{2,2,3,7,1,6,2,6,8,6,8,1,3,3,8,8,2,7,2,1,4,7,8,9,6,8,1,8,1,3,1,5,1,1,7,7,4,7,3,3,3,2,4,7,6,3,5,8},
		{8,5,5,9,4,1,7,5,9,9,1,5,3,2,7,7,1,3,2,5,9,9,6,7,9,4,7,4,6,9,2,4,4,2,3,3,3,9,4,6,1,1,4,1,2,5,5,5},
		{1,6,4,2,1,7,9,4,8,5,4,8,2,4,2,6,3,6,3,4,8,7,5,2,1,5,4,7,9,5,5,8,9,6,5,3,6,6,2,9,5,5,3,2,8,8,6,4},
		{1,9,5,6,3,2,6,8,2,7,4,7,3,7,3,7,3,9,3,9,6,9,3,9,8,1,4,6,1,4,1,8,7,4,4,6,7,1,1,8,8,5,5,1,2,9,1,3},
		{2,1,1,4,9,6,9,4,3,9,4,9,7,8,3,3,6,4,5,4,3,9,5,2,9,6,6,1,7,6,3,2,5,3,9,5,8,2,6,7,7,5,4,6,8,8,2,3},
		{4,6,6,4,6,1,7,5,7,3,6,5,8,5,4,7,1,3,7,1,7,2,6,3,5,5,3,9,1,3,7,1,3,9,4,5,4,3,5,1,1,9,1,8,8,7,1,1},
		{2,6,5,2,8,5,6,6,7,6,9,7,1,9,2,5,6,5,5,7,6,5,8,5,6,6,5,3,8,7,6,4,2,9,3,8,5,9,5,2,8,2,3,8,5,8,8,2},
		{4,4,9,8,5,6,5,1,9,3,8,6,8,5,1,9,1,4,2,7,5,8,3,4,7,5,8,2,5,9,2,3,5,8,8,6,8,2,5,4,6,9,2,5,3,3,3,7},
		{9,1,3,9,6,2,1,1,1,3,5,7,2,3,4,6,9,5,6,6,5,8,1,5,2,1,8,5,9,8,7,1,7,6,4,8,8,6,2,3,3,4,6,7,5,3,7,9},
		{3,5,8,5,1,1,7,7,6,8,4,6,2,3,4,1,3,4,4,8,1,1,3,4,9,4,4,2,2,7,1,3,4,1,1,9,5,8,7,1,4,8,2,5,2,7,8,2},
		{4,2,1,8,6,3,8,9,1,3,6,1,4,8,1,6,2,8,7,7,6,2,8,7,3,9,5,5,3,8,6,2,4,1,4,1,1,7,8,2,1,6,4,1,2,6,9,3},
		{6,6,5,8,9,4,3,4,8,1,1,6,1,1,2,2,8,1,5,7,1,7,1,1,8,1,7,3,9,7,8,3,2,6,5,3,7,1,4,9,2,7,2,8,7,6,8,2},
		{7,5,4,7,6,3,9,4,6,9,2,5,2,5,2,7,9,9,9,3,9,2,1,1,2,8,3,6,7,6,3,3,1,3,1,5,7,6,3,3,8,2,8,5,7,2,7,8},
		{5,7,7,8,4,4,5,8,8,6,8,9,1,3,2,7,5,3,5,6,5,4,6,1,2,9,9,4,7,8,3,2,3,8,1,4,1,9,4,3,2,5,5,3,2,7,7,7},
		{3,2,3,4,4,2,4,5,6,1,3,3,6,8,4,8,2,5,7,6,8,9,1,4,3,4,4,2,1,3,4,5,8,8,8,2,4,5,4,7,5,8,4,3,2,2,6,8},
		{3,7,5,1,3,9,6,3,1,8,6,5,4,5,4,2,7,8,7,2,6,3,1,6,3,4,2,1,4,6,9,5,1,1,8,7,1,1,6,8,6,3,1,2,1,3,7,3},
		{4,8,9,7,2,2,9,7,8,8,2,9,1,7,3,1,3,1,8,3,5,6,7,6,1,7,9,5,6,9,2,9,2,1,6,1,9,1,9,1,6,3,3,5,1,3,1,2},
		{9,8,2,6,9,5,4,6,5,7,5,7,8,2,9,9,1,8,2,2,5,6,5,6,2,7,9,3,9,9,2,2,5,2,2,1,7,8,7,4,4,5,3,6,4,2,8,5},
		{1,7,7,7,3,3,9,3,2,2,5,2,8,7,5,4,4,7,9,8,3,2,5,7,5,3,3,6,4,3,1,3,6,4,9,3,4,2,9,7,1,6,2,1,1,6,9,1},
		{5,2,3,9,3,3,5,8,4,9,9,8,6,2,6,4,7,8,3,4,9,7,2,1,4,1,2,9,4,6,9,5,5,3,9,1,1,3,1,1,1,8,1,3,4,6,3,9},
		{5,3,9,2,5,6,7,3,6,2,9,6,7,5,9,6,7,1,9,3,1,7,5,5,2,1,6,5,1,9,9,4,4,1,2,3,7,4,3,3,1,9,3,8,7,5,8,6},
		{8,9,4,3,5,6,4,1,1,4,2,4,7,5,4,8,7,7,6,1,1,3,4,1,8,9,5,7,9,9,1,5,5,5,6,7,9,8,4,6,4,9,5,1,7,2,3,8},
		{2,7,9,6,8,7,6,9,3,3,9,2,5,4,8,6,1,9,4,6,6,8,5,1,2,4,3,1,6,1,3,9,9,9,9,2,3,8,4,8,2,3,8,2,9,2,3,6},
		{8,2,2,2,4,4,6,6,7,7,4,7,7,3,6,9,6,4,4,4,7,7,3,9,7,3,2,1,3,2,6,7,5,3,3,6,1,1,8,8,3,5,8,9,2,4,7,7},
		{7,7,1,9,8,1,2,4,1,8,1,6,2,4,2,3,6,3,2,5,6,7,3,6,8,5,1,6,6,8,4,3,4,2,2,4,4,6,5,2,7,2,5,2,5,8,8,8},
		{3,4,8,7,1,6,3,2,3,9,1,7,3,1,1,5,3,5,9,9,1,2,7,3,7,9,8,5,8,6,5,2,1,4,9,3,2,5,3,2,6,1,5,8,2,2,8,9},
		{2,2,9,3,9,9,2,5,7,9,1,9,1,5,9,4,6,2,9,4,6,1,9,5,4,9,7,2,9,9,2,1,9,6,1,8,9,6,5,8,7,5,9,2,4,3,9,6},
		{3,9,1,9,5,4,2,7,2,2,1,7,3,9,6,5,4,7,3,5,7,9,2,9,3,9,3,5,4,4,7,2,4,2,6,1,9,6,5,7,6,4,8,1,1,8,4,2},
		{7,7,2,7,1,8,1,5,6,3,6,6,7,7,6,7,8,2,7,3,3,5,7,4,1,9,2,6,1,3,4,7,4,2,9,9,3,1,5,9,8,3,6,5,1,9,2,5},
		{2,8,5,2,7,9,4,3,1,5,6,5,2,1,8,1,2,8,4,3,3,4,2,6,1,7,8,3,2,8,8,2,1,6,2,5,9,3,6,9,3,8,7,4,5,9,4,9},
		{4,6,6,5,4,4,5,5,7,8,3,6,4,2,3,2,6,1,8,5,5,6,3,7,9,9,2,8,3,9,5,9,3,9,4,8,7,2,2,3,3,7,6,6,7,8,3,9},
		{7,8,4,4,2,4,1,1,4,4,9,8,3,4,7,6,4,2,5,1,4,7,5,8,4,2,5,2,8,2,8,7,7,2,9,2,4,6,8,4,1,5,2,8,9,4,3,1}},

		{{3,3,6,7,7,9,6,1,2,4,1,5,1,1,1,9,3,1,5,9,8,5,1,7,7,3,3,4,3,7,1,5,9,7,5,9,2,9,5,3,1,7,3,5,9,9,5,8},
		{5,6,5,3,7,9,7,1,2,4,1,4,9,1,1,5,2,1,6,4,1,2,8,2,2,8,9,1,3,7,7,3,7,2,7,9,5,2,1,2,2,1,7,4,3,3,9,4},
		{6,7,6,6,9,5,5,1,8,3,9,8,7,6,7,5,5,6,6,2,9,7,9,4,3,3,5,9,1,1,5,1,7,1,3,5,4,2,3,2,2,2,5,7,2,4,2,2},
		{1,2,6,2,6,7,6,2,9,2,3,6,3,7,2,8,9,6,2,6,2,3,2,4,5,4,6,7,6,9,4,9,6,5,7,2,8,6,4,2,3,8,9,5,7,9,1,7},
		{2,8,5,1,9,5,6,6,3,2,6,8,2,2,3,9,5,1,6,9,4,9,8,3,5,5,2,7,3,4,5,4,5,5,1,1,6,5,4,3,3,6,5,7,5,9,1,3},
		{1,9,6,6,4,9,3,8,4,9,2,4,5,7,4,6,9,1,2,3,4,5,9,2,5,1,1,6,2,4,8,1,1,5,6,8,4,4,8,6,9,3,4,4,3,5,2,6},
		{7,6,1,1,1,8,4,6,6,5,8,1,1,9,6,4,8,3,3,1,7,8,2,1,3,4,7,4,1,3,4,2,5,6,3,3,9,6,8,1,8,8,2,4,2,7,5,2},
		{5,3,4,6,7,9,4,1,1,7,6,3,3,3,6,6,4,7,6,6,7,8,4,3,6,1,2,3,6,6,8,2,4,7,6,6,8,4,2,2,7,6,8,4,6,1,3,8},
		{8,8,3,4,9,4,7,3,4,2,3,8,4,9,5,7,1,2,3,6,4,6,7,5,9,5,5,8,4,9,2,4,3,7,4,4,9,7,2,9,8,7,8,3,4,9,6,7},
		{9,3,2,6,7,6,6,3,4,1,2,7,5,6,7,2,1,2,2,9,5,2,7,2,1,6,6,4,6,6,4,3,7,8,7,8,1,5,3,7,2,6,8,5,9,7,1,5},
		{2,2,5,9,3,2,3,3,2,3,5,4,3,1,9,4,6,3,2,9,9,1,5,9,8,5,4,8,3,9,7,6,2,1,5,1,1,9,6,5,7,9,4,4,2,7,3,2},
		{8,9,1,7,5,2,1,3,5,1,1,1,4,2,2,9,4,3,9,1,8,9,1,5,9,6,9,2,3,3,4,2,3,6,9,8,8,1,3,1,2,5,5,5,8,5,1,4},
		{2,4,3,5,4,5,6,4,9,1,6,2,8,5,6,5,2,7,5,6,2,4,8,1,5,2,3,5,7,7,1,1,7,7,8,1,6,8,9,8,3,9,4,9,1,5,6,6},
		{1,9,7,6,7,8,2,2,7,7,9,9,9,6,6,8,8,3,8,7,2,7,2,2,4,7,7,9,6,8,5,3,7,7,4,9,5,2,8,5,2,5,1,4,5,9,1,8},
		{6,2,8,3,4,5,5,5,6,4,1,8,5,1,4,7,9,9,5,1,9,6,8,2,4,5,5,5,3,8,9,9,8,5,4,5,9,5,6,1,4,1,2,8,8,2,7,7},
		{8,9,7,8,5,6,1,1,6,6,5,5,6,4,4,1,9,9,4,6,1,9,7,4,7,6,6,5,4,4,4,1,3,8,5,2,6,5,7,2,1,4,9,1,5,4,7,7},
		{3,5,1,1,2,7,6,6,2,8,6,6,1,1,6,3,8,2,1,1,2,2,5,2,4,6,2,7,8,8,1,6,8,6,8,8,4,4,9,1,6,1,8,9,1,1,3,4},
		{6,6,8,6,3,1,4,3,9,3,5,9,9,3,5,8,2,3,2,1,9,3,9,3,4,4,4,1,3,7,4,4,2,7,4,5,3,3,4,6,9,2,8,9,6,1,1,5},
		{4,1,5,7,2,5,2,7,3,8,4,2,9,2,3,8,3,6,2,5,5,9,5,1,7,7,4,8,2,2,1,8,4,8,9,7,6,3,8,9,2,1,9,5,9,3,2,2},
		{3,7,8,2,3,2,7,6,9,9,8,4,8,7,2,5,1,1,4,3,2,4,9,2,7,1,5,1,9,3,3,2,3,5,3,1,2,2,1,2,3,9,6,2,7,8,7,9},
		{2,3,2,7,2,5,4,4,5,3,3,9,6,7,4,8,7,3,6,6,8,8,5,5,7,5,8,8,5,3,3,7,4,6,1,8,5,8,5,8,4,8,2,5,8,8,5,5},
		{6,3,2,6,1,4,6,4,8,7,1,2,3,3,8,1,6,9,1,7,2,6,6,1,1,7,4,1,9,5,8,6,3,7,1,4,1,4,8,4,5,6,6,1,6,6,2,4},
		{9,8,1,7,5,2,7,1,8,6,4,6,6,5,8,8,3,5,5,9,6,2,6,9,6,6,8,6,6,8,7,4,3,1,4,1,5,6,7,1,7,5,1,6,8,7,2,7},
		{3,4,4,3,1,7,6,4,4,5,2,8,6,3,9,2,6,4,7,1,6,4,9,3,6,7,7,1,1,2,1,2,9,8,2,4,9,7,9,1,5,5,5,4,8,5,1,2},
		{3,2,8,7,2,1,3,9,9,5,9,7,2,2,6,5,9,2,1,8,5,6,3,3,1,4,6,5,6,4,2,8,6,6,8,5,1,9,5,9,1,8,5,2,3,6,1,5},
		{6,3,4,2,9,7,6,1,2,2,6,9,7,9,8,4,5,6,7,5,6,6,7,4,2,9,7,9,3,6,3,7,8,3,5,5,5,7,8,1,6,6,4,6,1,1,3,6},
		{7,3,2,2,3,9,2,4,3,8,7,1,6,1,6,1,6,4,8,1,6,9,5,7,5,6,8,9,3,6,7,8,3,7,5,4,1,3,2,2,9,4,8,1,3,2,9,4},
		{8,8,5,4,7,4,4,4,8,8,7,9,5,2,4,7,5,6,9,5,1,8,7,1,1,1,5,8,9,5,4,3,2,8,7,3,8,1,1,7,9,1,3,2,5,8,2,9},
		{4,3,7,1,5,8,2,6,4,5,5,9,3,4,6,6,2,8,8,7,9,3,9,5,3,7,7,2,1,3,5,9,6,2,3,6,7,3,2,6,7,6,3,3,9,1,2,9},
		{9,2,2,9,7,5,7,5,8,9,6,8,3,1,7,9,3,6,1,4,8,8,2,4,1,6,4,2,8,3,2,1,6,1,9,1,8,5,7,1,8,1,3,5,6,6,6,8},
		{1,4,6,4,9,8,4,1,9,7,3,4,5,5,3,5,6,1,6,4,2,9,3,6,7,9,5,9,4,1,3,4,3,8,1,2,5,4,3,9,3,5,3,9,2,5,5,6},
		{8,5,7,1,1,4,1,6,3,5,8,6,9,8,8,9,2,4,4,5,4,4,9,8,8,2,4,3,8,2,9,5,4,6,7,4,1,6,1,1,5,1,9,5,4,3,9,9}}
};


static int __once__; // just for trace
static int __nw__; // just for trace
static int __picName__;

// ----------------------------------------------------------------------------


// Dump functions -------------------------------------------------------------
#if __TEST_SUPORT_ADVANCED_DUMP
static void dumpXorMatch_SSh(char * m1, short v1)
{
	if (! __once__) {
		d_dumpSSh("ts_xor_match.log", m1, v1);
	}

}
#endif


#if __TEST_SUPORT_ADVANCED_DUMP
static void dumpXorMatch_SSh_SSh_SSh(char * m1, short v1,
		char * m2, MEC_Sample v2,
		char * m3, MEC_Sample v3)
{

	if (! __once__) {
		d_dumpSSh_SSh_SSh("ts_xor_match.log", m1, v1, m2, v2, m3, v3);
	}
}
#endif


#if __TEST_SUPORT_ADVANCED_DUMP
static void dumpXorMatch_SSh_SI(char * m1, short v1,
		char * m2, int v2)
{
	if (! __once__) {
		d_dumpSSh_SI("ts_xor_match.log", m1, v1, m2, v2);
	}

}
#endif


#if __TEST_SUPORT_ADVANCED_DUMP
static void dumpXorMatch_SI(char * m, int v)
{
	if (! __once__) {
		d_dumpSI("ts_xor_match.log", m, v);
	}
}
#endif

// ----------------------------------------------------------------------------


void setUpTestSuport(enum ReferenceMatchAlgorithms algorithm, PictureMotionEstimator * pictureMotionEstimator)
{
	int matchAlgorihm = pictureMotionEstimator->pme_informMatchAlgorithm();

	if (algorithm == XOR_V1_MATCH_ALGORITHM) {
		Assert(matchAlgorihm == XOR_MATCH_ALGORITHM, "DMEC App", "unknown match algorithm");
	}
	else {
		Assert(algorithm == REFERENCE_FULL_SEARCH_V01, "", "");
		Assert(matchAlgorihm == FULLSEARCH_MATCH_ALGORITHM, "", "");
	}

	__once__ = 0;
	__picName__ = 0;
	__nw__ = 0;

	reference_match_algorithm = algorithm;
}


MEC_Sample ** _createSampleMatrix(int width, int height)
{
	MEC_Sample ** samples;
	samples = (MEC_Sample **) calloc(height, sizeof(MEC_Sample *));

	int i;
	for (i = 0; i < height; ++ i) {
		samples[i] = (MEC_Sample *) calloc(width, sizeof(MEC_Sample));
	}

	int row, col;
	for (row = 0; row < height; ++ row) {
		for (col = 0; col < width; ++ col) {
			samples[row][col] = random_give_non_zero_int_smaller_than(MAX_PEL_SIZE);
		}
	}

	return samples;
}


MEC_Sample ** _createSpecificSampleMatrix(int width, int height, unsigned int dataSet) {
	MEC_Sample ** samples = (MEC_Sample **) calloc(height, sizeof(MEC_Sample *));

	int i;
	for (i = 0; i < height; ++ i) {
		samples[i] = (MEC_Sample *) calloc(width, sizeof(MEC_Sample));
	}

	int row, col;
	for (row = 0; row < height; ++ row) {
		for (col = 0; col < width; ++ col) {
			samples[row][col] = __samples[dataSet][row%32][col%48]; /* These
			modules (%) guarantee that row and columns of __samples are
			respected, although it is not the best solution of the world.  */
		}
	}

	return samples;
}



MEC_Picture * _createA_Picture(uint width, uint height)
{
#if __TEST_SUPORT_GENERAL_TRACE
	cout << "_createA_Picture\n";
#endif

    MEC_Picture * picture;

    MEC_Sample ** Y_samples = _createSampleMatrix(width, height);
    MEC_Sample ** Cb_samples = _createSampleMatrix(width / 2, height / 2);
    MEC_Sample ** Cr_samples = _createSampleMatrix(width / 2, height / 2);

    picture = new_Picture(width, height, width / 2, height / 2,
    		Y_samples, Cb_samples, Cr_samples);

#if 0 /* __TEST_SUPORT_DUMP */
    char fileName[50];
    cout << fileName, 50, "mec_picture_%i.log", __picName__);
    mec_picture_dumpPicture(picture, fileName);
    __picName__ ++;
#endif

    return picture;
}


MEC_Picture * _createSpecificPicture(unsigned int width, unsigned int height, unsigned int dataSet)
{
#if __TEST_SUPORT_GENERAL_TRACE
	printf("_createSpecificPicture\n");
#endif

	MEC_Picture * picture;

    MEC_Sample ** Y_samples = _createSpecificSampleMatrix(width, height, dataSet);

    picture = new_Picture(width, height, 0, 0, Y_samples, 0, 0);

#if __TEST_SUPORT_DUMP
    char fileName[50];
    snprintf(fileName, 50, "mec_picture_%i.log", __picName__);
    Dumper::dumpMEC_Picture(fileName, picture);
    __picName__ ++;
#endif

    return picture;
}


// --------------------------
static inline void __testXOR_v1_match_algorithm(ReferencePictureMotionCounterpart * rpmc,
		MEC_Picture * currentPicture,
		MEC_Picture * referencePicture)
{
	//~ trace("__testXOR_v1_match_algorithm");

	// cout << "context (flat PMC node)";
	//--- cout << "...in __testXOR_v1_match_algorithm, auxFlatPCM_Node.xInPicture: %i\n", auxFlatPCM_Node.xInPicture);
#if __TEST_SUPORT_ADVANCED_DUMP
	flatH264PMC_Node_context_dump(&auxFlatPMC_Node, "ts_xor_match.log";
#endif

	WorkerParameters * workerParameters;
	workerParameters = __pictureMotionEstimatorWorkerProxy_getWorkerParameters(rpmc->generatedByWorker);

	//~ dumpXorMatch_SI("generated_by_worker:", rpmc->generatedByWorker);
	/* dumpXorMatch_SI("picturePartitionWidth:", workerParameters->picturePartitionWidth); */
	/* dumpXorMatch_SI("picturePartitionHeight:", workerParameters->picturePartitionHeight); */

    short xor_sum = 0;

    /* unsigned int luma_width = currentPicture->Y->width; */
    unsigned int luma_width = workerParameters->picturePartitionWidth;

	/* unsigned int luma_height = currentPicture->Y->height; */
    unsigned int luma_height = workerParameters->picturePartitionHeight;

	/* assert(luma_width == referencePicture->Y->width); */
	/* assert(luma_height == referencePicture->Y->height); */

	unsigned int row, col;
	unsigned int firstRow = workerParameters->picturePartitionY;
	unsigned int firstCol = workerParameters->picturePartitionX;

	MEC_Sample current_sample, reference_sample;

	/* dumpXorMatch_SI("1stfor, firstRow", firstRow); */
	/* dumpXorMatch_SI("1stfor, luma_height", luma_height); */

	for (row = firstRow; row < (firstRow + luma_height); ++ row) {

	    /* dumpXorMatch_SI("2ndfor, firstCol", firstCol); */
	    /* dumpXorMatch_SI("2ndfor, luma_width", luma_width); */


		for (col = firstCol; col < (firstCol + luma_width); ++ col) {
			current_sample = currentPicture->Y->samples[row][col];
			reference_sample = referencePicture->Y->samples[row][col];

		    /* dumpXorMatch_SI("in 2ndfor, row", row); */
		    /* dumpXorMatch_SI("in 2ndfor, col", col); */


			if (! __once__) {
				//~ traceSSh("Y, current_sample", current_sample);
				//~ traceSSh("Y, reference_sample", reference_sample);
			}

			//~ dumpXorMatch_SSh_SSh_SSh("t01", xor_sum, "cs", current_sample, "rs", reference_sample);
			xor_sum = xor_sum ^ (current_sample ^ reference_sample);
			//~ dumpXorMatch_SSh("t02", xor_sum);
		}
	}

#if ENABLE_CHROMA_PLANES
	/* unsigned int chroma_width = currentPicture->Cb->width; */
	unsigned int chroma_width = luma_width / 2;

	/* unsigned int chroma_height = currentPicture->Cb->height; */
	unsigned int chroma_height = luma_height / 2;
#endif

	/* assert(chroma_width == currentPicture->Cr->width); */
	/* assert(chroma_width == referencePicture->Cb->width); */
	/* assert(chroma_width == referencePicture->Cr->width); */
	/* assert(chroma_height == currentPicture->Cr->height); */
	/* assert(chroma_height == referencePicture->Cb->height); */
	/* assert(chroma_height == referencePicture->Cr->height); */

	//~ dumpXorMatch_SSh_SI("t13", xor_sum, "xpic", auxFlatPMC_Node.xInPicture);
	xor_sum = xor_sum ^ auxFlatPMC_Node.xInPicture;

	//~ dumpXorMatch_SSh_SI("t14", xor_sum, "ypic", auxFlatPMC_Node.yInPicture);
	xor_sum = xor_sum ^ auxFlatPMC_Node.yInPicture;

	//~ dumpXorMatch_SSh_SI("t15", xor_sum, "mbid", auxFlatPMC_Node.macroblockID);
	xor_sum = xor_sum ^ auxFlatPMC_Node.macroblockID;

	//~ dumpXorMatch_SSh_SI("t16", xor_sum, "mode", auxFlatPMC_Node.h264BlockMode);
	xor_sum = xor_sum ^ auxFlatPMC_Node.h264BlockMode;

	//~ dumpXorMatch_SSh_SI("t17", xor_sum, "xmb", auxFlatPMC_Node.xInMacroblock);
	xor_sum = xor_sum ^ auxFlatPMC_Node.xInMacroblock;

	//~ dumpXorMatch_SSh_SI("t18", xor_sum, "ymb", auxFlatPMC_Node.yInMacroblock);
	xor_sum = xor_sum ^ auxFlatPMC_Node.yInMacroblock;

	//~ dumpXorMatch_SSh_SI("t19", xor_sum, "ln", auxFlatPMC_Node.referencePictureListNumber);
	xor_sum = xor_sum ^ auxFlatPMC_Node.referencePictureListNumber;

	//~ dumpXorMatch_SSh_SI("t20", xor_sum, "ref", auxFlatPMC_Node.referencePictureID);
	xor_sum = xor_sum ^ auxFlatPMC_Node.referencePictureID;
	//~ dumpXorMatch_SSh("t21", xor_sum);

	// debug - begin
#if 0
	int error = 0;
	if (rpmc->translation.mv_x != xor_sum) {
		cout << "...mv_x: %i, xor_sum: %i\n", rpmc->translation.mv_x, xor_sum);
		error = 1;
	}
	if(rpmc->translation.mv_y != ~xor_sum) {
		error = 1;
	}
	if(rpmc->cost != (xor_sum ^ xor_sum)) {
		error = 1;
	}

	if (error) {
		cout << "context (flat PMC node)";
		flatH264PMC_Node_print(&auxFlatPMC_Node);
	}
#endif

	if (xor_sum) {
		if (rpmc->translation.mv_x != xor_sum) {
			cout << "...error mv_x != xor_sum\n";
			cout << "...mv_x: " << rpmc->translation.mv_x << ", xor_sum: " << xor_sum << "\n";
			cout << "context (flat PMC node)";
			// flatH264PMC_Node_print(&auxFlatPMC_Node);
		}

		Assert(rpmc->translation.mv_x == xor_sum, "XOR test", "mv_x must be xor_sum");
	}
	else {
		if (rpmc->translation.mv_x != 3) {
			cout << "...error mv_x != 3\n";
			cout << "...mv_x: " << rpmc->translation.mv_x << ", xor_sum: " << xor_sum << "\n";
			cout << "context (flat PMC node)";
			// flatH264PMC_Node_print(&auxFlatPMC_Node);
		}

		Assert(rpmc->translation.mv_x == 3, "XOR test", "error on mv_x");
	}

	if (~xor_sum) {
		Assert(rpmc->translation.mv_y == ~xor_sum, "", "");
	}
	else {
		Assert(rpmc->translation.mv_y == 6, "", "");
	}

	if (xor_sum ^ 0x7FFF) {
		Assert((short) rpmc->cost == (xor_sum ^ 0x7FFF), "", "");
	}
	else {
		Assert(rpmc->cost == 4, "", "");
	}

	Assert(rpmc->pred_mv[0] == rpmc->translation.mv_x, "XOR test", "error on mv_x"); /* Valid only for XOR algorithm */
	Assert(rpmc->pred_mv[1] == rpmc->translation.mv_y, "XOR test", "error on mv_y");


#if 0 /* never sets __once__*/
	if (__nw__ >= 6) {
		__once__ = 1;
	}
	__nw__ ++;
#endif
	// debug - end
}

static inline void __testFixed_v1_match_algorithm(ReferencePictureMotionCounterpart * rpmc,
		MEC_Picture * currentPicture,
		MEC_Picture * referencePicture)
{
#if 0
	assert(rpmc->translation.mv_x == referencePicture->ID);
	assert(rpmc->translation.mv_y == referencePicture->ID + currentPicture->ID);
	assert(rpmc->cost == referencePicture->ID - currentPicture->ID);
#endif
}

static void __testRPMC(ReferencePictureMotionCounterpart * rpmc,
		MEC_Picture * currentPicture,
		MEC_Picture * referencePicture)
{
#if 0
    assert(rpmc->referencePicture->ID == referencePicture->ID);

    auxFlatPMC_Node.referencePictureID = referencePicture->ID;
#endif

    switch (reference_match_algorithm) {

		case FIXED_V1_MATCH_ALGORITHM:
			__testFixed_v1_match_algorithm(rpmc, currentPicture, referencePicture);
			break;

		case XOR_V1_MATCH_ALGORITHM:
			__testXOR_v1_match_algorithm(rpmc, currentPicture, referencePicture);
			break;

		default:
			__testXOR_v1_match_algorithm(rpmc, currentPicture, referencePicture);
			break;
    }

}

static void __testMMC_and_beyond(PictureMotionCounterpart * pmc,
        MEC_Picture * currentPicture,
        MEC_Picture **list0,
        unsigned int list0Size,
        MEC_Picture ** list1,
        unsigned int list1Size,
        int macroblockID)
{
    unsigned int h264BlockMode, blockID, referencePictures;

    for (h264BlockMode = 0; h264BlockMode < NUM_H264_MODES; ++h264BlockMode) {
    	//~ dumpXorMatch_SI("mode", h264BlockMode);

        auxFlatPMC_Node.h264BlockMode = h264BlockMode;

        for (blockID = 0; blockID < (unsigned) bmt_getNumBlocks((BlockModeType) h264BlockMode); ++blockID) {
            ReferencePictureMotionCounterpart * rpmc;
            int xInMacroblock[1];
            int yInMacroblock[1];
            pmc_getBlockCoordinates(blockID, (BlockModeType) h264BlockMode,
                    xInMacroblock, yInMacroblock);

            auxFlatPMC_Node.xInMacroblock = xInMacroblock[0];
            auxFlatPMC_Node.yInMacroblock = yInMacroblock[0];

            /*~
            dumpXorMatch_SSh_SSh_SSh("block_num", blockID,
                			"xmb", auxFlatPMC_Node.xInMacroblock,
                			"ymb", auxFlatPMC_Node.yInMacroblock);
                			*/

            for (referencePictures = 0; referencePictures < list0Size; ++referencePictures) {
                auxFlatPMC_Node.referencePictureListNumber = 0;

                rpmc= pmc_findReferencePictureMotionCounterpart(pmc,
                        macroblockID,
                        (BlockModeType) h264BlockMode,
                        xInMacroblock[0],
                        yInMacroblock[0],
                        0,
                        referencePictures);
                __testRPMC(rpmc, currentPicture, list0[referencePictures]);
            }
            for (referencePictures = 0; referencePictures < list1Size; ++referencePictures) {
                auxFlatPMC_Node.referencePictureListNumber = 1;

                rpmc= pmc_findReferencePictureMotionCounterpart(pmc,
                        macroblockID,
                        (BlockModeType) h264BlockMode,
                        xInMacroblock[0],
                        yInMacroblock[0],
                        1,
                        referencePictures);
                __testRPMC(rpmc, currentPicture, list1[referencePictures]);
            }

        }
    }
}


void testPMC_and_beyond_v2(PictureMotionCounterpart * pmc,
		unsigned int pictureWidth, unsigned int pictureHeight,
		MEC_Picture * currentPicture,
		MEC_Picture ** list0, unsigned int list0Size,
		unsigned int max_reference_pictures)
{
	// partitioning
	// trace("testPMC_and_beyond_v2");
#if __MAIN_DETAILED_TRACE
    cout << "testPMC_and_beyond_v2\n";
#endif

	unsigned int numPartitions[1];

	enum PartitionModel partitionModel;
	switch (NUM_WORKERS) {
		case 1:
			partitionModel = SINGLE_PARTITION;
			break;
		case 2:
			partitionModel = ONExTWO_PARTITION;
			break;
		case 3:
			partitionModel = THREExONE_PARTITION;
			break;
		case 4:
			partitionModel = TWOxTWO_PARTITION;
			break;
		case 5:
			partitionModel = THREE_TWOxTWO_PARTITION;
			break;
		case 6:
			partitionModel = THREExTWO_PARTITION;
			break;
		default:
			Assert(false, "Coordinator", "Unknown partition type");
			break;
	}

	// trace("testPMC_and_beyond_v2, creating picture partitions");
	PicturePartitionInfo ** ppi = 0;
	ppi = pp_createPartitions(numPartitions, partitionModel, pictureWidth, pictureHeight);
	// assert(ppi != 0x0);

	// trace("testPMC_and_beyond_v2, creating picture partitions DONE");

	Assert(numPartitions[0] == NUM_WORKERS, "", "");

	unsigned int flatPMC_MaxSize = MAX_FLAT_PMC_SIZE_FOR_COORDINATOR;

	MEC_SearchWindow searchWindow;
	searchWindow.centerX = 0;
	searchWindow.centerY = 0;
	searchWindow.searchRange = SEARCH_RANGE;

	PictureMotionCounterpart * referencePMC = (PictureMotionCounterpart *) malloc(sizeof(PictureMotionCounterpart));
	setUpPictureMotionCounterpart(referencePMC,
				pictureWidth,
				pictureHeight,
				max_reference_pictures);

	pmc_set(referencePMC, list0, list0Size, 0, 0);

    unsigned int i;

    // trace("testPMC_and_beyond_v2, will iterate over partitions");
    for (i = 0; i < numPartitions[0]; i++) {
    	// match
    	FlatH264PMC_Node * flatPMC = (FlatH264PMC_Node *) calloc(flatPMC_MaxSize, sizeof(FlatH264PMC_Node));
    	unsigned int out_flatPMC_Size[1];

    	referenceBMA_match(flatPMC,
    			out_flatPMC_Size,
    			flatPMC_MaxSize,
				currentPicture,
				list0,
				list0Size,
				pictureWidth,
				pictureHeight,
				&searchWindow,
				ppi[i]->workerID,
				ppi[i]->picturePartitionX,
				ppi[i]->picturePartitionY,
				ppi[i]->picturePartitionWidth,
				ppi[i]->picturePartitionHeight);

    	// gathering results
    	// assert(out_flatPMC_Size[0] > 0);
    	// assert(out_flatPMC_Size[0] < flatPMC_MaxSize);
    	// assert(ppi[i]->workerID >=0 && ppi[i]->workerID <= numPartitions[0]);
		pmc_fillPMC_WithFlatPMC(referencePMC, flatPMC, out_flatPMC_Size[0], ppi[i]->workerID);
		free(flatPMC);
    }

    //trace("testPMC_and_beyond_v2, iterate over partitions DONE");
    // test
    Assert(pmc_equals(pmc, referencePMC), "DMEC_App", "ME not performed correctly");

    // deletions
    for (i = 0; i < numPartitions[0]; i++) {
    	free(ppi[i]);
    }

    free(ppi);
    free(referencePMC);
}


/* Works with Vectorial PictureMotionCounterpart */
void testPMC_and_beyond(PictureMotionCounterpart * pmc,
		unsigned int pictureWidth, unsigned int pictureHeight,
		MEC_Picture * currentPicture,
		MEC_Picture ** list0, unsigned int list0Size,
		MEC_Picture ** list1, unsigned int list1Size,
		unsigned int max_reference_pictures)
{
#if __TEST_SUPORT_GENERAL_TRACE &&  LINUX
	cout << "rf_testPCM_and_beyond" << "\n";
#endif

#if __TEST_SUPORT_DETAILED_TRACE
	cout << "...printing PMC\n";
	printPictureMotionCounterpart(pmc);
#endif


	if (reference_match_algorithm == REFERENCE_FULL_SEARCH_V01) {
		// trace("going to call testPMC_and_beyond_v2");
		testPMC_and_beyond_v2(pmc,
				pictureWidth, pictureHeight,
				currentPicture,
				list0, list0Size,
				max_reference_pictures);
		// trace("called testPMC_and_beyond_v2");
	}
	else {
		// trace("going to call testPMC_and_beyond_v1");

		unsigned int i, j;
		int x;
		int y = 0;
		int macroblockID = 0;

		for (i = 0; i < (pictureHeight / MACROBLOCK_SIZE); ++ i) {
			x = 0;

			for (j = 0; j < (pictureWidth / MACROBLOCK_SIZE); ++ j) {
				auxFlatPMC_Node.xInPicture = x;
				auxFlatPMC_Node.yInPicture = y;
				auxFlatPMC_Node.macroblockID = macroblockID;

				//dumpXorMatch_SI("macroblockID", macroblockID);

	#if 1 && LINUX
				cout << "TestSuport, testing macroblock: "<< macroblockID << "\n";
	#endif

				__testMMC_and_beyond(pmc,
						currentPicture,
						list0, list0Size,
						list1, list1Size,
						macroblockID);

				x += MACROBLOCK_SIZE;
				macroblockID ++;
			}

			y += MACROBLOCK_SIZE;
		}
	}
}


// ----------------------------------------------------------------------------

