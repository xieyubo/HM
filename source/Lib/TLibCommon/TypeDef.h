/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.  
 *
 * Copyright (c) 2010-2011, ITU/ISO/IEC
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
    \brief    Define basic types, new types and enumerations
*/

#ifndef _TYPEDEF__
#define _TYPEDEF__

////////////////////////////
// BoG Context Reduction
////////////////////////////

#define MODIFIED_MVD_CODING               1           ///< F455 / F423:        modified binarization, coding and bin reordering of MVD
#define MODIFIED_LAST_CODING              1           ///< F375:               modified binarization and coding of last significant position
#define DNB_MERGE_FLAG                    1           ///< F429:               disable of neighbour evaluation for merge_flag
#define DNB_INTRA_CHR_PRED_MODE           1           ///< F497:               disable of neighbour evaluation for intra_chroma_pred_mode
#define DNB_REF_FRAME_IDX                 1           ///< F429 / F497 / F606: disable of neighbour evaluation for ref_frame_idx
#define DNB_ALF_CTRL_FLAG                 1           ///< F429:               disable of neighbour evaluation for alf_ctrl_flag
#define DNB_INTER_PRED_MODE               1           ///< F606:               disable of neighbour evaluation for inter_pred_flag (use depth for that)
#define DNB_CHROMA_CBF_FLAGS              1           ///< F606:               disable of neighbour evaluation for chroma cbf (use depth for that)
#define DNB_LUMA_CBF_FLAGS                1           ///< F429:               disable of neighbour evaluation for luma cbf

////////////////////////////
// JCT-VC E start
////////////////////////////

#define MOT_TUPU_MAXDEPTH1                1           ///< E364, "Implicit TU" derivation when there is no TU tree (i.e., "max depth = 1"), the transform blocks should not cross PU boundaries
////////////////////////////
// JCT-VC E end
////////////////////////////

#define HHI_RQT_INTRA_SPEEDUP             1           ///< tests one best mode with full rqt
#define HHI_RQT_INTRA_SPEEDUP_MOD         0           ///< tests two best modes with full rqt

#define PART_MRG                          0            // If the number of partitions is two and size > 8, only merging mode is enabled for the first partition & do not code merge_flag for the first partition
#define HHI_MRG_SKIP                      1            // (JCTVC-E481 - merge skip) replaces the AMVP based skip by merge based skip (E481 - MERGE skip)

#if HHI_RQT_INTRA_SPEEDUP_MOD && !HHI_RQT_INTRA_SPEEDUP
#error
#endif

#define VERBOSE_RATE 0 ///< Print additional rate information in encoder

// COLOCATED PREDICTOR
// FOR MERGE
#define MRG_NEIGH_COL                     1           ///< use of colocated MB in MERGE
#define FT_TCTR_MRG                       1           ///< central colocated in MERGE
#if !FT_TCTR_MRG
#define PANASONIC_MERGETEMPORALEXT        1           ///< 
#endif
#define MTK_TMVP_H_MRG                    1           ///< (JCTVC-E481 - D125 2.1) right-bottom collocated for merge
#define PANASONIC_MRG_TMVP_REFIDX         1           ///< (JCTVC-E481 - D274 (2) ) refidx derivation for merge TMVP  
// FOR AMVP
#define AMVP_NEIGH_COL                    1           ///< use of colocated MB in AMVP
#define FT_TCTR_AMVP                      1           ///< central colocated in AMVP
#if !FT_TCTR_AMVP
#define PANASONIC_AMVPTEMPORALEXT         1           ///< 
#endif
#define MTK_TMVP_H_AMVP                   1           ///< (JCTVC-E481 - D125 2.1) right-bottom collocated for amvp 
// FOR BOTH
#define PANASONIC_AMVPTEMPORALMOD         1           ///< (JCTVC-E481 - D125 2.4' / D274 3')
#define AMVP_BUFFERCOMPRESS               1           ///< motion vector buffer compression
#define AMVP_DECIMATION_FACTOR            4
#define MV_COMPRESS_MODE_REFIDX           1           ///< (JCTVC-E147) compress all inter prediction parameters according to 1)

#define REMOVE_INTERMEDIATE_CLIPPING      1          // No intermediate clipping in bi-prediction JCTVC-E242

