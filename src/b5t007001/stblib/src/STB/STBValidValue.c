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

#include "STBValidValue.h"

/*Value range check*/
#define IS_OUT_RANGE( val , min , max ) ( ( (val) < (min) ) || ( (max) < (val) ) )
#define IS_OUT_VALUE( val , min , max , accept ) ( IS_OUT_RANGE( val , min , max ) && ( (val) != (accept) )  )
#define IS_OUT_FR_UID( val , min , max , acceptA , acceptB , acceptC  ) ( IS_OUT_RANGE( val , min , max ) && ( (val) != (acceptA) ) && ( (val) != (acceptB) ) && ( (val) != (acceptC) ) )
#define IS_OUT_FR_SCORE( val , min , max , acceptA , acceptB ) ( IS_OUT_RANGE( val , min , max ) && ( (val) != (acceptA) ) && ( (val) != (acceptB) )  )

/*------------------------------------------------------------------------------------------------------------------*/
/* STB_IsValidValue                                                                                            */
/*------------------------------------------------------------------------------------------------------------------*/
STB_INT32 STB_IsValidValue(const STB_FRAME_RESULT *input, STBExecFlg *execFlg)
{
    STB_INT32 i ,j;



    if( execFlg->bodyTr == STB_TRUE )
    {
        if( IS_OUT_RANGE( input->bodys.nCount , STB_BODY_CNT_MIN , STB_BODY_CNT_MAX )    ){ return STB_FALSE;}
        for( i = 0 ; i < input->bodys.nCount ; i++)
        {
            if( IS_OUT_RANGE( input->bodys.body[i].center.nX    , STB_BODY_XY_MIN    , STB_BODY_XY_MAX    )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->bodys.body[i].center.nY    , STB_BODY_XY_MIN    , STB_BODY_XY_MAX    )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->bodys.body[i].nSize        , STB_BODY_SIZE_MIN , STB_BODY_SIZE_MAX )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->bodys.body[i].nConfidence    , STB_BODY_CONF_MIN , STB_BODY_CONF_MAX )    ){ return STB_FALSE;}
        }
        
    }

    if( execFlg->faceTr == STB_TRUE )
    {
        if( IS_OUT_RANGE( input->faces.nCount , STB_FACE_CNT_MIN , STB_FACE_CNT_MAX )    ){ return STB_FALSE;}
        for( i = 0 ; i < input->faces.nCount ; i++)
        {
            if( IS_OUT_RANGE( input->faces.face[i].center.nX    , STB_FACE_XY_MIN    , STB_FACE_XY_MAX    )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->faces.face[i].center.nY    , STB_FACE_XY_MIN    , STB_FACE_XY_MAX    )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->faces.face[i].nSize        , STB_FACE_SIZE_MIN , STB_FACE_SIZE_MAX )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->faces.face[i].nConfidence    , STB_FACE_CONF_MIN , STB_FACE_CONF_MAX )    ){ return STB_FALSE;}
        }
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
        for( i = 0 ; i < input->faces.nCount ; i++)
        {
            if( IS_OUT_RANGE( input->faces.face[i].direction.nLR            , STB_FACE_DIR_LR_MIN    , STB_FACE_DIR_LR_MAX    )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->faces.face[i].direction.nUD            , STB_FACE_DIR_UD_MIN    , STB_FACE_DIR_UD_MAX    )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->faces.face[i].direction.nRoll            , STB_FACE_DIR_ROLL_MIN    , STB_FACE_DIR_ROLL_MAX )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->faces.face[i].direction.nConfidence    , STB_FACE_DIR_CONF_MIN    , STB_FACE_DIR_CONF_MAX )    ){ return STB_FALSE;}
        }
    }


    if(    execFlg->age    == STB_TRUE )
    {
        for( i = 0 ; i < input->faces.nCount ; i++)
        {
            if( IS_OUT_VALUE( input->faces.face[i].age.nAge            , STB_FACE_AGE_VAL_MIN    , STB_FACE_AGE_VAL_MAX    ,STB_ERR_PE_CANNOT )    ){ return STB_FALSE;}
            if( IS_OUT_VALUE( input->faces.face[i].age.nConfidence    , STB_FACE_AGE_CONF_MIN    , STB_FACE_AGE_CONF_MAX    ,STB_ERR_PE_CANNOT )    ){ return STB_FALSE;}
        }
    }

    if(    execFlg->gen    == STB_TRUE )
    {
        for( i = 0 ; i < input->faces.nCount ; i++)
        {
            if( IS_OUT_VALUE( input->faces.face[i].gender.nGender        , STB_FACE_GEN_VAL_MIN    , STB_FACE_GEN_VAL_MAX    ,STB_ERR_PE_CANNOT )    ){ return STB_FALSE;}
            if( IS_OUT_VALUE( input->faces.face[i].gender.nConfidence    , STB_FACE_GEN_CONF_MIN    , STB_FACE_GEN_CONF_MAX    ,STB_ERR_PE_CANNOT )    ){ return STB_FALSE;}
        }
    }

    if(    execFlg->gaz    == STB_TRUE )
    {
        for( i = 0 ; i < input->faces.nCount ; i++)
        {
            if( IS_OUT_VALUE( input->faces.face[i].gaze.nLR    , STB_FACE_GAZE_LR_MIN    , STB_FACE_GAZE_LR_MAX    ,STB_ERR_PE_CANNOT )    ){ return STB_FALSE;}
            if( IS_OUT_VALUE( input->faces.face[i].gaze.nUD    , STB_FACE_GAZE_UD_MIN    , STB_FACE_GAZE_UD_MAX    ,STB_ERR_PE_CANNOT )    ){ return STB_FALSE;}
        }
    }

    if(    execFlg->bli    == STB_TRUE )
    {
        for( i = 0 ; i < input->faces.nCount ; i++)
        {
            if( IS_OUT_VALUE( input->faces.face[i].blink.nLeftEye    , STB_FACE_BLI_L_MIN    , STB_FACE_BLI_L_MAX    ,STB_ERR_PE_CANNOT)    ){ return STB_FALSE;}
            if( IS_OUT_VALUE( input->faces.face[i].blink.nRightEye    , STB_FACE_BLI_R_MIN    , STB_FACE_BLI_R_MAX    ,STB_ERR_PE_CANNOT)    ){ return STB_FALSE;}
        }
    }

    if(    execFlg->exp    == STB_TRUE )
    {
        for( i = 0 ; i < input->faces.nCount ; i++)
        {
            if( IS_OUT_VALUE( input->faces.face[i].expression.nDegree    , STB_FACE_EXP_DEG_MIN    , STB_FACE_EXP_DEG_MAX    ,STB_ERR_PE_CANNOT)    ){ return STB_FALSE;}
            for( j = 0 ; j < STB_EX_MAX ; j++)
            {
                if( IS_OUT_VALUE( input->faces.face[i].expression.anScore[j]    ,STB_FACE_EXP_SCORE_MIN    , STB_FACE_EXP_SCORE_MAX    ,STB_ERR_PE_CANNOT)    ){ return STB_FALSE;}
            }
            
        }
    }

    if(    execFlg->fr    == STB_TRUE )
    {
        for( i = 0 ; i < input->faces.nCount ; i++)
        {
            if( IS_OUT_FR_UID( input->faces.face[i].recognition.nUID    , STB_FACE_FR_UID_MIN    , STB_FACE_FR_UID_MAX    ,STB_ERR_FR_CANNOT ,STB_ERR_FR_NOID ,STB_ERR_FR_NOALBUM ) ){ return STB_FALSE;}
            if( IS_OUT_FR_SCORE( input->faces.face[i].recognition.nScore    , STB_FACE_FR_SCORE_MIN    , STB_FACE_FR_SCORE_MAX    ,STB_ERR_FR_CANNOT ,STB_ERR_FR_NOALBUM)    ){ return STB_FALSE;}
        }
    }


    return STB_TRUE;
}