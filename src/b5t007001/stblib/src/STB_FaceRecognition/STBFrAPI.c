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

#include "STBFrAPI.h"

/*---------------------------------------------------------------------*/
//    FrSlideFacesRec
/*---------------------------------------------------------------------*/
void FrSlideFacesRec ( STB_FR_DET *facesRec )
{
    STB_INT32 t , i ;

    for( t = STB_FR_BACK_MAX  - 2 ; t >= 0 ;  t-- ) 
    {
        facesRec [ t + 1 ].num = facesRec[ t + 0 ].num;
        for( i = 0 ; i < facesRec [ t + 1 ].num ;  i++ ) 
        {
            facesRec [ t + 1 ].fcDet[i].nDetID            = facesRec [ t ].fcDet[i].nDetID        ;
            facesRec [ t + 1 ].fcDet[i].nTraID            = facesRec [ t ].fcDet[i].nTraID        ;

            facesRec [ t + 1 ].fcDet[i].dirDetPitch        = facesRec [ t ].fcDet[i].dirDetPitch    ;
            facesRec [ t + 1 ].fcDet[i].dirDetRoll        = facesRec [ t ].fcDet[i].dirDetRoll    ;
            facesRec [ t + 1 ].fcDet[i].dirDetYaw        = facesRec [ t ].fcDet[i].dirDetYaw        ;
            facesRec [ t + 1 ].fcDet[i].dirDetConf        = facesRec [ t ].fcDet[i].dirDetConf    ;
            facesRec [ t + 1 ].fcDet[i].frDetID            = facesRec [ t ].fcDet[i].frDetID        ;
            facesRec [ t + 1 ].fcDet[i].frDetConf        = facesRec [ t ].fcDet[i].frDetConf        ;
            facesRec [ t + 1 ].fcDet[i].frStatus        = facesRec [ t ].fcDet[i].frStatus        ;

        }
    }
}
/*---------------------------------------------------------------------*/
//    FrCurFaces
/*---------------------------------------------------------------------*/
void FrCurFaces ( STB_FR_DET *facesRec , STB_FR_DET *srcFace )
{
    STB_INT32  i ;


    facesRec [ 0 ].num = srcFace->num;
    for( i = 0 ; i < facesRec [ 0 ].num ;  i++ ) 
    {
            facesRec [ 0 ].fcDet[i].nDetID        = srcFace->fcDet[i].nDetID        ;
            facesRec [ 0 ].fcDet[i].nTraID        = srcFace->fcDet[i].nTraID        ;
            facesRec [ 0 ].fcDet[i].dirDetPitch    = srcFace->fcDet[i].dirDetPitch    ;
            facesRec [ 0 ].fcDet[i].dirDetRoll    = srcFace->fcDet[i].dirDetRoll    ;
            facesRec [ 0 ].fcDet[i].dirDetYaw    = srcFace->fcDet[i].dirDetYaw    ;
            facesRec [ 0 ].fcDet[i].dirDetConf    = srcFace->fcDet[i].dirDetConf    ;
            facesRec [ 0 ].fcDet[i].frDetID        = srcFace->fcDet[i].frDetID        ;
            facesRec [ 0 ].fcDet[i].frDetConf    = srcFace->fcDet[i].frDetConf    ;
            facesRec [ 0 ].fcDet[i].frStatus    = STB_STATUS_NO_DATA            ;

    }

}