#define UNIFIED_SCAN                      1           // JCTVC-F288+F132(F746) Unified Scans for significance map and coefficient level
#if UNIFIED_SCAN
#define SCAN_SET_SIZE                     16
#define LOG2_SCAN_SET_SIZE                4
#endif
#define DIAG_SCAN                         1
#define CABAC_COEFF_DATA_REORDER          1           ///< JCTVC-F130: reordering of CABAC coefficient data
#define QC_MDIS                           1           // JCTVC-D282: enable mode dependent intra smoothing
#define QC_MDCS                           1           // JCTVC-D393: mode dependent coefficients coding 
#define RUNLEVEL_TABLE_CUT                1           // JCTVC-E384: Run-Level table size reduction
#if RUNLEVEL_TABLE_CUT
#define CAVLC_COEF_LRG_BLK                1           // JCTVC-E383: enable large block coeff. coding
#endif


#define ENABLE_FORCECOEFF0  0

#define FAST_UDI_MAX_RDMODE_NUM               35          ///< maximum number of RD comparison in fast-UDI estimation loop 

#define ZERO_MVD_EST                          0           ///< Zero Mvd Estimation in normal mode

#define LM_CHROMA                             1           // JCTVC-E266: Chroma intra prediction based on luma signal

#define UNIFY_INTER_TABLE                     1           // JCTVC-E381 CAVLC: Inter pred coding

#define MTK_AMVP_SMVP_DERIVATION          1              ///< (JCTVC-E481 - D125 2.3) amvp spatial candidate derivation
#define TI_AMVP_SMVP_SIMPLIFIED           1              ///< (JCTVC-E481 - F)amvp spatial candidate simplified scanning
#define MV_SCALE_ROUNDING_F142            1           ///< Modified rounding when scaling motion vectors (JCTVC-F142)

#define ADD_PLANAR_MODE                   1           ///< enable/disable Planar mode for intra prediction (JCTVC-E321)

#if ADD_PLANAR_MODE || LM_CHROMA
#define NUM_INTRA_MODE 36
#define PLANAR_IDX     34
#define LM_CHROMA_IDX  35
#define PLANAR_F483 1 ///< Modify samples used for planar prediction as per JCTVC-F483
#endif
#define CHROMA_CODEWORD_SWITCH 0 ///< Enable chroma codeword switch (disabled as per JCTVC-F465)

#define IBDI_DISTORTION                0           ///< enable/disable SSE modification when IBDI is used (JCTVC-D152)
#define FIXED_ROUNDING_FRAME_MEMORY    0           ///< enable/disable fixed rounding to 8-bitdepth of frame memory when IBDI is used  

#define MS_LCEC_UNI_EXCEPTION_THRES     1           // for GPB case, uni-prediction, > MS_LCEC_UNI_EXCEPTION_THRES is exception
#define CAVLC_COUNTER_ADAPT             1          // counter based CAVLC adaptation, JCTVC-E143
#if CAVLC_COUNTER_ADAPT
#define CAVLC_RQT_CBP                   1           //CAVLC coding of cbf and split flag, JCTVC-E404
#endif


#define AVOID_ZERO_MERGE_CANDIDATE      1           // (JCTVC-E146/E118) insert zero MV if no merge candidates are available
#define CHANGE_MERGE_CONTEXT            1           // (JCTVC-E146/E118) change merge flag context derivation
#define CHANGE_GET_MERGE_CANDIDATE      1           // (JCTVC-E146/E118) merge flag parsing independent of number of merge candidates
#if CHANGE_GET_MERGE_CANDIDATE && !CHANGE_MERGE_CONTEXT
#error CHANGE_GET_MERGE_CANDIDATE can only be defined with CHANGE_MERGE_CONTEXT
#endif

#define MTK_NONCROSS_INLOOP_FILTER        1           ///< Allow non-cross-slice-boundary in-loop filtering, including DB & ALF (JCTVC-D128)

#define RVM_VCEGAM10 1 // RVM model proposed in VCEG-AM10
#if RVM_VCEGAM10
#define RVM_VCEGAM10_M 4
#endif

#define MTK_DCM_MPM 1 // MostProbableModeSignaling

