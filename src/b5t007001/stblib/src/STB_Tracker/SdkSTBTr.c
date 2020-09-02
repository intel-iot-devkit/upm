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

#include "SdkSTBTr.h"
#include "TrInterface.h"

/*This layer only defines the API function */

/*Create/Delete handle*/
STB_TR_HANDLE STB_Tr_CreateHandle( const STBExecFlg* execFlg ,const STB_INT32 nDetCntMax, const STB_INT32 nTraCntMax){
    return (STB_TR_HANDLE)TrCreateHandle( execFlg , nDetCntMax,  nTraCntMax );
}

STB_INT32 STB_Tr_DeleteHandle(STB_TR_HANDLE handle){
    return TrDeleteHandle((TRHANDLE)handle);
}

/*set frame information*/
STB_INT32 STB_Tr_SetDetect(STB_TR_HANDLE handle,const STB_TR_DET *stbTrDet){
    return TrSetDetect((TRHANDLE)handle,stbTrDet);
}

/*Main process execution*/
STB_INT32 STB_Tr_Execute(STB_TR_HANDLE handle){
    return TrExecute((TRHANDLE)handle);
}

/*get the result*/
STB_INT32 STB_Tr_GetResult(STB_TR_HANDLE handle,STB_TR_RES_FACES* fcResult,STB_TR_RES_BODYS* bdResult){
    return TrGetResult((TRHANDLE)handle,fcResult,bdResult);
}

/*Clear*/
STB_INT32 STB_Tr_Clear(  STB_TR_HANDLE handle  ){
    return TrClear((TRHANDLE)handle);
}

/*RetryCount*/
STB_INT32 STB_Tr_SetRetryCount(STB_TR_HANDLE handle , STB_INT32 nRetryCount){
    return TrSetRetryCount((TRHANDLE)handle,nRetryCount);
}
STB_INT32 STB_Tr_GetRetryCount        ( STB_TR_HANDLE handle , STB_INT32* nRetryCount )
{
    return TrGetRetryCount((TRHANDLE)handle,nRetryCount);
}
/* Stediness */
STB_INT32 STB_Tr_SetStedinessParam    ( STB_TR_HANDLE handle , STB_INT32    nStedinessPos , STB_INT32  nStedinessSize )
{
    return TrSetStedinessParam ((TRHANDLE)handle,nStedinessPos,nStedinessSize);
}
STB_INT32 STB_Tr_GetStedinessParam    ( STB_TR_HANDLE handle , STB_INT32*    nStedinessPos , STB_INT32* nStedinessSize )
{
    return TrGetStedinessParam((TRHANDLE)handle,nStedinessPos,nStedinessSize);
}

