/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2025, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     TypeDef.h
    \brief    Define macros, basic types, new types and enumerations
*/

#ifndef __TYPEDEF__
#define __TYPEDEF__

#ifndef __COMMONDEF__
#error Include CommonDef.h not TypeDef.h
#endif

#include <vector>
#include <utility>
#include <iostream>
#include <sstream>


//! \ingroup TLibCommon
//! \{

#define JCTVC_AD0021_SEI_MANIFEST                         1  // JCTVC_AD0021(JVET-T0056): SEI manifest SEI message
#define JCTVC_AD0021_SEI_PREFIX_INDICATION                1  // JCTVC_AD0021(JVET-T0056): SEI prefix indication SEI message
#define JVET_X0079_MODIFIED_BITRATES                      1 // JVET-X0079: Addition of level 6.3

#define JVET_V0078                                        1 // JVET-V0078: QP control for very smooth blocks
#define JVET_Y0077_BIM                                    1 // JVET-Y0077: Block Importance Mapping

#define JVET_Y0105_SW_AND_QDF                             1 // An adaptive smooth window (SW) size and extension of quality dependency factor (QDF) to low frame rate in rate control 


#define JVET_AK0194_DSC_SEI_DECODER_SYNTAX                1 // read the Digitally Signed Content SEIs at the decoder

// Keep the following define. Can be switched off by CMake, when OpenSSL is not found
#ifndef JVET_AK0194_DSC_SEI
#define JVET_AK0194_DSC_SEI   1                      // Digitally signed content signing and verification (requires OpenSSL v3)
#endif

// ====================================================================================================================
// Debugging
// ====================================================================================================================

#define DEBUG_STRING                                      0 ///< When enabled, prints out final decision debug info at encoder and decoder
#define DEBUG_ENCODER_SEARCH_BINS                         0 ///< When enabled, prints out each bin as it is coded during encoder search
#define DEBUG_CABAC_BINS                                  0 ///< When enabled, prints out each bin as it is coded during final encode and decode
#define DEBUG_INTRA_SEARCH_COSTS                          0 ///< When enabled, prints out the cost for each mode during encoder search
#define DEBUG_TRANSFORM_AND_QUANTISE                      0 ///< When enabled, prints out each TU as it passes through the transform-quantise-dequantise-inverseTransform process

#define ENVIRONMENT_VARIABLE_DEBUG_AND_TEST               0 ///< When enabled, allows control of debug modifications via environment variables
#define PRINT_MACRO_VALUES                                1 ///< When enabled, the encoder prints out a list of the non-environment-variable controlled macros and their values on startup

// TODO: rename this macro to DECODER_DEBUG_BIT_STATISTICS (may currently cause merge issues with other branches)
// This can be enabled by the makefile
#ifndef RExt__DECODER_DEBUG_BIT_STATISTICS
#define RExt__DECODER_DEBUG_BIT_STATISTICS                0 ///< 0 (default) = decoder reports as normal, 1 = decoder produces bit usage statistics (will impact decoder run time by up to ~10%)
#endif

// This can be enabled by the makefile
#ifndef ENC_DEC_TRACE
#define ENC_DEC_TRACE                                     0
#endif
#define DEC_NUH_TRACE                                     0 ///< When trace enabled, enable tracing of NAL unit headers at the decoder (currently not possible at the encoder)

#define PRINT_RPS_INFO                                    0 ///< Enable/disable the printing of bits used to send the RPS.

#define MCTS_EXTRACTION                                   1 ///< Additional project for MCTS Extraction as in JCTVC-AC1005

// ====================================================================================================================
// Tool Switches - transitory (these macros are likely to be removed in future revisions)
// ====================================================================================================================

#define JVET_V0056_MCTF                                   1 // JVET-V0056: Changes to MCTF

#define JVET_K0390_RATE_CTRL                              1
#if JVET_K0390_RATE_CTRL
#define JVET_M0600_RATE_CTRL                              1
#endif
#define DECODER_CHECK_SUBSTREAM_AND_SLICE_TRAILING_BYTES  1 ///< TODO: integrate this macro into a broader conformance checking system.
#define MCTS_ENC_CHECK                                    1  ///< Temporal MCTS encoder constraint and decoder checks. Also requires SEITMCTSTileConstraint to be enabled to enforce constraint
#define SHUTTER_INTERVAL_SEI_MESSAGE                      1  ///< support for shutter interval SEI message 
#define JVET_AE0101_PHASE_INDICATION_SEI_MESSAGE          1  ///< support for phase indication SEI message
#define SEI_ENCODER_CONTROL                               1  ///< add encoder control for the following SEI: film grain characteristics, content light level, ambient viewing environment
#define DPB_ENCODER_USAGE_CHECK                           1 ///< Adds DPB encoder usage check.
#define JVET_X0048_X0103_FILM_GRAIN                       1 ///< JVET-X0048-X0103: SMPTE RDD-5 based film grain analysis and synthesis model for film grain characterstics (FGC) SEI

#define JVET_T0050_ANNOTATED_REGIONS_SEI                  1 ///< Detect static objects and use it in Annotated Regions SEI message

#if SHUTTER_INTERVAL_SEI_MESSAGE
#define SHUTTER_INTERVAL_SEI_PROCESSING                   1 ///< JCTVC-AM0024: pre-/post-processing to use shutter interval SEI
#endif
#define JVET_AK0107_MODALITY_INFORMATION                  1 // Implementation of Modality Information SEI message
// ====================================================================================================================
// Tool Switches
// ====================================================================================================================

// Please also refer to "TDecConformance.h" for DECODER_PARTIAL_CONFORMANCE_CHECK

