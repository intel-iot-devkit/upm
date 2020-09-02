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

#include "FrInterface.h"
#include "STBFrAPI.h"

/*Value range check*/
#define ISVALID_RANGE( val , min , max )    ( ( (min) <= (val) ) && ( (val) <= (max) ) )

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*error check*/
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
static STB_INT32 FrIsValidValue(
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
static STB_INT32 FrIsValidPointer( const VOID* pPointer )    
{
    STB_INT32 nRet;
    for( nRet = STB_ERR_INVALIDPARAM; nRet != STB_NORMAL; nRet = STB_NORMAL ){
        if( NULL == pPointer ){ break; }
    }
    return nRet;
}

/*------------------------------------------------------------------------------------------------------------------*/
/*    CalcFrSize                                                                                                        */
/*------------------------------------------------------------------------------------------------------------------*/
STB_UINT32 CalcFrSize ( STB_UINT32 nTraCntMax )
{
    STB_UINT32 retVal ;

    retVal = 0 ;

    retVal += 100 ;///Margin  : alignment



    retVal    +=    sizeof( FR_DET        ) * nTraCntMax                ;    // frDet.fcDet
    retVal    +=    sizeof( STB_FR_DET    ) * STB_FR_BACK_MAX            ;    // frDetRec
    retVal    +=    sizeof( FR_DET        ) * nTraCntMax * nTraCntMax    ;    // frDetRec[t].fcDet
    retVal    +=    sizeof( FR_RES        ) * nTraCntMax                ;    // frRes.frFace

    return retVal;
}
/*------------------------------------------------------------------------------------------------------------------*/
/*    ShareFrSize                                                                                                        */
/*------------------------------------------------------------------------------------------------------------------*/
void ShareFrSize ( FRHANDLE    handle )
{

    STB_UINT32    t;
    STB_INT8    *stbPtr        = handle->frPtr        ;
    STB_UINT32    nTraCntMax    = handle->frCntMax    ;

    handle->frDet.fcDet        =    ( FR_DET*        ) stbPtr;        stbPtr    += ( sizeof( FR_DET        ) * nTraCntMax    );
    handle->frDetRec        =    ( STB_FR_DET*    ) stbPtr;        stbPtr    += ( sizeof( STB_FR_DET    ) * STB_FR_BACK_MAX);
    for( t = 0 ; t < STB_FR_BACK_MAX ; t++ )
    {
        handle->frDetRec[t].fcDet = ( FR_DET*    ) stbPtr;        stbPtr    += ( sizeof( FR_DET        ) * nTraCntMax    );
    }
    handle->frRes.frFace    =    ( FR_RES    *    ) stbPtr;        stbPtr    += ( sizeof( FR_RES        ) * nTraCntMax    );

}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*Create handle*/
FRHANDLE FrCreateHandle(  const STB_INT32 nTraCntMax    )

{

    FRHANDLE    handle;
    STB_INT32    t , i ;
    STB_INT32    tmpVal;



    if( nTraCntMax < 1 || STB_FR_TRA_CNT_MAX < nTraCntMax )
    {
        return NULL;
    }

    /*do handle's Malloc here*/
    handle = (FRHANDLE)malloc(sizeof(*handle));
    if(handle == NULL)
    {
        return NULL;
    }


    /*initial value---------------------------------------------------------------------*/
    handle->frFaceDirUDMax     = STB_FR_DIR_MAX_UD_INI        ;
    handle->frFaceDirUDMin    = STB_FR_DIR_MIN_UD_INI        ;
    handle->frFaceDirLRMax     = STB_FR_DIR_MAX_LR_INI        ;
    handle->frFaceDirLRMin    = STB_FR_DIR_MIN_LR_INI        ;
    handle->frCntMax        = nTraCntMax                ;//Maximum number of tracking people
    handle->frFaceDirThr    = STB_FR_DIR_THR_INI            ;//If the confidence of Face direction estimation doesn't exceed the reference value, the recognition result isn't trusted.
    handle->frFrameCount    = STB_FR_FRAME_CNT_INI            ;
    handle->frFrameRatio    = STB_FR_FRAME_RATIO_INI        ;
    handle->frPtr            = NULL;
    handle->frDet.num        = 0;
    handle->frDet.fcDet        = NULL;
    handle->frDetRec        = NULL;
    handle->frRes.frCnt        = 0;
    handle->frRes.frFace    = NULL;

    tmpVal            = CalcFrSize (    nTraCntMax        );    /*calculate necessary amount in the Fr handle*/
    handle->frPtr    = NULL;
    handle->frPtr    = ( STB_INT8 * )malloc( tmpVal    );    /*keeping necessary amount in the Fr handle*/
    if( handle->frPtr == NULL )
    {
        free ( handle->frPtr    );
        free ( handle            );
        return NULL;
    }

    /* Malloc-area is allocated to things that need Malloc in FR handle */
    ShareFrSize ( handle     );




    for( t = 0 ; t < STB_FR_BACK_MAX  ;  t++ ) 
    {
        handle->frDetRec [ t ].num = 0;
        for( i = 0 ; i < handle->frCntMax ;  i++ ) 
        {
            handle->frDetRec [ t ].fcDet[i].nDetID        = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].nTraID        = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].dirDetPitch    = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].dirDetRoll    = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].dirDetYaw    = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].frDetConf    = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].frDetID        = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].frDetConf    = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].frStatus    = STB_STATUS_NO_DATA ;

        }
    }

    return handle;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*Delete handle*/
