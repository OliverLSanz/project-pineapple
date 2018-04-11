/*******************************************************************************
Copyright � 2015, STMicroelectronics International N.V.
All rights reserved.

Use and Redistribution are permitted only in accordance with licensing terms 
available at www.st.com under software reference X-CUBE-6180XA1, and provided
that the following conditions are met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of STMicroelectronics nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROTECTED BY STMICROELECTRONICS PATENTS AND COPYRIGHTS.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS ARE DISCLAIMED.
IN NO EVENT SHALL STMICROELECTRONICS INTERNATIONAL N.V. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************/

/*
 * $Date: 2015-11-10 11:21:53 +0100 (Tue, 10 Nov 2015) $
 * $Revision: 2612 $
 */

#include "tof_gestures.h"
#include "tof_gestures_DIRSWIPE_1.h"

int tof_gestures_initDIRSWIPE_1(int32_t rangeThreshold_mm, long minSwipeDuration, long maxSwipeDuration, Gesture_DIRSWIPE_1_Data_t *data){
    int status=0;
    // Init left and right motion
    status |= tof_initMotion(rangeThreshold_mm, &(data->motionDetectorLeft));
    status |= tof_initMotion(rangeThreshold_mm, &(data->motionDetectorRight));
    data->minSwipeDuration = minSwipeDuration;
    data->maxSwipeDuration = maxSwipeDuration;
    data->state = GESTURES_DIRSWIPE_START;
    data->timestamp = 0;
    data->gesture_start_from_right = -1;
    return status;
}

int tof_gestures_detectDIRSWIPE_1(int32_t left_range_mm, int32_t right_range_mm, Gesture_DIRSWIPE_1_Data_t *data){
    int return_code = GESTURES_NULL;
    int r_motion_code, l_motion_code;
    unsigned long timestamp = GET_TIME_STAMP();
    long duration=0;
    
    // Get left and right motion
    l_motion_code = tof_getMotion(left_range_mm, &(data->motionDetectorLeft));
    r_motion_code = tof_getMotion(right_range_mm, &(data->motionDetectorRight));
    
    //TOF_GESTURES_DEBUG(DIRSWIPE_1,"lmotion=%d, rmotion=%d", l_motion_code, r_motion_code);
    
    switch(data->state){
        case GESTURES_DIRSWIPE_START:
            if(l_motion_code == GESTURES_MOTION_DOWN_STATE && r_motion_code == GESTURES_MOTION_RAISE_UP){
                if(data->motionDetectorRight.duration > data->minSwipeDuration){
                    data->gesture_start_from_right = 1;
                    data->state = GESTURES_DIRSWIPE_END;
                    data->timestamp = timestamp;
                    //TOF_GESTURES_DEBUG(DIRSWIPE_1,"Start right to left (%d ms)",data->motionDetectorRight.duration);
                }else{
                    return_code = GESTURES_DISCARDED_TOO_FAST;
                }
            }else if(r_motion_code == GESTURES_MOTION_DOWN_STATE && l_motion_code == GESTURES_MOTION_RAISE_UP){
                if(data->motionDetectorRight.duration > data->minSwipeDuration){
                    data->gesture_start_from_right = 0;
                    data->state = GESTURES_DIRSWIPE_END;
                    data->timestamp = timestamp;
                    //TOF_GESTURES_DEBUG(DIRSWIPE_1,"Start left to right(%d ms)",data->motionDetectorRight.duration);
                }else{
                    return_code = GESTURES_DISCARDED_TOO_FAST;
                }
            }   
            break;
        case GESTURES_DIRSWIPE_END:
            duration = timestamp - data->timestamp;
            if(duration > data->maxSwipeDuration){
                // Gesture is too long : discard it
                data->state = GESTURES_DIRSWIPE_START;
                data->gesture_start_from_right = -1;
                //TOF_GESTURES_DEBUG(DIRSWIPE_1,"Too slow");
                return_code = GESTURES_DISCARDED_TOO_SLOW;
            }else if ((data->gesture_start_from_right == 1 && l_motion_code == GESTURES_MOTION_RAISE_UP && r_motion_code == GESTURES_MOTION_UP_STATE) || (data->gesture_start_from_right == 0 && r_motion_code == GESTURES_MOTION_RAISE_UP && l_motion_code == GESTURES_MOTION_UP_STATE)){
                // Gesture detected
                data->state = GESTURES_DIRSWIPE_START;
                return_code = (data->gesture_start_from_right) ? GESTURES_SWIPE_RIGHT_LEFT : GESTURES_SWIPE_LEFT_RIGHT;
                //TOF_GESTURES_DEBUG(DIRSWIPE_1,"DETECTED");
            }else{
                // Waiting...
                //TOF_GESTURES_DEBUG(DIRSWIPE_1,"Waiting...");
                return_code = GESTURES_STARTED;
            }             
            break;
    }
    return return_code;
}
