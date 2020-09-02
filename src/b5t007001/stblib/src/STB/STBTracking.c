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

#include "STBTracking.h"
#include "STB_Debug.h"

/*------------------------------------------------------------------------------------------------------------------*/
/* SetTrackingObjectBody                                                                                            */
/*------------------------------------------------------------------------------------------------------------------*/
VOID SetTrackingObjectBody(const STB_FRAME_RESULT_BODYS* stbINPUTbodys,TraObj *bodys)
{

    STB_INT32 nCount;
    STB_INT32 i;


    /*make the human body information*/
    nCount = stbINPUTbodys->nCount;
    
    for ( i = 0; i < nCount; i++)
    {
        bodys[i].nDetID        = i;
        bodys[i].pos.x        = stbINPUTbodys->body[i].center.nX    ;
        bodys[i].pos.y        = stbINPUTbodys->body[i].center.nY    ;
        bodys[i].conf        = stbINPUTbodys->body[i].nConfidence;
        bodys[i].size        = stbINPUTbodys->body[i].nSize        ;
        bodys[i].nTraID        = STB_STATUS_NO_DATA;
    }

}
/*------------------------------------------------------------------------------------------------------------------*/
/* SetTrackingObjectFace                                                                                            */
/*------------------------------------------------------------------------------------------------------------------*/
VOID SetTrackingObjectFace ( const STB_FRAME_RESULT_FACES *stbINPUTfaces    ,TraObj *faces    )
{

    STB_INT32 nCount;
    STB_INT32 i;


    /*make the human body information*/
    nCount = stbINPUTfaces->nCount;
    for ( i = 0; i < nCount; i++)
    {
        faces[i].nDetID        = i;
        faces[i].pos.x        = stbINPUTfaces->face[i].center.nX    ;
        faces[i].pos.y        = stbINPUTfaces->face[i].center.nY    ;
        faces[i].conf        = stbINPUTfaces->face[i].nConfidence;
        faces[i].size        = stbINPUTfaces->face[i].nSize        ;
        faces[i].nTraID        = STB_STATUS_NO_DATA;
    }

}
/*------------------------------------------------------------------------------------------------------------------*/
/* SetSrcTrFace                                                                                                        */
/*------------------------------------------------------------------------------------------------------------------*/
VOID SetSrcTrFace ( STB_INT32 nDetCntFace , TraObj *trFace, STB_TR_DET *trSrcInfo)
{
    STB_INT32 i;

    trSrcInfo->fcNum = nDetCntFace;
    for( i = 0; i < nDetCntFace; i++)
    {
        trSrcInfo->fcDet[i].conf    = trFace[i].conf    ;
        trSrcInfo->fcDet[i].posX    = trFace[i].pos    .x    ;
        trSrcInfo->fcDet[i].posY    = trFace[i].pos    .y    ;
        trSrcInfo->fcDet[i].size    = trFace[i].size    ;
    }
}
/*------------------------------------------------------------------------------------------------------------------*/
/* SetSrcTrBody                                                                                                        */
/*------------------------------------------------------------------------------------------------------------------*/
VOID SetSrcTrBody ( STB_INT32 nDetCntBody , TraObj *trBody, STB_TR_DET *trSrcInfo)
{
    STB_INT32 i;

    trSrcInfo->bdNum = nDetCntBody;

    for( i = 0; i < nDetCntBody; i++)
    {
        trSrcInfo->bdDet[i].conf    = trBody[i].conf    ;
        trSrcInfo->bdDet[i].posX    = trBody[i].pos    .x    ;
        trSrcInfo->bdDet[i].posY    = trBody[i].pos    .y    ;
        trSrcInfo->bdDet[i].size    = trBody[i].size    ;
    }
}

/*------------------------------------------------------------------------------------------------------------------*/
/* SetTrackingInfoToFace : Reflect tracking result in structure of detection result */
/*------------------------------------------------------------------------------------------------------------------*/
VOID SetTrackingInfoToFace( STB_TR_RES_FACES *fdResult ,STB_INT32 *pnTrackingNum ,TraObj *faces )
{
    STB_INT32 nIdx;

    *pnTrackingNum = fdResult->cnt;
    for (nIdx = 0; nIdx < *pnTrackingNum; nIdx++)
    {
        faces[nIdx].nDetID    = fdResult->face[nIdx].nDetID    ;
        faces[nIdx].nTraID    = fdResult->face[nIdx].nTraID    ;
        faces[nIdx].pos    .x    = fdResult->face[nIdx].pos.x    ;
        faces[nIdx].pos    .y  = fdResult->face[nIdx].pos.y    ;
        faces[nIdx].size    = fdResult->face[nIdx].size        ;
        faces[nIdx].conf    = fdResult->face[nIdx].conf        ;
    }

    return;
}

VOID SetTrackingInfoToBody(STB_TR_RES_BODYS *bdResult,STB_INT32 *pnTrackingNum,TraObj *bodys)
{
    STB_INT32 nIdx;

    *pnTrackingNum = bdResult->cnt;
    for (nIdx = 0; nIdx < *pnTrackingNum; nIdx++)
    {
        bodys[nIdx].nDetID    = bdResult->body[nIdx].nDetID    ;
        bodys[nIdx].nTraID    = bdResult->body[nIdx].nTraID    ;
        bodys[nIdx].pos    .x  = bdResult->body[nIdx].pos.x    ;
        bodys[nIdx].pos    .y  = bdResult->body[nIdx].pos.y    ;
        bodys[nIdx].size    = bdResult->body[nIdx].size        ;
        bodys[nIdx].conf    = bdResult->body[nIdx].conf        ;
    }

    return;
}