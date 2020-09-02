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

#if !defined( _SDK_STBFR_H_ )
#define _SDK_STBFR_H_
#include "STBFrTypedef.h"

#if !defined( STB_DEF_FR_HANDLE )
    #define     STB_DEF_FR_HANDLE
    typedef  VOID*  STB_FR_HANDLE;
#endif

STB_FR_HANDLE    STB_Fr_CreateHandle            ( const STB_INT32 nTraCntMax                            );/*Create/Delete handle*/
STB_INT32        STB_Fr_DeleteHandle            ( STB_FR_HANDLE handle                                    );/*Create/Delete handle*/
STB_INT32        STB_Fr_SetDetect            ( STB_FR_HANDLE handle, const STB_FR_DET *stbFrDet        );/*Frame information settings*/
STB_INT32        STB_Fr_Execute                ( STB_FR_HANDLE handle                                    );/*Main process execution*/
STB_INT32        STB_Fr_GetResult            ( STB_FR_HANDLE handle, STB_FR_RES* frResult            );/*Get result*/
STB_INT32        STB_Fr_Clear                ( STB_FR_HANDLE handle                                    );/*Clear*/

/*parameter*/
STB_INT32        STB_Fr_SetFaceDirMinMax        ( STB_FR_HANDLE handle , STB_INT32 nMinUDAngle    , STB_INT32 nMaxUDAngle        ,STB_INT32 nMinLRAngle    , STB_INT32 nMaxLRAngle    );/* FaceDirMinMax */
STB_INT32        STB_Fr_GetFaceDirMinMax        ( STB_FR_HANDLE handle , STB_INT32 *pnMinUDAngle, STB_INT32 *pnMaxUDAngle    ,STB_INT32 *pnMinLRAngle, STB_INT32 *pnMaxLRAngle);
STB_INT32        STB_Fr_Clear                ( STB_FR_HANDLE handle                            );/* ClearID */
STB_INT32        STB_Fr_SetFaceDirThreshold    ( STB_FR_HANDLE handle , STB_INT32    threshold    );/* FaceDirThreshold */
STB_INT32        STB_Fr_GetFaceDirThreshold    ( STB_FR_HANDLE handle , STB_INT32*    threshold    );
STB_INT32        STB_Fr_SetFrameCount        ( STB_FR_HANDLE handle , STB_INT32    nFrameCount    );
STB_INT32        STB_Fr_GetFrameCount        ( STB_FR_HANDLE handle , STB_INT32*    nFrameCount    );
STB_INT32        STB_Fr_SetMinRatio            ( STB_FR_HANDLE handle , STB_INT32    nMinRatio    );
STB_INT32        STB_Fr_GetMinRatio            ( STB_FR_HANDLE handle , STB_INT32*    nMinRatio    );

#endif
