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

#include "STBFrValidValue.h"

/*Value range check*/
#define IS_OUT_RANGE( val , min , max ) ( ( (val) < (min) ) || ( (max) < (val) ) )
#define IS_OUT_VALUE( val , min , max , accept ) ( IS_OUT_RANGE( val , min , max ) && ( (val) != (accept) )  )
#define IS_OUT_FR_UID( val , min , max , acceptA , acceptB , acceptC  ) ( IS_OUT_RANGE( val , min , max ) && ( (val) != (acceptA) ) && ( (val) != (acceptB) ) && ( (val) != (acceptC) ) )
#define IS_OUT_FR_SCORE( val , min , max , acceptA , acceptB ) ( IS_OUT_RANGE( val , min , max ) && ( (val) != (acceptA) ) && ( (val) != (acceptB) )  )

/*------------------------------------------------------------------------------------------------------------------*/
/* STB_IsValidValue                                                                                            */
/*------------------------------------------------------------------------------------------------------------------*/
STB_INT32 STB_FrIsValidValue(const STB_FR_DET *input)
{
    STB_INT32 i ;

    for( i = 0 ; i < input->num ; i++)
    {

        if( IS_OUT_VALUE( input->fcDet[i].dirDetYaw        , STB_FACE_DIR_LR_MIN    , STB_FACE_DIR_LR_MAX    , STB_ERR_DIR_CANNOT )    ){ return STB_FALSE;}
        if( IS_OUT_VALUE( input->fcDet[i].dirDetPitch    , STB_FACE_DIR_UD_MIN    , STB_FACE_DIR_UD_MAX    , STB_ERR_DIR_CANNOT )    ){ return STB_FALSE;}
        if( IS_OUT_VALUE( input->fcDet[i].dirDetRoll    , STB_FACE_DIR_ROLL_MIN    , STB_FACE_DIR_ROLL_MAX , STB_ERR_DIR_CANNOT )    ){ return STB_FALSE;}
        if( IS_OUT_VALUE( input->fcDet[i].dirDetConf    , STB_FACE_DIR_CONF_MIN    , STB_FACE_DIR_CONF_MAX , STB_ERR_PE_CANNOT )    ){ return STB_FALSE;}

    }

    for( i = 0 ; i < input->num ; i++)
    {
        if( IS_OUT_FR_UID( input->fcDet[i].frDetID    , STB_FACE_FR_UID_MIN    , STB_FACE_FR_UID_MAX    ,STB_ERR_FR_CANNOT ,STB_ERR_FR_NOID ,STB_ERR_FR_NOALBUM ) ){ return STB_FALSE;}
        if( IS_OUT_FR_SCORE( input->fcDet[i].frDetConf    , STB_FACE_FR_SCORE_MIN    , STB_FACE_FR_SCORE_MAX    ,STB_ERR_FR_CANNOT ,STB_ERR_FR_NOALBUM)    ){ return STB_FALSE;}
    }



    return STB_TRUE;
}