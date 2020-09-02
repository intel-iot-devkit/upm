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

#include "Interface.h"
#include "STBValidValue.h"
#include "STBCommonDef.h"
#include "STBTracking.h"
#include "STBFaceInfo.h"
#include "STBMakeResult.h"


/*Value range check*/
#define ISVALID_RANGE( val , min , max )    ( ( (min) <= (val) ) && ( (val) <= (max) ) )


/*------------------------------------------------------------------------------------------------------------------*/
/*IsValidValue : error check*/
/*------------------------------------------------------------------------------------------------------------------*/
static STB_INT32 IsValidValue(
    const STB_INT32 nValue ,
    const STB_INT32 nLimitMin ,
    const STB_INT32 nLimitMax )
{
    STB_INT32 nRet;
    for( nRet = STB_ERR_INVALIDPARAM; nRet != STB_NORMAL; nRet = STB_NORMAL )
    {
        if( ! ISVALID_RANGE( nValue , nLimitMin , nLimitMax ) )
        {    
            break; 
        }
    }
    return nRet;
}
/*------------------------------------------------------------------------------------------------------------------*/
/* IsValidPointer                                                                                                    */
/*------------------------------------------------------------------------------------------------------------------*/
static STB_INT32 IsValidPointer( const VOID* pPointer )    
{
    STB_INT32 nRet;
    for( nRet = STB_ERR_INVALIDPARAM; nRet != STB_NORMAL; nRet = STB_NORMAL )
    {
        if( NULL == pPointer ){ break; }
    }
    return nRet;
}