#ifndef EXTENSION_360_VIDEO
#define EXTENSION_360_VIDEO                               0   ///< extension for 360/spherical video coding support; this macro should be controlled by makefile, as it would be used to control whether the library is built and linked
#endif

#define REDUCED_ENCODER_MEMORY                            1 ///< When 1, the encoder will allocate TComPic memory when required and release it when no longer required.

#define ADAPTIVE_QP_SELECTION                             1 ///< G382: Adaptive reconstruction levels, non-normative part for adaptive QP selection

#define AMP_ENC_SPEEDUP                                   1 ///< encoder only speed-up by AMP mode skipping
#if AMP_ENC_SPEEDUP
#define AMP_MRG                                           1 ///< encoder only force merge for AMP partition (no motion search for AMP)
#endif

#define FAST_BIT_EST                                      1 ///< G763: Table-based bit estimation for CABAC

#define HHI_RQT_INTRA_SPEEDUP                             1 ///< tests one best mode with full rqt
#define HHI_RQT_INTRA_SPEEDUP_MOD                         0 ///< tests two best modes with full rqt

#if HHI_RQT_INTRA_SPEEDUP_MOD && !HHI_RQT_INTRA_SPEEDUP
#error
#endif

#define MATRIX_MULT                                       0 ///< Brute force matrix multiplication instead of partial butterfly

#define O0043_BEST_EFFORT_DECODING                        0 ///< 0 (default) = disable code related to best effort decoding, 1 = enable code relating to best effort decoding [ decode-side only ].

#define ME_ENABLE_ROUNDING_OF_MVS                         1 ///< 0 (default) = disables rounding of motion vectors when right shifted,  1 = enables rounding

#define RDOQ_CHROMA_LAMBDA                                1 ///< F386: weighting of chroma for RDOQ

// This can be enabled by the makefile
#ifndef RExt__HIGH_BIT_DEPTH_SUPPORT
#define RExt__HIGH_BIT_DEPTH_SUPPORT                      0 ///< 0 (default) use data type definitions for 8-10 bit video, 1 = use larger data types to allow for up to 16-bit video (originally developed as part of N0188)
#endif

#if defined __SSE2__ || defined __AVX2__ || defined __AVX__ || defined _M_AMD64 || defined _M_X64
#define VECTOR_CODING__INTERPOLATION_FILTER               1 ///< enable vector coding for the interpolation filter. 1 (default if SSE possible) disable SSE vector coding. Should not affect RD costs/decisions. Code back-ported from JEM2.0.
#define VECTOR_CODING__DISTORTION_CALCULATIONS            1 ///< enable vector coding for distortion calculations   1 (default if SSE possible) disable SSE vector coding. Should not affect RD costs/decisions. Code back-ported from JEM2.0.
#else
#define VECTOR_CODING__INTERPOLATION_FILTER               0 ///< enable vector coding for the interpolation filter. 0 (default if SSE not possible) disable SSE vector coding. Should not affect RD costs/decisions. Code back-ported from JEM2.0.
#define VECTOR_CODING__DISTORTION_CALCULATIONS            0 ///< enable vector coding for distortion calculations   0 (default if SSE not possible) disable SSE vector coding. Should not affect RD costs/decisions. Code back-ported from JEM2.0.
#endif

// ====================================================================================================================
// Derived macros
// ====================================================================================================================

#if RExt__HIGH_BIT_DEPTH_SUPPORT
#define FULL_NBIT                                         1 ///< When enabled, use distortion measure derived from all bits of source data, otherwise discard (bitDepth - 8) least-significant bits of distortion
#define RExt__HIGH_PRECISION_FORWARD_TRANSFORM            1 ///< 0 use original 6-bit transform matrices for both forward and inverse transform, 1 (default) = use original matrices for inverse transform and high precision matrices for forward transform
#else
#define FULL_NBIT                                         1 ///< When enabled, use distortion measure derived from all bits of source data, otherwise discard (bitDepth - 8) least-significant bits of distortion
#define RExt__HIGH_PRECISION_FORWARD_TRANSFORM            0 ///< 0 (default) use original 6-bit transform matrices for both forward and inverse transform, 1 = use original matrices for inverse transform and high precision matrices for forward transform
#endif

#if FULL_NBIT
# define DISTORTION_PRECISION_ADJUSTMENT(x)  0
#else
# define DISTORTION_PRECISION_ADJUSTMENT(x) (x)
#endif

#if DEBUG_STRING
  #define DEBUG_STRING_PASS_INTO(name) , name
  #define DEBUG_STRING_PASS_INTO_OPTIONAL(name, exp) , (exp==0)?0:name
  #define DEBUG_STRING_FN_DECLARE(name) , std::string &name
  #define DEBUG_STRING_FN_DECLAREP(name) , std::string *name
  #define DEBUG_STRING_NEW(name) std::string name;
  #define DEBUG_STRING_OUTPUT(os, name) os << name;
  #define DEBUG_STRING_APPEND(str1, str2) str1+=str2;
  #define DEBUG_STRING_SWAP(str1, str2) str1.swap(str2);
  #define DEBUG_STRING_CHANNEL_CONDITION(compID) (true)
  #include <sstream>
  #include <iomanip>
#else
  #define DEBUG_STRING_PASS_INTO(name)
  #define DEBUG_STRING_PASS_INTO_OPTIONAL(name, exp)
  #define DEBUG_STRING_FN_DECLARE(name)
  #define DEBUG_STRING_FN_DECLAREP(name)
  #define DEBUG_STRING_NEW(name)
  #define DEBUG_STRING_OUTPUT(os, name)
  #define DEBUG_STRING_APPEND(str1, str2)
  #define DEBUG_STRING_SWAP(srt1, str2)
  #define DEBUG_STRING_CHANNEL_CONDITION(compID)
