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

#if !defined( _INTERFACE_H_ )
#define _INTERFACE_H_
#include "STBTrTypedef.h"
#include "STBCommonDef.h"
#include "STBCommonType.h"
#include "STBTrValidValue.h"

#ifdef  __cplusplus
extern "C" {
#endif


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///////////                            Define                            //////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
    
/* refer to past "STB_BACK_MAX-1" frames of results */
#define STB_TR_BACK_MAX 2 

#define STB_TR_DET_CNT_MAX 35 
#define STB_TR_TRA_CNT_MAX 35

//If the face isn't find out during tracking, set until how many frames can look for it.
//In the case of tracking failed with a specified number of frames consecutively, end of tracking as the face lost.
#define STB_TR_INI_RETRY        2 
#define STB_TR_MIN_RETRY        0 
#define STB_TR_MAX_RETRY        300 

//Specifies settings %
//For example, about the percentage of detected position change, setting the value to 30(<- initialize value)
//in the case of position change under 30 percentage from the previous frame, output detected position of the previous frame
//When it exceeds 30%, the detection position coordinate is output as it is.
#define STB_TR_INI_STEADINESS_POS        30
#define STB_TR_MIN_STEADINESS_POS          0
#define STB_TR_MAX_STEADINESS_POS        100

//Specifies settings %
//In the case of  the percentage of detection size change setting to 30(<- initialize value)
//in the case of size change under 30 percentage from the previous frame, output detected size of the previous frame
//When it exceeds 30%, the detection size is output as it is.
#define STB_TR_INI_STEADINESS_SIZE        30
#define STB_TR_MIN_STEADINESS_SIZE        0
#define STB_TR_MAX_STEADINESS_SIZE        100


//////////////////////////////////////////////////////////////////////////////////    
//////////////////////////////////////////////////////////////////////////////////
///////////                            Struct                            //////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////    


typedef struct{
    STB_INT32    cnt            ;
    STB_INT32   *nDetID        ; /*previous detected result ID*/
    STB_INT32   *nTraID        ; /*Tracking ID*/
    STB_INT32   *posX        ; /* Center x-coordinate    */
    STB_INT32   *posY        ; /* Center y-coordinate    */
    STB_INT32   *size        ; /* Size                    */
    STB_INT32   *conf        ; /* Degree of confidence    */
    STB_INT32   *retryN        ; /*Continuous retry count*/
}ROI_SYS;


/*---------------------------------------------------------------------------*/
typedef struct tagPEHANDLE {
    STB_INT8            *trPtr            ;
    STB_INT32            detCntMax        ;//Maximum of detected people
    STB_INT32            traCntMax        ;//Maximum number of tracking people
    STB_INT32            retryCnt        ;//Retry count
    STB_INT32            stedPos            ;//stabilization parameter(position)
    STB_INT32            stedSize        ;//stabilization parameter(size)
    STB_INT32            fcCntAcc        ;//Number of faces (cumulative)
    STB_INT32            bdCntAcc        ;//a number of human bodies(cumulative)
    STB_TR_DET            *stbTrDet        ;//Present data before the stabilization(input).
    ROI_SYS                *fcRec            ;//past data
    ROI_SYS                *bdRec            ;//past data
    STB_TR_RES_FACES    *resFaces        ;//present data after the stabilization(output)
    STB_TR_RES_BODYS    *resBodys        ;//present data after the stabilization(output)
    STB_INT32            *wIdPreCur        ;
    STB_INT32            *wIdCurPre        ;
    STB_INT32            *wDstTbl        ;
    STBExecFlg            *execFlg        ;
    ROI_SYS                *wRoi            ;
} *TRHANDLE;

//////////////////////////////////////////////////////////////////////////////////    
//////////////////////////////////////////////////////////////////////////////////
///////////                            Func                            //////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

TRHANDLE    TrCreateHandle        ( const STBExecFlg* execFlg ,const STB_INT32 nDetCntMax, const STB_INT32 nTraCntMax );
STB_INT32    TrDeleteHandle        ( TRHANDLE handle);
STB_INT32    TrSetDetect            ( TRHANDLE handle , const STB_TR_DET *stbTrDet);
STB_INT32    TrExecute            ( TRHANDLE handle);
STB_INT32    TrClear                ( TRHANDLE handle);
STB_INT32    TrGetResult            ( TRHANDLE handle , STB_TR_RES_FACES* fcResult,STB_TR_RES_BODYS* bdResult);
STB_INT32    TrSetRetryCount        ( TRHANDLE handle , STB_INT32  nRetryCount    );
STB_INT32    TrGetRetryCount        ( TRHANDLE handle , STB_INT32* nRetryCount    );
STB_INT32    TrSetStedinessParam    ( TRHANDLE handle , STB_INT32  nStedinessPos , STB_INT32  nStedinessSize );
STB_INT32    TrGetStedinessParam    ( TRHANDLE handle , STB_INT32* nStedinessPos , STB_INT32* nStedinessSize );



#ifdef  __cplusplus
}
#endif

#endif
