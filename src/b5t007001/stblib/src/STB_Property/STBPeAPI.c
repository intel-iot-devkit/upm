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

#include "STBPeAPI.h"


/*---------------------------------------------------------------------*/
//    PeSlideFacesRec
/*---------------------------------------------------------------------*/
void PeSlideFacesRec ( STB_PE_DET *facesRec ,        STBExecFlg    *execFlg)
{
    STB_INT32 t , i ,j;

    for( t = STB_PE_BACK_MAX -2 ; t >= 0 ;  t-- ) 
    {
        facesRec [ t + 1 ].num = facesRec[ t + 0 ].num;
        for( i = 0 ; i < facesRec [ t + 1 ].num ;  i++ ) 
        {
            facesRec[ t + 1 ].fcDet[i].nDetID        = facesRec[ t ].fcDet[i].nDetID        ;
            facesRec[ t + 1 ].fcDet[i].nTraID        = facesRec[ t ].fcDet[i].nTraID        ;
            if( execFlg->gen == STB_TRUE )
            {
                facesRec[ t + 1 ].fcDet[i].genDetVal    = facesRec[ t ].fcDet[i].genDetVal    ;
                facesRec[ t + 1 ].fcDet[i].genStatus    = facesRec[ t ].fcDet[i].genStatus    ;
                facesRec[ t + 1 ].fcDet[i].genDetConf    = facesRec[ t ].fcDet[i].genDetConf    ;
            }
            if( execFlg->age == STB_TRUE )
            {
                facesRec[ t + 1 ].fcDet[i].ageDetVal    = facesRec[ t ].fcDet[i].ageDetVal    ;
                facesRec[ t + 1 ].fcDet[i].ageStatus    = facesRec[ t ].fcDet[i].ageStatus    ;
                facesRec[ t + 1 ].fcDet[i].ageDetConf    = facesRec[ t ].fcDet[i].ageDetConf    ;
            }
            if( execFlg->exp == STB_TRUE )
            {
                facesRec[ t + 1 ].fcDet[i].expDetConf    = facesRec[ t ].fcDet[i].expDetConf    ;
                for( j = 0 ; j < STB_EX_MAX ; j++)
                {
                        facesRec[ t + 1 ].fcDet[ i ].expDetVal[ j ]    
                    = facesRec[ t + 0 ].fcDet[ i ].expDetVal[ j ];
                }
            }
            if( execFlg->gaz == STB_TRUE )
            {
                facesRec[ t + 1 ].fcDet[i].gazDetLR        = facesRec[ t ].fcDet[i].gazDetLR    ;
                facesRec[ t + 1 ].fcDet[i].gazDetUD        = facesRec[ t ].fcDet[i].gazDetUD    ;
            }
            //if( execFlg->dir == STB_TRUE )
            //{
                facesRec[ t + 1 ].fcDet[i].dirDetRoll    = facesRec[ t ].fcDet[i].dirDetRoll    ;
                facesRec[ t + 1 ].fcDet[i].dirDetPitch    = facesRec[ t ].fcDet[i].dirDetPitch;
                facesRec[ t + 1 ].fcDet[i].dirDetYaw    = facesRec[ t ].fcDet[i].dirDetYaw    ;
                facesRec[ t + 1 ].fcDet[i].dirDetConf    = facesRec[ t ].fcDet[i].dirDetConf    ;
            //}
            if( execFlg->bli == STB_TRUE )
            {
                facesRec[ t + 1 ].fcDet[i].bliDetL        = facesRec[ t ].fcDet[i].bliDetL    ;
                facesRec[ t + 1 ].fcDet[i].bliDetR        = facesRec[ t ].fcDet[i].bliDetR    ;
            }
        }
    }

}
/*---------------------------------------------------------------------*/
//    PeCurFaces
/*---------------------------------------------------------------------*/
void PeCurFaces ( STB_PE_DET *facesRec , STB_PE_DET *srcFace ,STBExecFlg    *execFlg)
{
    STB_INT32  i ,j;


    facesRec [ 0 ].num = srcFace->num;
    for( i = 0 ; i < facesRec [ 0 ].num ;  i++ ) 
    {
        facesRec[ 0 ].fcDet[ i ].nDetID            = srcFace[ 0 ].fcDet[ i ].nDetID    ;
        facesRec[ 0 ].fcDet[ i ].nTraID            = srcFace[ 0 ].fcDet[ i ].nTraID    ;
        if( execFlg->gen == STB_TRUE )
        {
            facesRec[ 0 ].fcDet[ i ].genDetVal        = srcFace[ 0 ].fcDet[ i ].genDetVal    ;
            facesRec[ 0 ].fcDet[ i ].genStatus        = STB_STATUS_NO_DATA                ;
            facesRec[ 0 ].fcDet[ i ].genDetConf        = srcFace[ 0 ].fcDet[ i ].genDetConf;
        }
        if( execFlg->age == STB_TRUE )
        {
            facesRec[ 0 ].fcDet[ i ].ageDetVal        = srcFace[ 0 ].fcDet[ i ].ageDetVal    ;
            facesRec[ 0 ].fcDet[ i ].ageStatus        = STB_STATUS_NO_DATA                ;
            facesRec[ 0 ].fcDet[ i ].ageDetConf        = srcFace[ 0 ].fcDet[ i ].ageDetConf;
        }
        if( execFlg->exp == STB_TRUE )
        {
            facesRec[ 0 ].fcDet[ i ].expDetConf        = srcFace[ 0 ].fcDet[ i ].expDetConf    ;
            for( j = 0 ; j < STB_EX_MAX ; j++)
            {
                facesRec[ 0 ].fcDet[ i ].expDetVal[ j]    = srcFace[ 0 ].fcDet[ i ].expDetVal[ j];
            }
        }
        if( execFlg->gaz == STB_TRUE )
        {
            facesRec[ 0 ].fcDet[ i ].gazDetLR        = srcFace[ 0 ].fcDet[ i ].gazDetLR    ;
            facesRec[ 0 ].fcDet[ i ].gazDetUD        = srcFace[ 0 ].fcDet[ i ].gazDetUD    ;
        }
        //if( execFlg->dir == STB_TRUE )
        //{
            facesRec[ 0 ].fcDet[ i ].dirDetRoll        = srcFace[ 0 ].fcDet[ i ].dirDetRoll    ;
            facesRec[ 0 ].fcDet[ i ].dirDetYaw        = srcFace[ 0 ].fcDet[ i ].dirDetYaw    ;
            facesRec[ 0 ].fcDet[ i ].dirDetPitch    = srcFace[ 0 ].fcDet[ i ].dirDetPitch    ;
            facesRec[ 0 ].fcDet[ i ].dirDetConf        = srcFace[ 0 ].fcDet[ i ].dirDetConf    ;
        //}
        if( execFlg->bli == STB_TRUE )
        {
            facesRec[ 0 ].fcDet[ i ].bliDetL        = srcFace[ 0 ].fcDet[ i ].bliDetL        ;
            facesRec[ 0 ].fcDet[ i ].bliDetR        = srcFace[ 0 ].fcDet[ i ].bliDetR        ;
        }
    }
}

