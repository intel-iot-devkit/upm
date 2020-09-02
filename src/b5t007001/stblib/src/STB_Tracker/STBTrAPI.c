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

#include "STBTrAPI.h"

#define STB_INT_MAX       2147483647    /* maximum (signed) int value */

/*---------------------------------------------------------------------*/
//    TrSlideRec
/*---------------------------------------------------------------------*/
void TrSlideRec ( ROI_SYS *rec )
{
    STB_INT32 t , i ;

    for( t = STB_TR_BACK_MAX  - 2 ; t >= 0 ;  t-- ) 
    {
        rec [ t + 1 ].cnt = rec[ t + 0 ].cnt;
        for( i = 0 ; i < rec [ t + 1 ].cnt ;  i++ ) 
        {
            rec [ t + 1 ].nDetID    [i]    = rec [ t ].nDetID    [i] ;
            rec [ t + 1 ].nTraID    [i]    = rec [ t ].nTraID    [i] ;
            rec [ t + 1 ].posX        [i]    = rec [ t ].posX    [i] ;
            rec [ t + 1 ].posY        [i]    = rec [ t ].posY    [i] ;
            rec [ t + 1 ].size        [i]    = rec [ t ].size    [i]  ;
            rec [ t + 1 ].conf        [i]    = rec [ t ].conf    [i]  ;
            rec [ t + 1 ].retryN    [i]    = rec [ t ].retryN    [i]  ;
        }
    }
}
/*---------------------------------------------------------------------*/
//    TrCurRec
/*---------------------------------------------------------------------*/
void TrCurRec ( ROI_SYS *rec , ROI_DET *det , STB_INT32 num)
{
    STB_INT32  i ;


    rec [ 0 ].cnt =num;
    for( i = 0 ; i < rec [ 0 ].cnt ;  i++ ) 
    {
        rec [ 0 ].nDetID    [i]    = i                    ;
        rec [ 0 ].nTraID    [i]    = -1                ;
        rec [ 0 ].posX        [i]    = det[i].posX        ;
        rec [ 0 ].posY        [i]    = det[i].posY        ;
        rec [ 0 ].size        [i]    = det[i].size        ;
        rec [ 0 ].conf        [i]    = det[i].conf        ;
        rec [ 0 ].retryN    [i]    = 0                    ;
    }

}


