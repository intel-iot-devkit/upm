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

#include "PeInterface.h"
#include "STBPeAPI.h"

/*Value range check*/
#define ISVALID_RANGE( val , min , max )    ( ( (min) <= (val) ) && ( (val) <= (max) ) )

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*error check*/
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
static STB_INT32 PeIsValidValue(
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
static STB_INT32 PeIsValidPointer( const VOID* pPointer )    
{
    STB_INT32 nRet;
    for( nRet = STB_ERR_INVALIDPARAM; nRet != STB_NORMAL; nRet = STB_NORMAL ){
        if( NULL == pPointer ){ break; }
    }
    return nRet;
}

/*------------------------------------------------------------------------------------------------------------------*/
/*    PeCalcPeSize                                                                                                        */
/*------------------------------------------------------------------------------------------------------------------*/
STB_UINT32 PeCalcPeSize ( STB_UINT32 nTraCntMax )
{
    STB_UINT32 retVal ;

    retVal = 0 ;

    retVal += 100 ;///Margin  : alignment



    retVal    +=    sizeof( FACE_DET    ) * nTraCntMax                    ;    // peDet.fcDet
    retVal    +=    sizeof( STB_PE_DET    ) * STB_PE_BACK_MAX                ;    // peDetRec
    retVal    +=    sizeof( FACE_DET    ) * nTraCntMax * STB_PE_BACK_MAX;    // handle->peDetRec[t].fcDet
    retVal    +=    sizeof( STB_PE_FACE ) * nTraCntMax                    ;    // peRes.peFace
    retVal    +=    sizeof( STBExecFlg  )                                ;    // execFlg

    return retVal;
}
/*------------------------------------------------------------------------------------------------------------------*/
/*    PeSharePeSize                                                                                                        */
/*------------------------------------------------------------------------------------------------------------------*/
void PeSharePeSize ( PEHANDLE    handle )
{

    STB_UINT32    t;
    STB_INT8    *stbPtr        = handle->pePtr        ;
    STB_UINT32    nTraCntMax    = handle->peCntMax;

    handle->peDet.fcDet        =    ( FACE_DET*        ) stbPtr;        stbPtr    += ( sizeof( FACE_DET    ) * nTraCntMax    );
    handle->peDetRec        =    ( STB_PE_DET*    ) stbPtr;        stbPtr    += ( sizeof( STB_PE_DET    ) * STB_PE_BACK_MAX);
    for( t = 0 ; t < STB_PE_BACK_MAX ; t++ )
    {
        handle->peDetRec[t].fcDet = ( FACE_DET*    ) stbPtr;        stbPtr    += ( sizeof( FACE_DET    ) * nTraCntMax    );
    }
    handle->peRes.peFace    =    ( STB_PE_FACE*    ) stbPtr;        stbPtr    += ( sizeof( STB_PE_FACE) * nTraCntMax    );
    handle->execFlg            =    ( STBExecFlg*    ) stbPtr;        stbPtr    += ( sizeof( STBExecFlg    )                );

}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*Create handle*/
PEHANDLE PeCreateHandle(  const STBExecFlg* execFlg ,const STB_INT32 nTraCntMax  ){
    
    PEHANDLE    handle;
    STB_INT32    t , i ,j;
    STB_INT32    tmpVal    ;
    STB_INT32    nRet    ;

    nRet = PeIsValidPointer(execFlg);
    if(nRet != STB_NORMAL)
    {
        return NULL;
    }

    if( nTraCntMax < 1 || STB_PE_TRA_CNT_MAX < nTraCntMax )
    {
        return NULL;
    }



    /*do handle's Malloc here*/
    handle = ( PEHANDLE )malloc( sizeof(*handle) );
    if(handle == NULL)
    {
        return NULL;
    }

    /*initial value---------------------------------------------------------------------*/
    handle->peFaceDirUDMin    = STB_PE_DIR_MIN_UD_INI;//The face on top/down allowable range min.
    handle->peFaceDirUDMax    = STB_PE_DIR_MAX_UD_INI;//The face on top/down allowable range max.
    handle->peFaceDirLRMin    = STB_PE_DIR_MIN_LR_INI;//The face on left /right side allowable range min.
    handle->peFaceDirLRMax    = STB_PE_DIR_MAX_LR_INI;//The face on left /right side allowable range max.
    handle->peFaceDirThr    = STB_PE_DIR_THR_INI    ;//If the confidence of Face direction estimation doesn't exceed the reference value, the recognition result isn't trusted.
    handle->peFrameCount    = STB_PE_FRAME_CNT_INI    ;
    handle->peCntMax        = nTraCntMax        ;//Maximum number of tracking people
    handle->pePtr            = NULL;
    handle->peDet.num        = 0;
    handle->peDet.fcDet        = NULL;
    handle->peDetRec        = NULL;
    handle->peRes.peCnt        = 0;
    handle->peRes.peFace    = NULL;
    handle->execFlg            = NULL;

    tmpVal            = PeCalcPeSize (    nTraCntMax        );    /*calculate necessary amount in the Pe handle*/
    handle->pePtr    = NULL;
    handle->pePtr    = ( STB_INT8 * )malloc( tmpVal    );    /*keeping necessary amount in the Pe handle*/
    if( handle->pePtr == NULL )
    {
        free ( handle->pePtr    );
        free ( handle            );
        return NULL;
    }

    /*Malloc-area is allocated to things that need Malloc in TR handle*/
    PeSharePeSize ( handle     );

    for( t = 0 ; t < STB_PE_BACK_MAX ;  t++ ) 
    {
        handle->peDetRec [ t ].num = 0;
        for( i = 0 ; i < handle->peCntMax ;  i++ ) 
        {
            handle->peDetRec[t].fcDet[i].nDetID            = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].nTraID            = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].dirDetRoll        = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].dirDetPitch    = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].dirDetYaw        = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].dirDetConf        = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].ageDetVal        = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].ageStatus        = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].ageDetConf        = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].genDetVal        = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].genStatus        = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].genDetConf        = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].gazDetLR        = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].gazDetUD        = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].bliDetL        = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].bliDetR        = STB_STATUS_NO_DATA            ;

            handle->peDetRec[t].fcDet[i].expDetConf        = STB_STATUS_NO_DATA            ;
            for( j = 0 ; j < STB_EX_MAX ; j++)
            {
                handle->peDetRec[t].fcDet[i].expDetVal[ j ]    = STB_STATUS_NO_DATA;
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
STB_INT32 PeDeleteHandle(PEHANDLE handle){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = PeIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    free ( handle->pePtr    );
    free ( handle            );

    return STB_NORMAL;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*Set the result*/
STB_INT32 PeSetDetect(PEHANDLE handle,const STB_PE_DET *stbPeDet){

    STB_INT32 nRet;
    STB_INT32 i,j;

    /*NULL check*/
    nRet = PeIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    nRet = PeIsValidPointer(stbPeDet);
    if(nRet != STB_NORMAL){
        return nRet;
    }

    /*Input value check*/
    nRet = STB_PeIsValidValue ( stbPeDet ,handle->execFlg );
    if(nRet != STB_TRUE)
    {
        return STB_ERR_INVALIDPARAM;
    }


    /*Set the received result to the handle*/
    /* Face */
    if( stbPeDet->num > handle->peCntMax )
    {
        return STB_ERR_PROCESSCONDITION;
    }


    handle->peDet.num = stbPeDet->num;
    for( i = 0 ; i < handle->peDet.num ; i++ )
    {
        handle->peDet.fcDet[i].nDetID            = stbPeDet->fcDet[i].nDetID            ;
        handle->peDet.fcDet[i].nTraID            = stbPeDet->fcDet[i].nTraID            ;
        if( handle->execFlg->gen == STB_TRUE )
        {
            handle->peDet.fcDet[i].genDetVal        = stbPeDet->fcDet[i].genDetVal        ;
            handle->peDet.fcDet[i].genDetConf        = stbPeDet->fcDet[i].genDetConf        ;
            handle->peDet.fcDet[i].genStatus        = STB_STATUS_NO_DATA                ;
        }
        if( handle->execFlg->age == STB_TRUE )
        {
            handle->peDet.fcDet[i].ageDetVal        = stbPeDet->fcDet[i].ageDetVal        ;
            handle->peDet.fcDet[i].ageDetConf        = stbPeDet->fcDet[i].ageDetConf        ;
            handle->peDet.fcDet[i].ageStatus        = STB_STATUS_NO_DATA                ;
        }
        if( handle->execFlg->exp == STB_TRUE )
        {
            handle->peDet.fcDet[i].expDetConf        = stbPeDet->fcDet[i].expDetConf        ;
            for( j = 0 ; j < STB_EX_MAX ; j++)
            {
                handle->peDet.fcDet[i].expDetVal[ j ]  = stbPeDet->fcDet[i].expDetVal[ j ];
            }
        }
        if( handle->execFlg->gaz == STB_TRUE )
        {
            handle->peDet.fcDet[i].gazDetLR            = stbPeDet->fcDet[i].gazDetLR        ;
            handle->peDet.fcDet[i].gazDetUD            = stbPeDet->fcDet[i].gazDetUD        ;
        }
        //if( handle->execFlg->dir == STB_TRUE )// dir is obligation.
        //{
            handle->peDet.fcDet[i].dirDetRoll        = stbPeDet->fcDet[i].dirDetRoll        ;
            handle->peDet.fcDet[i].dirDetPitch        = stbPeDet->fcDet[i].dirDetPitch    ;
            handle->peDet.fcDet[i].dirDetYaw        = stbPeDet->fcDet[i].dirDetYaw        ;
            handle->peDet.fcDet[i].dirDetConf        = stbPeDet->fcDet[i].dirDetConf        ;
        //}
        if( handle->execFlg->bli == STB_TRUE )
        {
            handle->peDet.fcDet[i].bliDetL            = stbPeDet->fcDet[i].bliDetL        ;
            handle->peDet.fcDet[i].bliDetR            = stbPeDet->fcDet[i].bliDetR        ;
        }
    }


    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*Main process execution*/
STB_INT32 PeExecute(PEHANDLE handle){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = PeIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    /*Main processing here*/
    nRet = StbPeExec ( handle );


    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
/*Get-Function of results*/
STB_INT32 PeGetResult( PEHANDLE handle, STB_PE_RES* peResult){

    STB_INT32 nRet;
    STB_INT32 i;

    /*NULL check*/
    nRet = PeIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    nRet = PeIsValidPointer(peResult);
    if(nRet != STB_NORMAL){
        return nRet;
    }

    /*Get result from handle*/
    peResult->peCnt = handle->peRes.peCnt ;
    for( i = 0 ; i < peResult->peCnt ; i++ )
    {
        peResult->peFace[i].nTraID            = handle->peRes.peFace[i].nTraID            ;
        peResult->peFace[i].gen.status        = handle->peRes.peFace[i].gen.status        ;
        peResult->peFace[i].gen.value        = handle->peRes.peFace[i].gen.value            ;
        peResult->peFace[i].gen.conf        = handle->peRes.peFace[i].gen.conf            ;
        peResult->peFace[i].age.status        = handle->peRes.peFace[i].age.status        ;
        peResult->peFace[i].age.value        = handle->peRes.peFace[i].age.value            ;
        peResult->peFace[i].age.conf        = handle->peRes.peFace[i].age.conf            ;
        peResult->peFace[i].exp.status        = handle->peRes.peFace[i].exp.status        ;
        peResult->peFace[i].exp.value        = handle->peRes.peFace[i].exp.value            ;
        peResult->peFace[i].exp.conf        = handle->peRes.peFace[i].exp.conf            ;
        peResult->peFace[i].gaz.status        = handle->peRes.peFace[i].gaz.status        ;
        peResult->peFace[i].gaz.LR            = handle->peRes.peFace[i].gaz.LR            ;
        peResult->peFace[i].gaz.UD            = handle->peRes.peFace[i].gaz.UD            ;
        peResult->peFace[i].gaz.conf        = handle->peRes.peFace[i].gaz.conf            ;
        peResult->peFace[i].dir.status        = handle->peRes.peFace[i].dir.status        ;
        peResult->peFace[i].dir.pitch        = handle->peRes.peFace[i].dir.pitch            ;
        peResult->peFace[i].dir.roll        = handle->peRes.peFace[i].dir.roll            ;
        peResult->peFace[i].dir.yaw            = handle->peRes.peFace[i].dir.yaw            ;
        peResult->peFace[i].dir.conf        = handle->peRes.peFace[i].dir.conf            ;
        peResult->peFace[i].bli.status        = handle->peRes.peFace[i].bli.status        ;
        peResult->peFace[i].bli.ratioL        = handle->peRes.peFace[i].bli.ratioL        ;
        peResult->peFace[i].bli.ratioR        = handle->peRes.peFace[i].bli.ratioR        ;
    }



    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32 PeSetFaceDirMinMax(PEHANDLE handle , STB_INT32 nMinUDAngle    , STB_INT32 nMaxUDAngle        ,STB_INT32 nMinLRAngle    , STB_INT32 nMaxLRAngle )
{

    STB_INT32 nRet;
    nRet = PeIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    
    if( nMinUDAngle < STB_PE_DIR_MIN_UD_MIN || STB_PE_DIR_MIN_UD_MAX < nMinUDAngle)
    {
        return STB_ERR_INVALIDPARAM;
    }
    if( nMaxUDAngle < STB_PE_DIR_MAX_UD_MIN || STB_PE_DIR_MAX_UD_MAX < nMaxUDAngle)
    {
        return STB_ERR_INVALIDPARAM;
    }
    if( nMaxUDAngle < nMinUDAngle)
    {
        return STB_ERR_INVALIDPARAM;
    }


    if( nMinLRAngle < STB_PE_DIR_MIN_LR_MIN || STB_PE_DIR_MIN_LR_MAX < nMinLRAngle)
    {
        return STB_ERR_INVALIDPARAM;
    }
    if( nMaxLRAngle < STB_PE_DIR_MAX_LR_MIN || STB_PE_DIR_MAX_LR_MAX < nMaxLRAngle)
    {
        return STB_ERR_INVALIDPARAM;
    }
    if( nMaxLRAngle < nMinLRAngle)
    {
        return STB_ERR_INVALIDPARAM;
    }

    handle->peFaceDirUDMin = nMinUDAngle;
    handle->peFaceDirUDMax = nMaxUDAngle;
    handle->peFaceDirLRMin = nMinLRAngle;
    handle->peFaceDirLRMax = nMaxLRAngle;


    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32 PeGetFaceDirMinMax(PEHANDLE handle ,STB_INT32 *pnMinUDAngle, STB_INT32 *pnMaxUDAngle    ,STB_INT32 *pnMinLRAngle, STB_INT32 *pnMaxLRAngle )
{
    STB_INT32 nRet;
    nRet = PeIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    nRet = PeIsValidPointer(pnMinUDAngle);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    nRet = PeIsValidPointer(pnMaxUDAngle);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    nRet = PeIsValidPointer(pnMinLRAngle);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }    
    nRet = PeIsValidPointer(pnMaxLRAngle);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    
    *pnMinUDAngle = handle->peFaceDirUDMin ;
    *pnMaxUDAngle = handle->peFaceDirUDMax ;

    *pnMinLRAngle = handle->peFaceDirLRMin ;
    *pnMaxLRAngle = handle->peFaceDirLRMax ;

    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32    PeClear            ( PEHANDLE handle  )
{
    //clear processing
    STB_INT32 t , i ,j;
    STB_INT32 nRet;

    /*NULL check*/
    nRet = PeIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    for( t = 0 ; t < STB_PE_BACK_MAX ;  t++ ) 
    {
        handle->peDetRec [ t ].num = 0;
        for( i = 0 ; i < handle->peCntMax ;  i++ ) 
        {
            handle->peDetRec[t].fcDet[i].nDetID            = STB_STATUS_NO_DATA            ;
            handle->peDetRec[t].fcDet[i].nTraID            = STB_STATUS_NO_DATA            ;
            if( handle->execFlg->dir == STB_TRUE )
            {
                handle->peDetRec[t].fcDet[i].dirDetRoll        = STB_STATUS_NO_DATA            ;
                handle->peDetRec[t].fcDet[i].dirDetPitch    = STB_STATUS_NO_DATA            ;
                handle->peDetRec[t].fcDet[i].dirDetYaw        = STB_STATUS_NO_DATA            ;
                handle->peDetRec[t].fcDet[i].dirDetConf        = STB_STATUS_NO_DATA            ;
            }
            if( handle->execFlg->age == STB_TRUE )
            {
                handle->peDetRec[t].fcDet[i].ageDetVal        = STB_STATUS_NO_DATA            ;
                handle->peDetRec[t].fcDet[i].ageStatus        = STB_STATUS_NO_DATA            ;
                handle->peDetRec[t].fcDet[i].ageDetConf        = STB_STATUS_NO_DATA            ;
            }
            if( handle->execFlg->gen == STB_TRUE )
            {
                handle->peDetRec[t].fcDet[i].genDetVal        = STB_STATUS_NO_DATA            ;
                handle->peDetRec[t].fcDet[i].genStatus        = STB_STATUS_NO_DATA            ;
                handle->peDetRec[t].fcDet[i].genDetConf        = STB_STATUS_NO_DATA            ;
            }
            if( handle->execFlg->gaz == STB_TRUE )
            {
                handle->peDetRec[t].fcDet[i].gazDetLR        = STB_STATUS_NO_DATA            ;
                handle->peDetRec[t].fcDet[i].gazDetUD        = STB_STATUS_NO_DATA            ;
            }
            if( handle->execFlg->bli == STB_TRUE )
            {
                handle->peDetRec[t].fcDet[i].bliDetL        = STB_STATUS_NO_DATA            ;
                handle->peDetRec[t].fcDet[i].bliDetR        = STB_STATUS_NO_DATA            ;
            }
            if( handle->execFlg->exp == STB_TRUE )
            {
                handle->peDetRec[t].fcDet[i].expDetConf        = STB_STATUS_NO_DATA            ;
                for( j = 0 ; j < STB_EX_MAX ; j++)
                {
                    handle->peDetRec[t].fcDet[i].expDetVal[j]    = STB_STATUS_NO_DATA            ;
                }
            }
        }
    }

    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32 PeSetFaceDirThreshold(PEHANDLE handle , STB_INT32 threshold )
{
    STB_INT32 nRet;
    nRet = PeIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    if( threshold < STB_PE_DIR_THR_MIN || STB_PE_DIR_THR_MAX < threshold ){
        return STB_ERR_INVALIDPARAM;
    }

    handle->peFaceDirThr = threshold;
    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32 PeGetFaceDirThreshold(PEHANDLE handle , STB_INT32* threshold )
{
    STB_INT32 nRet;
    nRet = PeIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    nRet = PeIsValidPointer(threshold);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    
    *threshold = handle->peFaceDirThr ;
    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32 PeSetFrameCount(PEHANDLE handle , STB_INT32 nFrameCount )
{
    
    STB_INT32 nRet;
    nRet = PeIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    
    if( nFrameCount < 1 || nFrameCount > STB_PE_BACK_MAX  )
    {
        return STB_ERR_INVALIDPARAM;
    }

    handle->peFrameCount = nFrameCount;
    return STB_NORMAL;
}
/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
STB_INT32 PeGetFrameCount(PEHANDLE handle , STB_INT32* nFrameCount )
{
    STB_INT32 nRet;
    nRet = PeIsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    nRet = PeIsValidPointer(nFrameCount);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    
    *nFrameCount = handle->peFrameCount ;
    return STB_NORMAL;
}