#define FAST_UDI_USE_MPM 1
#define SUB_LCU_DQP  1                               ///< syntax change of sub-LCU-level dQP (JCTVC-E051/220/391/436/217/D038/D258)

#define TI_ALF_MAX_VSIZE_7 1

#define FULL_NBIT 0 ///< When enabled, does not use g_uiBitIncrement anymore to support > 8 bit data

/////////////////////////////////
// AHG SLICES defines section start
/////////////////////////////////
#define FINE_GRANULARITY_SLICES 1
#define AD_HOC_SLICES_FIXED_NUMBER_OF_LCU_IN_SLICE      1          ///< OPTION IDENTIFIER. mode==1 -> Limit maximum number of largest coding tree blocks in a slice
#define AD_HOC_SLICES_FIXED_NUMBER_OF_BYTES_IN_SLICE    2          ///< OPTION IDENTIFIER. mode==2 -> Limit maximum number of bins/bits in a slice

// Entropy slice options
#define SHARP_FIXED_NUMBER_OF_LCU_IN_ENTROPY_SLICE            1          ///< OPTION IDENTIFIER. Limit maximum number of largest coding tree blocks in an entropy slice
#define SHARP_MULTIPLE_CONSTRAINT_BASED_ENTROPY_SLICE         2          ///< OPTION IDENTIFIER. Limit maximum number of bins/bits in an entropy slice
/////////////////////////////////
// AHG SLICES defines section end
/////////////////////////////////

#define MTK_SAO                           1           // JCTVC-E049: Sample adaptive offset

#define MQT_ALF_NPASS                       1

#define MQT_BA_RA                        1  // JCTVC-E323+E046
#if MQT_BA_RA
#define VAR_SIZE_H           4
#define VAR_SIZE_W           4
#define NO_VAR_BIN          16
#endif

#if QC_MDIS
#define MN_MDIS_SIMPLIFICATION       1       ///< JCTVC-E069: simplification of MDIS
#endif
#define MN_DC_PRED_FILTER            1       ///< JCTVC-E069: DC prediction samples filtering

#define MVD_CTX            1           // JCTVC-E324: Modified context selection for MVD
#define PARALLEL_DEBLK_DECISION      1 // JCTC-E224: Parallel decisions
#define PARALLEL_MERGED_DEBLK        1 // JCTC-E224, JCTVC-E181: Parallel decisions + Parallel filtering
#define REFERENCE_SAMPLE_PADDING                1   // JCTVC-E488 padding of unavailable reference samples for intra prediction

#define MATRIX_MULT                             0   // Brute force matrix multiplication instead of partial butterfly

// Discrete Sine Transform (DST) Type - 7
// Currently DST operates with E-243 only
#define INTRA_DST_TYPE_7                      1           // JCTVC-E125 4x4 DST
#if INTRA_DST_TYPE_7
#define REG_DCT 65535
#endif

#define E057_INTRA_PCM                      1 // JCTVC-E057 PCM operation mode 2: Signal I_PCM flag when CU is 2Nx2N intra and its size is larger than or equal to 1<<(LOG2_MIN_I_PCM_CODING_BLOCK_SIZE_MINUS3+3).
#if E057_INTRA_PCM
#define E192_SPS_PCM_BIT_DEPTH_SYNTAX       1 // JCTVC-E192: PCM bit depth
#define E192_SPS_PCM_FILTER_DISABLE_SYNTAX  1 // JCTVC-E192: PCM filter disable flag
#endif

#define E045_SLICE_COMMON_INFO_SHARING 1 //JCTVC-E045: Slice common information sharing

// ====================================================================================================================
// Basic type redefinition
// ====================================================================================================================

typedef       void                Void;
typedef       bool                Bool;

typedef       char                Char;
typedef       unsigned char       UChar;
typedef       short               Short;
typedef       unsigned short      UShort;
typedef       int                 Int;
typedef       unsigned int        UInt;
typedef       long                Long;
typedef       unsigned long       ULong;
typedef       double              Double;

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
// Type definition
// ====================================================================================================================

typedef       UChar           Pxl;        ///< 8-bit pixel type
typedef       Short           Pel;        ///< 16-bit pixel type
typedef       Int             TCoeff;     ///< transform coefficient