/*------------------------------------------------------------------------------------------------------------------*/
/*    GetVersion                                                                                                    */
/*            Interface between SDK layer and functional layer                                                      */
/*            Responsible for error check and function call                                                         */
/*------------------------------------------------------------------------------------------------------------------*/
STB_INT32 GetVersion( STB_INT8* pnMajorVersion , STB_INT8* pnMinorVersion ){
    STB_INT32 nRet;

    for( nRet = STB_ERR_INVALIDPARAM; nRet != STB_NORMAL; nRet = STB_NORMAL )
    {
        nRet = IsValidPointer( pnMajorVersion );
        if( STB_NORMAL != nRet )
        {
            break;
        }
        nRet = IsValidPointer( pnMinorVersion );
        if( STB_NORMAL != nRet )
        {
            break;
        }
        *pnMajorVersion = VERSION_MAJOR;
        *pnMinorVersion = VERSION_MINOR;
    }

    return nRet;
}
/*------------------------------------------------------------------------------------------------------------------*/
/*    CalcStbSize                                                                                                        */
/*------------------------------------------------------------------------------------------------------------------*/
STB_UINT32 CalcStbSize ( STBExecFlg *execFlg , STB_UINT32 nTraCntMax)
{
    STB_UINT32 retVal ;

    retVal = 0 ;

    retVal += 100 ;///Margin  : alignment




    retVal    +=    sizeof( STB_TR_DET        );//wSrcTr
    if( execFlg->bodyTr == STB_TRUE )
    {
        retVal    +=    sizeof( TraObj        ) * nTraCntMax    ;    // trBody
        retVal    +=    sizeof( ROI_DET        ) * nTraCntMax    ;    // wSrcTr->bdDet
        retVal    +=    sizeof( STB_TR_RES_BODYS    )        ;    // wDstTrBody
        retVal    +=    sizeof( STB_TR_RES    ) * nTraCntMax    ;    // wDstTrBody->body
    }
    if( execFlg->faceTr == STB_TRUE )
    {
        retVal    +=    sizeof( TraObj        ) * nTraCntMax    ;    // trFace
        retVal    +=    sizeof( ROI_DET        ) * nTraCntMax    ;    // wSrcTr->fcDet
        retVal    +=    sizeof( STB_TR_RES_FACES    )        ;    // wDstTrFace
        retVal    +=    sizeof( STB_TR_RES    ) * nTraCntMax    ;    // wDstTrFace->face
    }
    if(        execFlg->gen    == STB_TRUE     
        ||    execFlg->age    == STB_TRUE 
        ||    execFlg->fr        == STB_TRUE 
        ||    execFlg->exp    == STB_TRUE     
        ||    execFlg->dir    == STB_TRUE     
        ||    execFlg->gaz    == STB_TRUE 
        ||    execFlg->bli    == STB_TRUE 
        )
    {
        retVal    +=    sizeof( FaceObj        ) * nTraCntMax    ;    // infoFace
    }


    if(        execFlg->gen    == STB_TRUE     
        ||    execFlg->age    == STB_TRUE 
        //||    execFlg->fr    == STB_TRUE     
        ||    execFlg->exp    == STB_TRUE     
        ||    execFlg->dir    == STB_TRUE     
        ||    execFlg->gaz    == STB_TRUE 
        ||    execFlg->bli    == STB_TRUE 
        )
    {
        retVal    +=    sizeof( STB_PE_DET    )                ;    // wSrcPe
        retVal    +=    sizeof( FACE_DET    ) * nTraCntMax    ;    // wSrcPe->fcDet
        retVal    +=    sizeof( STB_PE_RES    )                ;    // wDstPe
        retVal    +=    sizeof( STB_PE_FACE    ) * nTraCntMax    ;    // wDstPe->peFace

    }

    if(    execFlg->fr    == STB_TRUE )
    {
        retVal    +=    sizeof( STB_FR_DET    )                ;    // wSrcFr
        retVal    +=    sizeof( FR_DET        ) * nTraCntMax    ;    // wSrcFr->fcDet
        retVal    +=    sizeof( STB_FR_RES    )                ;    // wDstFr
        retVal    +=    sizeof( FR_RES        ) * nTraCntMax    ;    // wDstFr->frFace
    }
    

    return retVal;
}
/*------------------------------------------------------------------------------------------------------------------*/
/*    ShareStbSize                                                                                                    */
/*------------------------------------------------------------------------------------------------------------------*/
void ShareStbSize ( STBHANDLE    handle , STB_INT8    *stbPtr )
{
    STB_UINT32    nTraCntMax;

    nTraCntMax = handle->nTraCntMax;


    handle->wSrcTr = (STB_TR_DET*)stbPtr;        stbPtr    += ( sizeof( STB_TR_DET    ) );
    if( handle->execFlg->bodyTr == STB_TRUE )
    {
        handle->trBody                = ( TraObj*                ) stbPtr;        stbPtr    += ( sizeof( TraObj                ) * nTraCntMax );
        handle->wSrcTr->bdDet        = ( ROI_DET*            ) stbPtr;        stbPtr    += ( sizeof( ROI_DET            ) * nTraCntMax );
        handle->wDstTrBody            = ( STB_TR_RES_BODYS*    ) stbPtr;        stbPtr    += ( sizeof( STB_TR_RES_BODYS    ));
        handle->wDstTrBody->body    = ( STB_TR_RES*            ) stbPtr;        stbPtr    += ( sizeof( STB_TR_RES            ) * nTraCntMax );
    }
    if( handle->execFlg->faceTr == STB_TRUE )
    {
        handle->trFace                = ( TraObj*                ) stbPtr;        stbPtr    += ( sizeof( TraObj                ) * nTraCntMax );
        handle->wSrcTr->fcDet        = ( ROI_DET*            ) stbPtr;        stbPtr    += ( sizeof( ROI_DET            ) * nTraCntMax );
        handle->wDstTrFace            = ( STB_TR_RES_FACES*    ) stbPtr;        stbPtr    += ( sizeof( STB_TR_RES_FACES    ));
        handle->wDstTrFace->face    = ( STB_TR_RES*            ) stbPtr;        stbPtr    += ( sizeof( STB_TR_RES            ) * nTraCntMax );
    }

    if(        handle->execFlg->gen    == STB_TRUE     
        ||    handle->execFlg->age    == STB_TRUE 
        ||    handle->execFlg->fr        == STB_TRUE 
        ||    handle->execFlg->exp    == STB_TRUE     
        ||    handle->execFlg->dir    == STB_TRUE     
        ||    handle->execFlg->gaz    == STB_TRUE 
        ||    handle->execFlg->bli    == STB_TRUE 
        )
    {
        handle->infoFace            = ( FaceObj*    ) stbPtr;        stbPtr    += ( sizeof( FaceObj    ) * nTraCntMax );
    }

    if(        handle->execFlg->gen    == STB_TRUE     
        ||    handle->execFlg->age    == STB_TRUE 
        //|||    handle->execFlg->fr    == STB_TRUE     
        ||    handle->execFlg->exp    == STB_TRUE     
        ||    handle->execFlg->dir    == STB_TRUE     
        ||    handle->execFlg->gaz    == STB_TRUE 
        ||    handle->execFlg->bli    == STB_TRUE 
        )
    {
        handle->wSrcPe                = ( STB_PE_DET*    ) stbPtr;        stbPtr    += ( sizeof( STB_PE_DET    ) );
        handle->wSrcPe->fcDet        = ( FACE_DET*    ) stbPtr;        stbPtr    += ( sizeof( FACE_DET    ) * nTraCntMax );
        handle->wDstPe                = ( STB_PE_RES*    ) stbPtr;        stbPtr    += ( sizeof( STB_PE_RES    )  );
        handle->wDstPe->peFace        = ( STB_PE_FACE*) stbPtr;        stbPtr    += ( sizeof( STB_PE_FACE) * nTraCntMax );
    }

    if(    handle->execFlg->fr        == STB_TRUE )
    {
        handle->wSrcFr                = ( STB_FR_DET*    ) stbPtr;        stbPtr    += ( sizeof( STB_FR_DET    ) );
        handle->wSrcFr->fcDet        = ( FR_DET*        ) stbPtr;        stbPtr    += ( sizeof( FR_DET        ) * nTraCntMax );
        handle->wDstFr                = ( STB_FR_RES*    ) stbPtr;        stbPtr    += ( sizeof( STB_FR_RES    ) );
        handle->wDstFr->frFace        = ( FR_RES*        ) stbPtr;        stbPtr    += ( sizeof( FR_RES        ) * nTraCntMax );
    }


}
/*------------------------------------------------------------------------------------------------------------------*/
/*Create handle*/
/*------------------------------------------------------------------------------------------------------------------*/
STBHANDLE CreateHandle ( STB_UINT32 stbExecFlg )
{

    STBHANDLE        handle;
    STB_UINT32        tmpVal;
    STB_UINT32        tmpFLG;
    


    /*do STB handle's malloc here*/
    handle = NULL ;
    handle = ( STBHANDLE )malloc( sizeof( *handle ) );
    if( handle == NULL )
    {
        return NULL;/* FAIL : Create STB handle */
    }

    /* ExecFlg */
    handle->execFlg = ( STBExecFlg* )malloc( sizeof( STBExecFlg ) );
    if( handle->execFlg == NULL )
    {
        free ( handle )    ;/*Free of Malloc things at the present time*/
        return NULL        ;/* FAIL : Create STB handle ExecFlg */
    }
    if( ( stbExecFlg & STB_FUNC_BD )== STB_FUNC_BD ){    handle->execFlg->bodyTr    = STB_TRUE ;}else{    handle->execFlg->bodyTr    = STB_FALSE ;}
    if( ( stbExecFlg & STB_FUNC_DT )== STB_FUNC_DT ){    handle->execFlg->faceTr    = STB_TRUE ;}else{    handle->execFlg->faceTr    = STB_FALSE ;}
    if( ( stbExecFlg & STB_FUNC_PT )== STB_FUNC_PT ){    handle->execFlg->dir    = STB_TRUE ;}else{    handle->execFlg->dir    = STB_FALSE ;}
    if( ( stbExecFlg & STB_FUNC_AG )== STB_FUNC_AG ){    handle->execFlg->age    = STB_TRUE ;}else{    handle->execFlg->age    = STB_FALSE ;}
    if( ( stbExecFlg & STB_FUNC_GN )== STB_FUNC_GN ){    handle->execFlg->gen    = STB_TRUE ;}else{    handle->execFlg->gen    = STB_FALSE ;}
    if( ( stbExecFlg & STB_FUNC_GZ )== STB_FUNC_GZ ){    handle->execFlg->gaz    = STB_TRUE ;}else{    handle->execFlg->gaz    = STB_FALSE ;}
    if( ( stbExecFlg & STB_FUNC_BL )== STB_FUNC_BL ){    handle->execFlg->bli    = STB_TRUE ;}else{    handle->execFlg->bli    = STB_FALSE ;}
    if( ( stbExecFlg & STB_FUNC_EX )== STB_FUNC_EX ){    handle->execFlg->exp    = STB_TRUE ;}else{    handle->execFlg->exp    = STB_FALSE ;}
    if( ( stbExecFlg & STB_FUNC_FR )== STB_FUNC_FR ){    handle->execFlg->fr        = STB_TRUE ;}else{    handle->execFlg->fr        = STB_FALSE ;}
    handle->execFlg->pet    = STB_FALSE ;
    handle->execFlg->hand    = STB_FALSE ;


    if( handle->execFlg->faceTr == STB_FALSE )
    {
        if(        handle->execFlg->gen    == STB_TRUE     
            ||    handle->execFlg->age    == STB_TRUE 
            ||    handle->execFlg->fr        == STB_TRUE 
            ||    handle->execFlg->exp    == STB_TRUE     
            ||    handle->execFlg->dir    == STB_TRUE     
            ||    handle->execFlg->gaz    == STB_TRUE 
            ||    handle->execFlg->bli    == STB_TRUE 
            )
        {
            free ( handle->execFlg    );/*Free of Malloc things at the present time*/
            free ( handle            );/*Free of Malloc things at the present time*/
            return NULL        ;/*Invalid input parameter stbExecFlg*/
        }
    }


    /*Setting the initial value here.*/
    handle->nTraCntBody    = 0;
    handle->nTraCntFace    = 0;
    handle->nDetCntMax    = DETECT_CNT_MAX    ;/*A maximum number of detected(input) people*/
    handle->nTraCntMax    = TRACK_CNT_MAX        ;/*A maximum number of tracking(output) people*/
    handle->nExecuted   = STB_FALSE            ;
    handle->nInitialized= STB_FALSE            ;
    handle->nDetCntBody    = 0;
    handle->nDetCntFace    = 0;
    handle->trFace        = NULL;
    handle->trBody        = NULL;
    handle->infoFace    = NULL;
    handle->wSrcTr        = NULL;
    handle->wDstTrFace    = NULL;
    handle->wDstTrBody    = NULL;
    handle->wSrcPe        = NULL;
    handle->wDstPe        = NULL;
    handle->wSrcFr        = NULL;
    handle->wDstFr        = NULL;


    /* Do Malloc to things that need Malloc in STB handle */
    handle->stbPtr        = NULL    ;
    handle->hTrHandle    = NULL    ;
    handle->hPeHandle    = NULL    ;
    handle->hFrHandle    = NULL    ;
    tmpVal                = CalcStbSize ( handle->execFlg ,handle->nTraCntMax        );    /*calculate necessary amount in the STB handle*/
    handle->stbPtr        = ( STB_INT8 * )malloc( tmpVal    )                        ;    /*keep necessary amount in the STB handle*/
    if( handle->stbPtr    == NULL )
    {
            free ( handle->execFlg    );/*Free of Malloc things at the present time*/
            free ( handle            );/*Free of Malloc things at the present time*/
            return NULL        ;
    }
    ShareStbSize ( handle, handle->stbPtr);                                            /* Malloc-area is allocated to things that need Malloc in STB handle */

    /*Create handles for child functions*/
    tmpFLG = STB_TRUE;
    if(        handle->execFlg->bodyTr    == STB_TRUE 
        ||    handle->execFlg->faceTr    == STB_TRUE 
        )
    {
        handle->hTrHandle    = STB_Tr_CreateHandle( handle->execFlg ,handle->nDetCntMax, handle->nTraCntMax    );    
        if( handle->hTrHandle == NULL ){ tmpFLG = STB_FALSE; }
    }
    if(        handle->execFlg->gen    == STB_TRUE     
        ||    handle->execFlg->age    == STB_TRUE 
        //||    handle->execFlg->fr    == STB_TRUE     
        ||    handle->execFlg->exp    == STB_TRUE     
        ||    handle->execFlg->dir    == STB_TRUE     
        ||    handle->execFlg->gaz    == STB_TRUE 
        ||    handle->execFlg->bli    == STB_TRUE 
        )
    {
        handle->hPeHandle    = STB_Pe_CreateHandle( handle->execFlg ,handle->nTraCntMax                        );    
        if( handle->hPeHandle == NULL ){ tmpFLG = STB_FALSE; }
    }
    if(    handle->execFlg->fr    == STB_TRUE )
    {
        handle->hFrHandle    = STB_Fr_CreateHandle( handle->nTraCntMax                                        );    
        if( handle->hFrHandle == NULL ){ tmpFLG = STB_FALSE; }
    }

    if(    tmpFLG == STB_FALSE    )
    {
        /*When Malloc failed, Free of Malloc data at the present time*/
        if( handle->hTrHandle        != NULL )    {    STB_Tr_DeleteHandle    ( handle->hTrHandle )    ;}
        if( handle->hPeHandle        != NULL )    {    STB_Pe_DeleteHandle    ( handle->hPeHandle )    ;}
        if( handle->hFrHandle        != NULL )    {    STB_Fr_DeleteHandle    ( handle->hFrHandle )    ;}
        if( handle->stbPtr            != NULL )    {    free                ( handle->stbPtr    )    ;}/*Free of Malloc things at the present time*/
        if( handle->execFlg            != NULL )    {    free                ( handle->execFlg    )    ;}/*Free of Malloc things at the present time*/
        if( handle                    != NULL )    {    free                ( handle            )    ;}/*Free of Malloc things at the present time*/
        return NULL;
    }

    return handle;
}
//-------------------------------------------------------------------------------------------------------------------
// DeleteHandle            /*Delete handle*/
//-------------------------------------------------------------------------------------------------------------------
STB_INT32 DeleteHandle(STBHANDLE handle)
{

    STB_INT32 nRet;

    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    /*Malloc things here, do free*/
    if( handle->hTrHandle        != NULL )    {    STB_Tr_DeleteHandle    ( handle->hTrHandle )    ;}
    if( handle->hPeHandle        != NULL )    {    STB_Pe_DeleteHandle    ( handle->hPeHandle )    ;}
    if( handle->hFrHandle        != NULL )    {    STB_Fr_DeleteHandle    ( handle->hFrHandle )    ;}
    if( handle->stbPtr            != NULL )    {    free                ( handle->stbPtr    )    ;}
    if( handle->execFlg            != NULL )    {    free                ( handle->execFlg    )    ;}
    if( handle                    != NULL )    {    free                ( handle            )    ;}



    return STB_NORMAL;
}
/*------------------------------------------------------------------------------------------------------------------*/
/* SetFrameResult : Get the result of stbINPUT */
/*------------------------------------------------------------------------------------------------------------------*/
STB_INT32 SetFrameResult ( STBHANDLE handle    , const STB_FRAME_RESULT *stbINPUTResult    )
{

    STB_INT32 nRet;

    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL)
    {
        return STB_ERR_NOHANDLE;
    }

    nRet = IsValidPointer(stbINPUTResult);
    if(nRet != STB_NORMAL)
    {
        return STB_ERR_INVALIDPARAM;
    }


    /*Input value check*/
    nRet = STB_IsValidValue ( stbINPUTResult ,handle->execFlg );
    if(nRet != STB_TRUE)
    {
        return STB_ERR_INVALIDPARAM;
    }


    /*Clear the unexecuted state flag*/
    handle->nExecuted = STB_FALSE;

    /*Set the received result to the handle*/
    if( handle->execFlg->bodyTr == STB_TRUE )
    {
        handle->nDetCntBody = stbINPUTResult->bodys.nCount;
        SetTrackingObjectBody ( &(stbINPUTResult->bodys)    ,handle->trBody    );
    }
    if( handle->execFlg->faceTr == STB_TRUE )
    {
        handle->nDetCntFace = stbINPUTResult->faces.nCount;
        SetTrackingObjectFace ( &(stbINPUTResult->faces)    ,handle->trFace    );
    }

    /*Set detection result to Face/Property/Recognition data*/
    if(        handle->execFlg->gen    == STB_TRUE 
        ||    handle->execFlg->age    == STB_TRUE 
        ||    handle->execFlg->fr        == STB_TRUE 
        ||    handle->execFlg->exp    == STB_TRUE 
        ||    handle->execFlg->gaz    == STB_TRUE 
        ||    handle->execFlg->dir    == STB_TRUE 
        ||    handle->execFlg->bli    == STB_TRUE 
        )
    {
        SetFaceObject ( &(stbINPUTResult->faces)    ,handle->infoFace    ,handle->execFlg , handle->nTraCntMax );
    }


    handle->nInitialized = STB_TRUE;

    return STB_NORMAL;
}