/*---------------------------------------------------------------------*/
//    TrDelRetry
/*---------------------------------------------------------------------*/
void
TrDelRetry( ROI_SYS *preData , STB_INT32 thrRetryCnt )
{
    //delete data exceeding the number of retries
    //If the face isn't find out during tracking, set until how many frames can look for it.
    //If tracking fails for the specified number of consecutive frames, tracking is terminated assuming that face is lost.
    STB_INT32    i,  tmpCnt      ;

    tmpCnt        = 0;
    for( i = 0 ; i < preData->cnt ;  i++ ) 
    {
        if( preData->retryN[i] <=  thrRetryCnt )
        {

            preData->nDetID        [tmpCnt    ]    = preData->nDetID    [i] ; 
            preData->nTraID        [tmpCnt    ]    = preData->nTraID    [i]    ;
            preData->posX        [tmpCnt    ]    = preData->posX        [i]    ;
            preData->posY        [tmpCnt    ]    = preData->posY        [i]    ; 
            preData->size        [tmpCnt    ]    = preData->size        [i]    ;
            preData->conf        [tmpCnt    ]    = preData->conf        [i]    ;
            preData->retryN        [tmpCnt    ]    = preData->retryN    [i]    ;
            tmpCnt++;
        }
    }
    preData->cnt    = tmpCnt    ;


}
/*---------------------------------------------------------------------*/
//    TrCheckSameROI
/*---------------------------------------------------------------------*/
STB_INT32
TrCheckSameROI(    STB_INT32 curX        ,STB_INT32 curY        ,STB_INT32 curS        ,
                STB_INT32 preX        ,STB_INT32 preY        ,STB_INT32 preS        
                )
{

    STB_INT32 difP    ;//the percentage of detection position change
    STB_INT32 difS    ;//the percentage of detection size change
    float tmpVal;
    STB_INT32 retVal;

    if( preS < 1 )
    {
        return STB_INT_MAX;
    }

    //the percentage of detect position change
    //It is "Absolute value of detected position change amount from previous frame / Detected size of previous frame * 100".
    tmpVal = (float)sqrt( (float) (preX-curX)*(preX-curX) + (preY-curY)*(preY-curY) );
    difP   = (STB_INT32)( tmpVal * 100 / preS );
    //the percentage of detect size change
    //It is "Absolute value of detected size change amount from previous frame / Detected size of previous frame * 100".
    tmpVal =  (float)(preS-curS);
    if( tmpVal < 0 )
    {
        tmpVal *= (-1);
    }
    difS   = (STB_INT32)( tmpVal * 100 / preS );
    retVal = (difP+1)*(difS+1);

    return retVal;//The return value is the similarity of the rectangle. Always a value more than or equal to zero. The closer to zero, the more similar they are.

}
/*---------------------------------------------------------------------*/
//    TrSetDistTbl
/*---------------------------------------------------------------------*/
void
TrSetDistTbl
    (
        STB_INT32    *dst            ,
        ROI_SYS        *curData        ,
        ROI_SYS        *preData        ,
        STB_INT32    traCntMax
    )
{
    STB_INT32    ip ,ic ;
    STB_INT32    distMax = STB_INT_MAX;

    // init
    for( ip = 0 ; ip < traCntMax ;  ip++ ) 
    {
        for( ic = 0 ; ic < traCntMax ;  ic++ ) 
        {
            dst [ ip * traCntMax + ic ] = distMax;
        }
    }


    for( ip = 0 ; ip < preData->cnt ;  ip++ )        
    {
        for( ic = 0 ; ic < curData->cnt ;  ic++ )        
        {
            dst [ ip * traCntMax + ic ] 
                = TrCheckSameROI//The return value is the similarity of the rectangle. Always a value more than or equal to zero. The closer to zero, the more similar they are.
                    (
                        curData->posX[ic],curData->posY[ic],curData->size[ic],
                        preData->posX[ip],preData->posY[ip],preData->size[ip]
                    );
        }
    }

}
/*---------------------------------------------------------------------*/
//    TrSteadinessXYS
/*---------------------------------------------------------------------*/
void
TrSteadinessXYS
    (    
        STB_INT32    curX    ,STB_INT32    curY    ,STB_INT32    curS    ,
        STB_INT32    preX    ,STB_INT32    preY    ,STB_INT32    preS    ,
        STB_INT32*    dstX    ,STB_INT32*    dstY    ,STB_INT32*    dstS    ,
        STB_INT32    thrP    ,STB_INT32    thrS    
    )
{

    STB_INT32 difP    ;//the percentage of detection position change
    STB_INT32 difS    ;//the percentage of detection size change
    float tmpVal;


    if( preS < 1 )
    {
        *dstX = curX    ;        *dstY = curY    ;        *dstS = curS    ;
        return ;
    }

    //the percentage of detect position change
    //It is "Absolute value of detected position change amount from previous frame / Detected size of previous frame * 100".
    tmpVal = (float)sqrt( (float) (preX-curX)*(preX-curX) + (preY-curY)*(preY-curY) );
    difP   = (STB_INT32)( tmpVal * 100 / preS );
    if( difP <= thrP )
    {
        *dstX = preX    ;        *dstY = preY    ;
    }else
    {
        *dstX = curX    ;        *dstY = curY    ;
    }

    //the percentage of detect size change
    //It is "Absolute value of detected size change amount from previous frame / Detected size of previous frame * 100".
    tmpVal =  (float)(preS-curS);
    if( tmpVal < 0 )
    {
        tmpVal *= (-1);
    }
    difS   = (STB_INT32)( tmpVal * 100 / preS );
    if( difS <= thrS )
    {
        *dstS = preS    ;
    }else
    {
        *dstS = curS    ;
    }

}
/*---------------------------------------------------------------------*/
//    TrStabilizeTR
/*---------------------------------------------------------------------*/
void TrStabilizeTR
    (
        ROI_SYS        *wData            ,    //present data after the stabilization
        STB_INT32    *wCnt            ,    //a number of present data after the stabilization
        ROI_SYS        *rec            ,    //past data
        STB_INT32    *cntAcc            ,
        TRHANDLE    handle
    )
{

    STB_INT32    stedinessPos    = handle->stedPos    ;
    STB_INT32    stedinessSize    = handle->stedSize    ;
    STB_INT32    thrRetryCnt        = handle->retryCnt    ;
    STB_INT32    traCntMax        = handle->traCntMax    ;
    STB_INT32    *idPreCur        = handle->wIdPreCur    ;
    STB_INT32    *idCurPre        = handle->wIdCurPre    ;
    STB_INT32    *dstTbl            = handle->wDstTbl    ;
    ROI_SYS        *curData        = &rec[0];//current frame data
    ROI_SYS        *preData        = &rec[1];//previous frame data
    STB_INT32   tmpAccCnt        ;
    STB_INT32    ip    ,ic        ;
    STB_INT32    ipp ,icc    ;
    STB_INT32    tmpWCnt    ;
    STB_INT32    tmpVal        ;
    STB_INT32    tmpX,tmpY,tmpS        ;
    const STB_INT32 LinkNot        = -1    ;
    

    //------------------------------------------------------------------------------//
    //Initialization
    //------------------------------------------------------------------------------//
    for( ip = 0 ; ip < traCntMax ;  ip++ ) 
    {
        idPreCur[ip]    = LinkNot;
        idCurPre[ip]    = LinkNot;
    }


    //------------------------------------------------------------------------------//
    //previous preparation
    //------------------------------------------------------------------------------//
    //Delete the data exceeding the retry count from the previous frame data.
    TrDelRetry ( preData    ,thrRetryCnt );


    //------------------------------------------------------------------------------//
    //main processing
    //------------------------------------------------------------------------------//
    tmpWCnt    = 0    ;//a number of present data after the stabilization

    // "It's reflected in the previous frame" and "It's reflected in the current frame".
    //Create dstTbl. The value of dstTbl is the similarity of the rectangle. Always a value more than or equal to zero. The closer to zero, the more similar they are.
    TrSetDistTbl( dstTbl,curData,preData,    traCntMax);
    for( ;; )    
    {
        //Get the combination (icc, ipp) that minimizes the value of dstTbl.
        tmpVal = STB_INT_MAX;        icc = -1;        ipp = -1;
        for( ic = 0 ; ic < curData->cnt ;  ic++ ) 
        {
            for( ip = 0 ; ip < preData->cnt ;  ip++ )        
            {
                if( tmpVal > dstTbl [ ip * traCntMax + ic ]  )
                {
                    tmpVal = dstTbl [ ip * traCntMax + ic ] ;        icc = ic;    ipp = ip;
                }
            }
        }
        if( tmpVal == STB_INT_MAX )
        {
            break;
        }

        //Link ipp and icc
        idCurPre[ icc    ]    = ipp    ;
        idPreCur[ ipp    ]    = icc    ;
        // steadiness
        TrSteadinessXYS
            (    
                curData->posX[icc]    ,curData->posY[icc]    ,curData->size[icc]    ,
                preData->posX[ipp]    ,preData->posY[ipp]    ,preData->size[ipp]    ,
                &tmpX                ,&tmpY                ,&tmpS                ,
                stedinessPos        ,stedinessSize
            );
        // set 
        wData->nTraID[tmpWCnt]    = preData->nTraID[ipp];
        wData->nDetID[tmpWCnt]    = curData->nDetID[icc];
        wData->posX  [tmpWCnt]    = tmpX    ;    
        wData->posY  [tmpWCnt]    = tmpY    ;    
        wData->size  [tmpWCnt]    = tmpS    ;    
        wData->conf  [tmpWCnt]    = ( ( curData->conf[icc] + preData->conf[ipp] ) /2 );
        wData->retryN[tmpWCnt]    = 0    ;//"It's reflected(linked) in the current frame"so that 0.
        tmpWCnt++;
        //Renewal "dstTbl" not to refer the associated data.
        for( ic = 0 ; ic < curData->cnt ;  ic++ ) 
        {
            dstTbl [ ipp * traCntMax + ic  ]  = STB_INT_MAX ;
        }
        for( ip = 0 ; ip < preData->cnt ;  ip++ ) 
        {
            dstTbl [ ip  * traCntMax + icc ]  = STB_INT_MAX ;
        }

        if( tmpWCnt == traCntMax  )
        {
            *wCnt = tmpWCnt;
            return;
        }
    }

    // "It is reflected in the previous frame" and "It is not reflected in the current frame".
    for( ip = 0 ; ip < preData->cnt ;  ip++ )        //"It's reflected in the previous frame"
    {
        if( idPreCur[ip] == LinkNot )                //"It's not reflected in the current frame"
        {
            // set 
            wData->nTraID[tmpWCnt]    = preData->nTraID[ip];
            wData->nDetID[tmpWCnt]    = -1;//"It's not reflected in the current frame so the detection number is -1"
            wData->posX  [tmpWCnt]    = preData->posX     [ip];
            wData->posY  [tmpWCnt]    = preData->posY     [ip];
            wData->size  [tmpWCnt]    = preData->size     [ip];
            wData->conf  [tmpWCnt]    = preData->conf[ip];
            wData->retryN[tmpWCnt]    = preData->retryN[ip]  + 1 ;//"It's not reflected in the current frame"so that +1.
            tmpWCnt++;
        }
        if( tmpWCnt == traCntMax)
        {
            *wCnt = tmpWCnt        ;
            return;
        }
    }

    // "It is not reflected in the previous frame" and "It is reflected in the current frame".
    tmpAccCnt = *cntAcc;
    for( ic = 0 ; ic < curData->cnt ;  ic++ )        //"It's reflected in the current frame"
    {
        if( idCurPre[ic] == LinkNot )                //"It's not reflected in the previous frame"
        {
            // set 
            wData->nTraID[tmpWCnt]    = tmpAccCnt;
            wData->nDetID[tmpWCnt]    = curData->nDetID[ic];
            wData->posX  [tmpWCnt]    = curData->posX     [ic];
            wData->posY  [tmpWCnt]    = curData->posY     [ic];
            wData->size  [tmpWCnt]    = curData->size     [ic];
            wData->conf  [tmpWCnt]    = curData->conf[ic];
            wData->retryN[tmpWCnt]    = 0    ;//"It's reflected in the current frame" so that 0.
            tmpWCnt++;
            tmpAccCnt++;
        }
        if( tmpWCnt == traCntMax )
        {
            *wCnt    = tmpWCnt;
            *cntAcc = tmpAccCnt;
            return;
        }
    }

    
    *wCnt    = tmpWCnt    ;
    *cntAcc = tmpAccCnt;


}
/*---------------------------------------------------------------------*/
//    TrSetRes
/*---------------------------------------------------------------------*/
void TrSetRes( ROI_SYS* wRoi,STB_TR_RES* resData , STB_INT32* resCnt )
{
    STB_INT32 i;

    *resCnt = wRoi->cnt;
    for( i = 0 ; i < wRoi->cnt ; i++ )
    {
        resData[i].nTraID = wRoi->nTraID[i];
        resData[i].nDetID = wRoi->nDetID[i];
        resData[i].pos.x  = wRoi->posX  [i];
        resData[i].pos.y  = wRoi->posY  [i];
        resData[i].size   = wRoi->size  [i];
        resData[i].conf   = wRoi->conf[i];
    }
}
/*---------------------------------------------------------------------*/
//    TrEditCur
/*---------------------------------------------------------------------*/
void TrEditCur( ROI_SYS* wRoi,ROI_SYS* curData )
{
    STB_INT32 i;

    curData->cnt = wRoi->cnt;
    for( i = 0 ; i < wRoi->cnt ; i++ )
    {
        curData->nTraID[i] = wRoi->nTraID[i];
        curData->nDetID[i] = wRoi->nDetID[i];
        curData->posX  [i] = wRoi->posX  [i];
        curData->posY  [i] = wRoi->posY  [i];
        curData->size  [i] = wRoi->size  [i];
        curData->conf  [i] = wRoi->conf  [i];
        curData->retryN[i] = wRoi->retryN[i];
    }
}
/*---------------------------------------------------------------------*/
//    StbTrExec
/*---------------------------------------------------------------------*/
int  StbTrExec ( TRHANDLE handle )
{


    /* Face --------------------------------------*/
    if( handle->execFlg->faceTr    == STB_TRUE )
    {
        //Move the time series of past data.
        TrSlideRec( handle->fcRec );
        //"the present data" set to the past data 
        TrCurRec( handle->fcRec ,handle->stbTrDet->fcDet, handle->stbTrDet->fcNum );
        //Calculate "stabilized current data wRoi" from "past data".
        TrStabilizeTR( handle->wRoi ,&(handle->wRoi->cnt) ,handle->fcRec ,&(handle->fcCntAcc) ,handle );
        //Set "wRoi" data to output data "resFaces".
        TrSetRes( handle->wRoi, handle->resFaces->face, &(handle->resFaces->cnt) );
        //set "wRoi" data to accumulated data (current) "fcRec [0]".
        TrEditCur( handle->wRoi, &(handle->fcRec[0]) );
    }

    /* Body --------------------------------------*/
    if( handle->execFlg->bodyTr    == STB_TRUE )
    {
        //Move the time series of past data.
        TrSlideRec( handle->bdRec );
        //"the present data" set to the past data 
        TrCurRec( handle->bdRec ,handle->stbTrDet->bdDet ,handle->stbTrDet->bdNum );
        //Calculate "stabilized current data wRoi" from "past data".
        TrStabilizeTR( handle->wRoi ,&(handle->wRoi->cnt) , handle->bdRec, &(handle->bdCntAcc) , handle    );    
        //Set "wRoi" data to output data "resFaces".
        TrSetRes( handle->wRoi, handle->resBodys->body, &(handle->resBodys->cnt) );
        //set "wRoi" data to accumulated data (current) "bdRec [0]".
        TrEditCur( handle->wRoi, &(handle->bdRec[0]) );
    }


    return STB_NORMAL;
}



