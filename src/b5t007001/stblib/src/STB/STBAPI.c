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

#include "Interface.h"
#include "STBAPI.h"


/*This layer only defines the API function */

/*get the version*/
STB_INT32 STB_GetVersion(STB_INT8* pnMajorVersion, STB_INT8* pnMinorVersion){
    return GetVersion(pnMajorVersion, pnMinorVersion);
}
/*Create/Delete handle*/
HSTB STB_CreateHandle(STB_UINT32 stbExecFlg){
    return (HSTB)CreateHandle(stbExecFlg);
}
VOID STB_DeleteHandle(HSTB handle){
    DeleteHandle((STBHANDLE)handle);
}
/*set frame information*/
STB_INT32 STB_SetFrameResult(HSTB handle, const STB_FRAME_RESULT *stbINPUTResult){
    return SetFrameResult((STBHANDLE)handle, stbINPUTResult);
}
STB_INT32 STB_ClearFrameResults(HSTB handle){
    return Clear((STBHANDLE)handle);
}
/*Main process execution*/
STB_INT32 STB_Execute(HSTB handle){
    return Execute((STBHANDLE)handle);
}
/*get the result*/
STB_INT32 STB_GetFaces(HSTB handle, STB_UINT32 *face_count, STB_FACE face[35]){
    return GetFaces((STBHANDLE)handle, face_count, face);
}
STB_INT32 STB_GetBodies(HSTB handle, STB_UINT32 *body_count, STB_BODY body[35]){
    return GetBodies((STBHANDLE)handle, body_count, body);
}

/*Setting / Getting Function for tracking*/
STB_INT32   STB_SetTrRetryCount(HSTB hHandle, STB_INT32 nMaxRetryCount){
    return  SetTrackingRetryCount((STBHANDLE)hHandle, nMaxRetryCount);
}
STB_INT32 STB_GetTrRetryCount(HSTB hHandle, STB_INT32 *pnMaxRetryCount){
    return GetTrackingRetryCount((STBHANDLE)hHandle, pnMaxRetryCount);
}
STB_INT32 STB_SetTrSteadinessParam(HSTB hHandle, STB_INT32 nPosSteadinessParam, STB_INT32 nSizeSteadinessParam){
    return SetTrackingSteadinessParam((STBHANDLE)hHandle, nPosSteadinessParam, nSizeSteadinessParam);
}
STB_INT32 STB_GetTrSteadinessParam(HSTB hHandle, STB_INT32 *pnPosSteadinessParam, STB_INT32 *pnSizeSteadinessParam){
    return GetTrackingSteadinessParam((STBHANDLE)hHandle, pnPosSteadinessParam, pnSizeSteadinessParam);
}

/*Setting / Getting Function for property*/
STB_INT32 STB_SetPeThresholdUse(HSTB hHandle, STB_INT32 nThreshold){
    return SetPropertyThreshold((STBHANDLE)hHandle, nThreshold);
}
STB_INT32 STB_GetPeThresholdUse(HSTB hHandle, STB_INT32 *pnThreshold){
    return GetPropertyThreshold((STBHANDLE)hHandle, pnThreshold);
}
STB_INT32 STB_SetPeAngleUse(HSTB hHandle, STB_INT32 nMinUDAngle, STB_INT32 nMaxUDAngle, STB_INT32 nMinLRAngle, STB_INT32 nMaxLRAngle ){
    return SetPropertyAngle((STBHANDLE)hHandle, nMinUDAngle, nMaxUDAngle, nMinLRAngle, nMaxLRAngle);
}
STB_INT32 STB_GetPeAngleUse(HSTB hHandle, STB_INT32 *pnMinUDAngle, STB_INT32 *pnMaxUDAngle, STB_INT32 *pnMinLRAngle, STB_INT32 *pnMaxLRAngle ){
    return GetPropertyAngle((STBHANDLE)hHandle, pnMinUDAngle, pnMaxUDAngle, pnMinLRAngle, pnMaxLRAngle);
}
STB_INT32 STB_SetPeCompleteFrameCount(HSTB hHandle, STB_INT32 nFrameCount){
    return SetPropertyFrameCount((STBHANDLE)hHandle, nFrameCount);
}
STB_INT32 STB_GetPeCompleteFrameCount(HSTB hHandle, STB_INT32 *pnFrameCount){
    return GetPropertyFrameCount((STBHANDLE)hHandle, pnFrameCount);
}

/*Setting / Getting Function for recognition*/
STB_INT32 STB_SetFrThresholdUse(HSTB hHandle, STB_INT32 nThreshold){
    return SetRecognitionThreshold((STBHANDLE)hHandle, nThreshold);
}
STB_INT32 STB_GetFrThresholdUse(HSTB hHandle, STB_INT32 *pnThreshold){
    return GetRecognitionThreshold((STBHANDLE)hHandle, pnThreshold);
}
STB_INT32 STB_SetFrAngleUse(HSTB hHandle, STB_INT32 nMinUDAngle, STB_INT32 nMaxUDAngle, STB_INT32 nMinLRAngle, STB_INT32 nMaxLRAngle ){
    return SetRecognitionAngle((STBHANDLE)hHandle, nMinUDAngle, nMaxUDAngle, nMinLRAngle, nMaxLRAngle);
}
STB_INT32 STB_GetFrAngleUse(HSTB hHandle, STB_INT32 *pnMinUDAngle, STB_INT32 *pnMaxUDAngle, STB_INT32 *pnMinLRAngle, STB_INT32 *pnMaxLRAngle ){
    return GetRecognitionAngle((STBHANDLE)hHandle, pnMinUDAngle, pnMaxUDAngle, pnMinLRAngle, pnMaxLRAngle);
}
STB_INT32 STB_SetFrCompleteFrameCount(HSTB hHandle, STB_INT32 nFrameCount){
    return SetRecognitionFrameCount((STBHANDLE)hHandle, nFrameCount);
}
STB_INT32 STB_GetFrCompleteFrameCount(HSTB hHandle, STB_INT32 *pnFrameCount){
    return GetRecognitionFrameCount((STBHANDLE)hHandle, pnFrameCount);
}
STB_INT32 STB_SetFrMinRatio(HSTB hHandle, STB_INT32 nFrameRatio){
    return SetRecognitionRatio((STBHANDLE)hHandle, nFrameRatio);
}
STB_INT32 STB_GetFrMinRatio(HSTB hHandle, STB_INT32 *pnFrameRatio){
    return GetRecognitionRatio((STBHANDLE)hHandle, pnFrameRatio);
}

