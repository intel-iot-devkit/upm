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

#ifndef STBTRVALIDVALUE_H__
#define STBTRVALIDVALUE_H__


#include "STBCommonDef.h"
#include "STBCommonType.h"
#include "STBTrTypedef.h"

/*-------------------------------------------------------------------*/
/*Threshold for checking input value*/
/*-------------------------------------------------------------------*/
#define     STB_BODY_CNT_MIN        0    // body
#define     STB_BODY_CNT_MAX        35
#define     STB_BODY_XY_MIN            0
#define     STB_BODY_XY_MAX            8191
#define     STB_BODY_SIZE_MIN        20
#define     STB_BODY_SIZE_MAX        8192
#define     STB_BODY_CONF_MIN        0
#define     STB_BODY_CONF_MAX        1000
#define     STB_FACE_CNT_MIN        0    // face
#define     STB_FACE_CNT_MAX        35
#define     STB_FACE_XY_MIN            0
#define     STB_FACE_XY_MAX            8191
#define     STB_FACE_SIZE_MIN        20
#define     STB_FACE_SIZE_MAX        8192
#define     STB_FACE_CONF_MIN        0
#define     STB_FACE_CONF_MAX        1000
#define     STB_FACE_DIR_LR_MIN        -180
#define     STB_FACE_DIR_LR_MAX        179
#define     STB_FACE_DIR_UD_MIN        -180
#define     STB_FACE_DIR_UD_MAX        179
#define     STB_FACE_DIR_ROLL_MIN    -180
#define     STB_FACE_DIR_ROLL_MAX    179
#define     STB_FACE_DIR_CONF_MIN    0
#define     STB_FACE_DIR_CONF_MAX    1000
#define     STB_FACE_AGE_VAL_MIN    0
#define     STB_FACE_AGE_VAL_MAX    75
#define     STB_FACE_AGE_CONF_MIN    0
#define     STB_FACE_AGE_CONF_MAX    1000
#define     STB_FACE_GEN_VAL_MIN    0
#define     STB_FACE_GEN_VAL_MAX    1
#define     STB_FACE_GEN_CONF_MIN    0
#define     STB_FACE_GEN_CONF_MAX    1000
#define     STB_FACE_GAZE_LR_MIN    -90
#define     STB_FACE_GAZE_LR_MAX    90
#define     STB_FACE_GAZE_UD_MIN    -90
#define     STB_FACE_GAZE_UD_MAX    90
#define     STB_FACE_BLI_L_MIN        1
#define     STB_FACE_BLI_L_MAX        1000
#define     STB_FACE_BLI_R_MIN        1
#define     STB_FACE_BLI_R_MAX        1000
#define     STB_FACE_EXP_SCORE_MIN    0
#define     STB_FACE_EXP_SCORE_MAX    100    /* not 1000 */
#define     STB_FACE_EXP_DEG_MIN    -100
#define     STB_FACE_EXP_DEG_MAX    100
#define     STB_FACE_FR_UID_MIN        0
#define     STB_FACE_FR_UID_MAX        499
#define     STB_FACE_FR_SCORE_MIN    0
#define     STB_FACE_FR_SCORE_MAX    1000

/*-------------------------------------------------------------------*/
/*Permitted input value*/
/*-------------------------------------------------------------------*/
#define     STB_ERR_PE_CANNOT        -128 /*Estimation is not possible.*/
#define     STB_ERR_FR_CANNOT        -128 /*Recognition impossible*/
#define     STB_ERR_FR_NOID            -1   /*No corresponding ID*/
#define     STB_ERR_FR_NOALBUM        -127 /*Not-registered in Album*/


/*-------------------------------------------------------------------*/
/*  Func                                                             */
/*-------------------------------------------------------------------*/
STB_INT32 STB_TrIsValidValue(const STB_TR_DET *input, STBExecFlg *execFlg);

#endif  /* COMMONDEF_H__ */

