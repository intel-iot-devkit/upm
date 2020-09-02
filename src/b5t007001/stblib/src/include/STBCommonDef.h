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

#ifndef COMMONDEF_H__
#define COMMONDEF_H__
#include <stdlib.h>

/* Executed flag */
#define STB_FUNC_BD  (0x00000001U)  /* [LSB]bit0: Body Tracking            00000000001        */
#define STB_FUNC_DT  (0x00000004U)  /* [LSB]bit2: Face Tracking            00000000100        */
#define STB_FUNC_PT  (0x00000008U)  /* [LSB]bit3: Face Direction        00000001000        */
#define STB_FUNC_AG  (0x00000010U)  /* [LSB]bit4: Age Estimation        00000010000        */
#define STB_FUNC_GN  (0x00000020U)  /* [LSB]bit5: Gender Estimation        00000100000        */
#define STB_FUNC_GZ  (0x00000040U)  /* [LSB]bit6: Gaze Estimation        00001000000        */
#define STB_FUNC_BL  (0x00000080U)  /* [LSB]bit7: Blink Estimation        00010000000        */
#define STB_FUNC_EX  (0x00000100U)  /* [MSB]bit0: Expression Estimation    00100000000        */
#define STB_FUNC_FR  (0x00000200U)  /* [MSB]bit1: Face Recognition        01000000000        */



/*STB library's error code*/
#define     STB_NORMAL                 (0)       /*Successful completion*/
#define     STB_ERR_INITIALIZE         (-2)      /*Initialization error*/
#define     STB_ERR_INVALIDPARAM       (-3)      /*argument error*/
#define     STB_ERR_NOHANDLE           (-7)      /*handle error*/
#define     STB_ERR_PROCESSCONDITION   (-8)      /*When the processing condition is not satisfied*/

#define     STB_TRUE  (1)
#define     STB_FALSE (0)



#if !defined(STB_API)
/*Import(Application Default)*/
    #define STB_API        __declspec( dllimport ) 
#endif /* OKAO_API || OMCV_API */

#endif  /* COMMONDEF_H__ */

