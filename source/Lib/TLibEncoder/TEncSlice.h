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

/** \file     TEncSlice.h
    \brief    slice encoder class (header)
*/

#ifndef __TENCSLICE__
#define __TENCSLICE__

// Include files
#include "TLibCommon/CommonDef.h"
#include "TLibCommon/TComList.h"
#include "TLibCommon/TComPic.h"
#include "TLibCommon/TComPicYuv.h"
#include "TEncCu.h"
#include "WeightPredAnalysis.h"
#include "TEncRateCtrl.h"

//! \ingroup TLibEncoder
//! \{

class TEncTop;
class TEncGOP;

// ====================================================================================================================
// Class definition
// ====================================================================================================================

/// slice encoder class
class TEncSlice
  : public WeightPredAnalysis
{
private:
  // encoder configuration
  TEncCfg*                m_pcCfg;                              ///< encoder configuration class

  // pictures
  TComList<TComPic*>*     m_pcListPic;                          ///< list of pictures
  TComPicYuv              m_picYuvPred;                         ///< prediction picture buffer
  TComPicYuv              m_picYuvResi;                         ///< residual picture buffer

  // processing units
  TEncGOP*                m_pcGOPEncoder;                       ///< GOP encoder
  TEncCu*                 m_pcCuEncoder;                        ///< CU encoder

  // encoder search
  TEncSearch*             m_pcPredSearch;                       ///< encoder search class

  // coding tools
  TEncEntropy*            m_pcEntropyCoder;                     ///< entropy encoder
  TEncSbac*               m_pcSbacCoder;                        ///< SBAC encoder
  TEncBinCABAC*           m_pcBinCABAC;                         ///< Bin encoder CABAC
  TComTrQuant*            m_pcTrQuant;                          ///< transform & quantization

  // RD optimization
  TComRdCost*             m_pcRdCost;                           ///< RD cost computation
  TEncSbac***             m_pppcRDSbacCoder;                    ///< storage for SBAC-based RD optimization
  TEncSbac*               m_pcRDGoOnSbacCoder;                  ///< go-on SBAC encoder
  UInt64                  m_uiPicTotalBits;                     ///< total bits for the picture
  UInt64                  m_uiPicDist;                          ///< total distortion for the picture
  Double                  m_dPicRdCost;                         ///< picture-level RD cost
  std::vector<Double>     m_vdRdPicLambda;                      ///< array of lambda candidates
  std::vector<Double>     m_vdRdPicQp;                          ///< array of picture QP candidates (double-type for lambda)
  std::vector<Int>        m_viRdPicQp;                          ///< array of picture QP candidates (Int-type)
  TEncRateCtrl*           m_pcRateCtrl;                         ///< Rate control manager
  UInt                    m_uiSliceIdx;
  TEncSbac                m_lastSliceSegmentEndContextState;    ///< context storage for state at the end of the previous slice-segment (used for dependent slices only).
  TEncSbac                m_entropyCodingSyncContextState;      ///< context storate for state of contexts at the wavefront/WPP/entropy-coding-sync second CTU of tile-row
  SliceType               m_encCABACTableIdx;
  Int                     m_gopID;

  Double   calculateLambda( const TComSlice* pSlice, const Int GOPid, const Int depth, const Double refQP, const Double dQP, Int &iQP );
  Void     setUpLambda(TComSlice* slice, const Double dLambda, Int iQP);
  Void     calculateBoundingCtuTsAddrForSlice(UInt &startCtuTSAddrSlice, UInt &boundingCtuTSAddrSlice, Bool &haveReachedTileBoundary, TComPic* pcPic, const Int sliceMode, const Int sliceArgument);

public:
  TEncSlice();
  virtual ~TEncSlice();

  Void    create              ( Int iWidth, Int iHeight, ChromaFormat chromaFormat, UInt iMaxCUWidth, UInt iMaxCUHeight, UChar uhTotalDepth );
  Void    destroy             ();
  Void    init                ( TEncTop* pcEncTop );
  Void    resetEncoderDecisions() { m_encCABACTableIdx = I_SLICE; }

  /// preparation of slice encoding (reference marking, QP and lambda)
  Void    initEncSlice        ( TComPic*  pcPic, const Int pocLast, const Int pocCurr,
                                const Int iGOPid,   TComSlice*& rpcSlice, const Bool isField );
  Void    resetQP             ( TComPic* pic, Int sliceQP, Double lambda );
  Void    setGopID( Int iGopID )      { m_gopID = iGopID; }
  Int     getGopID() const            { return m_gopID;   }
  Void    updateLambda(TComSlice* pSlice, Double dQP);

  // compress and encode slice
  Void    precompressSlice    ( TComPic* pcPic                                     );      ///< precompress slice for multi-loop slice-level QP opt.
  Void    compressSlice       ( TComPic* pcPic, const Bool bCompressEntireSlice, const Bool bFastDeltaQP );      ///< analysis stage of slice
  Void    calCostSliceI       ( TComPic* pcPic );
  Void    encodeSlice         ( TComPic* pcPic, TComOutputBitstream* pcSubstreams, UInt &numBinsCoded );

  // misc. functions
  Void    setSearchRange      ( TComSlice* pcSlice  );                                  ///< set ME range adaptively

  TEncCu*        getCUEncoder() { return m_pcCuEncoder; }                        ///< CU encoder
  Void    xDetermineStartAndBoundingCtuTsAddr  ( UInt& startCtuTsAddr, UInt& boundingCtuTsAddr, TComPic* pcPic );
  UInt    getSliceIdx()         { return m_uiSliceIdx;                    }
  Void    setSliceIdx(UInt i)   { m_uiSliceIdx = i;                       }

  SliceType getEncCABACTableIdx() const           { return m_encCABACTableIdx;        }

private:
  Double  xGetQPValueAccordingToLambda ( Double lambda );
};

//! \}

#endif // __TENCSLICE__
