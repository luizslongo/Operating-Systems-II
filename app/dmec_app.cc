#include "dmec/include/test_support.h"

#if LINUX
#include<iostream>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

using namespace std;

#else
#include <utility/ostream.h>
#include <utility/malloc.h>

#endif


#define __MAIN_GENERAL_TRACE 1
#define __MAIN_DETAILED_TRACE 1

// Just sets of test functions.
void testPack10();
void testPack20();

// Test functions declaration.
static void __testRandomicSimplePrediction(unsigned int timesMatch, unsigned int pictureWidth, unsigned int pictureHeight);
static void __testFixedSimplePrediction(unsigned int timesMatch, unsigned int pictureWidth, unsigned int pictureHeight, unsigned int dataSetCurrentPicture, unsigned int dataSetReferencePicture);

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
	__testRandomicSimplePrediction(1, 176, 144);
}


void testPack20()
{
#if __MAIN_GENERAL_TRACE
    cout << "++++++++ testing 176x144 (1 match, fixed set, QCIF, simple prediction)\n";
#endif
    __testFixedSimplePrediction(1, 176, 144, 0, 1);
}


void __testRandomicSimplePrediction(unsigned int timesMatch,
		unsigned int pictureWidth,
		unsigned int pictureHeight)
{
    // creations
    PictureMotionEstimator* pictureMotionEstimator;

    pictureMotionEstimator = PictureMotionEstimator::getInstance(
    		pictureWidth,
    		pictureHeight,
    		1);

    MEC_Picture* currentPicture = TestSupport::createRandomPicture(pictureWidth, pictureHeight);
    MEC_Picture* referencePicture = TestSupport::createRandomPicture(pictureWidth, pictureHeight);

    PictureMotionCounterpart* pmc;
    unsigned int im;
    for (im = 0; im < timesMatch; ++ im) {
		// match
#if __MAIN_GENERAL_TRACE && LINUX
    	cout << "...match#: " << im+1 << " (of: " << timesMatch << ")\n";
#endif
		pmc = pictureMotionEstimator->match(currentPicture, referencePicture);

		// assertions
		TestSupport::testPictureMotionCounterpart(pmc,
		        pictureWidth, pictureHeight,
		        currentPicture,
		        referencePicture);

    }

    // deletions
	PictureMotionEstimator::deleteInstance();
	delete currentPicture;
    delete referencePicture;
	
}


void __testFixedSimplePrediction(unsigned int timesMatch,
		unsigned int pictureWidth,
		unsigned int pictureHeight,
		unsigned int dataSetCurrentPicture,
		unsigned int dataSetReferencePicture)
{
    PictureMotionEstimator * pictureMotionEstimator = PictureMotionEstimator::getInstance(pictureWidth, pictureHeight, 1);

    MEC_Picture* currentPicture = TestSupport::createPicture(pictureWidth, pictureHeight, dataSetCurrentPicture);
    MEC_Picture* referencePicture = TestSupport::createPicture(pictureWidth, pictureHeight, dataSetReferencePicture);

    PictureMotionCounterpart* pmc;

    unsigned int im;
    for (im = 0; im < timesMatch; ++ im) {
#if __MAIN_GENERAL_TRACE
    	cout << "...match#: " << im+1 << " (of: " << timesMatch << ")\n";
#endif
		pmc = pictureMotionEstimator->match(currentPicture, referencePicture);

		TestSupport::testPictureMotionCounterpart(pmc, pictureWidth, pictureHeight, currentPicture, referencePicture);
    }

	PictureMotionEstimator::deleteInstance();
	delete currentPicture;
	delete referencePicture;
}