#endif

// ====================================================================================================================
// Error checks
// ====================================================================================================================

#if ((RExt__HIGH_PRECISION_FORWARD_TRANSFORM != 0) && (RExt__HIGH_BIT_DEPTH_SUPPORT == 0))
#error ERROR: cannot enable RExt__HIGH_PRECISION_FORWARD_TRANSFORM without RExt__HIGH_BIT_DEPTH_SUPPORT
#endif

// ====================================================================================================================
// Basic type redefinition
// ====================================================================================================================

typedef       void                Void;
typedef       bool                Bool;

typedef       char                TChar; // Used for text/characters
typedef       signed char         SChar; // Signed 8-bit values
typedef       unsigned char       UChar; // Unsigned 8-bit values
typedef       short               Short;
typedef       unsigned short      UShort;
typedef       int                 Int;
typedef       unsigned int        UInt;
typedef       double              Double;
typedef       float               Float;


// ====================================================================================================================
// 64-bit integer type
// ====================================================================================================================

#ifdef _MSC_VER
typedef       __int64             Int64;

#if _MSC_VER <= 1200 // MS VC6
typedef       __int64             UInt64;   // MS VC6 does not support unsigned __int64 to double conversion
#else
typedef       unsigned __int64    UInt64;
#endif

#else

typedef       long long           Int64;
typedef       unsigned long long  UInt64;

#endif

// ====================================================================================================================
// Named numerical types
// ====================================================================================================================

#if RExt__HIGH_BIT_DEPTH_SUPPORT
typedef       Int             Pel;               ///< pixel type
typedef       Int64           TCoeff;            ///< transform coefficient
typedef       Int             TMatrixCoeff;      ///< transform matrix coefficient
typedef       Short           TFilterCoeff;      ///< filter coefficient
typedef       Int64           Intermediate_Int;  ///< used as intermediate value in calculations
typedef       UInt64          Intermediate_UInt; ///< used as intermediate value in calculations
#else
typedef       Short           Pel;               ///< pixel type
typedef       Int             TCoeff;            ///< transform coefficient
typedef       Short           TMatrixCoeff;      ///< transform matrix coefficient
typedef       Short           TFilterCoeff;      ///< filter coefficient
typedef       Int             Intermediate_Int;  ///< used as intermediate value in calculations
typedef       UInt            Intermediate_UInt; ///< used as intermediate value in calculations
#endif

#if FULL_NBIT
typedef       UInt64          Distortion;        ///< distortion measurement
#else
typedef       UInt            Distortion;        ///< distortion measurement
#endif

// ====================================================================================================================
// Enumeration
// ====================================================================================================================

enum RDPCMMode
{
  RDPCM_OFF             = 0,
  RDPCM_HOR             = 1,
  RDPCM_VER             = 2,
  NUMBER_OF_RDPCM_MODES = 3
};

enum RDPCMSignallingMode
{
  RDPCM_SIGNAL_IMPLICIT            = 0,
  RDPCM_SIGNAL_EXPLICIT            = 1,
  NUMBER_OF_RDPCM_SIGNALLING_MODES = 2
};

/// supported slice type
enum SliceType
{
  B_SLICE               = 0,
  P_SLICE               = 1,
  I_SLICE               = 2,
  NUMBER_OF_SLICE_TYPES = 3
};

/// chroma formats (according to semantics of chroma_format_idc)
enum ChromaFormat
{
  CHROMA_400        = 0,
  CHROMA_420        = 1,
  CHROMA_422        = 2,
  CHROMA_444        = 3,
  NUM_CHROMA_FORMAT = 4
};

enum ChannelType
{
  CHANNEL_TYPE_LUMA    = 0,
  CHANNEL_TYPE_CHROMA  = 1,
  MAX_NUM_CHANNEL_TYPE = 2
};

enum ComponentID
{
  COMPONENT_Y       = 0,
  COMPONENT_Cb      = 1,
  COMPONENT_Cr      = 2,
  MAX_NUM_COMPONENT = 3
};

enum InputColourSpaceConversion // defined in terms of conversion prior to input of encoder.
{
  IPCOLOURSPACE_UNCHANGED               = 0,
  IPCOLOURSPACE_YCbCrtoYCrCb            = 1, // Mainly used for debug!
  IPCOLOURSPACE_YCbCrtoYYY              = 2, // Mainly used for debug!
  IPCOLOURSPACE_RGBtoGBR                = 3,
  NUMBER_INPUT_COLOUR_SPACE_CONVERSIONS = 4
};

enum MATRIX_COEFFICIENTS // Table E.5 (Matrix coefficients)
{
  MATRIX_COEFFICIENTS_RGB                           = 0,
  MATRIX_COEFFICIENTS_BT709                         = 1,
  MATRIX_COEFFICIENTS_UNSPECIFIED                   = 2,
  MATRIX_COEFFICIENTS_RESERVED_BY_ITUISOIEC         = 3,
  MATRIX_COEFFICIENTS_USFCCT47                      = 4,
  MATRIX_COEFFICIENTS_BT601_625                     = 5,
  MATRIX_COEFFICIENTS_BT601_525                     = 6,
  MATRIX_COEFFICIENTS_SMPTE240                      = 7,
  MATRIX_COEFFICIENTS_YCGCO                         = 8,
  MATRIX_COEFFICIENTS_BT2020_NON_CONSTANT_LUMINANCE = 9,
  MATRIX_COEFFICIENTS_BT2020_CONSTANT_LUMINANCE     = 10,
};

