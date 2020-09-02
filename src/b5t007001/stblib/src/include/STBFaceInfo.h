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

#ifndef STBFACEINFO_H__
#define STBFACEINFO_H__
#include "STBTypedefInput.h"
#include "STBHandle.h"

VOID SetFaceObject        (const STB_FRAME_RESULT_FACES* stbINPUTfaces,FaceObj *faces , const STBExecFlg *execFlg , const STB_INT32 nTraCntMax    );
VOID SetTrackingIDToFace(STB_INT32 TrackingNum,STB_INT32 DetectNum, TraObj *track,FaceObj *faces, const STBExecFlg *execFlg );
VOID SetFaceToPeInfo    (STB_INT32 TrackingNum,FaceObj *faces,STB_PE_DET *peInfo);
VOID SetFaceToFrInfo    (STB_INT32 TrackingNum,FaceObj *faces,STB_FR_DET *frInfo);
VOID SetPeInfoToFace    (STB_INT32 TrackingNum,STB_PE_RES *peInfo,FaceObj *faces , const STBExecFlg *execFlg );
VOID SetFrInfoToFace    (STB_INT32 TrackingNum,STB_FR_RES *frInfo,FaceObj *faces);

#endif