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

#include "STBFrTypedef.h"
#include "STBCommonDef.h"
#include "STBCommonType.h"
#include "STBFrValidValue.h"

#ifdef  __cplusplus
extern "C" {
#endif


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///////////                            Define                            //////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
    

#define STB_FR_BACK_MAX        20 /* refer to past "STB_BACK_MAX" frames of results */

#define STB_FR_TRA_CNT_MAX 35
    
#define STB_FR_INVALID_UID  -999

#define STB_FR_DIR_MIN_UD_INI -15
#define STB_FR_DIR_MIN_UD_MIN -90
#define STB_FR_DIR_MIN_UD_MAX  90

#define STB_FR_DIR_MAX_UD_INI  20
#define STB_FR_DIR_MAX_UD_MIN -90
#define STB_FR_DIR_MAX_UD_MAX  90

#define STB_FR_DIR_MIN_LR_INI -30
#define STB_FR_DIR_MIN_LR_MIN -90
#define STB_FR_DIR_MIN_LR_MAX  90

#define STB_FR_DIR_MAX_LR_INI  30
#define STB_FR_DIR_MAX_LR_MIN -90
#define STB_FR_DIR_MAX_LR_MAX  90

#define STB_FR_DIR_THR_INI        300
#define STB_FR_DIR_THR_MIN        0
#define STB_FR_DIR_THR_MAX        1000

#define STB_FR_FRAME_CNT_INI    5
#define STB_FR_FRAME_CNT_MIN    0
#define STB_FR_FRAME_CNT_MAX    20

#define STB_FR_FRAME_RATIO_INI    60
#define STB_FR_FRAME_RATIO_MIN    0
#define STB_FR_FRAME_RATIO_MAX    100

//////////////////////////////////////////////////////////////////////////////////    
//////////////////////////////////////////////////////////////////////////////////
///////////                            Struct                            //////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////    
typedef struct tagFRHANDLE {
    
    STB_INT8        *frPtr            ;

    /* param */
    STB_INT32        frCntMax        ;//Maximum number of tracking people
    STB_INT32        frFaceDirUDMax    ;//The face on top/down allowable range max.
    STB_INT32        frFaceDirUDMin    ;//The face on top/down allowable range min.
    STB_INT32        frFaceDirLRMax    ;//The face on left /right side allowable range max.
    STB_INT32        frFaceDirLRMin    ;//The face on left /right side allowable range min.
    STB_INT32        frFaceDirThr    ;//If the confidence of Face direction estimation doesn't exceed the reference value, the recognition result isn't trusted.
    STB_INT32        frFrameCount    ;
    STB_INT32        frFrameRatio    ;
    /* FR_Face */
    STB_FR_DET        frDet        ;//Present data before the stabilization(input).
    STB_FR_DET        *frDetRec    ;//past data before the stabilization
    STB_FR_RES        frRes        ;//present data after the stabilization(output)

} *FRHANDLE;


//////////////////////////////////////////////////////////////////////////////////    
//////////////////////////////////////////////////////////////////////////////////
///////////                            Func                            //////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
FRHANDLE    FrCreateHandle    ( const STB_INT32 nTraCntMax );
STB_INT32    FrDeleteHandle    ( FRHANDLE handle);
STB_INT32    FrSetDetect        ( FRHANDLE handle,const STB_FR_DET *stbPeDet);
STB_INT32    FrExecute        ( FRHANDLE handle);
STB_INT32    FrClear            ( FRHANDLE handle  );
STB_INT32    FrGetResult        ( FRHANDLE handle , STB_FR_RES* peResult);

STB_INT32    FrSetFaceDirMinMax( FRHANDLE handle , STB_INT32 nMinUDAngle    , STB_INT32 nMaxUDAngle        ,STB_INT32 nMinLRAngle    , STB_INT32 nMaxLRAngle);
STB_INT32    FrGetFaceDirMinMax( FRHANDLE handle , STB_INT32 *pnMinUDAngle    , STB_INT32 *pnMaxUDAngle    ,STB_INT32 *pnMinLRAngle, STB_INT32 *pnMaxLRAngle );
STB_INT32    FrSetFaceDirThreshold ( FRHANDLE handle , STB_INT32  threshold );
STB_INT32    FrGetFaceDirThreshold ( FRHANDLE handle , STB_INT32* threshold );
STB_INT32    FrSetFrameCount    ( FRHANDLE handle , STB_INT32  nFrameCount );
STB_INT32    FrGetFrameCount    ( FRHANDLE handle , STB_INT32* nFrameCount );
STB_INT32    FrSetMinRatio    ( FRHANDLE handle , STB_INT32  nMinRatio );
STB_INT32    FrGetMinRatio    ( FRHANDLE handle , STB_INT32* nMinRatio );
#ifdef  __cplusplus
}
#endif

#endif
