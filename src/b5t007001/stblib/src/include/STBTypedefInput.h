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

#ifndef __STB_OKOA_RESULT_H__
#define __STB_OKOA_RESULT_H__

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

/*Face direction estimation*/
typedef struct {
    STB_INT32   nLR;
    STB_INT32   nUD;
    STB_INT32   nRoll;
    STB_INT32   nConfidence;
} STB_FRAME_RESULT_DIRECTION;

/*Age estimation*/
typedef struct {
    STB_INT32   nAge;
    STB_INT32   nConfidence;
} STB_FRAME_RESULT_AGE;

/*Gender estimation*/
typedef struct {
    STB_INT32   nGender;
    STB_INT32   nConfidence;
} STB_FRAME_RESULT_GENDER;

/*Gaze estimation*/
typedef struct {
    STB_INT32   nLR;
    STB_INT32   nUD;
} STB_FRAME_RESULT_GAZE;

/*Blink estimation*/
typedef struct {
    STB_INT32   nLeftEye;
    STB_INT32   nRightEye;
} STB_FRAME_RESULT_BLINK;

/*estimation of facial expression*/
typedef struct {
    STB_INT32   anScore[STB_Expression_Max] ;
    STB_INT32   nDegree;
} STB_FRAME_RESULT_EXPRESSION;

/*Face recognition*/
typedef struct {
    STB_INT32   nUID;
    STB_INT32   nScore;
} STB_FRAME_RESULT_RECOGNITION;

/*One detection result*/
typedef struct {
    STB_POINT   center;
    STB_INT32   nSize;
    STB_INT32   nConfidence ;
} STB_FRAME_RESULT_DETECTION;

/*Face detection and post-processing result (1 person)*/
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

/*One human body detection result*/
typedef struct {
    STB_INT32                   nCount;
    STB_FRAME_RESULT_DETECTION  body[35];
} STB_FRAME_RESULT_BODYS;

/*Face detection and post-processing result (1 frame)*/
typedef struct {
    STB_INT32                   nCount;
    STB_FRAME_RESULT_FACE       face[35];
} STB_FRAME_RESULT_FACES;


/*FRAME result (1 frame)*/
typedef struct {
    STB_FRAME_RESULT_BODYS      bodys;
    STB_FRAME_RESULT_FACES      faces;
} STB_FRAME_RESULT;

#endif /*__HVCW_RESULT_H__*/