/*---------------------------------------------------------------------*/
//    FrStbFaceEasy
/*---------------------------------------------------------------------*/
void 
FrStbFaceEasy
    (
        STB_FR_RES*    peRes        , 
        STB_FR_DET*    peRec        , 
        STB_INT32    dirThr        , 
        STB_INT32    dirUDMax    , 
        STB_INT32    dirUDMin    ,
        STB_INT32    dirLRMax    , 
        STB_INT32    dirLRMin    ,
        STB_INT32    frmCnt        ,
        STB_INT32    frmRatio    
        
        
        )
{
    STB_INT32    i, t, k;
    STB_INT32    trID;
    STB_INT32    recCnt;
    STB_INT32    recUID  [STB_FR_BACK_MAX];
    STB_INT32    recConf [STB_FR_BACK_MAX];

    STB_INT32    accUID  [STB_FR_BACK_MAX];
    STB_INT32    accCnt  [STB_FR_BACK_MAX];
    STB_INT32    accKind;
    STB_INT32    tmpUID;
    STB_INT32    tmpCnt;
    STB_INT32    tmpConf;
    STB_INT32    topUID;
    STB_INT32    topCnt;
    STB_STATUS    preStatus    ;
    STB_INT32    preUID        ;
    STB_INT32    preConf        ;


    for( t = 0; t < STB_FR_BACK_MAX    ; t++) 
    { 
        recUID  [t] = STB_FR_INVALID_UID;
        recConf [t] = 0;
        accUID  [t] = STB_FR_INVALID_UID;
        accCnt  [t] = 0;
    }


    /*Checking the past data here, fill in all peRes.*/
    /*do stabilization processing each tracking person*/

    peRes->frCnt = peRec[0].num;
    for( k = 0; k < peRes->frCnt; k++)
    {
        /*Tracking person number in the through frame*/
        trID = peRec[0].fcDet[k].nTraID;

        // peRes Add    -------------------------------------------------------------------------------------------------
        peRes->frFace[k].nTraID = trID;


        //in case of unregistered album for present UID(no album files)
        if( peRec[0].fcDet[k].frDetID == STB_ERR_FR_NOALBUM )
        {
            peRes->frFace[k].frRecog.value  = STB_ERR_FR_NOALBUM    ;
            peRes->frFace[k].frRecog.status = STB_STATUS_NO_DATA    ;
            peRes->frFace[k].frRecog.conf    = STB_CONF_NO_DATA        ;
            peRec[0].fcDet[k].frDetID        = STB_ERR_FR_NOALBUM    ;
            peRec[0].fcDet[k].frStatus        = STB_STATUS_NO_DATA    ;
            peRec[0].fcDet[k].frDetConf        = STB_CONF_NO_DATA        ;
            continue;
        }

        // preStatus    -------------------------------------------------------------------------------------------------
        preStatus    = STB_STATUS_NO_DATA    ;
        preUID        = STB_FR_INVALID_UID    ;
        preConf        = 0    ;
        for( i = 0; i < peRec[1].num    ; i++) 
        { 
            if(    peRec[1].fcDet[i].nTraID ==    trID  )
            {
                preUID        = peRec[1].fcDet[i].frDetID        ;
                preStatus    = peRec[1].fcDet[i].frStatus    ;
                preConf        = peRec[1].fcDet[i].frDetConf    ;
                break;
            }
        }


        //    -------------------------------------------------------------------------------------------------
        //    -------------------------------------------------------------------------------------------------
        if            (        preStatus == STB_STATUS_NO_DATA        //stabilization impossible: no data of the relevant people 
                        ||    preStatus == STB_STATUS_CALCULATING    //during stabilization : a number of data for relevant people aren't enough(a number of frames that relevant people are taken)
            )
        {
            //    -------------------------------------------------------------------------------------------------
            //Setting "recUID" to past data of Tracking ID(trID) : (Up to "frmCnt")
            //    -------------------------------------------------------------------------------------------------
            recCnt = 0;
            for( t = 0; t < STB_FR_BACK_MAX    ; t++) //previous t frame
            { 
                for( i = 0; i < peRec[t].num    ; i++) //a number of tracking people(previous t frame)
                { 
                    if(    
                            peRec [ t ].fcDet[i].nTraID            ==  trID    //the same tracking number
                        &&    peRec [ t ].fcDet[i].nDetID            >=    0        //not lost
                        &&    peRec [ t ].fcDet[i].dirDetConf        >=    dirThr    // Face angle : confidence
                        &&    peRec [ t ].fcDet[i].dirDetPitch    >=    dirUDMin     // Face angle : pitch
                        &&    peRec [ t ].fcDet[i].dirDetPitch    <=    dirUDMax    // Face angle : pitch
                        &&    peRec [ t ].fcDet[i].dirDetYaw        >=    dirLRMin     // Face angle : yaw
                        &&    peRec [ t ].fcDet[i].dirDetYaw        <=    dirLRMax    // Face angle : yaw
                        &&    peRec [ t ].fcDet[i].frDetID        !=    STB_ERR_FR_CANNOT    //Recognition impossible
                        &&    peRec [ t ].fcDet[i].frDetID        !=    STB_ERR_FR_NOALBUM    // Not-registered in Album
                        )
                    {
                        recUID [ recCnt ] = peRec [ t ].fcDet[ i ].frDetID    ;
                        recConf[ recCnt ] = peRec [ t ].fcDet[ i ].frDetConf;
                        recCnt++;
                        break;
                    }
                }
                if( recCnt == frmCnt )
                {
                    break    ;//Maximum number is frmCnt
                }
            }
            //    -------------------------------------------------------------------------------------------------
            // tmpConf
            //    -------------------------------------------------------------------------------------------------
            tmpConf = 0;
            for( i = 0 ; i < recCnt ; i++)
            {
                tmpConf    += recConf[ i ];
            }
            if( recCnt > 0 )
            {
                tmpConf /= recCnt;
            }else
            {
                tmpConf = 0    ;
            }
            //    -------------------------------------------------------------------------------------------------
            //Create a cumulative frequency distribution of recUID and set it to accUID [accKind] accCnt [accKind].
            //AccCnt [i] pieces of data (in the past) determined to be "accUID [i]".
            //    -------------------------------------------------------------------------------------------------
            accKind = 0;
            for(;;)
            {
                tmpUID = STB_FR_INVALID_UID;
                for( i = 0 ; i < recCnt ; i++)
                {
                    if( recUID[ i ] != STB_FR_INVALID_UID )
                    {
                        tmpUID = recUID[ i ];
                        break;
                    }
                }
                if( tmpUID == STB_FR_INVALID_UID )
                {
                    break;
                }
                tmpCnt = 0;
                for( i = 0 ; i < recCnt ; i++)
                {
                    if( recUID[ i ] == tmpUID )
                    {
                        recUID[ i ] = STB_FR_INVALID_UID ;
                        tmpCnt++;
                    }
                }
                accUID[accKind] = tmpUID;
                accCnt[accKind] = tmpCnt;
                accKind++;
            }
            //    -------------------------------------------------------------------------------------------------
            //Find the ID whose frequency is the maximum from the cumulative frequency distribution and set it to topUID.
            //    -------------------------------------------------------------------------------------------------
            topUID    =    STB_FR_INVALID_UID    ;
            topCnt    =    0            ;
            for( i = 0 ; i < accKind ; i++)
            {
                if( topCnt < accCnt[i] )
                {
                    topCnt = accCnt[i] ;
                    topUID = accUID[i] ;
                }
            }
            if( topUID == STB_FR_INVALID_UID )
            {
                peRes->frFace[k].frRecog.value  = STB_STATUS_NO_DATA    ;
                peRes->frFace[k].frRecog.conf   = STB_CONF_NO_DATA        ;
                peRes->frFace[k].frRecog.status = STB_STATUS_NO_DATA;//during stabilization : a number of data for relevant people aren't enough(a number of frames that relevant people are taken)
                peRec[0].fcDet[k].frStatus        = STB_STATUS_NO_DATA;
            }else
            {
                if( topCnt < frmCnt * frmRatio / 100 )
                {
                    peRes->frFace[k].frRecog.value  = topUID                ;
                    peRes->frFace[k].frRecog.conf   = STB_CONF_NO_DATA        ;
                    peRes->frFace[k].frRecog.status = STB_STATUS_CALCULATING;//during stabilization : a number of data for relevant people aren't enough(a number of frames that relevant people are taken)
                    peRec[0].fcDet[k].frStatus        = STB_STATUS_CALCULATING;
                }else
                {
                    peRes->frFace[k].frRecog.value  = topUID                ;
                    peRes->frFace[k].frRecog.conf   = tmpConf               ;
                    peRes->frFace[k].frRecog.status = STB_STATUS_COMPLETE    ;//Just after stabilization : The state immediately after the number of data of the relevant person is sufficient and fixed. When creating an entry log, it is better to log data immediately after stabilization.
                    peRec[0].fcDet[k].frDetID        = topUID                ;
                    peRec[0].fcDet[k].frStatus        = STB_STATUS_COMPLETE    ;
                    peRec[0].fcDet[k].frDetConf        = tmpConf                ;
                }
            }
        }else if    (        preStatus == STB_STATUS_COMPLETE        //Just after stabilization
                        ||    preStatus == STB_STATUS_FIXED            //already stabilized
                    )    
        {
            peRes->frFace[k].frRecog.value  = preUID            ;
            peRes->frFace[k].frRecog.status = STB_STATUS_FIXED    ;
            peRes->frFace[k].frRecog.conf    = preConf;
            peRec[0].fcDet[k].frDetID        = preUID            ;
            peRec[0].fcDet[k].frStatus        = STB_STATUS_FIXED    ;
            peRec[0].fcDet[k].frDetConf        = preConf;
            
        }


    }
}
/*---------------------------------------------------------------------*/
//    StbFrExec
/*---------------------------------------------------------------------*/
int  StbFrExec ( FRHANDLE handle )
{

    int retVal = 0 ;

    /* Face --------------------------------------*/
    FrSlideFacesRec    (   handle->frDetRec        );//Shift the time series of past data before stabilization.
    FrCurFaces        (   handle->frDetRec    ,
                      &(handle->frDet)            );//Setting "present data before the stabilization" to past data before the stabilization.

    FrStbFaceEasy        ( &(handle->frRes)            ,      
                        handle->frDetRec        ,
                        handle->frFaceDirThr    ,
                        handle->frFaceDirUDMax    ,
                        handle->frFaceDirUDMin    ,
                        handle->frFaceDirLRMax    ,
                        handle->frFaceDirLRMin    ,
                        handle->frFrameCount    ,
                        handle->frFrameRatio    );//Calculate "current data after stabilization" from "past data before stabilization".



    return retVal;
}



