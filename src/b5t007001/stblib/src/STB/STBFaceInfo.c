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

#include "STBCommonDef.h"
#include "STBFaceInfo.h"
#include "STB_Debug.h"
#include "STBValidValue.h"




/*------------------------------------------------------------------------------------------------------------------*/
/*CopyFace : Face information all copy*/
/*------------------------------------------------------------------------------------------------------------------*/
static VOID CopyFace(FaceObj *faceSrc,FaceObj *faceDst, const STBExecFlg *execFlg )
{

    STB_INT32    j;

    faceDst->nDetID        = faceSrc->nDetID        ;
    faceDst->nTraID        = faceSrc->nTraID        ;
    if( execFlg->gen == STB_TRUE )
    {
        faceDst->genConf    = faceSrc->genConf        ; 
        faceDst->genStatus  = faceSrc->genStatus    ;     
        faceDst->genVal     = faceSrc->genVal        ;
    }
    if( execFlg->age == STB_TRUE )
    {
        faceDst->ageConf    = faceSrc->ageConf        ; 
        faceDst->ageStatus  = faceSrc->ageStatus    ;     
        faceDst->ageVal     = faceSrc->ageVal        ;   
    }
    if( execFlg->fr == STB_TRUE )
    {
        faceDst->frConf        = faceSrc->frConf        ; 
        faceDst->frStatus    = faceSrc->frStatus        ;     
        faceDst->frVal        = faceSrc->frVal        ;
    }
    if( execFlg->exp == STB_TRUE )
    {
        faceDst->expConf    = faceSrc->expConf        ;
        faceDst->expStatus    = faceSrc->expStatus    ;
        faceDst->expVal        = faceSrc->expVal        ;
        for( j = 0 ; j < STB_EX_MAX ;j++)
        {
            faceDst->expScore[j] = faceSrc->expScore[j];
        }
    }
    if( execFlg->gaz == STB_TRUE )
    {
        faceDst->gazConf    = faceSrc->gazConf        ;
        faceDst->gazStatus  = faceSrc->gazStatus    ; 
        faceDst->gazLR      = faceSrc->gazLR        ;
        faceDst->gazUD      = faceSrc->gazUD        ;
    }
    if( execFlg->dir == STB_TRUE )
    {
        faceDst->dirConf    = faceSrc->dirConf        ;
        faceDst->dirYaw     = faceSrc->dirYaw        ;
        faceDst->dirRoll    = faceSrc->dirRoll        ;   
        faceDst->dirPitch   = faceSrc->dirPitch        ;  
        faceDst->dirStatus  = faceSrc->dirStatus    ;  
    }
    if( execFlg->bli == STB_TRUE )
    {
        faceDst->bliL       = faceSrc->bliL            ;
        faceDst->bliR       = faceSrc->bliR            ;
        faceDst->bliStatus  = faceSrc->bliStatus    ;
    }



}
/*------------------------------------------------------------------------------------------------------------------*/
/* ClearFace                                                                                                        */
/*------------------------------------------------------------------------------------------------------------------*/
static VOID ClearFace ( FaceObj *face , int  i , const STBExecFlg *execFlg )
{
    STB_INT32    j;

    face[i].nDetID        = -1    ;
    face[i].nTraID        = -1    ;

    if( execFlg->gen == STB_TRUE )
    {
        face[i].genConf      = STB_ERR_PE_CANNOT    ;
        face[i].genStatus   = STB_STATUS_NO_DATA    ;
        face[i].genVal      = STB_ERR_PE_CANNOT    ;
    }
    if( execFlg->age == STB_TRUE )
    {
        face[i].ageConf        = STB_ERR_PE_CANNOT        ;
        face[i].ageStatus   = STB_STATUS_NO_DATA    ;
        face[i].ageVal        = STB_ERR_PE_CANNOT        ;
    }
    if( execFlg->fr == STB_TRUE )
    {
        face[i].frConf      = STB_ERR_PE_CANNOT        ;
        face[i].frStatus    = STB_STATUS_NO_DATA    ;
        face[i].frVal       = STB_ERR_FR_CANNOT    ;
    }
    if( execFlg->exp == STB_TRUE )
    {
        face[i].expConf        = STB_ERR_PE_CANNOT    ;
        for (j = STB_EX_NEUTRAL; j < STB_EX_MAX;j++)
        {
            face[i].expScore[j]    = STB_ERR_PE_CANNOT    ;
        }
        face[i].expStatus    = STB_STATUS_NO_DATA    ;
        face[i].expVal        = STB_ERR_PE_CANNOT        ;
    }
    if( execFlg->gaz == STB_TRUE )
    {
        face[i].gazConf      = STB_ERR_PE_CANNOT        ;
        face[i].gazStatus   = STB_STATUS_NO_DATA    ;
        face[i].gazLR       = STB_ERR_PE_CANNOT    ;
        face[i].gazUD       = STB_ERR_PE_CANNOT    ;
    }
    if( execFlg->dir == STB_TRUE )
    {
        face[i].dirConf        = STB_ERR_PE_CANNOT        ;
        face[i].dirStatus    = STB_STATUS_NO_DATA    ;
        face[i].dirYaw        = STB_ERR_DIR_CANNOT    ;
        face[i].dirRoll        = STB_ERR_DIR_CANNOT    ;
        face[i].dirPitch    = STB_ERR_DIR_CANNOT    ;
    }
    if( execFlg->bli == STB_TRUE )
    {
        face[i].bliL        = STB_ERR_PE_CANNOT    ;
        face[i].bliR        = STB_ERR_PE_CANNOT    ;
        face[i].bliStatus    = STB_STATUS_NO_DATA    ;
    }
    
}
/*------------------------------------------------------------------------------------------------------------------*/
/* SetFaceObject : Copy the tracking information */
/*------------------------------------------------------------------------------------------------------------------*/
VOID SetFaceObject( const STB_FRAME_RESULT_FACES* stbINPUTfaces    ,FaceObj *faces    , const STBExecFlg *execFlg , const STB_INT32 nTraCntMax)
{
    STB_INT32 nCount;
    STB_INT32 i,nIdx1;


    nCount = stbINPUTfaces->nCount;


    for (i = 0; i < nCount; i++)
    {
        faces[i].nDetID    = i;
        faces[i].nTraID    = STB_STATUS_NO_DATA;
    }

    if( execFlg->dir == STB_TRUE )            /*Face direction*/
    {
        for (i = 0; i < nCount; i++)
        {
            faces[i].dirConf    = stbINPUTfaces->face[i].direction.nConfidence;
            faces[i].dirYaw        = stbINPUTfaces->face[i].direction.nLR;
            faces[i].dirRoll    = stbINPUTfaces->face[i].direction.nRoll;
            faces[i].dirPitch    = stbINPUTfaces->face[i].direction.nUD;
        }
    }
    if( execFlg->age == STB_TRUE )            /*Age*/
    {
        for (i = 0; i < nCount; i++)
        {
            faces[i].ageConf    = stbINPUTfaces->face[i].age.nConfidence;
            faces[i].ageStatus    = STB_STATUS_NO_DATA;
            faces[i].ageVal        = stbINPUTfaces->face[i].age.nAge;
        }
    }
    if( execFlg->exp == STB_TRUE )            /*Facial expression*/
    {
        for (i = 0; i < nCount; i++)
        {
            faces[i].expConf    = -1;// not degree 
            for (nIdx1 = STB_EX_NEUTRAL; nIdx1 < STB_EX_MAX;nIdx1++)
            {
                faces[i].expScore[ nIdx1]    = stbINPUTfaces->face[i].expression.anScore[nIdx1];
            }
            faces[i].expStatus    = STB_STATUS_NO_DATA;
            faces[i].expVal        = STB_EX_UNKNOWN;
        }
    }
    if( execFlg->gen == STB_TRUE )            /*Gender*/
    {
        for (i = 0; i < nCount; i++)
        {
            faces[i].genConf    = stbINPUTfaces->face[i].gender.nConfidence;
            faces[i].genStatus    = STB_STATUS_NO_DATA;
            faces[i].genVal        = stbINPUTfaces->face[i].gender.nGender;
        }
    }
    if( execFlg->gaz == STB_TRUE )            /*Gaze*/
    {
        for (i = 0; i < nCount; i++)
        {
            faces[i].gazConf    = stbINPUTfaces->face[i].direction.nConfidence;
            faces[i].gazStatus    = STB_STATUS_NO_DATA;
            faces[i].gazLR        = stbINPUTfaces->face[i].gaze.nLR;
            faces[i].gazUD        = stbINPUTfaces->face[i].gaze.nUD;
        }
    }
    if( execFlg->fr == STB_TRUE )            /*Face recognition*/
    {
        for (i = 0; i < nCount; i++)
        {
            faces[i].frConf        = stbINPUTfaces->face[i].recognition.nScore;
            faces[i].frStatus    = STB_STATUS_NO_DATA;
            faces[i].frVal        = stbINPUTfaces->face[i].recognition.nUID;
        }
    }
    if( execFlg->bli == STB_TRUE )            //blink
    {
        for (i = 0; i < nCount; i++)
        {
            faces[i].bliL        = stbINPUTfaces->face[i].blink.nLeftEye;
            faces[i].bliR        = stbINPUTfaces->face[i].blink.nRightEye;
        }
    }

    /*The results exceeding the detection number are initialized*/
    for ( i = nCount; i < nTraCntMax; i++)
    {
        ClearFace ( faces , i  , execFlg );
    }

}


