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

#include "STBPeValidValue.h"

/*Value range check*/
#define IS_OUT_RANGE( val , min , max ) ( ( (val) < (min) ) || ( (max) < (val) ) )
#define IS_OUT_VALUE( val , min , max , accept ) ( IS_OUT_RANGE( val , min , max ) && ( (val) != (accept) )  )
#define IS_OUT_FR_UID( val , min , max , acceptA , acceptB , acceptC  ) ( IS_OUT_RANGE( val , min , max ) && ( (val) != (acceptA) ) && ( (val) != (acceptB) ) && ( (val) != (acceptC) ) )

/*------------------------------------------------------------------------------------------------------------------*/
/* STB_PeIsValidValue                                                                                            */
/*------------------------------------------------------------------------------------------------------------------*/
STB_INT32 STB_PeIsValidValue(const STB_PE_DET *input, STBExecFlg *execFlg)
{
    STB_INT32 i ,j;

    if(        execFlg->gen    == STB_TRUE     
        ||    execFlg->age    == STB_TRUE 
        ||    execFlg->fr        == STB_TRUE 
        ||    execFlg->exp    == STB_TRUE     
        ||    execFlg->dir    == STB_TRUE     
        ||    execFlg->gaz    == STB_TRUE 
        ||    execFlg->bli    == STB_TRUE 
        )
    {
        for( i = 0 ; i < input->num ; i++)
        {
            if( IS_OUT_VALUE( input->fcDet[i].dirDetYaw        , STB_FACE_DIR_LR_MIN    , STB_FACE_DIR_LR_MAX    , STB_ERR_DIR_CANNOT )    ){ return STB_FALSE;}
            if( IS_OUT_VALUE( input->fcDet[i].dirDetPitch    , STB_FACE_DIR_UD_MIN    , STB_FACE_DIR_UD_MAX    , STB_ERR_DIR_CANNOT )    ){ return STB_FALSE;}
            if( IS_OUT_VALUE( input->fcDet[i].dirDetRoll    , STB_FACE_DIR_ROLL_MIN    , STB_FACE_DIR_ROLL_MAX , STB_ERR_DIR_CANNOT )    ){ return STB_FALSE;}
            if( IS_OUT_VALUE( input->fcDet[i].dirDetConf    , STB_FACE_DIR_CONF_MIN    , STB_FACE_DIR_CONF_MAX , STB_ERR_PE_CANNOT )    ){ return STB_FALSE;}
        }
    }


    if(    execFlg->age    == STB_TRUE )
    {
        for( i = 0 ; i < input->num ; i++)
        {
            if( IS_OUT_VALUE( input->fcDet[i].ageDetVal        , STB_FACE_AGE_VAL_MIN    , STB_FACE_AGE_VAL_MAX    ,STB_ERR_PE_CANNOT )    ){ return STB_FALSE;}
            if( IS_OUT_VALUE( input->fcDet[i].ageDetConf    , STB_FACE_AGE_CONF_MIN    , STB_FACE_AGE_CONF_MAX    ,STB_ERR_PE_CANNOT )    ){ return STB_FALSE;}
        }
    }

    if(    execFlg->gen    == STB_TRUE )
    {
        for( i = 0 ; i < input->num ; i++)
        {
            if( IS_OUT_VALUE( input->fcDet[i].genDetVal        , STB_FACE_GEN_VAL_MIN    , STB_FACE_GEN_VAL_MAX    ,STB_ERR_PE_CANNOT )    ){ return STB_FALSE;}
            if( IS_OUT_VALUE( input->fcDet[i].genDetConf    , STB_FACE_GEN_CONF_MIN    , STB_FACE_GEN_CONF_MAX    ,STB_ERR_PE_CANNOT )    ){ return STB_FALSE;}
        }
    }

    if(    execFlg->gaz    == STB_TRUE )
    {
        for( i = 0 ; i < input->num ; i++)
        {
            if( IS_OUT_VALUE( input->fcDet[i].gazDetLR    , STB_FACE_GAZE_LR_MIN    , STB_FACE_GAZE_LR_MAX    ,STB_ERR_PE_CANNOT )    ){ return STB_FALSE;}
            if( IS_OUT_VALUE( input->fcDet[i].gazDetUD    , STB_FACE_GAZE_UD_MIN    , STB_FACE_GAZE_UD_MAX    ,STB_ERR_PE_CANNOT )    ){ return STB_FALSE;}
        }
    }

    if(    execFlg->bli    == STB_TRUE )
    {
        for( i = 0 ; i < input->num ; i++)
        {
            if( IS_OUT_VALUE( input->fcDet[i].bliDetL    , STB_FACE_BLI_L_MIN    , STB_FACE_BLI_L_MAX    ,STB_ERR_PE_CANNOT)    ){ return STB_FALSE;}
            if( IS_OUT_VALUE( input->fcDet[i].bliDetR    , STB_FACE_BLI_R_MIN    , STB_FACE_BLI_R_MAX    ,STB_ERR_PE_CANNOT)    ){ return STB_FALSE;}
        }
    }

    if(    execFlg->exp    == STB_TRUE )
    {
        for( i = 0 ; i < input->num ; i++)
        {
            if( IS_OUT_VALUE( input->fcDet[i].expDetConf, STB_FACE_EXP_DEG_MIN    , STB_FACE_EXP_DEG_MAX    ,STB_ERR_PE_CANNOT)    ){ return STB_FALSE;}
            for( j = 0 ; j < STB_EX_MAX ; j++)
            {
                if( IS_OUT_VALUE( input->fcDet[i].expDetVal[j]    ,STB_FACE_EXP_SCORE_MIN    , STB_FACE_EXP_SCORE_MAX    ,STB_ERR_PE_CANNOT)    ){ return STB_FALSE;}
            }
            
        }
    }




    return STB_TRUE;
}