STB_INT32 FrDeleteHandle(FRHANDLE handle){
    STB_INT32 nRet;

    /*NULL check*/
    nRet = FrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return nRet;
    }

    free ( handle->frPtr    );
    free ( handle            );

    return nRet;
}


/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*Set the result*/
STB_INT32 FrSetDetect(FRHANDLE handle,const STB_FR_DET *stbFrDet){

    STB_INT32 nRet;
    STB_INT32 i;

    /*NULL check*/
    nRet = FrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    nRet = FrIsValidPointer(stbFrDet);
    if(nRet != STB_NORMAL){
        return nRet;
    }

    /*Input value check*/
    nRet = STB_FrIsValidValue ( stbFrDet );
    if(nRet != STB_TRUE)
    {
        return STB_ERR_INVALIDPARAM;
    }


    /*Set the received result to the handle*/
    /* Face */
    if( stbFrDet->num > handle->frCntMax )
    {
        return STB_ERR_PROCESSCONDITION;
    }
    handle->frDet.num = stbFrDet->num;
    for( i = 0 ; i < handle->frDet.num ; i++ )
    {

        handle->frDet.fcDet[i].nDetID        = stbFrDet->fcDet[i].nDetID        ;
        handle->frDet.fcDet[i].nTraID        = stbFrDet->fcDet[i].nTraID        ;
        handle->frDet.fcDet[i].dirDetPitch    = stbFrDet->fcDet[i].dirDetPitch;
        handle->frDet.fcDet[i].dirDetRoll    = stbFrDet->fcDet[i].dirDetRoll    ;
        handle->frDet.fcDet[i].dirDetYaw    = stbFrDet->fcDet[i].dirDetYaw    ;
        handle->frDet.fcDet[i].dirDetConf    = stbFrDet->fcDet[i].dirDetConf    ;
        handle->frDet.fcDet[i].frDetID        = stbFrDet->fcDet[i].frDetID    ;
        handle->frDet.fcDet[i].frDetConf    = stbFrDet->fcDet[i].frDetConf    ;

    }



    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*Main process execution*/
STB_INT32 FrExecute(FRHANDLE handle){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = FrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    /*Main processing here*/
    nRet = StbFrExec ( handle );

    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*Get-Function of results*/
STB_INT32 FrGetResult(FRHANDLE handle,STB_FR_RES* frResult){
    STB_INT32 nRet;
    int i;

    /*NULL check*/
    nRet = FrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    nRet = FrIsValidPointer(frResult);
    if(nRet != STB_NORMAL){
        return nRet;
    }

    /*Get result from handle*/
    frResult->frCnt = handle->frRes.frCnt ;
    for( i = 0 ; i < frResult->frCnt ; i++ ){
        frResult->frFace[i].nTraID                = handle->frRes.frFace[i].nTraID            ;
        frResult->frFace[i].frRecog.value        = handle->frRes.frFace[i].frRecog.value        ;
        frResult->frFace[i].frRecog.status        = handle->frRes.frFace[i].frRecog.status    ;
        frResult->frFace[i].frRecog.conf        = handle->frRes.frFace[i].frRecog.conf        ;
    }

    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32 FrSetFaceDirMinMax(FRHANDLE handle , STB_INT32 nMinUDAngle    , STB_INT32 nMaxUDAngle        ,STB_INT32 nMinLRAngle    , STB_INT32 nMaxLRAngle)
{
    
    STB_INT32 nRet;
    /*NULL check*/
    nRet = FrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    
    if( nMinUDAngle < STB_FR_DIR_MIN_UD_MIN || STB_FR_DIR_MIN_UD_MAX < nMinUDAngle)
    {
        return STB_ERR_INVALIDPARAM;
    }
    if( nMaxUDAngle < STB_FR_DIR_MAX_UD_MIN || STB_FR_DIR_MAX_UD_MAX < nMaxUDAngle)
    {
        return STB_ERR_INVALIDPARAM;
    }
    if( nMaxUDAngle < nMinUDAngle)
    {
        return STB_ERR_INVALIDPARAM;
    }


    if( nMinLRAngle < STB_FR_DIR_MIN_LR_MIN || STB_FR_DIR_MIN_LR_MAX < nMinLRAngle)
    {
        return STB_ERR_INVALIDPARAM;
    }
    if( nMaxLRAngle < STB_FR_DIR_MAX_LR_MIN || STB_FR_DIR_MAX_LR_MAX < nMaxLRAngle)
    {
        return STB_ERR_INVALIDPARAM;
    }
    if( nMaxLRAngle < nMinLRAngle)
    {
        return STB_ERR_INVALIDPARAM;
    }


    handle->frFaceDirUDMin = nMinUDAngle;
    handle->frFaceDirUDMax = nMaxUDAngle;
    handle->frFaceDirLRMin = nMinLRAngle;
    handle->frFaceDirLRMax = nMaxLRAngle;
    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32 FrGetFaceDirMinMax(FRHANDLE handle , STB_INT32 *pnMinUDAngle    , STB_INT32 *pnMaxUDAngle    ,STB_INT32 *pnMinLRAngle, STB_INT32 *pnMaxLRAngle )
{
    STB_INT32 nRet;
    /*NULL check*/
    nRet = FrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    nRet = FrIsValidPointer(pnMinUDAngle);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    nRet = FrIsValidPointer(pnMaxUDAngle);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    nRet = FrIsValidPointer(pnMinLRAngle);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    nRet = FrIsValidPointer(pnMaxLRAngle);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    
    
    *pnMinUDAngle = handle->frFaceDirUDMin ;
    *pnMaxUDAngle = handle->frFaceDirUDMax ;
    *pnMinLRAngle = handle->frFaceDirLRMin ;
    *pnMaxLRAngle = handle->frFaceDirLRMax ;
    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32    FrClear            ( FRHANDLE handle  )
{
    //clear processing

    STB_INT32 t , i ;
    STB_INT32 nRet;

    /*NULL check*/
    nRet = FrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    for( t = 0 ; t < STB_FR_BACK_MAX  ;  t++ ) 
    {
        handle->frDetRec [ t ].num = 0;
        for( i = 0 ; i < handle->frCntMax ;  i++ ) 
        {
            handle->frDetRec [ t ].fcDet[i].nDetID        = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].nTraID        = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].dirDetPitch    = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].dirDetRoll    = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].dirDetYaw    = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].dirDetConf    = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].frDetID        = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].frDetConf    = STB_STATUS_NO_DATA ;
            handle->frDetRec [ t ].fcDet[i].frStatus    = STB_STATUS_NO_DATA ;

        }
    }
    return STB_NORMAL;


}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32 FrSetFaceDirThreshold(FRHANDLE handle , STB_INT32 threshold )
{
    STB_INT32 nRet;
    /*NULL check*/
    nRet = FrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    
    if( threshold < STB_FR_DIR_THR_MIN || STB_FR_DIR_THR_MAX < threshold)
    {
        return STB_ERR_INVALIDPARAM;
    }
    handle->frFaceDirThr = threshold;
    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32 FrGetFaceDirThreshold(FRHANDLE handle , STB_INT32* threshold )
{
    STB_INT32 nRet;

    /*NULL check*/
    nRet = FrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    nRet = FrIsValidPointer(threshold);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    
    *threshold = handle->frFaceDirThr ;
    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32 FrSetFrameCount(FRHANDLE handle , STB_INT32 nFrameCount )
{

    STB_INT32 nRet;
    /*NULL check*/
    nRet = FrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    if( nFrameCount < STB_FR_FRAME_CNT_MIN || STB_FR_FRAME_CNT_MAX < nFrameCount)
    {
        return STB_ERR_INVALIDPARAM;
    }
    handle->frFrameCount = nFrameCount;
    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32 FrGetFrameCount(FRHANDLE handle , STB_INT32* nFrameCount )
{
    STB_INT32 nRet;
    /*NULL check*/
    nRet = FrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    nRet = FrIsValidPointer(nFrameCount);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    
    *nFrameCount = handle->frFrameCount ;
    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32 FrSetMinRatio(FRHANDLE handle , STB_INT32 nMinRatio )
{
    STB_INT32 nRet;
    /*NULL check*/
    nRet = FrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }    
    if( nMinRatio < STB_FR_FRAME_RATIO_MIN || STB_FR_FRAME_RATIO_MAX < nMinRatio)
    {
        return STB_ERR_INVALIDPARAM;
    }
    handle->frFrameRatio = nMinRatio;
    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32 FrGetMinRatio(FRHANDLE handle , STB_INT32* nMinRatio )
{
    STB_INT32 nRet;
    /*NULL check*/
    nRet = FrIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }    
    nRet = FrIsValidPointer(nMinRatio);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    
    *nMinRatio = handle->frFrameRatio ;
    return STB_NORMAL;
}
