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

#ifndef _SDK_STB_H_
#define _SDK_STB_H_

#include "STBTypedef.h"


#ifndef STB_DEF_HANDLE 
    #define     STB_DEF_HANDLE
    typedef  VOID*  HSTB;
#endif /* STB_DEF_HANDLE */

#ifdef  __cplusplus
extern "C" {
#endif

STB_INT32    STB_GetVersion(STB_INT8* pnMajorVersion, STB_INT8* pnMinorVersion);

/* Create/Delete handle */
HSTB         STB_CreateHandle(STB_UINT32 unUseFuncFlag);
VOID         STB_DeleteHandle(HSTB hSTB);

/* Set the one frame result of HVC into this library */
STB_INT32    STB_SetFrameResult(HSTB hSTB, const STB_FRAME_RESULT *stFrameResult);
/* Clear frame results */
STB_INT32    STB_ClearFrameResults(HSTB hSTB);

/* Main process execution */
STB_INT32    STB_Execute(HSTB hSTB);

/* Get the result */
STB_INT32    STB_GetFaces(HSTB hSTB, STB_UINT32 *punFaceCount, STB_FACE stFace[]);
STB_INT32    STB_GetBodies(HSTB hSTB, STB_UINT32 *punBodyCount, STB_BODY stBody[]);

/* Setting/Getting functions for tracking */
STB_INT32    STB_SetTrRetryCount(HSTB hSTB, STB_INT32 nMaxRetryCount);
STB_INT32    STB_GetTrRetryCount(HSTB hSTB, STB_INT32 *pnMaxRetryCount);
STB_INT32    STB_SetTrSteadinessParam(HSTB hSTB, STB_INT32 nPosSteadinessParam, STB_INT32 nSizeSteadinessParam);
STB_INT32    STB_GetTrSteadinessParam(HSTB hSTB, STB_INT32 *pnPosSteadinessParam, STB_INT32 *pnSizeSteadinessParam);
/* Setting/Getting functions for property */
STB_INT32    STB_SetPeThresholdUse(HSTB hSTB, STB_INT32 nThreshold);
STB_INT32    STB_GetPeThresholdUse(HSTB hSTB, STB_INT32 *pnThreshold);
STB_INT32    STB_SetPeAngleUse(HSTB hSTB, STB_INT32 nMinUDAngle, STB_INT32 nMaxUDAngle, STB_INT32 nMinLRAngle, STB_INT32 nMaxLRAngle);
STB_INT32    STB_GetPeAngleUse(HSTB hSTB, STB_INT32 *pnMinUDAngle, STB_INT32 *pnMaxUDAngle, STB_INT32 *pnMinLRAngle, STB_INT32 *pnMaxLRAngle);
STB_INT32    STB_SetPeCompleteFrameCount(HSTB hSTB, STB_INT32 nFrameCount);
STB_INT32    STB_GetPeCompleteFrameCount(HSTB hSTB, STB_INT32 *pnFrameCount);
/* Setting/Getting function for recognition */
STB_INT32    STB_SetFrThresholdUse(HSTB hSTB, STB_INT32 nThreshold);
STB_INT32    STB_GetFrThresholdUse(HSTB hSTB, STB_INT32 *pnThreshold);
STB_INT32    STB_SetFrAngleUse(HSTB hSTB, STB_INT32 nMinUDAngle, STB_INT32 nMaxUDAngle, STB_INT32 nMinLRAngle, STB_INT32 nMaxLRAngle);
STB_INT32    STB_GetFrAngleUse(HSTB hSTB, STB_INT32 *pnMinUDAngle, STB_INT32 *pnMaxUDAngle, STB_INT32 *pnMinLRAngle, STB_INT32 *pnMaxLRAngle);
STB_INT32    STB_SetFrCompleteFrameCount(HSTB hSTB, STB_INT32 nFrameCount);
STB_INT32    STB_GetFrCompleteFrameCount(HSTB hSTB, STB_INT32 *pnFrameCount);
STB_INT32    STB_SetFrMinRatio(HSTB hSTB, STB_INT32 nMinRatio);
STB_INT32    STB_GetFrMinRatio(HSTB hSTB, STB_INT32 *pnMinRatio);

#ifdef  __cplusplus
}
#endif

#endif /* _SDK_STB_H_ */
