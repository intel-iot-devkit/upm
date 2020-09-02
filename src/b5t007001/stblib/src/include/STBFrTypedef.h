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

#ifndef __STBFRTYPEDEF_H__
#define __STBFRTYPEDEF_H__

#include "STBTypedefOutput.h"
#include "STBCommonType.h"
#include "STBCommonDef.h"


/*----------------------------------------------------------------------------*/
/* Face Detection & Estimations results                                       */
/*----------------------------------------------------------------------------*/
typedef struct{
    STB_INT32   nDetID        ; /*Person number detected in the current frame*/
    STB_INT32   nTraID        ; /*Tracking person number in the through frame*/
    STB_INT32   dirDetYaw    ;
    STB_INT32   dirDetPitch    ; 
    STB_INT32   dirDetRoll    ; 
    STB_INT32   dirDetConf    ; 
    STB_INT32   frDetID        ;
    STB_INT32   frDetConf    ;
    STB_STATUS    frStatus    ;
}FR_DET;

/*----------------------------------------------------------------------------*/
/* Eesult data of Execute command                                             */
/*----------------------------------------------------------------------------*/
typedef struct{
    STB_INT32        num        ;
    FR_DET            *fcDet    ;/* Face Detection & Estimations results */
}STB_FR_DET;

/*----------------------------------------------------------------------------*/
/*                                                                                */
/*----------------------------------------------------------------------------*/
typedef struct{
    STB_INT32        nTraID    ;/*Tracking person number in the through frame*/
    STB_RES            frRecog    ;/* Stabilization result of human [nTrackingID] */
}FR_RES;

/*----------------------------------------------------------------------------*/
/*                                                                                */
/*----------------------------------------------------------------------------*/
typedef struct{
    STB_INT32        frCnt    ;/*a number of tracking people*/
    FR_RES            *frFace    ;      
}STB_FR_RES;


#endif /*__STBFRTYPEDEF_H__*/
