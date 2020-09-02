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

#ifndef __STB_TYPEDEF_H__
#define __STB_TYPEDEF_H__


#ifndef VOID
#define     VOID                void
#endif  /* VOID */

typedef     signed char         STB_INT8    ;  /*  8-bit signed integer   */
typedef     unsigned char       STB_UINT8   ;  /*  8-bit unsigned integer */
typedef     signed short        STB_INT16   ;  /* 16-bit signed integer   */
typedef     unsigned short      STB_UINT16  ;  /* 16-bit unsigned integer */
typedef     int                 STB_INT32   ;  /* 32 bit signed integer   */
typedef     unsigned int        STB_UINT32  ;  /* 32 bit unsigned integer */
typedef     float               STB_FLOAT32 ;  /* 32-bit floating point number */
typedef     double              STB_FLOAT64 ;  /* 64-bit floating point number */



/****************************************/
/*  INPUT data strucrure to STBLib.     */
/****************************************/

typedef enum {
    STB_Expression_Neutral,
    STB_Expression_Happiness,
    STB_Expression_Surprise,
    STB_Expression_Anger,
    STB_Expression_Sadness,
    STB_Expression_Max
} STB_OKAO_EXPRESSION;

typedef struct {
    STB_INT32 nX;
    STB_INT32 nY;
} STB_POINT;

/* Face direction estimation */
typedef struct {
    STB_INT32   nLR;
    STB_INT32   nUD;
    STB_INT32   nRoll;
    STB_INT32   nConfidence;
} STB_FRAME_RESULT_DIRECTION;

/* Age estimation */
typedef struct {
    STB_INT32   nAge;
    STB_INT32   nConfidence;
} STB_FRAME_RESULT_AGE;

/* Gender estimation */
typedef struct {
    STB_INT32   nGender;
    STB_INT32   nConfidence;
} STB_FRAME_RESULT_GENDER;

/* Gaze estimation */
typedef struct {
    STB_INT32   nLR;
    STB_INT32   nUD;
} STB_FRAME_RESULT_GAZE;

/* Blink estimation */
typedef struct {
    STB_INT32   nLeftEye;
    STB_INT32   nRightEye;
} STB_FRAME_RESULT_BLINK;

/* Facial expression estimation */
typedef struct {
    STB_INT32   anScore[STB_Expression_Max];
    STB_INT32   nDegree;
} STB_FRAME_RESULT_EXPRESSION;

/* Face recognition */
typedef struct {
    STB_INT32   nUID;
    STB_INT32   nScore;
} STB_FRAME_RESULT_RECOGNITION;

/* One detection result */
typedef struct {
    STB_POINT   center;
    STB_INT32   nSize;
    STB_INT32   nConfidence ;
} STB_FRAME_RESULT_DETECTION;

/* Face detection and post-processing result (1 person) */
typedef struct {
    STB_POINT                    center;
    STB_INT32                    nSize;
    STB_INT32                    nConfidence;
    STB_FRAME_RESULT_DIRECTION   direction;
    STB_FRAME_RESULT_AGE         age;
    STB_FRAME_RESULT_GENDER      gender;
    STB_FRAME_RESULT_GAZE        gaze;
    STB_FRAME_RESULT_BLINK       blink;
    STB_FRAME_RESULT_EXPRESSION  expression;
    STB_FRAME_RESULT_RECOGNITION recognition;
} STB_FRAME_RESULT_FACE;

/* One human body detection result */
typedef struct {
    STB_INT32                   nCount;
    STB_FRAME_RESULT_DETECTION  body[35];
} STB_FRAME_RESULT_BODYS;

/* Face detection and post-processing result (1 frame) */
typedef struct {
    STB_INT32                   nCount;
    STB_FRAME_RESULT_FACE       face[35];
} STB_FRAME_RESULT_FACES;

/* FRAME result (1 frame) */
typedef struct {
    STB_FRAME_RESULT_BODYS      bodys;
    STB_FRAME_RESULT_FACES      faces;
} STB_FRAME_RESULT;


/****************************************/
/*  OUTPUT data strucrure from STBLib.  */
/****************************************/

#define STB_CONF_NO_DATA      -1  /* No confidence (No data or in the case of stabilization time) */

typedef enum {
    STB_STATUS_NO_DATA      = -1, /* No data : No data for the relevant person */
    STB_STATUS_CALCULATING  = 0,  /* During stabilization : a number of data for relevant people aren't enough(a number of frames that relevant people are taken) */
    STB_STATUS_COMPLETE     = 1,  /* Stabilization done : the frames which done stabilization */
    STB_STATUS_FIXED        = 2,  /* Stabilization fixed : already stabilization done, the results is fixed */
} STB_STATUS; /* Status of stabilization */


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

/* General purpose stabilization result structure */
typedef struct  {
    STB_STATUS  status; /* Stabilization status */
    STB_INT32   conf;   /* Stabilization confidence */
    STB_INT32   value;
} STB_RES;

/* Result of Gaze estimation */
typedef struct {
    STB_STATUS  status; /* Stabilization status */
    STB_INT32   conf;   /* Stabilization confidence */
    STB_INT32   UD;
    STB_INT32   LR;
} STB_GAZE;

/* Result of Face direction estimation */
typedef struct {
    STB_STATUS  status; /* Stabilization status */
    STB_INT32   conf;   /* Stabilization confidence */
    STB_INT32   yaw;
    STB_INT32   pitch;
    STB_INT32   roll;
} STB_DIR;

/* Result of Blink estimation */
typedef struct {
    STB_STATUS  status; /* Stabilization status */
    STB_INT32   ratioL;
    STB_INT32   ratioR;
} STB_BLINK;

/* Detection position structure */
typedef struct  {
    STB_UINT32  x;
    STB_UINT32  y;
} STB_POS;

/* Face stabilization result structure */
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

/* Human body stabilization result structure */
typedef struct {
    STB_INT32   nDetectID;
    STB_INT32   nTrackingID;
    STB_POS     center;
    STB_UINT32  nSize;
    STB_INT32   conf;
} STB_BODY;


#endif /* __STB_TYPEDEF_H__ */

