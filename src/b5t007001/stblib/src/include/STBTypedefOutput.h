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


#ifndef STBTYPEDEF_H__
#define STBTYPEDEF_H__

#ifndef VOID
#define     VOID                void   
#endif

typedef     signed char         STB_INT8    ;    /*8-bit signed integer*/
typedef     unsigned char       STB_UINT8   ;    /*8-bit unsigned integer*/
typedef     signed short        STB_INT16   ;    /*16-bit signed integer*/
typedef     unsigned short      STB_UINT16  ;    /*16-bit unsigned integer*/
typedef     int                 STB_INT32   ;    /*32 bit signed integer*/
typedef     unsigned int        STB_UINT32  ;    /*32 bit unsigned integer*/
typedef     float               STB_FLOAT32 ;    /*32-bit floating point number*/
typedef     double              STB_FLOAT64 ;    /*64-bit floating point number*/


typedef enum {
    STB_STATUS_NO_DATA      = -1, /*No data : No data for the relevant person*/
    STB_STATUS_CALCULATING  = 0,  /* during stabilization : a number of data for relevant people aren't enough(a number of frames that relevant people are taken) */
    STB_STATUS_COMPLETE     = 1,  /*stabilization done : the frames which done stabilization*/
    STB_STATUS_FIXED        = 2,  /*stabilization fixed : already stabilization done, the results is fixed*/
} STB_STATUS;/*Status of stabilization*/

#define STB_CONF_NO_DATA      -1  /*No confidence(No data or in the case of stabilization time)*/

/* Expression */
typedef enum {
    STB_EX_UNKNOWN          = -1,
    STB_EX_NEUTRAL          = 0,
    STB_EX_HAPPINESS,
    STB_EX_SURPRISE,    
    STB_EX_ANGER,
    STB_EX_SADNESS,
    STB_EX_MAX
}STB_EXPRESSION;

/*General purpose stabilization result structure*/
typedef struct  {
    STB_STATUS  status;/* Stabilization status */
    STB_INT32   conf;  /* Stabilization confidence */
    STB_INT32   value;
} STB_RES;

/*result of Gaze estimation*/
typedef struct {
    STB_STATUS  status;/* Stabilization status */
    STB_INT32   conf;  /* Stabilization confidence */
    STB_INT32   UD;
    STB_INT32   LR;
} STB_GAZE;

/*Face direction result*/
typedef struct {
    STB_STATUS  status;/* Stabilization status */
    STB_INT32   conf;  /* Stabilization confidence */
    STB_INT32   yaw;
    STB_INT32   pitch;
    STB_INT32   roll;
} STB_DIR;

/*result of Blink estimation*/
typedef struct {
    STB_STATUS  status;/* Stabilization status */
    STB_INT32   ratioL;
    STB_INT32   ratioR;
} STB_BLINK;

/*Detection position structure*/
typedef struct  {
    STB_UINT32  x;
    STB_UINT32  y;
} STB_POS;

/*Face stabilization result structure*/
typedef struct {
    STB_INT32   nDetectID;
    STB_INT32   nTrackingID;
    STB_POS     center;
    STB_UINT32  nSize;
    STB_INT32   conf;
    STB_DIR     direction;
    STB_RES     age;
    STB_RES     gender;
    STB_GAZE    gaze;
    STB_BLINK   blink;
    STB_RES     expression;
    STB_RES     recognition;
} STB_FACE;

/*Human body result structure*/
typedef struct {
    STB_INT32   nDetectID;
    STB_INT32   nTrackingID;
    STB_POS     center;
    STB_UINT32  nSize;
    STB_INT32   conf;
} STB_BODY;



#endif /* STBTYPEDEF_H__ */
