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

#include "STBTrValidValue.h"

/*Value range check*/
#define IS_OUT_RANGE( val , min , max )( ( (val) < (min) ) || ( (max) < (val) ) )

/*------------------------------------------------------------------------------------------------------------------*/
/* STB_TrIsValidValue                                                                                            */
/*------------------------------------------------------------------------------------------------------------------*/
STB_INT32 STB_TrIsValidValue(const STB_TR_DET *input, STBExecFlg *execFlg)
{
    STB_INT32 i ;



    if( execFlg->bodyTr == STB_TRUE )
    {
        if( IS_OUT_RANGE( input->bdNum , STB_BODY_CNT_MIN , STB_BODY_CNT_MAX )    ){ return STB_FALSE;}
        for( i = 0 ; i < input->bdNum ; i++)
        {
            if( IS_OUT_RANGE( input->bdDet[i].posX    , STB_BODY_XY_MIN    , STB_BODY_XY_MAX    )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->bdDet[i].posY    , STB_BODY_XY_MIN    , STB_BODY_XY_MAX    )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->bdDet[i].size    , STB_BODY_SIZE_MIN , STB_BODY_SIZE_MAX )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->bdDet[i].conf    , STB_BODY_CONF_MIN , STB_BODY_CONF_MAX )    ){ return STB_FALSE;}
        }
        
    }

    if( execFlg->faceTr == STB_TRUE )
    {
        if( IS_OUT_RANGE( input->fcNum , STB_FACE_CNT_MIN , STB_FACE_CNT_MAX )    ){ return STB_FALSE;}
        for( i = 0 ; i < input->fcNum ; i++)
        {
            if( IS_OUT_RANGE( input->fcDet[i].posX    , STB_FACE_XY_MIN    , STB_FACE_XY_MAX    )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->fcDet[i].posY    , STB_FACE_XY_MIN    , STB_FACE_XY_MAX    )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->fcDet[i].size    , STB_FACE_SIZE_MIN , STB_FACE_SIZE_MAX )    ){ return STB_FALSE;}
            if( IS_OUT_RANGE( input->fcDet[i].conf    , STB_FACE_CONF_MIN , STB_FACE_CONF_MAX )    ){ return STB_FALSE;}
        }
    }



    return STB_TRUE;
}