/// parameters for adaptive loop filter
class TComPicSym;

#if MTK_SAO

#define NUM_DOWN_PART 4

enum QAOTypeLen
{
  SAO_EO_LEN    = 4, 
  SAO_EO_LEN_2D = 6, 
  SAO_BO_LEN    = 16
};

enum QAOType
{
  SAO_EO_0 = 0, 
  SAO_EO_1,
  SAO_EO_2, 
  SAO_EO_3,
  SAO_BO_0,
  SAO_BO_1,
  MAX_NUM_SAO_TYPE
};

typedef struct _SaoQTPart
{
  Bool        bEnableFlag;
  Int         iBestType;
  Int         iLength;
  Int         iOffset[32];

  Int         StartCUX;
  Int         StartCUY;
  Int         EndCUX;
  Int         EndCUY;

  Int         part_xs;
  Int         part_xe;
  Int         part_ys;
  Int         part_ye;
  Int         part_width;
  Int         part_height;

  Int         PartIdx;
  Int         PartLevel;
  Int         PartCol;
  Int         PartRow;

  Int         DownPartsIdx[NUM_DOWN_PART];
  Int         UpPartIdx;

  Int*        pSubPartList;
  Int         iLengthSubPartList;

  Bool        bBottomLevel;
  Bool        bSplit;
  //    Bool        bAvailable;

  //---- encoder only start -----//
  Int64***    pppiCorr; //[filt_type][corr_row][corr_col]
  Int**       ppCoeff;  //[filt_type][coeff]
  Bool        bProcessed;
  Double      dMinCost;
  Int64       iMinDist;
  Int         iMinRate;
  //---- encoder only end -----//
} SAOQTPart;

struct _SaoParam
{
  Bool       bSaoFlag;
  SAOQTPart* psSaoPart;
  Int        iMaxSplitLevel;
  Int        iNumClass[MAX_NUM_SAO_TYPE];
};

#endif

struct _AlfParam
{
  Int alf_flag;                           ///< indicates use of ALF
  Int cu_control_flag;                    ///< coding unit based control flag
  Int chroma_idc;                         ///< indicates use of ALF for chroma
#if TI_ALF_MAX_VSIZE_7
  Int tap;                                ///< number of filter taps - horizontal
  Int tapV;                               ///< number of filter taps - vertical
#else
  Int tap;                                ///< number of filter taps
#endif
  Int num_coeff;                          ///< number of filter coefficients
  Int *coeff;                             ///< filter coefficient array
  Int tap_chroma;                         ///< number of filter taps (chroma)
  Int num_coeff_chroma;                   ///< number of filter coefficients (chroma)
  Int *coeff_chroma;                      ///< filter coefficient array (chroma)
  //CodeAux related
  Int realfiltNo;
  Int filtNo;
#if MQT_BA_RA 
  Int filterPattern[NO_VAR_BIN];
#else
  Int filterPattern[16];
#endif
  Int startSecondFilter;
  Int noFilters;
#if MQT_BA_RA 
  Int varIndTab[NO_VAR_BIN];
#else
  Int varIndTab[16];
#endif
  
  //Coeff send related
  Int filters_per_group_diff; //this can be updated using codedVarBins
  Int filters_per_group;
#if MQT_BA_RA  
  Int codedVarBins[NO_VAR_BIN]; 
#else
  Int codedVarBins[16]; 
#endif 
  Int forceCoeff0;
  Int predMethod;
  Int **coeffmulti;
  Int minKStart;
  Int maxScanVal;
  Int kMinTab[42];
  UInt num_alf_cu_flag;
  UInt num_cus_in_frame;
  UInt alf_max_depth;
  UInt *alf_cu_flag;

#if MQT_BA_RA
  Int alf_pcr_region_flag; 
#endif
};

/// parameters for deblocking filter
typedef struct _LFCUParam
{
  Bool bInternalEdge;                     ///< indicates internal edge
  Bool bLeftEdge;                         ///< indicates left edge
  Bool bTopEdge;                          ///< indicates top edge
} LFCUParam;

/// parapeters for TENTM coefficient VLC
typedef struct _LastCoeffStruct
{
  int level;
  int last_pos;
} LastCoeffStruct;