enum DeblockEdgeDir
{
  EDGE_VER     = 0,
  EDGE_HOR     = 1,
  NUM_EDGE_DIR = 2
};

/// supported partition shape
enum PartSize
{
  SIZE_2Nx2N           = 0,           ///< symmetric motion partition,  2Nx2N
  SIZE_2NxN            = 1,           ///< symmetric motion partition,  2Nx N
  SIZE_Nx2N            = 2,           ///< symmetric motion partition,   Nx2N
  SIZE_NxN             = 3,           ///< symmetric motion partition,   Nx N
  SIZE_2NxnU           = 4,           ///< asymmetric motion partition, 2Nx( N/2) + 2Nx(3N/2)
  SIZE_2NxnD           = 5,           ///< asymmetric motion partition, 2Nx(3N/2) + 2Nx( N/2)
  SIZE_nLx2N           = 6,           ///< asymmetric motion partition, ( N/2)x2N + (3N/2)x2N
  SIZE_nRx2N           = 7,           ///< asymmetric motion partition, (3N/2)x2N + ( N/2)x2N
  NUMBER_OF_PART_SIZES = 8
};

/// supported prediction type
enum PredMode
{
  MODE_INTER                 = 0,     ///< inter-prediction mode
  MODE_INTRA                 = 1,     ///< intra-prediction mode
  NUMBER_OF_PREDICTION_MODES = 2,
};

/// reference list index
enum RefPicList
{
  REF_PIC_LIST_0               = 0,   ///< reference list 0
  REF_PIC_LIST_1               = 1,   ///< reference list 1
  NUM_REF_PIC_LIST_01          = 2,
  REF_PIC_LIST_X               = 100  ///< special mark
};

/// distortion function index
enum DFunc
{
  DF_DEFAULT         = 0,
  DF_SSE             = 1,      ///< general size SSE
  DF_SSE4            = 2,      ///<   4xM SSE
  DF_SSE8            = 3,      ///<   8xM SSE
  DF_SSE16           = 4,      ///<  16xM SSE
  DF_SSE32           = 5,      ///<  32xM SSE
  DF_SSE64           = 6,      ///<  64xM SSE
  DF_SSE16N          = 7,      ///< 16NxM SSE

  DF_SAD             = 8,      ///< general size SAD
  DF_SAD4            = 9,      ///<   4xM SAD
  DF_SAD8            = 10,     ///<   8xM SAD
  DF_SAD16           = 11,     ///<  16xM SAD
  DF_SAD32           = 12,     ///<  32xM SAD
  DF_SAD64           = 13,     ///<  64xM SAD
  DF_SAD16N          = 14,     ///< 16NxM SAD

  DF_SADS            = 15,     ///< general size SAD with step
  DF_SADS4           = 16,     ///<   4xM SAD with step
  DF_SADS8           = 17,     ///<   8xM SAD with step
  DF_SADS16          = 18,     ///<  16xM SAD with step
  DF_SADS32          = 19,     ///<  32xM SAD with step
  DF_SADS64          = 20,     ///<  64xM SAD with step
  DF_SADS16N         = 21,     ///< 16NxM SAD with step

  DF_HADS            = 22,     ///< general size Hadamard with step
  DF_HADS4           = 23,     ///<   4xM HAD with step
  DF_HADS8           = 24,     ///<   8xM HAD with step
  DF_HADS16          = 25,     ///<  16xM HAD with step
  DF_HADS32          = 26,     ///<  32xM HAD with step
  DF_HADS64          = 27,     ///<  64xM HAD with step
  DF_HADS16N         = 28,     ///< 16NxM HAD with step

  DF_SAD12           = 43,
  DF_SAD24           = 44,
  DF_SAD48           = 45,

  DF_SADS12          = 46,
  DF_SADS24          = 47,
  DF_SADS48          = 48,

  DF_SSE_FRAME       = 50,     ///< Frame-based SSE
  DF_TOTAL_FUNCTIONS = 64
};

/// index for SBAC based RD optimization
enum CI_IDX
{
  CI_CURR_BEST = 0,     ///< best mode index
  CI_NEXT_BEST,         ///< next best index
  CI_TEMP_BEST,         ///< temporal index
  CI_CHROMA_INTRA,      ///< chroma intra index
  CI_QT_TRAFO_TEST,
  CI_QT_TRAFO_ROOT,
  CI_NUM,               ///< total number
};

/// motion vector predictor direction used in AMVP
enum MVP_DIR
{
  MD_LEFT = 0,          ///< MVP of left block
  MD_ABOVE,             ///< MVP of above block
  MD_ABOVE_RIGHT,       ///< MVP of above right block
  MD_BELOW_LEFT,        ///< MVP of below left block
  MD_ABOVE_LEFT         ///< MVP of above left block
};

enum StoredResidualType
{
  RESIDUAL_RECONSTRUCTED          = 0,
  RESIDUAL_ENCODER_SIDE           = 1,
  NUMBER_OF_STORED_RESIDUAL_TYPES = 2
};

enum TransformDirection
{
  TRANSFORM_FORWARD              = 0,
  TRANSFORM_INVERSE              = 1,
  TRANSFORM_NUMBER_OF_DIRECTIONS = 2
};

/// supported ME search methods
enum MESearchMethod
{
  MESEARCH_FULL              = 0,
  MESEARCH_DIAMOND           = 1,
  MESEARCH_SELECTIVE         = 2,
  MESEARCH_DIAMOND_ENHANCED  = 3,
  MESEARCH_NUMBER_OF_METHODS = 4
};