/*------------------------------------------------------------------------------------------------------------------*/
/*Execute : Main process execution*/
/*------------------------------------------------------------------------------------------------------------------*/
STB_INT32 Execute ( STBHANDLE handle )
{
    STB_INT32            nRet        ;
    STB_TR_DET            *srcTr        = handle->wSrcTr    ;/*TR : input data*/
    STB_TR_RES_FACES    *dstTrFace    = handle->wDstTrFace;/*TR : output data*/
    STB_TR_RES_BODYS    *dstTrBody    = handle->wDstTrBody;/*TR : output data*/
    STB_PE_DET            *srcPe        = handle->wSrcPe    ;/*PR : Input data*/
    STB_PE_RES            *dstPe        = handle->wDstPe    ;/*PE : Output data*/
    STB_FR_DET            *srcFr        = handle->wSrcFr    ;/*FR : Input data*/
    STB_FR_RES            *dstFr        = handle->wDstFr    ;/*FR : Output data*/



    /*NULL check*/
    nRet = IsValidPointer ( handle );
    if( nRet != STB_NORMAL )
    {
        return STB_ERR_NOHANDLE;
    }

    if(    handle->nInitialized != STB_TRUE)
    {
        return STB_ERR_INITIALIZE;
    }
    handle->nInitialized = STB_FALSE;
    handle->nExecuted    = STB_FALSE;

    /* TR ------------------------------------------------------------------------------------------------*/
    if( handle->execFlg->faceTr    == STB_TRUE )
    {
        SetSrcTrFace ( handle->nDetCntFace , handle->trFace , srcTr );            /*Creation of tracking input data from handle information*/
    }
    if( handle->execFlg->bodyTr    == STB_TRUE )
    {
        SetSrcTrBody ( handle->nDetCntBody , handle->trBody , srcTr );            /*Creation of tracking input data from handle information*/
    }
    nRet = STB_Tr_SetDetect    ( handle->hTrHandle , srcTr);                        /*Frame information settings*/
    if( nRet != STB_NORMAL)    { return nRet;    }
    nRet = STB_Tr_Execute    ( handle->hTrHandle );                                /*execute tracking*/
    if( nRet != STB_NORMAL)    { return nRet;    }
    nRet = STB_Tr_GetResult    ( handle->hTrHandle , dstTrFace , dstTrBody );        /*get the tracking result*/
    if( nRet != STB_NORMAL)    { return nRet;    }
    if( handle->execFlg->faceTr    == STB_TRUE )
    {
        SetTrackingInfoToFace    ( dstTrFace,&(handle->nTraCntFace),handle->trFace);/*copy to handle the tracking result*/
    }
    if( handle->execFlg->bodyTr    == STB_TRUE )
    {
        SetTrackingInfoToBody    ( dstTrBody,&(handle->nTraCntBody),handle->trBody);/*copy to handle the tracking result*/
    }


    /*Association of face information and tracking ID--------------------------------------------------------------------------------*/
    if(        handle->execFlg->gen    == STB_TRUE 
        ||    handle->execFlg->age    == STB_TRUE 
        ||    handle->execFlg->fr        == STB_TRUE 
        ||    handle->execFlg->exp    == STB_TRUE 
        ||    handle->execFlg->gaz    == STB_TRUE 
        ||    handle->execFlg->dir    == STB_TRUE 
        ||    handle->execFlg->bli    == STB_TRUE 
        )
    {
        SetTrackingIDToFace        ( handle->nTraCntFace ,handle->nDetCntFace, handle->trFace,handle->infoFace , handle->execFlg );
    }

    /* Fr ------------------------------------------------------------------------------------------------*/
    if(        handle->execFlg->fr        == STB_TRUE )
    {
        SetFaceToFrInfo            ( handle->nTraCntFace,handle->infoFace,srcFr    );    /*Creation of recognition input data from handle information*/
        nRet = STB_Fr_SetDetect    ( handle->hFrHandle,srcFr                        );    /*Pass to the recognized stabilization*/
        if(nRet != STB_NORMAL    ){    return nRet;    }
        nRet = STB_Fr_Execute    ( handle->hFrHandle                                );    /* Recognized stabilization execution*/
        if(nRet != STB_NORMAL    ){    return nRet;    }
        nRet = STB_Fr_GetResult    ( handle->hFrHandle,dstFr                        );    /*get the recognized stabilization results*/
        if(nRet != STB_NORMAL    ){    return nRet;    }
        SetFrInfoToFace            ( handle->nTraCntFace,dstFr,handle->infoFace    );    /*Copy to handle the recognized stabilization results*/
    }

    /* Pe ------------------------------------------------------------------------------------------------*/
    if(        handle->execFlg->gen    == STB_TRUE 
        ||    handle->execFlg->age    == STB_TRUE 
        //||    handle->execFlg->fr    == STB_TRUE 
        ||    handle->execFlg->exp    == STB_TRUE 
        ||    handle->execFlg->gaz    == STB_TRUE 
        ||    handle->execFlg->dir    == STB_TRUE 
        ||    handle->execFlg->bli    == STB_TRUE 
        )
    {
        SetFaceToPeInfo            ( handle->nTraCntFace,handle->infoFace,srcPe    );    /*Creation of property input data from handle information*/
        nRet = STB_Pe_SetDetect    ( handle->hPeHandle,srcPe                        );    /*Pass to property stabilization*/
        if( nRet != STB_NORMAL    ){    return nRet;    }
        nRet = STB_Pe_Execute    ( handle->hPeHandle                                );    /*Property stabilization execution*/
        if( nRet != STB_NORMAL    ){    return nRet;    }
        nRet = STB_Pe_GetResult    ( handle->hPeHandle,dstPe                        );    /*get the property stabilization results*/
        if( nRet != STB_NORMAL    ){    return nRet;    }
        SetPeInfoToFace            ( handle->nTraCntFace,dstPe,handle->infoFace , handle->execFlg );    /*Copy to handle the property stabilization results*/
    }



    /*Set execution completion flag--------------------------------------------------*/
    handle->nExecuted = STB_TRUE;

    return STB_NORMAL;
}
/*------------------------------------------------------------------------------------------------------------------*/
/* GetFaces : Getting stabilization results of face*/
/*------------------------------------------------------------------------------------------------------------------*/
STB_INT32 GetFaces(STBHANDLE handle, STB_UINT32 *face_count, STB_FACE *face )
{
    STB_INT32 nRet , i;

    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL)
    {
        return STB_ERR_NOHANDLE;
    }
    nRet = IsValidPointer(face_count);
    if(nRet != STB_NORMAL)
    {
        return nRet;
    }
    nRet = IsValidPointer(face);
    if(nRet != STB_NORMAL)
    {
        return nRet;
    }
    if(    handle->nExecuted != STB_TRUE)
    {
        return STB_ERR_INITIALIZE;
    }

    /*init*/
    *face_count = 0;
    for( i = 0 ; i < handle->nTraCntMax ; i++ )
    {
        face[i].nDetectID                = -1;
        face[i].nTrackingID                = -1;
        face[i].center.x                = 0;
        face[i].center.y                = 0;
        face[i].nSize                    = 0;
        face[i].conf                    = STB_CONF_NO_DATA    ;
        face[i].age.conf                = STB_CONF_NO_DATA    ;
        face[i].age.status                = STB_STATUS_NO_DATA;
        face[i].age.value                = -1;
        face[i].blink.ratioL            = -1;
        face[i].blink.ratioR            = -1;
        face[i].blink.status            = STB_STATUS_NO_DATA;
        face[i].direction.conf            = STB_CONF_NO_DATA    ;
        face[i].direction.pitch            = -1;
        face[i].direction.roll            = -1;
        face[i].direction.status        = STB_STATUS_NO_DATA;
        face[i].direction.yaw            = -1;
        face[i].expression.conf            = STB_CONF_NO_DATA    ;
        face[i].expression.status        = STB_STATUS_NO_DATA;
        face[i].expression.value        = -1;
        face[i].gaze.conf                = STB_CONF_NO_DATA    ;
        face[i].gaze.LR                    = -1;
        face[i].gaze.status                = STB_STATUS_NO_DATA;
        face[i].gaze.UD                    = -1;
        face[i].gender.conf                = STB_CONF_NO_DATA    ;
        face[i].gender.status            = STB_STATUS_NO_DATA;
        face[i].gender.value            = -1;
        face[i].recognition.conf        = STB_CONF_NO_DATA    ;
        face[i].recognition.status        = STB_STATUS_NO_DATA;
        face[i].recognition.value        = -1;
    }

    /*Set the result to the structure*/
    if( handle->execFlg->faceTr == STB_TRUE )
    {
        *face_count = handle->nTraCntFace;
        SetFaceToResult ( handle->nTraCntFace ,handle->trFace ,handle->infoFace ,face , handle->execFlg );
    }
    
    return STB_NORMAL;
}
/*------------------------------------------------------------------------------------------------------------------*/
/* GetBodies : Getting stabilization results of body */
/*------------------------------------------------------------------------------------------------------------------*/
STB_INT32 GetBodies(STBHANDLE handle, STB_UINT32 *body_count, STB_BODY *body)
{
    STB_INT32 nRet , i;

    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    nRet = IsValidPointer(body_count);
    if(nRet != STB_NORMAL){
        return nRet;
    }
    nRet = IsValidPointer(body);
    if(nRet != STB_NORMAL){
        return nRet;
    }
    if(    handle->nExecuted != STB_TRUE){
        return STB_ERR_INITIALIZE;
    }


    /*init*/
    *body_count = 0;
    for( i = 0 ; i < handle->nTraCntMax ; i++ )
    {
        body[i].nDetectID        = -1;
        body[i].nTrackingID        = -1;
        body[i].center.x        = 0;
        body[i].center.y        = 0;
        body[i].nSize            = 0;
        body[i].conf            = STB_CONF_NO_DATA    ;
    }

    /*Set the result to the structure*/
    if( handle->execFlg->bodyTr == STB_TRUE )
    {
        *body_count = handle->nTraCntBody;
        SetBodyToResult(handle->nTraCntBody,handle->trBody, body);
    }
    return STB_NORMAL;
}
/*------------------------------------------------------------------------------------------------------------------*/
/* Clear                                                                                                            */
/*------------------------------------------------------------------------------------------------------------------*/
STB_INT32 Clear(STBHANDLE handle)
{
    STB_INT32 nRet;

    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    STB_Tr_Clear( handle->hTrHandle    );
    STB_Pe_Clear( handle->hPeHandle    );
    if( handle->execFlg->fr    == STB_TRUE )
    {
        STB_Fr_Clear( handle->hFrHandle    );
    }

    handle->nInitialized = STB_FALSE;
    handle->nExecuted     = STB_FALSE;


    return STB_NORMAL;

}
/*------------------------------------------------------------------------------------------------------------------*/
/*Setting function (wrapper for child libraries)*/
/*------------------------------------------------------------------------------------------------------------------*/
STB_INT32 SetTrackingRetryCount(STBHANDLE handle, STB_INT32 nMaxRetryCount){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    return STB_Tr_SetRetryCount(handle->hTrHandle,nMaxRetryCount);
}

