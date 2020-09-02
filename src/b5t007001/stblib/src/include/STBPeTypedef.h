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

#ifndef __STBPETYPEDEF_H__
#define __STBPETYPEDEF_H__

#include "STBTypedefOutput.h"
#include "STBCommonType.h"
#include "STBCommonDef.h"


/*----------------------------------------------------------------------------*/
/* Face Detection & Estimations result (Property estimation input infomation) */
/*----------------------------------------------------------------------------*/
typedef struct{
    STB_INT32   nDetID        ; /*Person number detected in the current frame*/
    STB_INT32   nTraID        ; /*Tracking person number in the through frame*/
    STB_INT32   dirDetYaw    ;
    STB_INT32   dirDetPitch    ; 
    STB_INT32   dirDetRoll    ; 
    STB_INT32   dirDetConf    ; 
    STB_INT32   ageDetVal    ;   
    STB_INT32   ageDetConf    ;   
    STB_STATUS    ageStatus    ;
    STB_INT32   genDetVal    ; 
    STB_INT32   genDetConf    ; 
    STB_STATUS    genStatus    ;
    STB_INT32   gazDetLR    ;
    STB_INT32   gazDetUD    ;
    STB_INT32   bliDetL        ;
    STB_INT32   bliDetR        ;
    STB_INT32   expDetVal[STB_EX_MAX];  
    STB_INT32   expDetConf    ;
}FACE_DET;


/*----------------------------------------------------------------------------*/
/* Result data of Execute command (Property estimation input infomation)      */
/*----------------------------------------------------------------------------*/
typedef struct{
    STB_INT32     num    ;   /*a number of tracking people*/
    FACE_DET     *fcDet    ;   /* Detection & Estimations result */
}STB_PE_DET;

/*----------------------------------------------------------------------------*/
/* Property estimation output infomation                                      */
/*----------------------------------------------------------------------------*/
typedef struct {
    STB_INT32   nTraID    ;                /*Tracking person number in the through frame*/
    STB_RES     gen        ;                /* Stabilization result of human [nTrackingID] */
    STB_RES     age        ;                /* Stabilization result of human [nTrackingID] */
    STB_RES     exp        ;                /* Stabilization result of human [nTrackingID] */
    STB_GAZE    gaz        ;                /* Stabilization result of human [nTrackingID] */
    STB_DIR        dir        ;
    STB_BLINK    bli        ;
} STB_PE_FACE;

/*----------------------------------------------------------------------------*/
/* Property estimation output infomation                                      */
/*----------------------------------------------------------------------------*/
typedef struct {
    STB_INT32        peCnt    ;    /*a number of tracking people*/
    STB_PE_FACE     *peFace    ;        
} STB_PE_RES;    

#endif /*__STBPETYPEDEF_H__*/