/// coefficient scanning type used in ACS
enum COEFF_SCAN_TYPE
{
  SCAN_DIAG = 0,        ///< up-right diagonal scan
  SCAN_HOR  = 1,        ///< horizontal first scan
  SCAN_VER  = 2,        ///< vertical first scan
  SCAN_NUMBER_OF_TYPES = 3
};

enum COEFF_SCAN_GROUP_TYPE
{
  SCAN_UNGROUPED   = 0,
  SCAN_GROUPED_4x4 = 1,
  SCAN_NUMBER_OF_GROUP_TYPES = 2
};

enum SignificanceMapContextType
{
  CONTEXT_TYPE_4x4    = 0,
  CONTEXT_TYPE_8x8    = 1,
  CONTEXT_TYPE_NxN    = 2,
  CONTEXT_TYPE_SINGLE = 3,
  CONTEXT_NUMBER_OF_TYPES = 4
};

enum ScalingListMode
{
  SCALING_LIST_OFF,
  SCALING_LIST_DEFAULT,
  SCALING_LIST_FILE_READ
};

enum ScalingListSize
{
  SCALING_LIST_4x4 = 0,
  SCALING_LIST_8x8,
  SCALING_LIST_16x16,
  SCALING_LIST_32x32,
  SCALING_LIST_SIZE_NUM
};

// Slice / Slice segment encoding modes
enum SliceConstraint
{
  NO_SLICES              = 0,          ///< don't use slices / slice segments
  FIXED_NUMBER_OF_CTU    = 1,          ///< Limit maximum number of largest coding tree units in a slice / slice segments
  FIXED_NUMBER_OF_BYTES  = 2,          ///< Limit maximum number of bytes in a slice / slice segment
  FIXED_NUMBER_OF_TILES  = 3,          ///< slices / slice segments span an integer number of tiles
  NUMBER_OF_SLICE_CONSTRAINT_MODES = 4
};

// For use with decoded picture hash SEI messages, generated by encoder.
enum HashType
{
  HASHTYPE_MD5             = 0,
  HASHTYPE_CRC             = 1,
  HASHTYPE_CHECKSUM        = 2,
  HASHTYPE_NONE            = 3,
  NUMBER_OF_HASHTYPES      = 4
};

enum SAOMode //mode
{
  SAO_MODE_OFF = 0,
  SAO_MODE_NEW,
  SAO_MODE_MERGE,
  NUM_SAO_MODES
};

enum SAOModeMergeTypes
{
  SAO_MERGE_LEFT =0,
  SAO_MERGE_ABOVE,
  NUM_SAO_MERGE_TYPES
};


enum SAOModeNewTypes
{
  SAO_TYPE_START_EO =0,
  SAO_TYPE_EO_0 = SAO_TYPE_START_EO,
  SAO_TYPE_EO_90,
  SAO_TYPE_EO_135,
  SAO_TYPE_EO_45,

  SAO_TYPE_START_BO,
  SAO_TYPE_BO = SAO_TYPE_START_BO,

  NUM_SAO_NEW_TYPES
};
#define NUM_SAO_EO_TYPES_LOG2 2

enum SAOEOClasses
{
  SAO_CLASS_EO_FULL_VALLEY = 0,
  SAO_CLASS_EO_HALF_VALLEY = 1,
  SAO_CLASS_EO_PLAIN       = 2,
  SAO_CLASS_EO_HALF_PEAK   = 3,
  SAO_CLASS_EO_FULL_PEAK   = 4,
  NUM_SAO_EO_CLASSES,
};

#define NUM_SAO_BO_CLASSES_LOG2  5
#define NUM_SAO_BO_CLASSES       (1<<NUM_SAO_BO_CLASSES_LOG2)

namespace Profile
{
  enum Name
  {
    NONE = 0,
    MAIN = 1,
    MAIN10 = 2,
    MAINSTILLPICTURE = 3,
    MAINREXT = 4,
    HIGHTHROUGHPUTREXT = 5
  };
}

namespace Level
{
  enum Tier
  {
    MAIN = 0,
    HIGH = 1,
    NUMBER_OF_TIERS = 2
  };

  enum Name
  {
    // code = (level * 30)
    NONE     = 0,
    LEVEL1   = 30,
    LEVEL2   = 60,
    LEVEL2_1 = 63,
    LEVEL3   = 90,
    LEVEL3_1 = 93,
    LEVEL4   = 120,
    LEVEL4_1 = 123,
    LEVEL5   = 150,
    LEVEL5_1 = 153,
    LEVEL5_2 = 156,
    LEVEL6   = 180,
    LEVEL6_1 = 183,
    LEVEL6_2 = 186,
#if JVET_X0079_MODIFIED_BITRATES
    LEVEL6_3 = 189,
#endif
    LEVEL8_5 = 255,
  };
}

enum CostMode
{
  COST_STANDARD_LOSSY              = 0,
  COST_SEQUENCE_LEVEL_LOSSLESS     = 1,
  COST_LOSSLESS_CODING             = 2,
  COST_MIXED_LOSSLESS_LOSSY_CODING = 3
};

enum WeightedPredictionMethod
{
  WP_PER_PICTURE_WITH_SIMPLE_DC_COMBINED_COMPONENT                          =0,
  WP_PER_PICTURE_WITH_SIMPLE_DC_PER_COMPONENT                               =1,
  WP_PER_PICTURE_WITH_HISTOGRAM_AND_PER_COMPONENT                           =2,
  WP_PER_PICTURE_WITH_HISTOGRAM_AND_PER_COMPONENT_AND_CLIPPING              =3,
  WP_PER_PICTURE_WITH_HISTOGRAM_AND_PER_COMPONENT_AND_CLIPPING_AND_EXTENSION=4
};

