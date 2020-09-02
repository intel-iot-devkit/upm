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

#include "STBMakeResult.h"


/*------------------------------------------------------------------------------------------------------------------*/
/* SetFaceToResult                                                                                                    */
/*------------------------------------------------------------------------------------------------------------------*/
VOID SetFaceToResult ( STB_INT32 TraCnt , TraObj* trObj , FaceObj* faceObj, STB_FACE* result, const STBExecFlg* execFlg )
{
    STB_INT32 i,j;
    STB_INT32 tmpFlg;
    
    tmpFlg =    execFlg->gen        
            ||    execFlg->age    
            ||    execFlg->fr        
            ||    execFlg->exp        
            ||    execFlg->dir        
            ||    execFlg->gaz    
            ||    execFlg->bli    ;


    for( i = 0; i < TraCnt; i++)
    {
        result[i].nDetectID        = trObj[i].nDetID    ;
        result[i].nTrackingID    = trObj[i].nTraID    ;
        result[i].center.x        = trObj[i].pos    .x    ;
        result[i].center.y        = trObj[i].pos    .y    ;
        result[i].nSize            = trObj[i].size        ;
        result[i].conf            = trObj[i].conf        ;

        if(    tmpFlg    )
        {
            for( j = 0; j < TraCnt; j++)
            {
                if( trObj[i].nTraID == faceObj[j].nTraID )
                {
                    if(    execFlg->age    == STB_TRUE )
                    {
                        result[i].age.status            = faceObj[j].ageStatus            ;
                        result[i].age.value                = faceObj[j].ageVal                ;
                        result[i].age.conf                = faceObj[j].ageConf    ;
                    }
                    if(    execFlg->bli    == STB_TRUE )
                    {
                        result[i].blink.ratioL            = faceObj[j].bliL                ;
                        result[i].blink.ratioR            = faceObj[j].bliR                ;
                        result[i].blink.status            = faceObj[j].bliStatus            ;
                    }
                    if(    execFlg->dir    == STB_TRUE )
                    {
                        result[i].direction.pitch        = faceObj[j].dirPitch            ;
                        result[i].direction.roll        = faceObj[j].dirRoll            ;
                        result[i].direction.yaw            = faceObj[j].dirYaw                ;
                        result[i].direction.status        = faceObj[j].dirStatus            ;
                        result[i].direction.conf        = faceObj[j].dirConf    ;
                    }
                    if(    execFlg->exp    == STB_TRUE )
                    {
                        result[i].expression.status        = faceObj[j].expStatus            ;
                        result[i].expression.value        = faceObj[j].expVal                ;
                        result[i].expression.conf        = faceObj[j].expConf    ;
                    }
                    if(    execFlg->gaz    == STB_TRUE )
                    {
                        result[i].gaze.status            = faceObj[j].gazStatus            ;
                        result[i].gaze.LR                = faceObj[j].gazLR                ;
                        result[i].gaze.UD                = faceObj[j].gazUD                ;
                        result[i].gaze.conf                = faceObj[j].gazConf    ;
                    }
                    if(    execFlg->gen    == STB_TRUE )
                    {
                        result[i].gender.status            = faceObj[j].genStatus            ;
                        result[i].gender.value            = faceObj[j].genVal                ;
                        result[i].gender.conf            = faceObj[j].genConf    ;
                    }
                    if(    execFlg->fr        == STB_TRUE )
                    {
                        result[i].recognition.status    = faceObj[j].frStatus            ;
                        result[i].recognition.value        = faceObj[j].frVal                ;
                        result[i].recognition.conf        = faceObj[j].frConf    ;
                    }
                    break;
                }//if( trObj[i].nTraID == faceObj[j].nTraID )
            }//for( j = 0; j < TraCnt; j++)
        }//if(    tmpFlg    )
    }//for( i = 0; i < TraCnt; i++)
}
/*------------------------------------------------------------------------------------------------------------------*/
/* SetBodyToResult                                                                                                    */
/*------------------------------------------------------------------------------------------------------------------*/
VOID SetBodyToResult(STB_INT32 TraCnt,TraObj* trObj, STB_BODY* result)
{
    STB_INT32 i;
    for( i = 0; i < TraCnt; i++)
    {
        result[i].nDetectID   = trObj[i].nDetID    ;
        result[i].nTrackingID = trObj[i].nTraID    ;
        result[i].center.x    = trObj[i].pos.x    ;
        result[i].center.y    = trObj[i].pos.y    ;
        result[i].nSize       = trObj[i].size    ;
        result[i].conf        = trObj[i].conf    ;
    }
    
}
