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

#ifndef STBTRACKING_H__
#define STBTRACKING_H__
#include "STBTypedefInput.h"
#include "STBHandle.h"

VOID SetTrackingObjectBody    ( const STB_FRAME_RESULT_BODYS* stbINPUTbodys,TraObj *bodys);
VOID SetTrackingObjectFace    ( const STB_FRAME_RESULT_FACES *stbINPUTfaces,TraObj *faces);

VOID SetTrackingInfoToFace    ( STB_TR_RES_FACES *fdResult,STB_INT32 *pnTrackingNum,TraObj *faces);
VOID SetTrackingInfoToBody    ( STB_TR_RES_BODYS *bdResult,STB_INT32 *pnTrackingNum,TraObj *bodys);

VOID SetSrcTrFace            ( STB_INT32 nDetCntFace , TraObj *trFace, STB_TR_DET *trSrcInfo);
VOID SetSrcTrBody            ( STB_INT32 nDetCntBody , TraObj *trBody, STB_TR_DET *trSrcInfo);

#endif