enum FastInterSearchMode
{
  FASTINTERSEARCH_DISABLED = 0,
  FASTINTERSEARCH_MODE1    = 1, // TODO: assign better names to these.
  FASTINTERSEARCH_MODE2    = 2,
  FASTINTERSEARCH_MODE3    = 3
};

enum SPSExtensionFlagIndex
{
  SPS_EXT__REXT           = 0,
//SPS_EXT__MVHEVC         = 1, //for use in future versions
//SPS_EXT__SHVC           = 2, //for use in future versions
  NUM_SPS_EXTENSION_FLAGS = 8
};

enum PPSExtensionFlagIndex
{
  PPS_EXT__REXT           = 0,
//PPS_EXT__MVHEVC         = 1, //for use in future versions
//PPS_EXT__SHVC           = 2, //for use in future versions
  NUM_PPS_EXTENSION_FLAGS = 8
};

// TODO: Existing names used for the different NAL unit types can be altered to better reflect the names in the spec.
//       However, the names in the spec are not yet stable at this point. Once the names are stable, a cleanup
//       effort can be done without use of macros to alter the names used to indicate the different NAL unit types.
enum NalUnitType
{
  NAL_UNIT_CODED_SLICE_TRAIL_N = 0, // 0
  NAL_UNIT_CODED_SLICE_TRAIL_R,     // 1

  NAL_UNIT_CODED_SLICE_TSA_N,       // 2
  NAL_UNIT_CODED_SLICE_TSA_R,       // 3

  NAL_UNIT_CODED_SLICE_STSA_N,      // 4
  NAL_UNIT_CODED_SLICE_STSA_R,      // 5

  NAL_UNIT_CODED_SLICE_RADL_N,      // 6
  NAL_UNIT_CODED_SLICE_RADL_R,      // 7

  NAL_UNIT_CODED_SLICE_RASL_N,      // 8
  NAL_UNIT_CODED_SLICE_RASL_R,      // 9

  NAL_UNIT_RESERVED_VCL_N10,
  NAL_UNIT_RESERVED_VCL_R11,
  NAL_UNIT_RESERVED_VCL_N12,
  NAL_UNIT_RESERVED_VCL_R13,
  NAL_UNIT_RESERVED_VCL_N14,
  NAL_UNIT_RESERVED_VCL_R15,

  NAL_UNIT_CODED_SLICE_BLA_W_LP,    // 16
  NAL_UNIT_CODED_SLICE_BLA_W_RADL,  // 17
  NAL_UNIT_CODED_SLICE_BLA_N_LP,    // 18
  NAL_UNIT_CODED_SLICE_IDR_W_RADL,  // 19
  NAL_UNIT_CODED_SLICE_IDR_N_LP,    // 20
  NAL_UNIT_CODED_SLICE_CRA,         // 21
  NAL_UNIT_RESERVED_IRAP_VCL22,
  NAL_UNIT_RESERVED_IRAP_VCL23,

  NAL_UNIT_RESERVED_VCL24,
  NAL_UNIT_RESERVED_VCL25,
  NAL_UNIT_RESERVED_VCL26,
  NAL_UNIT_RESERVED_VCL27,
  NAL_UNIT_RESERVED_VCL28,
  NAL_UNIT_RESERVED_VCL29,
  NAL_UNIT_RESERVED_VCL30,
  NAL_UNIT_RESERVED_VCL31,

  NAL_UNIT_VPS,                     // 32
  NAL_UNIT_SPS,                     // 33
  NAL_UNIT_PPS,                     // 34
  NAL_UNIT_ACCESS_UNIT_DELIMITER,   // 35
  NAL_UNIT_EOS,                     // 36
  NAL_UNIT_EOB,                     // 37
  NAL_UNIT_FILLER_DATA,             // 38
  NAL_UNIT_PREFIX_SEI,              // 39
  NAL_UNIT_SUFFIX_SEI,              // 40

  NAL_UNIT_RESERVED_NVCL41,
  NAL_UNIT_RESERVED_NVCL42,
  NAL_UNIT_RESERVED_NVCL43,
  NAL_UNIT_RESERVED_NVCL44,
  NAL_UNIT_RESERVED_NVCL45,
  NAL_UNIT_RESERVED_NVCL46,
  NAL_UNIT_RESERVED_NVCL47,
  NAL_UNIT_UNSPECIFIED_48,
  NAL_UNIT_UNSPECIFIED_49,
  NAL_UNIT_UNSPECIFIED_50,
  NAL_UNIT_UNSPECIFIED_51,
  NAL_UNIT_UNSPECIFIED_52,
  NAL_UNIT_UNSPECIFIED_53,
  NAL_UNIT_UNSPECIFIED_54,
  NAL_UNIT_UNSPECIFIED_55,
  NAL_UNIT_UNSPECIFIED_56,
  NAL_UNIT_UNSPECIFIED_57,
  NAL_UNIT_UNSPECIFIED_58,
  NAL_UNIT_UNSPECIFIED_59,
  NAL_UNIT_UNSPECIFIED_60,
  NAL_UNIT_UNSPECIFIED_61,
  NAL_UNIT_UNSPECIFIED_62,
  NAL_UNIT_UNSPECIFIED_63,
  NAL_UNIT_INVALID,
};