STB_INT32 GetTrackingRetryCount(STBHANDLE handle, STB_INT32 *pnMaxRetryCount){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    return STB_Tr_GetRetryCount(handle->hTrHandle,pnMaxRetryCount);
}

STB_INT32 SetTrackingSteadinessParam(STBHANDLE handle, STB_INT32 nPosSteadinessParam, STB_INT32 nSizeSteadinessParam){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    return STB_Tr_SetStedinessParam(handle->hTrHandle, nPosSteadinessParam, nSizeSteadinessParam);
}

STB_INT32 GetTrackingSteadinessParam(STBHANDLE handle, STB_INT32 *pnPosSteadinessParam, STB_INT32 *pnSizeSteadinessParam){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    return STB_Tr_GetStedinessParam(handle->hTrHandle, pnPosSteadinessParam, pnSizeSteadinessParam);
}

STB_INT32 SetPropertyThreshold(STBHANDLE handle, STB_INT32 nThreshold){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }

    if(        handle->execFlg->gen    == STB_TRUE     
        ||    handle->execFlg->age    == STB_TRUE 
        ||    handle->execFlg->exp    == STB_TRUE     
        ||    handle->execFlg->dir    == STB_TRUE     
        ||    handle->execFlg->gaz    == STB_TRUE 
        ||    handle->execFlg->bli    == STB_TRUE 
        )
    {
    }else
    {
        return STB_NORMAL;
    }

    return STB_Pe_SetFaceDirThreshold(handle->hPeHandle, nThreshold);
}

