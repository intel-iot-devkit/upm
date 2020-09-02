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

#include "SdkSTBFr.h"
#include "FrInterface.h"

/*This layer only defines the API function */

/*Create/Delete handle*/
STB_FR_HANDLE STB_Fr_CreateHandle( const STB_INT32 nTraCntMax ){
    return (STB_FR_HANDLE)FrCreateHandle( nTraCntMax );
}

STB_INT32 STB_Fr_DeleteHandle(STB_FR_HANDLE handle){
    return FrDeleteHandle((FRHANDLE)handle);
}

/*set frame information*/
STB_INT32 STB_Fr_SetDetect(STB_FR_HANDLE handle,const STB_FR_DET *stbFrDet){
    return FrSetDetect((FRHANDLE)handle,stbFrDet);
}

/*Main process execution*/
STB_INT32 STB_Fr_Execute(STB_FR_HANDLE handle){
    return FrExecute((FRHANDLE)handle);
}

/*get the result*/
STB_INT32 STB_Fr_GetResult(STB_FR_HANDLE handle, STB_FR_RES* peResult){
    return FrGetResult((FRHANDLE)handle,peResult);
}

STB_INT32 STB_Fr_Clear(STB_FR_HANDLE handle){
    return FrClear((FRHANDLE)handle);
}

/* FaceDirMinMax */
STB_INT32    STB_Fr_SetFaceDirMinMax    ( STB_FR_HANDLE handle ,  STB_INT32 nMinUDAngle    , STB_INT32 nMaxUDAngle        ,STB_INT32 nMinLRAngle    , STB_INT32 nMaxLRAngle        )
{
    return FrSetFaceDirMinMax((FRHANDLE)handle,nMinUDAngle,nMaxUDAngle,nMinLRAngle,nMaxLRAngle );
}
STB_INT32    STB_Fr_GetFaceDirMinMax    ( STB_FR_HANDLE handle , STB_INT32 *pnMinUDAngle    , STB_INT32 *pnMaxUDAngle    ,STB_INT32 *pnMinLRAngle, STB_INT32 *pnMaxLRAngle    )
{
    return FrGetFaceDirMinMax((FRHANDLE)handle,pnMinUDAngle,pnMaxUDAngle,pnMinLRAngle,pnMaxLRAngle );
}

/* FaceDirThreshold */
STB_INT32    STB_Fr_SetFaceDirThreshold    ( STB_FR_HANDLE handle , STB_INT32    threshold )
{
    return FrSetFaceDirThreshold((FRHANDLE)handle,threshold );
}
STB_INT32    STB_Fr_GetFaceDirThreshold    ( STB_FR_HANDLE handle , STB_INT32*    threshold )
{
    return FrGetFaceDirThreshold((FRHANDLE)handle,threshold );
}
/* FrameCount */
STB_INT32    STB_Fr_SetFrameCount    ( STB_FR_HANDLE handle , STB_INT32    nFrameCount )
{
    return FrSetFrameCount((FRHANDLE)handle,nFrameCount );
}
STB_INT32    STB_Fr_GetFrameCount    ( STB_FR_HANDLE handle , STB_INT32*    nFrameCount )
{
    return FrGetFrameCount((FRHANDLE)handle,nFrameCount );
}

/* FrameShare */
STB_INT32    STB_Fr_SetMinRatio    ( STB_FR_HANDLE handle , STB_INT32    nMinRatio )
{
    return FrSetMinRatio((FRHANDLE)handle,nMinRatio );
}
STB_INT32    STB_Fr_GetMinRatio    ( STB_FR_HANDLE handle , STB_INT32*    nMinRatio )
{
    return FrGetMinRatio((FRHANDLE)handle,nMinRatio );
}
