/*---------------------------------------------------------------------------*/
/* Copyright(C)  2017  OMRON Corporation                                     */
/*                                                                           */
/* Licensed under the Apache License, Version 2.0 (the "License");           */
/* you may not use this file except in compliance with the License.          */
/* You may obtain a copy of the License at                                   */
/*                                                                           */
/*     http://www.apache.org/licenses/LICENSE-2.0                            */
/*                                                                           */
/* Unless required by applicable law or agreed to in writing, software       */
/* distributed under the License is distributed on an "AS IS" BASIS,         */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  */
/* See the License for the specific language governing permissions and       */
/* limitations under the License.                                            */
/*---------------------------------------------------------------------------*/

#ifndef __STBHANDLE_H__
#define __STBHANDLE_H__

#include "STBTypedefOutput.h"
#include "STBCommonType.h"
#include "SdkSTBTr.h"
#include "SdkSTBPe.h"
#include "SdkSTBFr.h"




typedef struct {

    STB_INT32        nDetID        ; 
    STB_INT32        nTraID        ;
    STB_STATUS        genStatus    ;//Gender
    STB_INT32        genConf        ; 
    STB_INT32        genVal        ;
    STB_STATUS        ageStatus    ;//Age
    STB_INT32        ageConf        ; 
    STB_INT32        ageVal        ;
    STB_STATUS        frStatus    ;//Face recognition
    STB_INT32        frConf        ; 
    STB_INT32        frVal        ;
    STB_STATUS        expStatus    ;//Facial expression
    STB_INT32        expVal        ;
    STB_INT32        expScore[STB_EX_MAX]    ;
    STB_INT32        expConf        ;
    STB_INT32        gazUD        ;//Gaze
    STB_INT32        gazLR        ;
    STB_STATUS        gazStatus    ;
    STB_INT32        gazConf        ;
    STB_INT32        dirRoll        ;//Face direction
    STB_INT32        dirPitch    ;    
    STB_INT32        dirYaw        ;    
    STB_STATUS        dirStatus    ;
    STB_INT32        dirConf        ;
    STB_INT32        bliL        ;//Blink
    STB_INT32        bliR        ;    
    STB_STATUS        bliStatus    ;

} FaceObj;

typedef struct {
    STB_INT32        nDetID        ; 
    STB_INT32        nTraID        ;
    STB_POS            pos            ;
    STB_INT32        size        ;
    STB_INT32        conf        ;
} TraObj;

typedef struct {
    /*------------------------------*/
    STB_INT32        nInitialized;/* SetFrameResult already executed */
    STB_INT32        nExecuted    ;/*Execute done*/
    STBExecFlg        *execFlg    ;
    /*------------------------------*/
    STB_TR_HANDLE    hTrHandle    ;
    STB_INT32        nDetCntBody    ;
    STB_INT32        nDetCntFace    ;
    STB_INT32        nTraCntBody    ;
    STB_INT32        nTraCntFace    ;
    TraObj            *trFace        ;
    TraObj            *trBody        ;
    /*------------------------------*/
    STB_PE_HANDLE    hPeHandle    ;
    STB_FR_HANDLE    hFrHandle    ;
    FaceObj            *infoFace    ;
    /*------------------------------*/
    STB_INT8        *stbPtr        ;
    STB_INT32        nDetCntMax    ;
    STB_INT32        nTraCntMax    ;


    STB_TR_DET            *wSrcTr        ;/*TR : input data*/
    STB_TR_RES_FACES    *wDstTrFace    ;/*TR : output data*/
    STB_TR_RES_BODYS    *wDstTrBody    ;/*TR : output data*/
    STB_PE_DET            *wSrcPe        ;/*PR : Input data*/
    STB_PE_RES            *wDstPe        ;/*PE : Output data*/
    STB_FR_DET            *wSrcFr        ;/*FR : Input data*/
    STB_FR_RES            *wDstFr        ;/*FR : Output data*/
} *STBHANDLE;



#endif /*__STBHANDLE_H__*/