STB_INT32 GetPropertyThreshold(STBHANDLE handle, STB_INT32 *pnThreshold){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    if(        handle->execFlg->gen    == STB_TRUE     
        ||    handle->execFlg->age    == STB_TRUE 
        ||    handle->execFlg->exp    == STB_TRUE     
        ||    handle->execFlg->dir    == STB_TRUE     
        ||    handle->execFlg->gaz    == STB_TRUE 
        ||    handle->execFlg->bli    == STB_TRUE 
        )
    {
    }else
    {
        return STB_NORMAL;
    }
    return STB_Pe_GetFaceDirThreshold(handle->hPeHandle, pnThreshold);
}

STB_INT32 SetPropertyAngle(STBHANDLE handle,STB_INT32 nMinUDAngle, STB_INT32 nMaxUDAngle, 
                                            STB_INT32 nMinLRAngle, STB_INT32 nMaxLRAngle){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    if(        handle->execFlg->gen    == STB_TRUE     
        ||    handle->execFlg->age    == STB_TRUE 
        ||    handle->execFlg->exp    == STB_TRUE     
        ||    handle->execFlg->dir    == STB_TRUE     
        ||    handle->execFlg->gaz    == STB_TRUE 
        ||    handle->execFlg->bli    == STB_TRUE 
        )
    {
    }else
    {
        return STB_NORMAL;
    }
    return STB_Pe_SetFaceDirMinMax(handle->hPeHandle, nMinUDAngle, nMaxUDAngle, nMinLRAngle, nMaxLRAngle);
}

