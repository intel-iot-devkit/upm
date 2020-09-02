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
#include "STBPeTypedef.h"
#include "STBPeValidValue.h"


#ifdef  __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///////////                            Define                            //////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
    

#define STB_PE_BACK_MAX        20     /* refer to past "STB_BACK_MAX" frames of results */
#define STB_PE_EX_MAX            5    //A type of Facial expression

#define STB_PE_TRA_CNT_MAX 35

#define STB_PE_DIR_MIN_UD_INI -15
#define STB_PE_DIR_MIN_UD_MIN -90
#define STB_PE_DIR_MIN_UD_MAX  90

#define STB_PE_DIR_MAX_UD_INI  20
#define STB_PE_DIR_MAX_UD_MIN -90
#define STB_PE_DIR_MAX_UD_MAX  90

#define STB_PE_DIR_MIN_LR_INI -30
#define STB_PE_DIR_MIN_LR_MIN -90
#define STB_PE_DIR_MIN_LR_MAX  90

#define STB_PE_DIR_MAX_LR_INI  30
#define STB_PE_DIR_MAX_LR_MIN -90
#define STB_PE_DIR_MAX_LR_MAX    90


#define STB_PE_FRAME_CNT_INI    5
#define STB_PE_FRAME_CNT_MIN    1
#define STB_PE_FRAME_CNT_MAX    20

#define STB_PE_DIR_THR_INI        300
#define STB_PE_DIR_THR_MIN        0
#define STB_PE_DIR_THR_MAX        1000

//////////////////////////////////////////////////////////////////////////////////    
//////////////////////////////////////////////////////////////////////////////////
///////////                            Struct                            //////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////    
typedef struct tagPEHANDLE {

    STB_INT8        *pePtr            ;
    /* param */
    STB_INT32        peCntMax        ;//Maximum number of tracking people
    STB_INT32        peFaceDirUDMin    ;//The face on top/down allowable range min.
    STB_INT32        peFaceDirUDMax    ;//The face on top/down allowable range max.
    STB_INT32        peFaceDirLRMin    ;//The face on left /right side allowable range min.
    STB_INT32        peFaceDirLRMax    ;//The face on left /right side allowable range max.
    STB_INT32        peFaceDirThr    ;//If the confidence of Face direction estimation doesn't exceed the reference value, the recognition result isn't trusted.
    STB_INT32        peFrameCount    ;

    /* PE_Face */
    STB_PE_DET        peDet            ;//Present data before the stabilization(input).
    STB_PE_DET        *peDetRec        ;//past data before the stabilization
    STB_PE_RES        peRes            ;//present data after the stabilization(output)
    STBExecFlg        *execFlg        ;

} *PEHANDLE;

//////////////////////////////////////////////////////////////////////////////////    
//////////////////////////////////////////////////////////////////////////////////
///////////                            Func                            //////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
PEHANDLE    PeCreateHandle        ( const STBExecFlg* execFlg ,const STB_INT32 nTraCntMax);
STB_INT32    PeDeleteHandle        ( PEHANDLE handle);
STB_INT32    PeSetDetect            ( PEHANDLE handle,const STB_PE_DET *stbPeDet);
STB_INT32    PeExecute            ( PEHANDLE handle);
STB_INT32    PeGetResult            ( PEHANDLE handle , STB_PE_RES* peResult);
STB_INT32    PeSetFaceDirMinMax    ( PEHANDLE handle , STB_INT32 nMinUDAngle    , STB_INT32 nMaxUDAngle        ,STB_INT32 nMinLRAngle    , STB_INT32 nMaxLRAngle );
STB_INT32    PeGetFaceDirMinMax    ( PEHANDLE handle , STB_INT32 *pnMinUDAngle, STB_INT32 *pnMaxUDAngle    ,STB_INT32 *pnMinLRAngle, STB_INT32 *pnMaxLRAngle );
STB_INT32    PeClear                ( PEHANDLE handle                            );
STB_INT32    PeSetFaceDirThreshold ( PEHANDLE handle , STB_INT32  threshold    );
STB_INT32    PeGetFaceDirThreshold ( PEHANDLE handle , STB_INT32* threshold    );
STB_INT32    PeSetFrameCount        ( PEHANDLE handle , STB_INT32 nFrameCount    );
STB_INT32    PeGetFrameCount        ( PEHANDLE handle , STB_INT32* nFrameCount    );

#ifdef  __cplusplus
}
#endif

#endif