/*----------------------------------------------------------------------------------------------------*/
/* PeExpressID */
/*----------------------------------------------------------------------------------------------------*/
STB_INT32 PeExpressID( STB_INT32* exp )
{
    int i;
    int tmpVal;
    int retVal;

    retVal = 0;
    tmpVal = 0;
    for( i = 0 ; i < STB_EX_MAX ; i++)
    {
        if( tmpVal < exp[i] && exp[i] != STB_ERR_PE_CANNOT )
        {
            tmpVal = exp[i];
            retVal = i;
        }
    }
    return retVal;
}
/*---------------------------------------------------------------------*/
//    PeStbFaceEasy
/*---------------------------------------------------------------------*/
void PeStbFaceEasy
    (
        STB_PE_RES *peRes        , 
        STB_PE_DET *peRec        , 
        STB_INT32    dirThr        , 
        STB_INT32    dirUDMax    , 
        STB_INT32    dirUDMin    ,
        STB_INT32    dirLRMax    , 
        STB_INT32    dirLRMin    ,
        STB_INT32    frmMax        ,
        STBExecFlg    *execFlg
    )
{
    

    /*Checking the past data here, fill in all peRes.*/
    STB_INT32    k ,t,i ;
    STB_INT32    trID;

    STB_INT32    recCnt;
    STB_INT32    recVal    [STB_PE_BACK_MAX];
    STB_INT32    recConf    [STB_PE_BACK_MAX];
    STB_INT32    tmpVal;
    STB_INT32    tmpConf;
    STB_INT32    expVal    [STB_EX_MAX] = {0};
    
    STB_STATUS    preSAge    ;
    STB_STATUS    preSGen    ;
    STB_INT32    preVAge    ;
    STB_INT32    preVGen    ;
    STB_INT32    preCAge    ;
    STB_INT32    preCGen    ;
    STB_STATUS    tmpS    ;



    for( t = 0; t < STB_PE_BACK_MAX    ; t++) 
    { 
        recVal [t] = 0;
        recConf[t] = 0;
    }
    

    /*do stabilization processing each tracking person*/
    peRes->peCnt = peRec[0].num    ;//a number of tracking people(present)
    for( k = 0; k < peRes->peCnt ; k++) 
    {
        trID = peRec[0].fcDet[k].nTraID;/*Tracking person number in the through frame*/


        // peRes Add    -------------------------------------------------------------------------------------------------
        peRes->peFace[k].nTraID = trID;


        // preStatus    -------------------------------------------------------------------------------------------------
        preSAge    = STB_STATUS_NO_DATA    ;
        preSGen    = STB_STATUS_NO_DATA    ;
        preVAge    = 0    ;
        preVGen = 0    ;
        preCAge    = 0    ;
        preCGen = 0    ;
        if( execFlg->age == STB_TRUE  || execFlg->gen == STB_TRUE  )
        {
            for( i = 0; i < peRec[1].num    ; i++) 
            { 
                if(    peRec[1].fcDet[i].nTraID ==    trID  )
                {
                    preSAge    = peRec[1].fcDet[i].ageStatus    ;
                    preSGen    = peRec[1].fcDet[i].genStatus    ;
                    preVAge    = peRec[1].fcDet[i].ageDetVal    ;
                    preVGen    = peRec[1].fcDet[i].genDetVal    ;
                    preCAge    = peRec[1].fcDet[i].ageDetConf    ;
                    preCGen    = peRec[1].fcDet[i].genDetConf    ;
                    break;
                }
            }
        }

        // age            -------------------------------------------------------------------------------------------------
        if( execFlg->age == STB_TRUE )
        {
            if            (        preSAge == STB_STATUS_NO_DATA        //stabilization impossible: no data of the relevant people 
                            ||    preSAge == STB_STATUS_CALCULATING    //during stabilization : a number of data for relevant people aren't enough(a number of frames that relevant people are taken)
                )
            {
                recCnt = 0;
                for( t = 0; t < STB_PE_BACK_MAX    ; t++) //previous t frame
                { 
                for( i = 0; i < peRec[t].num    ; i++) //a number of tracking people(previous t frame)
                { 
                    if(    
                            peRec[t].fcDet[i].nTraID        ==  trID        //the same tracking number
                        &&    peRec[t].fcDet[i].nDetID        >=    0            //not lost
                        &&    peRec[t].fcDet[i].dirDetConf    >=    dirThr        // Face angle : confidence
                        &&    peRec[t].fcDet[i].dirDetPitch    >=    dirUDMin     // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetPitch    <=    dirUDMax    // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetYaw        >=    dirLRMin     // Face angle : yaw
                        &&    peRec[t].fcDet[i].dirDetYaw        <=    dirLRMax    // Face angle : yaw
                        &&    peRec[t].fcDet[i].ageDetVal        !=    STB_ERR_PE_CANNOT    // 
                        &&    peRec[t].fcDet[i].ageDetConf    !=    STB_ERR_PE_CANNOT    // 
                        )
                    {
                        recVal [ recCnt ] = peRec[ t ].fcDet[ i ].ageDetVal        ;
                        recConf[ recCnt ] = peRec[ t ].fcDet[ i ].ageDetConf    ;
                        recCnt++;
                    }
                }
                }
                tmpS    = STB_STATUS_NO_DATA;
                if        ( recCnt == 0            )        {    tmpS    = STB_STATUS_NO_DATA    ;    }//stabilization impossible
                else if    ( recCnt <  frmMax      )        {    tmpS    = STB_STATUS_CALCULATING;    }//during stabilization
                else if    ( recCnt >= frmMax      )        {    tmpS    = STB_STATUS_COMPLETE    ;    }//Just after stabilization
                tmpVal    = 0;
                tmpConf = 0;
                for( i = 0; i < recCnt    ; i++    )        
                {     
                    tmpVal    += recVal [ i ]            ;    
                    tmpConf    += recConf[ i ]            ;    
                }
                if ( recCnt == 0                )        {    recCnt    =  1                    ;    }
                tmpVal    /= recCnt;
                tmpConf /= recCnt;
                peRes->peFace[k].age.value            = tmpVal            ;
                peRes->peFace[k].age.conf            = STB_CONF_NO_DATA    ;
                peRes->peFace[k].age.status            = tmpS                ;
                peRec[0].fcDet[k].ageStatus            = tmpS                ;
                if( tmpS    == STB_STATUS_COMPLETE    )//Just after stabilization
                {
                    peRec[0].fcDet[k].ageDetVal        = tmpVal        ;
                    peRec[0].fcDet[k].ageDetConf    = tmpConf        ;
                }
            }else if    (        preSAge == STB_STATUS_COMPLETE        //Just after stabilization
                            ||    preSAge == STB_STATUS_FIXED            //already stabilized
                        )    
            {
                peRes->peFace[k].age.value        = preVAge            ;
                peRes->peFace[k].age.conf        = preCAge;
                peRes->peFace[k].age.status        = STB_STATUS_FIXED    ;//already stabilized
                peRec[0].fcDet[k].ageDetVal        = preVAge            ;
                peRec[0].fcDet[k].ageDetConf    = preCAge            ;
                peRec[0].fcDet[k].ageStatus        = STB_STATUS_FIXED    ;//already stabilized
            }
        }


        // gender        -------------------------------------------------------------------------------------------------
        if( execFlg->gen == STB_TRUE )
        {
            if            (        preSGen == STB_STATUS_NO_DATA        //stabilization impossible: no data of the relevant people 
                            ||    preSGen == STB_STATUS_CALCULATING    //during stabilization : a number of data for relevant people aren't enough(a number of frames that relevant people are taken)
                )
            {
                recCnt = 0;
                for( t = 0; t < STB_PE_BACK_MAX    ; t++) 
                { 
                for( i = 0; i < peRec[t].num    ; i++) //a number of tracking people(previous t frame)
                { 
                    if(    
                            peRec[t].fcDet[i].nTraID        ==  trID        //the same tracking number
                        &&    peRec[t].fcDet[i].nDetID        >=    0            //not lost
                        &&    peRec[t].fcDet[i].dirDetConf    >=    dirThr        // Face angle : confidence
                        &&    peRec[t].fcDet[i].dirDetPitch    >=    dirUDMin     // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetPitch    <=    dirUDMax    // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetYaw        >=    dirLRMin     // Face angle : yaw
                        &&    peRec[t].fcDet[i].dirDetYaw        <=    dirLRMax    // Face angle : yaw
                        &&    peRec[t].fcDet[i].genDetVal        !=    STB_ERR_PE_CANNOT    // 
                        &&    peRec[t].fcDet[i].genDetConf    !=    STB_ERR_PE_CANNOT    // 
                        )
                    {
                        recVal [ recCnt ] = peRec[ t ].fcDet[ i ].genDetVal ;// 1:man 0:woman
                        recConf[ recCnt ] = peRec[ t ].fcDet[ i ].genDetConf;
                        recCnt++;
                    }
                }
                }
                tmpS    = STB_STATUS_NO_DATA;
                if        ( recCnt == 0            )        {    tmpS    = STB_STATUS_NO_DATA    ;    }//stabilization impossible
                else if    ( recCnt <  frmMax      )        {    tmpS    = STB_STATUS_CALCULATING;    }//during stabilization
                else if    ( recCnt >= frmMax      )        {    tmpS    = STB_STATUS_COMPLETE    ;    }//Just after stabilization
                tmpVal  = 0;
                tmpConf = 0;
                for( i = 0; i < recCnt    ; i++    )        
                {     
                    tmpVal  += recVal [ i ]                ;
                    tmpConf += recConf[ i ]                ;    
                }
                if ( recCnt == 0                )        {    recCnt    =  1                        ;        }
                tmpConf /= recCnt;
                if        ( tmpVal * 2 <= recCnt    )    
                {    
                    peRes->peFace[k].gen.value    = 0                    ;// 1:man 0:woman    
                    peRes->peFace[k].gen.status    = tmpS                ;
                    peRes->peFace[k].gen.conf    = STB_CONF_NO_DATA    ;
                    peRec[0].fcDet[k].genStatus = tmpS                ;
                    if( tmpS    == STB_STATUS_COMPLETE    )//Just after stabilization
                    {
                        peRec[0].fcDet[k].genDetVal        = 0            ;
                        peRec[0].fcDet[k].genDetConf    = tmpConf    ;
                    }
                }
                else
                {
                    peRes->peFace[k].gen.value    = 1                    ;// 1:man 0:woman    
                    peRes->peFace[k].gen.status    = tmpS                ;
                    peRes->peFace[k].gen.conf    = STB_CONF_NO_DATA    ;
                    peRec[0].fcDet[k].genStatus = tmpS                ;
                    if( tmpS    == STB_STATUS_COMPLETE    )//Just after stabilization
                    {
                        peRec[0].fcDet[k].genDetVal        = 1            ;
                        peRec[0].fcDet[k].genDetConf    = tmpConf    ;
                    }    
                }
            }else if    (        preSGen == STB_STATUS_COMPLETE        //Just after stabilization
                            ||    preSGen == STB_STATUS_FIXED        //already stabilized
                        )    
            {
                peRes->peFace[k].gen.value        = preVGen            ;
                peRes->peFace[k].gen.conf        = preCGen;
                peRes->peFace[k].gen.status        = STB_STATUS_FIXED    ;//already stabilized
                peRec[0].fcDet[k].genDetVal        = preVGen            ;
                peRec[0].fcDet[k].genStatus        = STB_STATUS_FIXED    ;//already stabilized
                peRec[0].fcDet[k].genDetConf    = preCGen            ;
            }
        }

        // gazeLR                -------------------------------------------------------------------------------------------------
        if( execFlg->gaz == STB_TRUE )
        {
            recCnt = 0;
            for( t = 0; t < STB_PE_BACK_MAX    ; t++) 
            { 
            for( i = 0; i < peRec[t].num    ; i++) //a number of tracking people(previous t frame)
            { 
                if(    
                            peRec[t].fcDet[i].nTraID        ==  trID        //the same tracking number
                        &&    peRec[t].fcDet[i].nDetID        >=    0            //not lost
                        &&    peRec[t].fcDet[i].dirDetConf    >=    dirThr        // Face angle : confidence
                        &&    peRec[t].fcDet[i].dirDetPitch    >=    dirUDMin     // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetPitch    <=    dirUDMax    // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetYaw        >=    dirLRMin     // Face angle : yaw
                        &&    peRec[t].fcDet[i].dirDetYaw        <=    dirLRMax    // Face angle : yaw
                        &&    peRec[t].fcDet[i].gazDetLR        !=    STB_ERR_PE_CANNOT    // 
                    )
                {
                    recVal[ recCnt ] = peRec[ t ].fcDet [ i ].gazDetLR    ;
                    recCnt++;
                }
            }
            }
            if        ( recCnt     == 0                )        {    peRes->peFace[k].gaz.status    = STB_STATUS_NO_DATA    ;        }//stabilization impossible
            else                                            {    peRes->peFace[k].gaz.status    = STB_STATUS_CALCULATING;        }//during stabilization
            peRes->peFace[k].gaz.conf    =  STB_CONF_NO_DATA;//no Confidence
            tmpVal = 0;
            for( i = 0; i < recCnt    ; i++            )        {     tmpVal        += recVal[ i ]    ;    }
            if ( recCnt == 0                        )        {    recCnt        =  1            ;    }
            peRes->peFace[k].gaz.LR    = tmpVal / recCnt;
            // gazeUD                -------------------------------------------------------------------------------------------------
            recCnt = 0;
            for( t = 0; t < STB_PE_BACK_MAX    ; t++) 
            { 
            for( i = 0; i < peRec[t].num    ; i++) //a number of tracking people(previous t frame)
            { 
                if(    
                            peRec[t].fcDet[i].nTraID        ==  trID        //the same tracking number
                        &&    peRec[t].fcDet[i].nDetID        >=    0            //not lost
                        &&    peRec[t].fcDet[i].dirDetConf    >=    dirThr        // Face angle : confidence
                        &&    peRec[t].fcDet[i].dirDetPitch    >=    dirUDMin     // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetPitch    <=    dirUDMax    // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetYaw        >=    dirLRMin     // Face angle : yaw
                        &&    peRec[t].fcDet[i].dirDetYaw        <=    dirLRMax    // Face angle : yaw
                        &&    peRec[t].fcDet[i].gazDetUD        !=    STB_ERR_PE_CANNOT    // 
                    )
                {
                    recVal[ recCnt ] = peRec[ t ].fcDet [ i ].gazDetUD    ;
                    recCnt++;
                }
            }
            }
            tmpVal = 0;
            for( i = 0; i < recCnt    ; i++            )        {     tmpVal        += recVal[ i ]    ;    }
            if ( recCnt == 0                        )        {    recCnt        =  1            ;    }
            peRes->peFace[k].gaz.UD    = tmpVal / recCnt;
        }


        // expression            -------------------------------------------------------------------------------------------------
        if( execFlg->exp == STB_TRUE )
        {
            recCnt = 0;
            for( t = 0; t < STB_PE_BACK_MAX    ; t++) 
            { 
            for( i = 0; i < peRec[t].num    ; i++) //a number of tracking people(previous t frame)
            { 
                if(    
                            peRec[t].fcDet[i].nTraID        ==  trID        //the same tracking number
                        &&    peRec[t].fcDet[i].nDetID        >=    0            //not lost
                        &&    peRec[t].fcDet[i].dirDetConf    >=    dirThr        // Face angle : confidence
                        &&    peRec[t].fcDet[i].dirDetPitch    >=    dirUDMin     // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetPitch    <=    dirUDMax    // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetYaw        >=    dirLRMin     // Face angle : yaw
                        &&    peRec[t].fcDet[i].dirDetYaw        <=    dirLRMax    // Face angle : yaw
                        &&    peRec[t].fcDet[i].expDetConf    !=    STB_ERR_PE_CANNOT    // 
                    )
                {
                    recVal[ recCnt ] = PeExpressID (  peRec[ t ].fcDet[i].expDetVal );
                    recCnt++;
                }
            }
            }
            for( i = 0; i < STB_EX_MAX; i++            )        {     expVal[ i ]                =  0        ;    }
            for( i = 0; i < recCnt    ; i++            )        {     expVal[ recVal[ i ] ]    += 1        ;    }
            peRes->peFace[k].exp.value    =  PeExpressID ( expVal );
            peRes->peFace[k].exp.conf    =  STB_CONF_NO_DATA;//no Confidence
            if        ( recCnt     == 0                )        {    peRes->peFace[k].exp.status    = STB_STATUS_NO_DATA    ;        }//stabilization impossible
            else                                            {    peRes->peFace[k].exp.status    = STB_STATUS_CALCULATING;        }//during stabilization
        }

        // blink L                -------------------------------------------------------------------------------------------------
        if( execFlg->bli == STB_TRUE )
        {
            recCnt = 0;
            for( t = 0; t < STB_PE_BACK_MAX    ; t++) 
            { 
            for( i = 0; i < peRec[t].num    ; i++) //a number of tracking people(previous t frame)
            { 
                if(    
                            peRec[t].fcDet[i].nTraID        ==  trID        //the same tracking number
                        &&    peRec[t].fcDet[i].nDetID        >=    0            //not lost
                        &&    peRec[t].fcDet[i].dirDetConf    >=    dirThr        // Face angle : confidence
                        &&    peRec[t].fcDet[i].dirDetPitch    >=    dirUDMin     // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetPitch    <=    dirUDMax    // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetYaw        >=    dirLRMin     // Face angle : yaw
                        &&    peRec[t].fcDet[i].dirDetYaw        <=    dirLRMax    // Face angle : yaw
                        &&    peRec[t].fcDet[i].bliDetL        !=    STB_ERR_PE_CANNOT    //
                    )
                {
                    recVal[ recCnt ] = peRec[ t ].fcDet[ i ].bliDetL    ;
                    recCnt++;
                }
            }
            }
            if        ( recCnt     == 0                )        {    peRes->peFace[k].bli.status    = STB_STATUS_NO_DATA    ;        }//stabilization impossible
            else                                            {    peRes->peFace[k].bli.status    = STB_STATUS_CALCULATING;        }//during stabilization
            tmpVal = 0;
            for( i = 0; i < recCnt    ; i++            )        {     tmpVal        += recVal[ i ]    ;    }
            if ( recCnt == 0                        )        {    recCnt        =  1            ;    }
            peRes->peFace[k].bli.ratioL    = tmpVal / recCnt;
            // blink R                -------------------------------------------------------------------------------------------------
            recCnt = 0;
            for( t = 0; t < STB_PE_BACK_MAX    ; t++) 
            { 
            for( i = 0; i < peRec[t].num    ; i++) //a number of tracking people(previous t frame)
            { 
                if(    
                            peRec[t].fcDet[i].nTraID        ==  trID        //the same tracking number
                        &&    peRec[t].fcDet[i].nDetID        >=    0            //not lost
                        &&    peRec[t].fcDet[i].dirDetConf    >=    dirThr        // Face angle : confidence
                        &&    peRec[t].fcDet[i].dirDetPitch    >=    dirUDMin     // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetPitch    <=    dirUDMax    // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetYaw        >=    dirLRMin     // Face angle : yaw
                        &&    peRec[t].fcDet[i].dirDetYaw        <=    dirLRMax    // Face angle : yaw
                        &&    peRec[t].fcDet[i].bliDetR        !=    STB_ERR_PE_CANNOT    //
                    )
                {
                    recVal[ recCnt ] = peRec[ t ].fcDet [ i ].bliDetR;
                    recCnt++;
                }
            }
            }
            tmpVal = 0;
            for( i = 0; i < recCnt    ; i++            )        {     tmpVal        += recVal[ i ]    ;    }
            if ( recCnt == 0                        )        {    recCnt        =  1            ;    }
            peRes->peFace[k].bli.ratioR    = tmpVal / recCnt;
        }



        // dirYaw                -------------------------------------------------------------------------------------------------
        if( execFlg->dir == STB_TRUE )
        {
            recCnt = 0;
            for( t = 0; t < STB_PE_BACK_MAX    ; t++) 
            { 
            for( i = 0; i < peRec[t].num    ; i++) //a number of tracking people(previous t frame)
            { 
                if(    
                            peRec[t].fcDet[i].nTraID        ==  trID        //the same tracking number
                        &&    peRec[t].fcDet[i].nDetID        >=    0            //not lost
                        &&    peRec[t].fcDet[i].dirDetConf    >=    dirThr        // Face angle : confidence
                        &&    peRec[t].fcDet[i].dirDetPitch    >=    dirUDMin     // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetPitch    <=    dirUDMax    // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetYaw        >=    dirLRMin     // Face angle : yaw
                        &&    peRec[t].fcDet[i].dirDetYaw        <=    dirLRMax    // Face angle : yaw
                    )
                {
                    recVal[ recCnt ] = peRec[ t ].fcDet [ i ].dirDetYaw    ;
                    recCnt++;
                }
            }
            }
            if        ( recCnt     == 0                )        {    peRes->peFace[k].dir.status    = STB_STATUS_NO_DATA    ;        }//stabilization impossible
            else                                            {    peRes->peFace[k].dir.status    = STB_STATUS_CALCULATING;        }//during stabilization
            peRes->peFace[k].dir.conf    =  STB_CONF_NO_DATA;//no Confidence
            tmpVal = 0;
            for( i = 0; i < recCnt    ; i++            )        {     tmpVal        += recVal[ i ]    ;    }
            if ( recCnt == 0                        )        {    recCnt        =  1            ;    }
            peRes->peFace[k].dir.yaw    = tmpVal / recCnt;
            // dirRoll                -------------------------------------------------------------------------------------------------
            recCnt = 0;
            for( t = 0; t < STB_PE_BACK_MAX    ; t++) 
            { 
            for( i = 0; i < peRec[t].num    ; i++) //a number of tracking people(previous t frame)
            { 
                if(    
                            peRec[t].fcDet[i].nTraID        ==  trID        //the same tracking number
                        &&    peRec[t].fcDet[i].nDetID        >=    0            //not lost
                        &&    peRec[t].fcDet[i].dirDetConf    >=    dirThr        // Face angle : confidence
                        &&    peRec[t].fcDet[i].dirDetPitch    >=    dirUDMin     // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetPitch    <=    dirUDMax    // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetYaw        >=    dirLRMin     // Face angle : yaw
                        &&    peRec[t].fcDet[i].dirDetYaw        <=    dirLRMax    // Face angle : yaw
                    )
                {
                    recVal[ recCnt ] = peRec[ t ].fcDet [ i ].dirDetRoll;
                    recCnt++;
                }
            }
            }
            tmpVal = 0;
            for( i = 0; i < recCnt    ; i++            )        {     tmpVal        += recVal[ i ]    ;    }
            if ( recCnt == 0                        )        {    recCnt        =  1            ;    }
            peRes->peFace[k].dir.roll    = tmpVal / recCnt;
            // dirPitch                -------------------------------------------------------------------------------------------------
            recCnt = 0;
            for( t = 0; t < STB_PE_BACK_MAX    ; t++) 
            { 
            for( i = 0; i < peRec[t].num    ; i++) //a number of tracking people(previous t frame)
            { 
                if(    
                            peRec[t].fcDet[i].nTraID        ==  trID        //the same tracking number
                        &&    peRec[t].fcDet[i].nDetID        >=    0            //not lost
                        &&    peRec[t].fcDet[i].dirDetConf    >=    dirThr        // Face angle : confidence
                        &&    peRec[t].fcDet[i].dirDetPitch    >=    dirUDMin     // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetPitch    <=    dirUDMax    // Face angle : pitch
                        &&    peRec[t].fcDet[i].dirDetYaw        >=    dirLRMin     // Face angle : yaw
                        &&    peRec[t].fcDet[i].dirDetYaw        <=    dirLRMax    // Face angle : yaw
                    )
                {
                    recVal[ recCnt ] = peRec[ t ].fcDet [ i ].dirDetPitch    ;
                    recCnt++;
                }
            }
            }
            tmpVal = 0;
            for( i = 0; i < recCnt    ; i++            )        {     tmpVal        += recVal[ i ]    ;    }
            if ( recCnt == 0                        )        {    recCnt        =  1            ;    }
            peRes->peFace[k].dir.pitch    = tmpVal / recCnt;
        }

    }//for( k = 0; k < peRes->peCnt ; k++) 

}
/*---------------------------------------------------------------------*/
//    StbPeExec
/*---------------------------------------------------------------------*/
int  StbPeExec ( PEHANDLE handle )
{

    int retVal = 0 ;

    /* Face --------------------------------------*/
    PeSlideFacesRec    (     handle->peDetRec        ,
                          handle->execFlg            );//Shift the time series of past data before stabilization.
    PeCurFaces        (     handle->peDetRec        ,
                        &(handle->peDet)        ,    
                          handle->execFlg            );//Setting "present data before the stabilization" to past data before the stabilization.

    PeStbFaceEasy        (   &(handle->peRes)        ,      
                        handle->peDetRec        ,
                        handle->peFaceDirThr    ,
                        handle->peFaceDirUDMax    ,
                        handle->peFaceDirUDMin    ,
                        handle->peFaceDirLRMax    ,
                        handle->peFaceDirLRMin    ,
                        handle->peFrameCount    ,
                        handle->execFlg            );//Calculate "current data after stabilization" from "past data before stabilization".




    return retVal;
}