STB_INT32 GetPropertyAngle(STBHANDLE handle, STB_INT32 *pnMinUDAngle    , STB_INT32 *pnMaxUDAngle    , 
                                             STB_INT32 *pnMinLRAngle    , STB_INT32 *pnMaxLRAngle    ){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    if(        handle->execFlg->gen    == STB_TRUE     
        ||    handle->execFlg->age    == STB_TRUE 
        ||    handle->execFlg->exp    == STB_TRUE     
        ||    handle->execFlg->dir    == STB_TRUE     
        ||    handle->execFlg->gaz    == STB_TRUE 
        ||    handle->execFlg->bli    == STB_TRUE 
        )
    {
    }else
    {
        return STB_NORMAL;
    }
    return STB_Pe_GetFaceDirMinMax(handle->hPeHandle, pnMinUDAngle, pnMaxUDAngle, pnMinLRAngle, pnMaxLRAngle);
}
STB_INT32 SetPropertyFrameCount(STBHANDLE handle, STB_INT32 nFrameCount){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    if(        handle->execFlg->gen    == STB_TRUE     
        ||    handle->execFlg->age    == STB_TRUE 
        ||    handle->execFlg->exp    == STB_TRUE     
        ||    handle->execFlg->dir    == STB_TRUE     
        ||    handle->execFlg->gaz    == STB_TRUE 
        ||    handle->execFlg->bli    == STB_TRUE 
        )
    {
    }else
    {
        return STB_NORMAL;
    }
    return STB_Pe_SetFrameCount(handle->hPeHandle, nFrameCount);
}
STB_INT32 GetPropertyFrameCount(STBHANDLE handle, STB_INT32 *pnFrameCount){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    if(        handle->execFlg->gen    == STB_TRUE 
        ||    handle->execFlg->age    == STB_TRUE 
        ||    handle->execFlg->exp    == STB_TRUE     
        ||    handle->execFlg->dir    == STB_TRUE     
        ||    handle->execFlg->gaz    == STB_TRUE 
        ||    handle->execFlg->bli    == STB_TRUE 
        )
    {
    }else
    {
        return STB_NORMAL;
    }
    return STB_Pe_GetFrameCount(handle->hPeHandle, pnFrameCount);
}
STB_INT32 SetRecognitionThreshold(STBHANDLE handle, STB_INT32 nThreshold){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    if( handle->execFlg->fr == STB_FALSE )
    {
        return STB_NORMAL;
    }
    return STB_Fr_SetFaceDirThreshold(handle->hFrHandle, nThreshold);
}
STB_INT32 GetRecognitionThreshold(STBHANDLE handle, STB_INT32 *pnThreshold){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    if( handle->execFlg->fr == STB_FALSE )
    {
        return STB_NORMAL;
    }
    return STB_Fr_GetFaceDirThreshold(handle->hFrHandle, pnThreshold);
}

