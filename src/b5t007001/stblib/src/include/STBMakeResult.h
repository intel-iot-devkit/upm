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

#ifndef __STBMAKERESULT_h__
#define __STBMAKERESULT_H__

#include "STBHandle.h"

VOID SetFaceToResult(STB_INT32 TrackingNum,TraObj* dtfaces,FaceObj* faces, STB_FACE* result , const STBExecFlg* execFlg );
VOID SetBodyToResult(STB_INT32 TrackingNum,TraObj* dtbodys, STB_BODY* result);

#endif /*__STBMAKERESULT_H__*/
