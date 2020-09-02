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

#ifndef __STBTRTYPEDEF_H__
#define __STBTRTYPEDEF_H__

#include "STBTypedefOutput.h"
#include "STBCommonType.h"
#include "STBCommonDef.h"

/*----------------------------------------------------------------------------*/
/* Struct                                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Detection result (Tracking input infomation)                               */
/*----------------------------------------------------------------------------*/
typedef struct{
    STB_INT32   posX    ; /* Center x-coordinate    */
    STB_INT32   posY    ; /* Center y-coordinate    */
    STB_INT32   size    ; /* Size                    */
    STB_INT32   conf    ; /* Degree of confidence    */
}ROI_DET;
/*----------------------------------------------------------------------------*/
/* Result data  (Tracking input infomation)                                   */
/*----------------------------------------------------------------------------*/
typedef struct{
    STB_INT32    fcNum    ; /*a number of detected face*/
    ROI_DET  *    fcDet    ; /* face rectangle data */
    STB_INT32    bdNum    ; /*a number of body detection*/
    ROI_DET  *    bdDet    ; /*Body rectangular data*/
}STB_TR_DET;
/*----------------------------------------------------------------------------*/
/* Tracking object result (Tracking output infomation)                        */
/*----------------------------------------------------------------------------*/
typedef struct {
    STB_INT32   nDetID    ; /*previous detected result ID*/
    STB_INT32   nTraID    ; /*Tracking ID*/
    STB_POS     pos        ; /* Stabilization of coordinates */
    STB_INT32   size    ; /*Stabilization of face size*/
    STB_INT32   conf    ; /*tracking confidence*/
} STB_TR_RES;
/*----------------------------------------------------------------------------*/
/* Faces tracking result (Tracking output infomation)                         */
/*----------------------------------------------------------------------------*/
typedef struct {
    STB_INT32      cnt        ; /*a number of facial information during tracking*/
    STB_TR_RES*    face    ; /*the facial information during tracking */
} STB_TR_RES_FACES;
/*----------------------------------------------------------------------------*/
/* Faces tracking result (Tracking output infomation)                         */
/*----------------------------------------------------------------------------*/
typedef struct {
    STB_INT32      cnt        ; /*a number of human body during tracking*/
    STB_TR_RES*    body    ; /*the human body information during tracking*/
} STB_TR_RES_BODYS;



#endif /*__STBTRTYPEDEF_H__*/