STB_INT32 SetRecognitionAngle(STBHANDLE handle, STB_INT32 nMinUDAngle, STB_INT32 nMaxUDAngle, 
                                                STB_INT32 nMinLRAngle, STB_INT32 nMaxLRAngle){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    if( handle->execFlg->fr == STB_FALSE )
    {
        return STB_NORMAL;
    }
    return STB_Fr_SetFaceDirMinMax(handle->hFrHandle, nMinUDAngle, nMaxUDAngle, nMinLRAngle , nMaxLRAngle);
}

STB_INT32 GetRecognitionAngle(STBHANDLE handle, STB_INT32 *pnMinUDAngle    , STB_INT32 *pnMaxUDAngle    , 
                                                STB_INT32 *pnMinLRAngle    , STB_INT32 *pnMaxLRAngle){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    nRet = IsValidPointer(pnMinUDAngle);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    nRet = IsValidPointer(pnMaxUDAngle);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    nRet = IsValidPointer(pnMinLRAngle);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }
    nRet = IsValidPointer(pnMaxLRAngle);
    if(nRet != STB_NORMAL){
        return STB_ERR_INVALIDPARAM;
    }

    if( handle->execFlg->fr == STB_FALSE )
    {
        return STB_NORMAL;
    }
    return STB_Fr_GetFaceDirMinMax(handle->hFrHandle, pnMinUDAngle, pnMaxUDAngle, pnMinLRAngle , pnMaxLRAngle);
}

