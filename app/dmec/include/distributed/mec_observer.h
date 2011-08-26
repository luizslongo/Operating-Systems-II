/*
 Author: Mateus Krepsky Ludwich.

*/

#ifndef MEC_OBSERVER_H_
#define MEC_OBSERVER_H_

enum ObservableMethod {
    MEC_OBSERVER_setUp_PictureMotionEstimatorWorkerProxy_END,
    MEC_OBSERVER_commonMatch_END,
    MEC_OBSERVER_fillPMC_WithFlatPMC_BEGIN
};

void mec_observer_enable();
void mec_observer_disable();

void mec_observer_update(enum ObservableMethod method, int arg0, void * arg1, unsigned int arg2);

void mec_observer_setExpectedWorkerProxyValues(int workerID,
        int picturePartitionX, int picturePartitionY,
        unsigned int picturePartitionWidth, unsigned int picturePartitionHeight,
        int picturePartitionFirstMacroblockID);

void mec_observer_setExpectedNumMacroblocks(int numMacroblocks);
void mec_observer_setExpectedMacroblockXY_Values(int macroblockID, int xInPicture, int yInPicture);

void mec_observer_setExpectedNumMacroblocksInWorkerDomain(int workerID, unsigned int n);
void mec_observer_putExpectedWorker2MacroblockMapping(int workerID, int macroblockID);


void mec_observer_registerExpected_ea_s_picture(char * caller,
		unsigned long ea_picture, unsigned long ea_ID,
		unsigned long ea_Y_samples,
		unsigned long ea_Cb_samples, unsigned long ea_Cr_samples);

void mec_observer_check_ea_s_picture(char * caller,
		unsigned long ea_picture, unsigned long ea_ID,
		unsigned long ea_Y_samples,
		unsigned long ea_Cb_samples, unsigned long ea_Cr_samples);

#endif /* MEC_OBSERVER_H_ */
