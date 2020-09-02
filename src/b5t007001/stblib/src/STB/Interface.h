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
#include "STBTypedefInput.h"
#include "STBHandle.h"

#define VERSION_MAJOR        (    1    )
#define VERSION_MINOR        (    1    )

#define DETECT_CNT_MAX        (    35    )    /*A maximum number of detected(input) people*/
#define TRACK_CNT_MAX        (    35    )    /*A maximum number of tracking(output) people*/

#ifdef  __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------------------------------------------------*/
STB_INT32 GetVersion        (STB_INT8* pnMajorVersion , STB_INT8* pnMinorVersion );
STBHANDLE CreateHandle        (STB_UINT32 stbExecFlg );
STB_INT32 DeleteHandle        (STBHANDLE handle);
STB_INT32 SetFrameResult    (STBHANDLE handle,const STB_FRAME_RESULT *stbINPUTResult);
STB_INT32 Execute            (STBHANDLE handle);
/*-------------------------------------------------------------------------------------------------------------------*/
STB_INT32 GetFaces    (STBHANDLE handle, STB_UINT32 *face_count, STB_FACE face[35]);
STB_INT32 GetBodies    (STBHANDLE handle, STB_UINT32 *body_count, STB_BODY body[35]);
STB_INT32 Clear        (STBHANDLE handle);
/*-------------------------------------------------------------------------------------------------------------------*/
STB_INT32 SetTrackingRetryCount            (STBHANDLE handle, STB_INT32 nMaxRetryCount        );
STB_INT32 GetTrackingRetryCount            (STBHANDLE handle, STB_INT32 *pnMaxRetryCount    );
STB_INT32 SetTrackingSteadinessParam    (STBHANDLE handle, STB_INT32 nPosSteadinessParam    , STB_INT32 nSizeSteadinessParam    );
STB_INT32 GetTrackingSteadinessParam    (STBHANDLE handle, STB_INT32 *pnPosSteadinessParam    , STB_INT32 *pnSizeSteadinessParam    );
/*-------------------------------------------------------------------------------------------------------------------*/
STB_INT32 SetPropertyThreshold            (STBHANDLE handle, STB_INT32 nThreshold        );
STB_INT32 GetPropertyThreshold            (STBHANDLE handle, STB_INT32 *pnThreshold    );
STB_INT32 SetPropertyAngle                (STBHANDLE handle, STB_INT32 nMinUDAngle    , STB_INT32 nMaxUDAngle        ,STB_INT32 nMinLRAngle    , STB_INT32 nMaxLRAngle        );
STB_INT32 GetPropertyAngle                (STBHANDLE handle, STB_INT32 *pnMinUDAngle    , STB_INT32 *pnMaxUDAngle    ,STB_INT32 *pnMinLRAngle, STB_INT32 *pnMaxLRAngle    );
STB_INT32 SetPropertyFrameCount            (STBHANDLE handle, STB_INT32 nFrameCount    );
STB_INT32 GetPropertyFrameCount            (STBHANDLE handle, STB_INT32 *pnFrameCount    );
/*-------------------------------------------------------------------------------------------------------------------*/
STB_INT32 SetRecognitionThreshold        (STBHANDLE handle, STB_INT32 nThreshold        );
STB_INT32 GetRecognitionThreshold        (STBHANDLE handle, STB_INT32 *pnThreshold    );
STB_INT32 SetRecognitionAngle            (STBHANDLE handle, STB_INT32 nMinUDAngle    , STB_INT32 nMaxUDAngle        ,STB_INT32 nMinLRAngle    , STB_INT32 nMaxLRAngle        );
STB_INT32 GetRecognitionAngle            (STBHANDLE handle, STB_INT32 *pnMinUDAngle    , STB_INT32 *pnMaxUDAngle    ,STB_INT32 *pnMinLRAngle, STB_INT32 *pnMaxLRAngle    );
STB_INT32 SetRecognitionFrameCount        (STBHANDLE handle, STB_INT32 nFrameCount    );
STB_INT32 GetRecognitionFrameCount        (STBHANDLE handle, STB_INT32 *pnFrameCount    );
STB_INT32 SetRecognitionRatio            (STBHANDLE handle, STB_INT32 nFrameShare    );
STB_INT32 GetRecognitionRatio            (STBHANDLE handle, STB_INT32 *pnFrameShare    );
/*-------------------------------------------------------------------------------------------------------------------*/
#ifdef  __cplusplus
}
#endif

#endif