// ====================================================================================================================
// Enumeration
// ====================================================================================================================

/// supported slice type
enum SliceType
{
  I_SLICE,
  P_SLICE,
  B_SLICE
};

/// supported partition shape
enum PartSize
{
  SIZE_2Nx2N,           ///< symmetric motion partition,  2Nx2N
  SIZE_2NxN,            ///< symmetric motion partition,  2Nx N
  SIZE_Nx2N,            ///< symmetric motion partition,   Nx2N
  SIZE_NxN,             ///< symmetric motion partition,   Nx N
  
  SIZE_NONE = 15
};

/// supported prediction type
enum PredMode
{
  MODE_SKIP,            ///< SKIP mode
  MODE_INTER,           ///< inter-prediction mode
  MODE_INTRA,           ///< intra-prediction mode
  MODE_NONE = 15
};

/// texture component type
enum TextType
{
  TEXT_LUMA,            ///< luma
  TEXT_CHROMA,          ///< chroma (U+V)
  TEXT_CHROMA_U,        ///< chroma U
  TEXT_CHROMA_V,        ///< chroma V
  TEXT_ALL,             ///< Y+U+V
  TEXT_NONE = 15
};

/// reference list index
enum RefPicList
{
  REF_PIC_LIST_0 = 0,   ///< reference list 0
  REF_PIC_LIST_1 = 1,   ///< reference list 1
  REF_PIC_LIST_C = 2,   ///< combined reference list for uni-prediction in B-Slices
  REF_PIC_LIST_X = 100  ///< special mark
};

/// distortion function index
enum DFunc
{
  DF_DEFAULT  = 0,
  DF_SSE      = 1,      ///< general size SSE
  DF_SSE4     = 2,      ///<   4xM SSE
  DF_SSE8     = 3,      ///<   8xM SSE
  DF_SSE16    = 4,      ///<  16xM SSE
  DF_SSE32    = 5,      ///<  32xM SSE
  DF_SSE64    = 6,      ///<  64xM SSE
  DF_SSE16N   = 7,      ///< 16NxM SSE
  
  DF_SAD      = 8,      ///< general size SAD
  DF_SAD4     = 9,      ///<   4xM SAD
  DF_SAD8     = 10,     ///<   8xM SAD
  DF_SAD16    = 11,     ///<  16xM SAD
  DF_SAD32    = 12,     ///<  32xM SAD
  DF_SAD64    = 13,     ///<  64xM SAD
  DF_SAD16N   = 14,     ///< 16NxM SAD
  
  DF_SADS     = 15,     ///< general size SAD with step
  DF_SADS4    = 16,     ///<   4xM SAD with step
  DF_SADS8    = 17,     ///<   8xM SAD with step
  DF_SADS16   = 18,     ///<  16xM SAD with step
  DF_SADS32   = 19,     ///<  32xM SAD with step
  DF_SADS64   = 20,     ///<  64xM SAD with step
  DF_SADS16N  = 21,     ///< 16NxM SAD with step
  
  DF_HADS     = 22,     ///< general size Hadamard with step
  DF_HADS4    = 23,     ///<   4xM HAD with step
  DF_HADS8    = 24,     ///<   8xM HAD with step
  DF_HADS16   = 25,     ///<  16xM HAD with step
  DF_HADS32   = 26,     ///<  32xM HAD with step
  DF_HADS64   = 27,     ///<  64xM HAD with step
  DF_HADS16N  = 28,     ///< 16NxM HAD with step
  
  DF_SSE_FRAME = 33     ///< Frame-based SSE
};

/// index for reference type
enum  ERBIndex
{
  ERB_NONE    = 0,      ///< normal case
  ERB_LTR     = 1       ///< long-term reference
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

/// motion vector prediction mode used in AMVP
enum AMVP_MODE
{
  AM_NONE = 0,          ///< no AMVP mode
  AM_EXPL,              ///< explicit signalling of motion vector index
};

#if QC_MDCS
/// coefficient scanning type used in ACS
enum COEFF_SCAN_TYPE
{
  SCAN_ZIGZAG = 0,      ///< typical zigzag scan
  SCAN_HOR,             ///< horizontal first scan
  SCAN_VER              ///< vertical first scan
};
#endif //QC_MDCS

#endif