STB_INT32 SetRecognitionFrameCount(STBHANDLE handle, STB_INT32 nFrameCount){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    if( handle->execFlg->fr == STB_FALSE )
    {
        return STB_NORMAL;
    }
    return STB_Fr_SetFrameCount(handle->hFrHandle, nFrameCount);
}
STB_INT32 GetRecognitionFrameCount(STBHANDLE handle, STB_INT32 *pnFrameCount){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    if( handle->execFlg->fr == STB_FALSE )
    {
        return STB_NORMAL;
    }
    return STB_Fr_GetFrameCount(handle->hFrHandle, pnFrameCount);
}

STB_INT32 SetRecognitionRatio    (STBHANDLE handle, STB_INT32 nMinRatio){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    if( handle->execFlg->fr == STB_FALSE )
    {
        return STB_NORMAL;
    }
    return STB_Fr_SetMinRatio(handle->hFrHandle, nMinRatio);
}
STB_INT32 GetRecognitionRatio    (STBHANDLE handle, STB_INT32 *pnMinRatio){
    STB_INT32 nRet;
    /*NULL check*/
    nRet = IsValidPointer(handle);
    if(nRet != STB_NORMAL){
        return STB_ERR_NOHANDLE;
    }
    if( handle->execFlg->fr == STB_FALSE )
    {
        return STB_NORMAL;
    }
    return STB_Fr_GetMinRatio(handle->hFrHandle, pnMinRatio);
}