/*------------------------------------------------------------------------------------------------------------------*/
/* SetTrackingIDToFace : Tracking result ID is linked to face information */
/*------------------------------------------------------------------------------------------------------------------*/
VOID SetTrackingIDToFace(STB_INT32 TrackingNum,STB_INT32 DetectNum, TraObj *track,FaceObj *faces , const STBExecFlg *execFlg )
{
    STB_INT32 i,j;


    /*If there is a detection result erased in the tracking result, it is deleted from the face information.*/
    for( i = 0 ; i < DetectNum; i++)
    {
        /*termination if no more detected results*/
        if(faces[i].nDetID < 0)
        {
            break;
        }

        /*Search for the same ID as the detection result from the tracking result*/
        for( j = 0; j < TrackingNum ; j++)
        {
            if(track[j].nDetID == faces[i].nDetID)
            {
                faces[i].nTraID = track[j].nTraID;
                break;
            }
        }

        if( j >= TrackingNum){
            /*If the detection ID is not included in the ID under tracking*/
            ClearFace ( faces , i , execFlg);
            for(j = j + 1 ; j < DetectNum; j++)
            {
                if(faces[j].nDetID < 0)
                {
                    /*Repeat until detection result disappears*/
                    break;
                }
                /*Stuff up ahead of erasure*/
                CopyFace ( (faces+j),(faces+j-1), execFlg );
            }
        }
    }
    /*After that, the processing for the face only during tracking (retry status)*/
    for( j = 0 ; j < TrackingNum; j++)
    {
        if( track[j].nDetID < 0)
        {
            ClearFace ( faces , i , execFlg );/*It should have been cleared but just in case*/
            faces[i].nDetID        = track[j].nDetID;
            faces[i].nTraID        = track[j].nTraID;
            i++;
        }
    }
    
    return;
}
/*------------------------------------------------------------------------------------------------------------------*/
/* SetFaceToPeInfo : Create input data for stabilization of property estimation from face information */
/*------------------------------------------------------------------------------------------------------------------*/
VOID SetFaceToPeInfo(STB_INT32 TrackingNum,FaceObj *faces,STB_PE_DET *peInfo)
{
    STB_INT32 nIdx,nIdx1;

    peInfo->num = TrackingNum;
    for(nIdx=0 ; nIdx < TrackingNum;nIdx++)
    {
        /*tracking result*/
        peInfo->fcDet[nIdx].nDetID        = faces[nIdx].nDetID    ;
        peInfo->fcDet[nIdx].nTraID        = faces[nIdx].nTraID    ;
        /*Face direction estimation*/
        peInfo->fcDet[nIdx].dirDetConf    = faces[nIdx].dirConf    ;
        peInfo->fcDet[nIdx].dirDetYaw    = faces[nIdx].dirYaw    ;
        peInfo->fcDet[nIdx].dirDetPitch    = faces[nIdx].dirPitch    ;
        peInfo->fcDet[nIdx].dirDetRoll    = faces[nIdx].dirRoll    ;
        /*Age estimation*/
        peInfo->fcDet[nIdx].ageDetVal   = faces[nIdx].ageVal    ;
        peInfo->fcDet[nIdx].ageDetConf    = faces[nIdx].ageConf    ;
        /*Gaze estimation*/
        peInfo->fcDet[nIdx].gazDetLR    = faces[nIdx].gazLR        ;
        peInfo->fcDet[nIdx].gazDetUD    = faces[nIdx].gazUD        ;
        /*Gender estimation*/
        peInfo->fcDet[nIdx].genDetVal    = faces[nIdx].genVal    ;
        peInfo->fcDet[nIdx].genDetConf    = faces[nIdx].genConf    ;
        /*estimation of facial expression*/
        peInfo->fcDet[nIdx].expDetConf    = faces[nIdx].expConf    ;
        for( nIdx1 = 0; nIdx1 < STB_EX_MAX; nIdx1++)
        {
            peInfo->fcDet[nIdx].expDetVal[nIdx1]  = faces[nIdx].expScore[nIdx1];
        }
        //blink
        peInfo->fcDet[nIdx].bliDetL        = faces[nIdx].bliL        ;
        peInfo->fcDet[nIdx].bliDetR        = faces[nIdx].bliR        ;
        

    }
    return;
}
/*------------------------------------------------------------------------------------------------------------------*/
/* SetFaceToFrInfo : Create input data for stabilization of face recognition from face information */
/*------------------------------------------------------------------------------------------------------------------*/
VOID SetFaceToFrInfo(STB_INT32 TrackingNum,FaceObj *faces,STB_FR_DET *frInfo)
{
    STB_INT32 nIdx;


    frInfo->num = TrackingNum;
    for(nIdx=0 ; nIdx < TrackingNum;nIdx++)
    {
        /*tracking result*/
        frInfo->fcDet[nIdx].nDetID        = faces[nIdx].nDetID    ;
        frInfo->fcDet[nIdx].nTraID        = faces[nIdx].nTraID    ;

        /*Face direction estimation*/
        frInfo->fcDet[nIdx].dirDetConf    = faces[nIdx].dirConf    ;
        frInfo->fcDet[nIdx].dirDetYaw    = faces[nIdx].dirYaw    ;
        frInfo->fcDet[nIdx].dirDetPitch = faces[nIdx].dirPitch    ;
        frInfo->fcDet[nIdx].dirDetRoll  = faces[nIdx].dirRoll    ;

        /*recognition result*/
        frInfo->fcDet[nIdx].frDetConf    = faces[nIdx].frConf    ;
        frInfo->fcDet[nIdx].frDetID        = faces[nIdx].frVal        ;

    }
    return;
}
/*------------------------------------------------------------------------------------------------------------------*/
/* SetPeInfoToFace : Copy stabilization result of property estimation to face information */
/*------------------------------------------------------------------------------------------------------------------*/
VOID SetPeInfoToFace(STB_INT32 TrackingNum,STB_PE_RES *peInfo,FaceObj *faces, const STBExecFlg *execFlg )
{
    STB_INT32 i,j;

    for( i = 0 ; i < TrackingNum; i++)
    {
        for( j = 0 ; j < TrackingNum; j++)
        {
            if(peInfo->peFace[j].nTraID == faces[i].nTraID    )
            {
                if( execFlg->gen == STB_TRUE )
                {
                    faces[i].genStatus    = peInfo->peFace[j].gen.status    ;
                    faces[i].genVal        = peInfo->peFace[j].gen.value    ;
                    faces[i].genConf    = peInfo->peFace[j].gen.conf    ;
                }
                if( execFlg->age == STB_TRUE )
                {
                    faces[i].ageStatus    = peInfo->peFace[j].age.status    ;
                    faces[i].ageVal        = peInfo->peFace[j].age.value    ;
                    faces[i].ageConf    = peInfo->peFace[j].age.conf    ;
                }
                if( execFlg->exp == STB_TRUE )
                {
                    faces[i].expStatus    = peInfo->peFace[j].exp.status    ;
                    faces[i].expVal        = peInfo->peFace[j].exp.value    ;
                    faces[i].expConf    = peInfo->peFace[j].exp.conf    ;
                }
                if( execFlg->gaz == STB_TRUE )
                {
                    faces[i].gazStatus    = peInfo->peFace[j].gaz.status    ;
                    faces[i].gazConf    = peInfo->peFace[j].gaz.conf    ;
                    faces[i].gazLR        = peInfo->peFace[j].gaz.LR        ;
                    faces[i].gazUD        = peInfo->peFace[j].gaz.UD        ;
                }
                if( execFlg->dir == STB_TRUE )
                {
                    faces[i].dirPitch    = peInfo->peFace[j].dir.pitch    ;
                    faces[i].dirRoll    = peInfo->peFace[j].dir.roll    ;
                    faces[i].dirYaw        = peInfo->peFace[j].dir.yaw        ;
                    faces[i].dirStatus    = peInfo->peFace[j].dir.status    ;
                    faces[i].dirConf    = peInfo->peFace[j].dir.conf    ;
                }
                if( execFlg->bli == STB_TRUE )
                {
                    faces[i].bliL        = peInfo->peFace[j].bli.ratioL    ;
                    faces[i].bliR        = peInfo->peFace[j].bli.ratioR    ;
                    faces[i].bliStatus    = peInfo->peFace[j].bli.status    ;
                }
                break;
            }
        }
    }

    return;
}
/*------------------------------------------------------------------------------------------------------------------*/
/* SetFrInfoToFace : Copy stabilization result of face recognition to face information */
/*------------------------------------------------------------------------------------------------------------------*/
VOID SetFrInfoToFace(STB_INT32 TrackingNum,STB_FR_RES *frInfo,FaceObj *faces)
{
    STB_INT32 i,j;

    for( i = 0 ; i < TrackingNum; i++)
    {
        for( j = 0 ; j < TrackingNum; j++)
        {
            if(frInfo->frFace[j].nTraID == faces[i].nTraID)
            {
                faces[i].frStatus    = frInfo->frFace[j].frRecog.status    ;
                faces[i].frVal        = frInfo->frFace[j].frRecog.value    ;
                faces[i].frConf        = frInfo->frFace[j].frRecog.conf    ;
                break;
            }
        }
    }
    return;
}