enum LumaLevelToDQPMode
{
  LUMALVL_TO_DQP_DISABLED   = 0,
  LUMALVL_TO_DQP_AVG_METHOD = 1, // use average of CTU to determine luma level
  LUMALVL_TO_DQP_MAX_METHOD = 2,  // use maximum value of CTU to determine luma level
  LUMALVL_TO_DQP_NUM_MODES  = 3
};

// ---------------------------------------------------------------------------
// exception class
// ---------------------------------------------------------------------------

class Exception : public std::exception
{
public:
  Exception( const std::string& _s ) : m_str( _s ) { }
  Exception( const Exception& _e ) : std::exception( _e ), m_str( _e.m_str ) { }
  virtual ~Exception() noexcept { };
  virtual const char* what() const noexcept { return m_str.c_str(); }
  Exception& operator=( const Exception& _e ) { std::exception::operator=( _e ); m_str = _e.m_str; return *this; }
  template<typename T> Exception& operator<<( T t ) { std::ostringstream oss; oss << t; m_str += oss.str(); return *this; }
private:
  std::string m_str;
};

#define THROW(x)            throw( Exception( "\nERROR: In function \"" ) << __FUNCTION__ << "\" in " << __FILE__ << ":" << __LINE__ << ": " << x )
#define CHECK(c,x)          if(c){ THROW(x); }
#define EXIT(x)             throw( Exception( "\n" ) << x << "\n" )
#define CHECK_NULLPTR(_ptr) CHECK( !( _ptr ), "Accessing an empty pointer pointer!" )

// ====================================================================================================================
// Type definition
// ====================================================================================================================

/// parameters for adaptive loop filter
class TComPicSym;

#define MAX_NUM_SAO_CLASSES  32  //(NUM_SAO_EO_GROUPS > NUM_SAO_BO_GROUPS)?NUM_SAO_EO_GROUPS:NUM_SAO_BO_GROUPS

struct SAOOffset
{
  SAOMode modeIdc; // NEW, MERGE, OFF
  Int typeIdc;     // union of SAOModeMergeTypes and SAOModeNewTypes, depending on modeIdc.
  Int typeAuxInfo; // BO: starting band index
  Int offset[MAX_NUM_SAO_CLASSES];

  SAOOffset();
  ~SAOOffset();
  Void reset();

  const SAOOffset& operator= (const SAOOffset& src);
};

struct SAOBlkParam
{

  SAOBlkParam();
  ~SAOBlkParam();
  Void reset();
  const SAOBlkParam& operator= (const SAOBlkParam& src);
  SAOOffset& operator[](Int compIdx){ return offsetParam[compIdx];}
private:
  SAOOffset offsetParam[MAX_NUM_COMPONENT];

};


struct BitDepths
{
#if JVET_X0048_X0103_FILM_GRAIN
  const int &operator[](const ChannelType ch) const { return recon[ch]; }
#endif
#if O0043_BEST_EFFORT_DECODING
  Int recon[MAX_NUM_CHANNEL_TYPE]; ///< the bit depth used for reconstructing the video
  Int stream[MAX_NUM_CHANNEL_TYPE];///< the bit depth used indicated in the SPS
#else
  Int recon[MAX_NUM_CHANNEL_TYPE]; ///< the bit depth as indicated in the SPS
#endif
};

/// parameters for deblocking filter
typedef struct _LFCUParam
{
  Bool bInternalEdge;                     ///< indicates internal edge
  Bool bLeftEdge;                         ///< indicates left edge
  Bool bTopEdge;                          ///< indicates top edge
} LFCUParam;



//TU settings for entropy encoding
struct TUEntropyCodingParameters
{
  const UInt            *scan;
  const UInt            *scanCG;
        COEFF_SCAN_TYPE  scanType;
        UInt             widthInGroups;
        UInt             heightInGroups;
        UInt             firstSignificanceMapContext;
};


struct TComPictureHash
{
  std::vector<UChar> hash;

  Bool operator==(const TComPictureHash &other) const
  {
    if (other.hash.size() != hash.size())
    {
      return false;
    }
    for(UInt i=0; i<UInt(hash.size()); i++)
    {
      if (other.hash[i] != hash[i])
      {
        return false;
      }
    }
    return true;
  }

  Bool operator!=(const TComPictureHash &other) const
  {
    return !(*this == other);
  }
};

struct TComSEITimeSet
{
  TComSEITimeSet() : clockTimeStampFlag(false),
                     numUnitFieldBasedFlag(false),
                     countingType(0),
                     fullTimeStampFlag(false),
                     discontinuityFlag(false),
                     cntDroppedFlag(false),
                     numberOfFrames(0),
                     secondsValue(0),
                     minutesValue(0),
                     hoursValue(0),
                     secondsFlag(false),
                     minutesFlag(false),
                     hoursFlag(false),
                     timeOffsetLength(0),
                     timeOffsetValue(0)
  { }
  Bool clockTimeStampFlag;
  Bool numUnitFieldBasedFlag;
  Int  countingType;
  Bool fullTimeStampFlag;
  Bool discontinuityFlag;
  Bool cntDroppedFlag;
  Int  numberOfFrames;
  Int  secondsValue;
  Int  minutesValue;
  Int  hoursValue;
  Bool secondsFlag;
  Bool minutesFlag;
  Bool hoursFlag;
  Int  timeOffsetLength;
  Int  timeOffsetValue;
};

struct TComSEIMasteringDisplay
{
  Bool      colourVolumeSEIEnabled;
  UInt      maxLuminance;
  UInt      minLuminance;
  UShort    primaries[3][2];
  UShort    whitePoint[2];
};

