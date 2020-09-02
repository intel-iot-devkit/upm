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

#include "TrInterface.h"
#include "STBTrAPI.h"
/*Value range check*/
#define ISVALID_RANGE( val , min , max )    ( ( (min) <= (val) ) && ( (val) <= (max) ) )

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*error check*/
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
static STB_INT32 TrIsValidValue(
    const STB_INT32 nValue ,
    const STB_INT32 nLimitMin ,
    const STB_INT32 nLimitMax )
{
    STB_INT32 nRet;
    for( nRet = STB_ERR_INVALIDPARAM; nRet != STB_NORMAL; nRet = STB_NORMAL ){
        if( ! ISVALID_RANGE( nValue , nLimitMin , nLimitMax ) ){ break; }
    }
    return nRet;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
static STB_INT32 TrIsValidPointer( const VOID* pPointer )    
{
    STB_INT32 nRet;
    for( nRet = STB_ERR_INVALIDPARAM; nRet != STB_NORMAL; nRet = STB_NORMAL ){
        if( NULL == pPointer ){ break; }
    }
    return nRet;
}

/*------------------------------------------------------------------------------------------------------------------*/
/*    TrCalcTrSize                                                                                                        */
/*------------------------------------------------------------------------------------------------------------------*/
STB_UINT32 TrCalcTrSize ( const STBExecFlg *execFlg , STB_UINT32 nTraCntMax , STB_UINT32 nDetCntMax )
{
    STB_UINT32 retVal ;

    retVal = 0 ;

    retVal += 100 ;///Margin  : alignment


    retVal        +=    sizeof( STB_TR_DET        );                                // stbTrDet

    if( execFlg->bodyTr  == STB_TRUE )
    {
        retVal    +=    sizeof( ROI_SYS            ) * STB_TR_BACK_MAX                ;// bdRec
        retVal    +=    sizeof( STB_INT32        ) * STB_TR_BACK_MAX * nTraCntMax    ;// bdRec[t].nDetID
        retVal    +=    sizeof( STB_INT32        ) * STB_TR_BACK_MAX * nTraCntMax    ;// bdRec[t].nTraID
        retVal    +=    sizeof( STB_INT32        ) * STB_TR_BACK_MAX * nTraCntMax    ;// bdRec[t].posX
        retVal    +=    sizeof( STB_INT32        ) * STB_TR_BACK_MAX * nTraCntMax    ;// bdRec[t].posY
        retVal    +=    sizeof( STB_INT32        ) * STB_TR_BACK_MAX * nTraCntMax    ;// bdRec[t].size
        retVal    +=    sizeof( STB_INT32        ) * STB_TR_BACK_MAX * nTraCntMax    ;// bdRec[t].conf
        retVal    +=    sizeof( STB_INT32        ) * STB_TR_BACK_MAX * nTraCntMax    ;// bdRec[t].retryN
        retVal    +=    sizeof( ROI_DET            ) * nDetCntMax                    ;// stbTrDet->bdDet
        retVal    +=    sizeof( STB_TR_RES_BODYS)                                ;// resBodys
        retVal    +=    sizeof( STB_TR_RES        ) * nTraCntMax                    ;// resBodys->body
    }
    if( execFlg->faceTr  == STB_TRUE )
    {
        retVal    +=    sizeof( ROI_SYS            ) * STB_TR_BACK_MAX                ;// fcRec
        retVal    +=    sizeof( STB_INT32        ) * STB_TR_BACK_MAX * nTraCntMax    ;// fcRec[t].nDetID
        retVal    +=    sizeof( STB_INT32        ) * STB_TR_BACK_MAX * nTraCntMax    ;// fcRec[t].nTraID
        retVal    +=    sizeof( STB_INT32        ) * STB_TR_BACK_MAX * nTraCntMax    ;// fcRec[t].posX
        retVal    +=    sizeof( STB_INT32        ) * STB_TR_BACK_MAX * nTraCntMax    ;// fcRec[t].posY
        retVal    +=    sizeof( STB_INT32        ) * STB_TR_BACK_MAX * nTraCntMax    ;// fcRec[t].size
        retVal    +=    sizeof( STB_INT32        ) * STB_TR_BACK_MAX * nTraCntMax    ;// fcRec[t].conf
        retVal    +=    sizeof( STB_INT32        ) * STB_TR_BACK_MAX * nTraCntMax    ;// fcRec[t].retryN
        retVal    +=    sizeof( ROI_DET            ) * nDetCntMax                    ;// stbTrDet->fcDet
        retVal    +=    sizeof( STB_TR_RES_FACES)                                 ;// resFaces
        retVal    +=    sizeof( STB_TR_RES        ) * nTraCntMax                     ;// resFaces->face
    }

    retVal    +=    sizeof( STB_INT32    ) * nTraCntMax                ;    // wIdPreCur
    retVal    +=    sizeof( STB_INT32    ) * nTraCntMax                ;    // wIdCurPre
    retVal    +=    sizeof( STB_INT32    ) * nTraCntMax * nTraCntMax    ;    // wDstTbl
    retVal    +=    sizeof( STBExecFlg    )                            ;    // execFlg

    retVal    += ( sizeof( ROI_SYS    )     );//wRoi
    retVal    += ( sizeof( STB_INT32    ) * nTraCntMax    );//wRoi->nDetID
    retVal    += ( sizeof( STB_INT32    ) * nTraCntMax    );//wRoi->nTraID
    retVal    += ( sizeof( STB_INT32    ) * nTraCntMax    );//wRoi->posX
    retVal    += ( sizeof( STB_INT32    ) * nTraCntMax    );//wRoi->posY
    retVal    += ( sizeof( STB_INT32    ) * nTraCntMax    );//wRoi->size
    retVal    += ( sizeof( STB_INT32    ) * nTraCntMax    );//wRoi->conf
    retVal    += ( sizeof( STB_INT32    ) * nTraCntMax    );//wRoi->retryN

    return retVal;
}
/*------------------------------------------------------------------------------------------------------------------*/
/*    ShareTrSize                                                                                                        */
/*------------------------------------------------------------------------------------------------------------------*/
void ShareTrSize ( TRHANDLE    handle , const STBExecFlg* execFlg )
{
    STB_UINT32    t    ;
    STB_UINT32    nTraCntMax    = handle->traCntMax    ;
    STB_UINT32    nDetCntMax    = handle->detCntMax    ;
    STB_INT8    *stbPtr        = handle->trPtr        ;



    handle->stbTrDet                    =  ( STB_TR_DET*) stbPtr;        stbPtr    += ( sizeof( STB_TR_DET    )         );

    if( execFlg->bodyTr  == STB_TRUE )
    {
        handle->bdRec                    =  ( ROI_SYS*    ) stbPtr;        stbPtr    += ( sizeof( ROI_SYS    ) * STB_TR_BACK_MAX);
        for( t = 0 ; t < STB_TR_BACK_MAX ; t++ )
        {
            handle->bdRec[t].nDetID        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
            handle->bdRec[t].nTraID        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
            handle->bdRec[t].posX        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
            handle->bdRec[t].posY        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
            handle->bdRec[t].size        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
            handle->bdRec[t].conf        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
            handle->bdRec[t].retryN        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
        }
        handle->stbTrDet->bdDet            =  ( ROI_DET*    ) stbPtr;        stbPtr    += ( sizeof( ROI_DET    ) * nDetCntMax    );
        handle->resBodys                =  ( STB_TR_RES_BODYS*    ) stbPtr;        stbPtr    += ( sizeof( STB_TR_RES_BODYS    )                );
        handle->resBodys->body            =  ( STB_TR_RES*) stbPtr;        stbPtr    += ( sizeof( STB_TR_RES    ) * nTraCntMax    );
    }


    if( execFlg->faceTr  == STB_TRUE )
    {
        handle->fcRec                =  ( ROI_SYS*    ) stbPtr;        stbPtr    += ( sizeof( ROI_SYS    ) * STB_TR_BACK_MAX    );
        for( t = 0 ; t < STB_TR_BACK_MAX ; t++ )
        {
            handle->fcRec[t].nDetID        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
            handle->fcRec[t].nTraID        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
            handle->fcRec[t].posX        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
            handle->fcRec[t].posY        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
            handle->fcRec[t].size        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
            handle->fcRec[t].conf        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
            handle->fcRec[t].retryN        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
        }
        handle->stbTrDet->fcDet            =  ( ROI_DET*    ) stbPtr;        stbPtr    += ( sizeof( ROI_DET    ) * nDetCntMax    );

        handle->resFaces                =  ( STB_TR_RES_FACES*    ) stbPtr;        stbPtr    += ( sizeof( STB_TR_RES_FACES    )                );
        handle->resFaces->face            =  ( STB_TR_RES*) stbPtr;        stbPtr    += ( sizeof( STB_TR_RES    ) * nTraCntMax    );
    }


    handle->wIdPreCur    =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
    handle->wIdCurPre    =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
    handle->wDstTbl        =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax * nTraCntMax    );
    handle->execFlg        =  ( STBExecFlg*) stbPtr;        stbPtr    +=   sizeof( STBExecFlg    );

    handle->wRoi        =  ( ROI_SYS*    ) stbPtr;        stbPtr    += ( sizeof( ROI_SYS    )     );
    handle->wRoi->nDetID=  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
    handle->wRoi->nTraID=  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
    handle->wRoi->posX    =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
    handle->wRoi->posY    =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
    handle->wRoi->size    =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
    handle->wRoi->conf    =  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
    handle->wRoi->retryN=  ( STB_INT32*    ) stbPtr;        stbPtr    += ( sizeof( STB_INT32    ) * nTraCntMax    );
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*Create handle*/
TRHANDLE TrCreateHandle( const STBExecFlg* execFlg ,const STB_INT32 nDetCntMax, const STB_INT32 nTraCntMax  )
{

    TRHANDLE    handle    ;
    STB_INT32    i    ,j    ;
    STB_INT32    tmpVal    ;
    STB_INT32    nRet    ;

    nRet = TrIsValidPointer(execFlg);
    if(nRet != STB_NORMAL)
    {
        return NULL;
    }

    if( nDetCntMax < 1 || STB_TR_DET_CNT_MAX < nDetCntMax )
    {
        return NULL;
    }
    if( nTraCntMax < 1 || STB_TR_TRA_CNT_MAX < nTraCntMax )
    {
        return NULL;
    }

    /*do handle's Malloc here*/
    handle = ( TRHANDLE )malloc( sizeof(*handle)    );
    if(handle == NULL)
    {
        return NULL;
    }

    handle->detCntMax            = nDetCntMax        ;
    handle->traCntMax            = nTraCntMax        ;
    handle->retryCnt            = STB_TR_INI_RETRY        ;
    handle->stedPos                = STB_TR_INI_STEADINESS_SIZE    ;//stabilization parameter(position)
    handle->stedSize            = STB_TR_INI_STEADINESS_POS    ;//stabilization parameter(size)
    handle->fcCntAcc            = 0                    ;
    handle->bdCntAcc            = 0                    ;
    handle->trPtr                = NULL;
    handle->stbTrDet            = NULL;
    handle->fcRec                = NULL;
    handle->bdRec                = NULL;
    handle->resFaces            = NULL;
    handle->resBodys            = NULL;
    handle->wIdPreCur            = NULL;
    handle->wIdCurPre            = NULL;
    handle->wDstTbl                = NULL;
    handle->execFlg                = NULL;

    tmpVal            = TrCalcTrSize ( execFlg ,nTraCntMax    , nDetCntMax);    /*calculate necessary amount in the TR handle*/
    handle->trPtr    = NULL;
    handle->trPtr    = ( STB_INT8 * )malloc( tmpVal    )    ;                /*keep necessary amount in the TR handle*/
    if( handle->trPtr == NULL )
    {
        free ( handle->trPtr    );
        free ( handle            );
        return NULL;
    }
    ShareTrSize ( handle , execFlg    );                                    /*Malloc-area is allocated to things that need Malloc in TR handle*/

    /*set initial value*/
    if( execFlg->faceTr == STB_TRUE )
    {
        for( i = 0 ; i < STB_TR_BACK_MAX  ; i++)
        {
            handle->fcRec[i].cnt= 0;
            for( j = 0 ; j < handle->traCntMax ; j++)
            {
                handle->fcRec[i].nDetID    [j]    = -1;
                handle->fcRec[i].nTraID    [j]    = -1;
                handle->fcRec[i].posX    [j]    = 0;
                handle->fcRec[i].posY    [j]    = 0;
                handle->fcRec[i].size    [j]    = -1;
                handle->fcRec[i].retryN    [j]    = -1;
                handle->fcRec[i].conf    [j]    = -1;
            }
        }
    }
    if( execFlg->bodyTr == STB_TRUE )
    {
        for( i = 0 ; i < STB_TR_BACK_MAX  ; i++)
        {
            handle->bdRec[i].cnt= 0;
            for( j = 0 ; j < handle->traCntMax ; j++)
            {
                handle->bdRec[i].nDetID    [j]    = -1;
                handle->bdRec[i].nTraID    [j]    = -1;
                handle->bdRec[i].posX    [j]    = 0;
                handle->bdRec[i].posY    [j]    = 0;
                handle->bdRec[i].size    [j]    = -1;
                handle->bdRec[i].retryN    [j]    = -1;
                handle->bdRec[i].conf    [j]    = -1;
            }
        }
    }



    handle->execFlg->pet        = execFlg->pet        ;
    handle->execFlg->hand        = execFlg->hand        ;
    handle->execFlg->bodyTr        = execFlg->bodyTr    ;
    handle->execFlg->faceTr        = execFlg->faceTr    ;
    handle->execFlg->gen        = execFlg->gen        ;
    handle->execFlg->age        = execFlg->age        ;
    handle->execFlg->fr            = execFlg->fr        ;
    handle->execFlg->exp        = execFlg->exp        ;
    handle->execFlg->gaz        = execFlg->gaz        ;
    handle->execFlg->dir        = execFlg->dir        ;
    handle->execFlg->bli        = execFlg->bli        ;


    return handle;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/

/*Delete handle*/
STB_INT32 TrDeleteHandle(TRHANDLE handle)
{
    STB_INT32 nRet;

    /*NULL check*/
    nRet = TrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    free ( handle->trPtr    );
    free ( handle            );

    return STB_NORMAL;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*Set the result*/
STB_INT32 TrSetDetect(TRHANDLE handle,const STB_TR_DET *stbTrDet){
    STB_INT32 nRet;
    STB_INT32 i;

    /*NULL check*/
    nRet = TrIsValidPointer(handle);
    if(nRet != STB_NORMAL)
    {
        return STB_ERR_NOHANDLE;
    }

    nRet = TrIsValidPointer(stbTrDet);
    if(nRet != STB_NORMAL)
    {
        return nRet;
    }

    /*Input value check*/
    nRet = STB_TrIsValidValue ( stbTrDet ,handle->execFlg );
    if(nRet != STB_TRUE)
    {
        return STB_ERR_INVALIDPARAM;
    }

    /*Set the received result to the handle (stbTrDet)*/
    /* Face */
    if( handle->execFlg->faceTr == STB_TRUE )
    {
        handle->stbTrDet->fcNum = stbTrDet->fcNum;
        for( i = 0 ; i < handle->stbTrDet->fcNum ; i++ )
        {
            handle->stbTrDet->fcDet[i].posX        = stbTrDet->fcDet[i].posX;
            handle->stbTrDet->fcDet[i].posY        = stbTrDet->fcDet[i].posY;
            handle->stbTrDet->fcDet[i].size        = stbTrDet->fcDet[i].size;
            handle->stbTrDet->fcDet[i].conf        = stbTrDet->fcDet[i].conf;
        }
    }else
    {
        handle->stbTrDet->fcNum = 0;
    }

    /* Body */
    if( handle->execFlg->bodyTr == STB_TRUE )
    {
        handle->stbTrDet->bdNum = stbTrDet->bdNum;
        for( i = 0 ; i < handle->stbTrDet->bdNum ; i++ )
        {
            handle->stbTrDet->bdDet[i].posX        = stbTrDet->bdDet[i].posX;
            handle->stbTrDet->bdDet[i].posY        = stbTrDet->bdDet[i].posY;
            handle->stbTrDet->bdDet[i].size        = stbTrDet->bdDet[i].size;
            handle->stbTrDet->bdDet[i].conf        = stbTrDet->bdDet[i].conf;
        }
    }else
    {
        handle->stbTrDet->bdNum = 0;
    }



    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*Main process execution*/
STB_INT32 TrExecute(TRHANDLE handle){

    STB_INT32 nRet;
    /*NULL check*/
    nRet = TrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    /*Main processing here*/
    nRet = StbTrExec ( handle );


    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*Get-Function of results*/
STB_INT32 TrGetResult(TRHANDLE handle,STB_TR_RES_FACES* fcResult,STB_TR_RES_BODYS* bdResult){

    STB_INT32 nRet;
    STB_INT32 i;

    /*NULL check*/
    nRet = TrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    if( handle->execFlg->faceTr == STB_TRUE )
    {
        nRet = TrIsValidPointer(fcResult);
        if(nRet != STB_NORMAL){
            return nRet;
        }
    }
    if( handle->execFlg->bodyTr == STB_TRUE )
    {
        nRet = TrIsValidPointer(bdResult);
        if(nRet != STB_NORMAL){
            return nRet;
        }
    }

    /*Get result from handle*/

    /* Face */
    if( handle->execFlg->faceTr == STB_TRUE )
    {
        fcResult->cnt = handle->resFaces->cnt ;
        for( i = 0 ; i < handle->resFaces->cnt ; i++ )
        {
            fcResult->face[i].nDetID    = handle->resFaces->face[i].nDetID  ;
            fcResult->face[i].nTraID    = handle->resFaces->face[i].nTraID  ;
            fcResult->face[i].pos.x        = handle->resFaces->face[i].pos.x    ;
            fcResult->face[i].pos.y        = handle->resFaces->face[i].pos.y    ;
            fcResult->face[i].size        = handle->resFaces->face[i].size    ;
            fcResult->face[i].conf        = handle->resFaces->face[i].conf    ;
        }
        for( i = handle->resFaces->cnt ; i < handle->traCntMax ; i++ )
        {
            fcResult->face[i].nDetID    = -1  ;
            fcResult->face[i].nTraID    = -1  ;
            fcResult->face[i].pos.x        = 0      ;
            fcResult->face[i].pos.y        = 0      ;
            fcResult->face[i].size        = -1  ;
            fcResult->face[i].conf        = STB_CONF_NO_DATA  ;
        }
    }

    /* Body */
    if( handle->execFlg->bodyTr == STB_TRUE )
    {
        bdResult->cnt = handle->resBodys->cnt ;
        for( i = 0 ; i < handle->resBodys->cnt ; i++ )
        {
            bdResult->body[i].nDetID    = handle->resBodys->body[i].nDetID    ;
            bdResult->body[i].nTraID    = handle->resBodys->body[i].nTraID    ;
            bdResult->body[i].pos.x        = handle->resBodys->body[i].pos.x    ;
            bdResult->body[i].pos.y        = handle->resBodys->body[i].pos.y    ;
            bdResult->body[i].size        = handle->resBodys->body[i].size    ;
            bdResult->body[i].conf        = handle->resBodys->body[i].conf    ;
        }
        for( i = handle->resBodys->cnt ; i < handle->traCntMax ; i++ )
        {
            bdResult->body[i].nDetID    = -1  ;
            bdResult->body[i].nTraID    = -1  ;
            bdResult->body[i].pos.x        = 0      ;
            bdResult->body[i].pos.y        = 0      ;
            bdResult->body[i].size        = -1  ;
            bdResult->body[i].conf        = STB_CONF_NO_DATA  ;
        }
    }

    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*Clear*/
STB_INT32 TrClear(TRHANDLE handle){

    STB_INT32 nRet;
    STB_INT32 i , j;


    /*NULL check*/
    nRet = TrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }


    if( handle->execFlg->faceTr == STB_TRUE )
    {
        for( i = 0 ; i < STB_TR_BACK_MAX  ; i++)
        {
            handle->fcRec[i].cnt= 0;
            for( j = 0 ; j < handle->traCntMax ; j++)
            {
                handle->fcRec[i].nDetID        [j]    = -1;
                handle->fcRec[i].nTraID        [j]    = -1;
                handle->fcRec[i].posX        [j]    = 0      ;
                handle->fcRec[i].posY        [j]    = 0      ;
                handle->fcRec[i].size        [j]    = -1;
                handle->fcRec[i].retryN        [j]    = -1;
                handle->fcRec[i].conf        [j]    = -1;
            }
        }
        handle->fcCntAcc = 0;
    }

    if( handle->execFlg->bodyTr == STB_TRUE )
    {
        for( i = 0 ; i < STB_TR_BACK_MAX  ; i++)
        {
            handle->bdRec[i].cnt= 0;
            for( j = 0 ; j < handle->traCntMax ; j++)
            {
                handle->bdRec[i].nDetID        [j]    = -1;
                handle->bdRec[i].nTraID        [j]    = -1;
                handle->bdRec[i].posX        [j]    = 0      ;
                handle->bdRec[i].posY        [j]    = 0      ;
                handle->bdRec[i].size        [j]    = -1;
                handle->bdRec[i].retryN        [j]    = -1;
                handle->bdRec[i].conf        [j]    = -1;
            }
        }
        handle->bdCntAcc = 0;
    }

    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/* */
STB_INT32 TrSetRetryCount(TRHANDLE handle, STB_INT32 nRetryCount) 
{
    STB_INT32 nRet;
    nRet = TrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    
    if( nRetryCount < STB_TR_MIN_RETRY || STB_TR_MAX_RETRY < nRetryCount)
    {
        return STB_ERR_INVALIDPARAM;
    }

    handle->retryCnt = nRetryCount;
    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32    TrGetRetryCount        ( TRHANDLE handle , STB_INT32* nRetryCount )
{
    STB_INT32 nRet;
    nRet = TrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    nRet = TrIsValidPointer(nRetryCount);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }    
    
    *nRetryCount = handle->retryCnt ;
    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32    TrSetStedinessParam    ( TRHANDLE handle , STB_INT32    nStedinessPos , STB_INT32  nStedinessSize )
{
    if( nStedinessPos < STB_TR_MIN_STEADINESS_POS || STB_TR_MAX_STEADINESS_POS < nStedinessPos)
    {
        return STB_ERR_INVALIDPARAM;
    }
    if( nStedinessSize < STB_TR_MIN_STEADINESS_SIZE || STB_TR_MAX_STEADINESS_SIZE < nStedinessSize)
    {
        return STB_ERR_INVALIDPARAM;
    }
    handle->stedPos        = nStedinessPos;
    handle->stedSize    = nStedinessSize;
    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32    TrGetStedinessParam    ( TRHANDLE handle , STB_INT32*    nStedinessPos , STB_INT32* nStedinessSize )
{
    STB_INT32 nRet;
    nRet = TrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    nRet = TrIsValidPointer(nStedinessPos);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }    
    nRet = TrIsValidPointer(nStedinessSize);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }    
    *nStedinessPos        = handle->stedPos    ;
    *nStedinessSize        = handle->stedSize ;
    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/