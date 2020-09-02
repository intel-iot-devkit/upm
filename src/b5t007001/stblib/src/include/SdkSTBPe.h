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

#if !defined( _SDK_STBPE_H_ )
#define _SDK_STBPE_H_
#include "STBPeTypedef.h"

#if !defined( STB_DEF_PE_HANDLE )
    #define     STB_DEF_PE_HANDLE
    typedef  VOID*  STB_PE_HANDLE;
#endif

STB_PE_HANDLE    STB_Pe_CreateHandle        ( const STBExecFlg* execFlg ,const STB_INT32 nTraCntMax    );/*Create/Delete handle*/
STB_INT32        STB_Pe_DeleteHandle        ( STB_PE_HANDLE handle                                    );/*Create/Delete handle*/
STB_INT32        STB_Pe_SetDetect        ( STB_PE_HANDLE handle, const STB_PE_DET *stbPeDet        );/*Frame information settings*/
STB_INT32        STB_Pe_Execute            ( STB_PE_HANDLE handle                                    );/*Main process execution*/
STB_INT32        STB_Pe_GetResult        ( STB_PE_HANDLE handle, STB_PE_RES* peResult            );/*Get result*/

/*parameter*/
STB_INT32        STB_Pe_SetFaceDirMinMax        ( STB_PE_HANDLE handle , STB_INT32 nMinUDAngle    , STB_INT32 nMaxUDAngle        ,STB_INT32 nMinLRAngle    , STB_INT32 nMaxLRAngle    );/* FaceDirMinMax */
STB_INT32        STB_Pe_GetFaceDirMinMax        ( STB_PE_HANDLE handle , STB_INT32 *pnMinUDAngle, STB_INT32 *pnMaxUDAngle    ,STB_INT32 *pnMinLRAngle, STB_INT32 *pnMaxLRAngle);
STB_INT32        STB_Pe_Clear                ( STB_PE_HANDLE handle                                );/* Clear */
STB_INT32        STB_Pe_SetFaceDirThreshold    ( STB_PE_HANDLE handle , STB_INT32    threshold        );/* FaceDirThreshold */
STB_INT32        STB_Pe_GetFaceDirThreshold    ( STB_PE_HANDLE handle , STB_INT32*    threshold        );
STB_INT32        STB_Pe_SetFrameCount        ( STB_PE_HANDLE handle , STB_INT32    nFrameCount        );
STB_INT32        STB_Pe_GetFrameCount        ( STB_PE_HANDLE handle , STB_INT32*    nFrameCount        );

#endif