struct LumaLevelToDeltaQPMapping
{
  LumaLevelToDQPMode                 mode;             ///< use deltaQP determined by block luma level
  Double                             maxMethodWeight;  ///< weight of max luma value when mode = 2
  std::vector< std::pair<Int, Int> > mapping;          ///< first=luma level, second=delta QP.
  Bool isEnabled() const { return mode!=LUMALVL_TO_DQP_DISABLED; }
};

struct WCGChromaQPControl
{
  Bool isEnabled() const { return enabled; }
  Bool   enabled;         ///< Enabled flag (0:default)
  Double chromaCbQpScale; ///< Chroma Cb QP Scale (1.0:default)
  Double chromaCrQpScale; ///< Chroma Cr QP Scale (1.0:default)
  Double chromaQpScale;   ///< Chroma QP Scale (0.0:default)
  Double chromaQpOffset;  ///< Chroma QP Offset (0.0:default)
};

struct TComSEIFisheyeVideoInfo
{
  struct ActiveAreaInfo
  {
    UInt m_fisheyeCircularRegionCentreX;
    UInt m_fisheyeCircularRegionCentreY;
    UInt m_fisheyeRectRegionTop;
    UInt m_fisheyeRectRegionLeft;
    UInt m_fisheyeRectRegionWidth;
    UInt m_fisheyeRectRegionHeight;
    UInt m_fisheyeCircularRegionRadius;
    UInt m_fisheyeSceneRadius;

    Int  m_fisheyeCameraCentreAzimuth;
    Int  m_fisheyeCameraCentreElevation;
    Int  m_fisheyeCameraCentreTilt;
    UInt m_fisheyeCameraCentreOffsetX;
    UInt m_fisheyeCameraCentreOffsetY;
    UInt m_fisheyeCameraCentreOffsetZ;
    UInt m_fisheyeFieldOfView;
    std::vector<Int> m_fisheyePolynomialCoeff;
  };

  Bool  m_fisheyeCancelFlag;
  Bool  m_fisheyePersistenceFlag;
  UInt  m_fisheyeViewDimensionIdc;
  std::vector<ActiveAreaInfo> m_fisheyeActiveAreas;
  TComSEIFisheyeVideoInfo() : m_fisheyeCancelFlag(false), m_fisheyePersistenceFlag(false), m_fisheyeViewDimensionIdc(0), m_fisheyeActiveAreas() { }
};

class Window
{
private:
  Bool m_enabledFlag;
  Int  m_winLeftOffset;
  Int  m_winRightOffset;
  Int  m_winTopOffset;
  Int  m_winBottomOffset;
public:
  Window()
  : m_enabledFlag    (false)
  , m_winLeftOffset  (0)
  , m_winRightOffset (0)
  , m_winTopOffset   (0)
  , m_winBottomOffset(0)
  { }

  Bool getWindowEnabledFlag() const   { return m_enabledFlag;                          }
  Int  getWindowLeftOffset() const    { return m_enabledFlag ? m_winLeftOffset : 0;    }
  Void setWindowLeftOffset(Int val)   { m_winLeftOffset = val; m_enabledFlag = true;   }
  Int  getWindowRightOffset() const   { return m_enabledFlag ? m_winRightOffset : 0;   }
  Void setWindowRightOffset(Int val)  { m_winRightOffset = val; m_enabledFlag = true;  }
  Int  getWindowTopOffset() const     { return m_enabledFlag ? m_winTopOffset : 0;     }
  Void setWindowTopOffset(Int val)    { m_winTopOffset = val; m_enabledFlag = true;    }
  Int  getWindowBottomOffset() const  { return m_enabledFlag ? m_winBottomOffset: 0;   }
  Void setWindowBottomOffset(Int val) { m_winBottomOffset = val; m_enabledFlag = true; }

  Void setWindow(Int offsetLeft, Int offsetRight, Int offsetTop, Int offsetBottom)
  {
    m_enabledFlag     = (offsetLeft || offsetRight || offsetTop || offsetBottom);
    m_winLeftOffset   = offsetLeft;
    m_winRightOffset  = offsetRight;
    m_winTopOffset    = offsetTop;
    m_winBottomOffset = offsetBottom;
  }
  Bool operator == (const Window &rhs) const
  {
    return ( m_enabledFlag && rhs.m_enabledFlag && 
      m_winLeftOffset == rhs.m_winLeftOffset &&
      m_winRightOffset == rhs.m_winRightOffset &&
      m_winTopOffset == rhs.m_winTopOffset &&
      m_winBottomOffset == rhs.m_winBottomOffset
      );
  }
};

class RNSEIWindow : public Window
{
private:
  UInt m_regionId;
public:
  RNSEIWindow() : Window(), m_regionId(0) {}
  Int  getRegionId() const     { return getWindowEnabledFlag() ? m_regionId : 0;    }
  Void setRegionId(UInt const val)   { m_regionId = val; }
  // Check two RNSEIWindows are identical
  Bool operator == (RNSEIWindow const &rhs) const
  {
    return ( (m_regionId == rhs.m_regionId) &&
      Window(*this) == Window(rhs) );
  }
  // Check if two windows are identical, even though regions may be different
  Bool checkSameRegion(RNSEIWindow const &rhs) const
  {
    return ( Window(*this) == Window(rhs) );
  }
  Bool checkSameID(RNSEIWindow const &rhs) const
  {
    return ( m_regionId == rhs.m_regionId );
  }
  friend std::ostream& operator<<(std::ostream  &os, RNSEIWindow const &region);
};
typedef std::vector<RNSEIWindow> RNSEIWindowVec;
//! \}

#endif


