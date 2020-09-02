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

#include "SdkSTBPe.h"
#include "PeInterface.h"

/*This layer only defines the API function */

/*Create/Delete handle*/
STB_PE_HANDLE STB_Pe_CreateHandle(  const STBExecFlg* execFlg ,const STB_INT32 nTraCntMax  ){
    return (STB_PE_HANDLE)PeCreateHandle(   execFlg , nTraCntMax );
}

STB_INT32 STB_Pe_DeleteHandle(STB_PE_HANDLE handle){
    return PeDeleteHandle((PEHANDLE)handle);
}

/*set frame information*/
STB_INT32 STB_Pe_SetDetect(STB_PE_HANDLE handle,const STB_PE_DET *stbPeDet){
    return PeSetDetect((PEHANDLE)handle,stbPeDet);
}

/*Main process execution*/
STB_INT32 STB_Pe_Execute(STB_PE_HANDLE handle){
    return PeExecute((PEHANDLE)handle);
}

/*get the result*/
STB_INT32 STB_Pe_GetResult(STB_PE_HANDLE handle, STB_PE_RES* peResult){
    return PeGetResult((PEHANDLE)handle,peResult);
}
STB_INT32    STB_Pe_Clear    ( STB_PE_HANDLE handle )
{
    return PeClear((PEHANDLE)handle );
}



/* FaceDirMinMax */
STB_INT32    STB_Pe_SetFaceDirMinMax    ( STB_PE_HANDLE handle , STB_INT32 nMinUDAngle    , STB_INT32 nMaxUDAngle        ,STB_INT32 nMinLRAngle    , STB_INT32 nMaxLRAngle    )
{
    return PeSetFaceDirMinMax((PEHANDLE)handle,nMinUDAngle,nMaxUDAngle,nMinLRAngle,nMaxLRAngle );
}
STB_INT32    STB_Pe_GetFaceDirMinMax    ( STB_PE_HANDLE handle , STB_INT32 *pnMinUDAngle, STB_INT32 *pnMaxUDAngle    ,STB_INT32 *pnMinLRAngle, STB_INT32 *pnMaxLRAngle)
{
    return PeGetFaceDirMinMax((PEHANDLE)handle,pnMinUDAngle,pnMaxUDAngle, pnMinLRAngle,pnMaxLRAngle);
}
/* FaceDirThreshold */
STB_INT32    STB_Pe_SetFaceDirThreshold    ( STB_PE_HANDLE handle , STB_INT32    threshold )
{
    return PeSetFaceDirThreshold((PEHANDLE)handle,threshold );
}
STB_INT32    STB_Pe_GetFaceDirThreshold    ( STB_PE_HANDLE handle , STB_INT32*    threshold )
{
    return PeGetFaceDirThreshold((PEHANDLE)handle,threshold );
}

/* FrameCount */
STB_INT32    STB_Pe_SetFrameCount    ( STB_PE_HANDLE handle , STB_INT32    nFrameCount )
{
    return PeSetFrameCount((PEHANDLE)handle,nFrameCount );
}
STB_INT32    STB_Pe_GetFrameCount    ( STB_PE_HANDLE handle , STB_INT32*    nFrameCount )
{
    return PeGetFrameCount((PEHANDLE)handle,nFrameCount );
}

