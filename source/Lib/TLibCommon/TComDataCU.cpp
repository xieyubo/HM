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

/** \file     TComDataCU.cpp
    \brief    CU data structure
    \todo     not all entities are documented
*/

#include "TComDataCU.h"
#include "TComTU.h"
#include "TComPic.h"

//! \ingroup TLibCommon
//! \{

// ====================================================================================================================
// Constructor / destructor / create / destroy
// ====================================================================================================================

TComDataCU::TComDataCU()
{
  m_pcPic              = NULL;
  m_pcSlice            = NULL;
  m_puhDepth           = NULL;

  m_skipFlag           = NULL;

  m_pePartSize         = NULL;
  m_pePredMode         = NULL;
  m_CUTransquantBypass = NULL;
  m_puhWidth           = NULL;
  m_puhHeight          = NULL;
  m_phQP               = NULL;
  m_ChromaQpAdj        = NULL;
  m_pbMergeFlag        = NULL;
  m_puhMergeIndex      = NULL;
  for(UInt i=0; i<MAX_NUM_CHANNEL_TYPE; i++)
  {
    m_puhIntraDir[i]     = NULL;
  }
  m_puhInterDir        = NULL;
  m_puhTrIdx           = NULL;

  for (UInt comp=0; comp<MAX_NUM_COMPONENT; comp++)
  {
    m_puhCbf[comp]                        = NULL;
    m_crossComponentPredictionAlpha[comp] = NULL;
    m_puhTransformSkip[comp]              = NULL;
    m_pcTrCoeff[comp]                     = NULL;
#if ADAPTIVE_QP_SELECTION
    m_pcArlCoeff[comp]                    = NULL;
#endif
    m_pcIPCMSample[comp]                  = NULL;
    m_explicitRdpcmMode[comp]             = NULL;
  }
#if ADAPTIVE_QP_SELECTION
  m_ArlCoeffIsAliasedAllocation = false;
#endif
  m_pbIPCMFlag         = NULL;

  m_pCtuAboveLeft      = NULL;
  m_pCtuAboveRight     = NULL;
  m_pCtuAbove          = NULL;
  m_pCtuLeft           = NULL;

  for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
  {
    m_apiMVPIdx[i]       = NULL;
    m_apiMVPNum[i]       = NULL;
  }

  m_bDecSubCu          = false;
}

TComDataCU::~TComDataCU()
{
}

Void TComDataCU::create( ChromaFormat chromaFormatIDC, UInt uiNumPartition, UInt uiWidth, UInt uiHeight, Bool bDecSubCu, Int unitSize
#if ADAPTIVE_QP_SELECTION
                        , TCoeff *pParentARLBuffer
#endif
                        )
{
  m_bDecSubCu = bDecSubCu;

  m_pcPic              = NULL;
  m_pcSlice            = NULL;
  m_uiNumPartition     = uiNumPartition;
  m_unitSize = unitSize;

  if ( !bDecSubCu )
  {
    m_phQP               = (SChar*    )xMalloc(SChar,    uiNumPartition);
    m_puhDepth           = (UChar*    )xMalloc(UChar,    uiNumPartition);
    m_puhWidth           = (UChar*    )xMalloc(UChar,    uiNumPartition);
    m_puhHeight          = (UChar*    )xMalloc(UChar,    uiNumPartition);

    m_ChromaQpAdj        = new UChar[ uiNumPartition ];
    m_skipFlag           = new Bool[ uiNumPartition ];
    m_pePartSize         = new SChar[ uiNumPartition ];
    memset( m_pePartSize, NUMBER_OF_PART_SIZES,uiNumPartition * sizeof( *m_pePartSize ) );
    m_pePredMode         = new SChar[ uiNumPartition ];
    m_CUTransquantBypass = new Bool[ uiNumPartition ];

    m_pbMergeFlag        = (Bool*  )xMalloc(Bool,   uiNumPartition);
    m_puhMergeIndex      = (UChar* )xMalloc(UChar,  uiNumPartition);

    for (UInt ch=0; ch<MAX_NUM_CHANNEL_TYPE; ch++)
    {
      m_puhIntraDir[ch] = (UChar* )xMalloc(UChar,  uiNumPartition);
    }
    m_puhInterDir        = (UChar* )xMalloc(UChar,  uiNumPartition);

    m_puhTrIdx           = (UChar* )xMalloc(UChar,  uiNumPartition);

    for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
    {
      const RefPicList rpl=RefPicList(i);
      m_apiMVPIdx[rpl]       = new SChar[ uiNumPartition ];
      m_apiMVPNum[rpl]       = new SChar[ uiNumPartition ];
      memset( m_apiMVPIdx[rpl], -1,uiNumPartition * sizeof( SChar ) );
    }

    for (UInt comp=0; comp<MAX_NUM_COMPONENT; comp++)
    {
      const ComponentID compID = ComponentID(comp);
      const UInt chromaShift = getComponentScaleX(compID, chromaFormatIDC) + getComponentScaleY(compID, chromaFormatIDC);
      const UInt totalSize   = (uiWidth * uiHeight) >> chromaShift;

      m_crossComponentPredictionAlpha[compID] = (SChar* )xMalloc(SChar,  uiNumPartition);
      m_puhTransformSkip[compID]              = (UChar* )xMalloc(UChar,  uiNumPartition);
      m_explicitRdpcmMode[compID]             = (UChar* )xMalloc(UChar,  uiNumPartition);
      m_puhCbf[compID]                        = (UChar* )xMalloc(UChar,  uiNumPartition);
      m_pcTrCoeff[compID]                     = (TCoeff*)xMalloc(TCoeff, totalSize);
      memset( m_pcTrCoeff[compID], 0, (totalSize * sizeof( TCoeff )) );

#if ADAPTIVE_QP_SELECTION
      if( pParentARLBuffer != 0 )
      {
        m_pcArlCoeff[compID] = pParentARLBuffer;
        m_ArlCoeffIsAliasedAllocation = true;
        pParentARLBuffer += totalSize;
      }
      else
      {
        m_pcArlCoeff[compID] = (TCoeff*)xMalloc(TCoeff, totalSize);
        m_ArlCoeffIsAliasedAllocation = false;
      }
#endif
      m_pcIPCMSample[compID] = (Pel*   )xMalloc(Pel , totalSize);
    }

    m_pbIPCMFlag         = (Bool*  )xMalloc(Bool, uiNumPartition);

    for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
    {
      m_acCUMvField[i].create( uiNumPartition );
    }

  }
  else
  {
    for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
    {
      m_acCUMvField[i].setNumPartition(uiNumPartition );
    }
  }

  // create motion vector fields

  m_pCtuAboveLeft      = NULL;
  m_pCtuAboveRight     = NULL;
  m_pCtuAbove          = NULL;
  m_pCtuLeft           = NULL;
}

Void TComDataCU::destroy()
{
  // encoder-side buffer free
  if ( !m_bDecSubCu )
  {
    if ( m_phQP )
    {
      xFree(m_phQP);
      m_phQP = NULL;
    }
    if ( m_puhDepth )
    {
      xFree(m_puhDepth);
      m_puhDepth = NULL;
    }
    if ( m_puhWidth )
    {
      xFree(m_puhWidth);
      m_puhWidth = NULL;
    }
    if ( m_puhHeight )
    {
      xFree(m_puhHeight);
      m_puhHeight = NULL;
    }

    if ( m_skipFlag )
    {
      delete[] m_skipFlag;
      m_skipFlag = NULL;
    }

    if ( m_pePartSize )
    {
      delete[] m_pePartSize;
      m_pePartSize = NULL;
    }
    if ( m_pePredMode )
    {
      delete[] m_pePredMode;
      m_pePredMode = NULL;
    }
    if ( m_ChromaQpAdj )
    {
      delete[] m_ChromaQpAdj;
      m_ChromaQpAdj = NULL;
    }
    if ( m_CUTransquantBypass )
    {
      delete[] m_CUTransquantBypass;
      m_CUTransquantBypass = NULL;
    }
    if ( m_puhInterDir )
    {
      xFree(m_puhInterDir);
      m_puhInterDir = NULL;
    }
    if ( m_pbMergeFlag )
    {
      xFree(m_pbMergeFlag);
      m_pbMergeFlag = NULL;
    }
    if ( m_puhMergeIndex )
    {
      xFree(m_puhMergeIndex);
      m_puhMergeIndex  = NULL;
    }

    for (UInt ch=0; ch<MAX_NUM_CHANNEL_TYPE; ch++)
    {
      xFree(m_puhIntraDir[ch]);
      m_puhIntraDir[ch] = NULL;
    }

    if ( m_puhTrIdx )
    {
      xFree(m_puhTrIdx);
      m_puhTrIdx = NULL;
    }

    for (UInt comp=0; comp<MAX_NUM_COMPONENT; comp++)
    {
      if ( m_crossComponentPredictionAlpha[comp] )
      {
        xFree(m_crossComponentPredictionAlpha[comp]);
        m_crossComponentPredictionAlpha[comp] = NULL;
      }
      if ( m_puhTransformSkip[comp] )
      {
        xFree(m_puhTransformSkip[comp]);
        m_puhTransformSkip[comp] = NULL;
      }
      if ( m_puhCbf[comp] )
      {
        xFree(m_puhCbf[comp]);
        m_puhCbf[comp] = NULL;
      }
      if ( m_pcTrCoeff[comp] )
      {
        xFree(m_pcTrCoeff[comp]);
        m_pcTrCoeff[comp] = NULL;
      }
      if ( m_explicitRdpcmMode[comp] )
      {
        xFree(m_explicitRdpcmMode[comp]);
        m_explicitRdpcmMode[comp] = NULL;
      }

#if ADAPTIVE_QP_SELECTION
      if (!m_ArlCoeffIsAliasedAllocation)
      {
        if ( m_pcArlCoeff[comp] )
        {
          xFree(m_pcArlCoeff[comp]);
          m_pcArlCoeff[comp] = NULL;
        }
      }
#endif

      if ( m_pcIPCMSample[comp] )
      {
        xFree(m_pcIPCMSample[comp]);
        m_pcIPCMSample[comp] = NULL;
      }
    }
    if ( m_pbIPCMFlag )
    {
      xFree(m_pbIPCMFlag );
      m_pbIPCMFlag = NULL;
    }

    for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
    {
      const RefPicList rpl=RefPicList(i);
      if ( m_apiMVPIdx[rpl] )
      {
        delete[] m_apiMVPIdx[rpl];
        m_apiMVPIdx[rpl] = NULL;
      }
      if ( m_apiMVPNum[rpl] )
      {
        delete[] m_apiMVPNum[rpl];
        m_apiMVPNum[rpl] = NULL;
      }
    }

    for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
    {
      const RefPicList rpl=RefPicList(i);
      m_acCUMvField[rpl].destroy();
    }
  }

  m_pcPic              = NULL;
  m_pcSlice            = NULL;

  m_pCtuAboveLeft      = NULL;
  m_pCtuAboveRight     = NULL;
  m_pCtuAbove          = NULL;
  m_pCtuLeft           = NULL;

}

Bool TComDataCU::CUIsFromSameTile            ( const TComDataCU *pCU /* Can be NULL */) const
{
  return pCU!=NULL &&
         pCU->getSlice() != NULL &&
         m_pcPic->getPicSym()->getTileIdxMap( pCU->getCtuRsAddr() ) == m_pcPic->getPicSym()->getTileIdxMap(getCtuRsAddr());
}

Bool TComDataCU::CUIsFromSameSliceAndTile    ( const TComDataCU *pCU /* Can be NULL */) const
{
  return pCU!=NULL &&
         pCU->getSlice() != NULL &&
         pCU->getSlice()->getSliceCurStartCtuTsAddr() == getSlice()->getSliceCurStartCtuTsAddr() &&
         m_pcPic->getPicSym()->getTileIdxMap( pCU->getCtuRsAddr() ) == m_pcPic->getPicSym()->getTileIdxMap(getCtuRsAddr())
         ;
}

Bool TComDataCU::CUIsFromSameSliceTileAndWavefrontRow( const TComDataCU *pCU /* Can be NULL */) const
{
  return CUIsFromSameSliceAndTile(pCU)
         && (!getSlice()->getPPS()->getEntropyCodingSyncEnabledFlag() || getPic()->getCtu(getCtuRsAddr())->getCUPelY() == getPic()->getCtu(pCU->getCtuRsAddr())->getCUPelY());
}

Bool TComDataCU::isLastSubCUOfCtu(const UInt absPartIdx) const
{
  const TComSPS &sps=*(getSlice()->getSPS());

  const UInt picWidth = sps.getPicWidthInLumaSamples();
  const UInt picHeight = sps.getPicHeightInLumaSamples();
  const UInt granularityWidth = sps.getMaxCUWidth();

  const UInt cuPosX = getCUPelX() + g_auiRasterToPelX[ g_auiZscanToRaster[absPartIdx] ];
  const UInt cuPosY = getCUPelY() + g_auiRasterToPelY[ g_auiZscanToRaster[absPartIdx] ];

  return (((cuPosX+getWidth( absPartIdx))%granularityWidth==0||(cuPosX+getWidth( absPartIdx)==picWidth ))
       && ((cuPosY+getHeight(absPartIdx))%granularityWidth==0||(cuPosY+getHeight(absPartIdx)==picHeight)));
}

// ====================================================================================================================
// Public member functions
// ====================================================================================================================

// --------------------------------------------------------------------------------------------------------------------
// Initialization
// --------------------------------------------------------------------------------------------------------------------

/**
 Initialize top-level CU: create internal buffers and set initial values before encoding the CTU.
 
 \param  pcPic       picture (TComPic) class pointer
 \param  ctuRsAddr   CTU address in raster scan order
 */
Void TComDataCU::initCtu( TComPic* pcPic, UInt ctuRsAddr )
{

  const UInt maxCUWidth = pcPic->getPicSym()->getSPS().getMaxCUWidth();
  const UInt maxCUHeight= pcPic->getPicSym()->getSPS().getMaxCUHeight();
  m_pcPic              = pcPic;
  m_pcSlice            = pcPic->getSlice(pcPic->getCurrSliceIdx());
  m_ctuRsAddr          = ctuRsAddr;
  m_uiCUPelX           = ( ctuRsAddr % pcPic->getFrameWidthInCtus() ) * maxCUWidth;
  m_uiCUPelY           = ( ctuRsAddr / pcPic->getFrameWidthInCtus() ) * maxCUHeight;
  m_absZIdxInCtu       = 0;
  m_dTotalCost         = MAX_DOUBLE;
  m_uiTotalDistortion  = 0;
  m_uiTotalBits        = 0;
  m_uiTotalBins        = 0;
  m_uiNumPartition     = pcPic->getNumPartitionsInCtu();

  memset( m_skipFlag          , false,                      m_uiNumPartition * sizeof( *m_skipFlag ) );

  memset( m_pePartSize        , NUMBER_OF_PART_SIZES,       m_uiNumPartition * sizeof( *m_pePartSize ) );
  memset( m_pePredMode        , NUMBER_OF_PREDICTION_MODES, m_uiNumPartition * sizeof( *m_pePredMode ) );
  memset( m_CUTransquantBypass, false,                      m_uiNumPartition * sizeof( *m_CUTransquantBypass) );
  memset( m_puhDepth          , 0,                          m_uiNumPartition * sizeof( *m_puhDepth ) );
  memset( m_puhTrIdx          , 0,                          m_uiNumPartition * sizeof( *m_puhTrIdx ) );
  memset( m_puhWidth          , maxCUWidth,                 m_uiNumPartition * sizeof( *m_puhWidth ) );
  memset( m_puhHeight         , maxCUHeight,                m_uiNumPartition * sizeof( *m_puhHeight ) );
  for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
  {
    const RefPicList rpl=RefPicList(i);
    memset( m_apiMVPIdx[rpl]  , -1,                         m_uiNumPartition * sizeof( *m_apiMVPIdx[rpl] ) );
    memset( m_apiMVPNum[rpl]  , -1,                         m_uiNumPartition * sizeof( *m_apiMVPNum[rpl] ) );
  }
  memset( m_phQP              , getSlice()->getSliceQp(),   m_uiNumPartition * sizeof( *m_phQP ) );
  memset( m_ChromaQpAdj       , 0,                          m_uiNumPartition * sizeof( *m_ChromaQpAdj ) );
  for(UInt comp=0; comp<MAX_NUM_COMPONENT; comp++)
  {
    memset( m_crossComponentPredictionAlpha[comp] , 0,                     m_uiNumPartition * sizeof( *m_crossComponentPredictionAlpha[comp] ) );
    memset( m_puhTransformSkip[comp]              , 0,                     m_uiNumPartition * sizeof( *m_puhTransformSkip[comp]) );
    memset( m_puhCbf[comp]                        , 0,                     m_uiNumPartition * sizeof( *m_puhCbf[comp] ) );
    memset( m_explicitRdpcmMode[comp]             , NUMBER_OF_RDPCM_MODES, m_uiNumPartition * sizeof( *m_explicitRdpcmMode[comp] ) );
  }
  memset( m_pbMergeFlag       , false,                    m_uiNumPartition * sizeof( *m_pbMergeFlag ) );
  memset( m_puhMergeIndex     , 0,                        m_uiNumPartition * sizeof( *m_puhMergeIndex ) );
  for (UInt ch=0; ch<MAX_NUM_CHANNEL_TYPE; ch++)
  {
    memset( m_puhIntraDir[ch] , ((ch==0) ? DC_IDX : 0),   m_uiNumPartition * sizeof( *(m_puhIntraDir[ch]) ) );
  }
  memset( m_puhInterDir       , 0,                        m_uiNumPartition * sizeof( *m_puhInterDir ) );
  memset( m_pbIPCMFlag        , false,                    m_uiNumPartition * sizeof( *m_pbIPCMFlag ) );

  const UInt numCoeffY    = maxCUWidth*maxCUHeight;
  for (UInt comp=0; comp<MAX_NUM_COMPONENT; comp++)
  {
    const UInt componentShift = m_pcPic->getComponentScaleX(ComponentID(comp)) + m_pcPic->getComponentScaleY(ComponentID(comp));
    memset( m_pcTrCoeff[comp], 0, sizeof(TCoeff)* numCoeffY>>componentShift );
#if ADAPTIVE_QP_SELECTION
    memset( m_pcArlCoeff[comp], 0, sizeof(TCoeff)* numCoeffY>>componentShift );
#endif
  }

  for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
  {
    m_acCUMvField[i].clearMvField();
  }

  // Setting neighbor CU
  m_pCtuLeft        = NULL;
  m_pCtuAbove       = NULL;
  m_pCtuAboveLeft   = NULL;
  m_pCtuAboveRight  = NULL;

  UInt frameWidthInCtus = pcPic->getFrameWidthInCtus();
  if ( m_ctuRsAddr % frameWidthInCtus )
  {
    m_pCtuLeft = pcPic->getCtu( m_ctuRsAddr - 1 );
  }

  if ( m_ctuRsAddr / frameWidthInCtus )
  {
    m_pCtuAbove = pcPic->getCtu( m_ctuRsAddr - frameWidthInCtus );
  }

  if ( m_pCtuLeft && m_pCtuAbove )
  {
    m_pCtuAboveLeft = pcPic->getCtu( m_ctuRsAddr - frameWidthInCtus - 1 );
  }

  if ( m_pCtuAbove && ( (m_ctuRsAddr%frameWidthInCtus) < (frameWidthInCtus-1) )  )
  {
    m_pCtuAboveRight = pcPic->getCtu( m_ctuRsAddr - frameWidthInCtus + 1 );
  }
}


/** Initialize prediction data with enabling sub-CTU-level delta QP.
*   - set CU width and CU height according to depth
*   - set qp value according to input qp
*   - set last-coded qp value according to input last-coded qp
*
* \param  uiDepth            depth of the current CU
* \param  qp                 qp for the current CU
* \param  bTransquantBypass  true for transquant bypass
*/
Void TComDataCU::initEstData( const UInt uiDepth, const Int qp, const Bool bTransquantBypass )
{
  m_dTotalCost         = MAX_DOUBLE;
  m_uiTotalDistortion  = 0;
  m_uiTotalBits        = 0;
  m_uiTotalBins        = 0;

  const UChar uhWidth  = getSlice()->getSPS()->getMaxCUWidth()  >> uiDepth;
  const UChar uhHeight = getSlice()->getSPS()->getMaxCUHeight() >> uiDepth;

  for (UInt ui = 0; ui < m_uiNumPartition; ui++)
  {
    for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
    {
      const RefPicList rpl=RefPicList(i);
      m_apiMVPIdx[rpl][ui]  = -1;
      m_apiMVPNum[rpl][ui]  = -1;
    }
    m_puhDepth  [ui]    = uiDepth;
    m_puhWidth  [ui]    = uhWidth;
    m_puhHeight [ui]    = uhHeight;
    m_puhTrIdx  [ui]    = 0;
    for(UInt comp=0; comp<MAX_NUM_COMPONENT; comp++)
    {
      m_crossComponentPredictionAlpha[comp][ui] = 0;
      m_puhTransformSkip             [comp][ui] = 0;
      m_explicitRdpcmMode            [comp][ui] = NUMBER_OF_RDPCM_MODES;
    }
    m_skipFlag[ui]      = false;
    m_pePartSize[ui]    = NUMBER_OF_PART_SIZES;
    m_pePredMode[ui]    = NUMBER_OF_PREDICTION_MODES;
    m_CUTransquantBypass[ui] = bTransquantBypass;
    m_pbIPCMFlag[ui]    = 0;
    m_phQP[ui]          = qp;
    m_ChromaQpAdj[ui]   = 0;
    m_pbMergeFlag[ui]   = 0;
    m_puhMergeIndex[ui] = 0;

    for (UInt ch=0; ch<MAX_NUM_CHANNEL_TYPE; ch++)
    {
      m_puhIntraDir[ch][ui] = ((ch==0) ? DC_IDX : 0);
    }

    m_puhInterDir[ui] = 0;
    for (UInt comp=0; comp<MAX_NUM_COMPONENT; comp++)
    {
      m_puhCbf[comp][ui] = 0;
    }
  }

  for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
  {
    m_acCUMvField[i].clearMvField();
  }

  const UInt numCoeffY = uhWidth*uhHeight;

  for (UInt comp=0; comp<MAX_NUM_COMPONENT; comp++)
  {
    const ComponentID component = ComponentID(comp);
    const UInt numCoeff = numCoeffY >> (getPic()->getComponentScaleX(component) + getPic()->getComponentScaleY(component));
    memset( m_pcTrCoeff[comp],    0, numCoeff * sizeof( TCoeff ) );
#if ADAPTIVE_QP_SELECTION
    memset( m_pcArlCoeff[comp],   0, numCoeff * sizeof( TCoeff ) );
#endif
    memset( m_pcIPCMSample[comp], 0, numCoeff * sizeof( Pel) );
  }
}


// initialize Sub partition
Void TComDataCU::initSubCU( TComDataCU* pcCU, UInt uiPartUnitIdx, UInt uiDepth, Int qp )
{
  assert( uiPartUnitIdx<4 );

  UInt uiPartOffset = ( pcCU->getTotalNumPart()>>2 )*uiPartUnitIdx;

  m_pcPic              = pcCU->getPic();
  m_pcSlice            = pcCU->getSlice();
  m_ctuRsAddr          = pcCU->getCtuRsAddr();
  m_absZIdxInCtu       = pcCU->getZorderIdxInCtu() + uiPartOffset;

  const UChar uhWidth  = getSlice()->getSPS()->getMaxCUWidth()  >> uiDepth;
  const UChar uhHeight = getSlice()->getSPS()->getMaxCUHeight() >> uiDepth;

  m_uiCUPelX           = pcCU->getCUPelX() + ( uhWidth )*( uiPartUnitIdx &  1 );
  m_uiCUPelY           = pcCU->getCUPelY() + ( uhHeight)*( uiPartUnitIdx >> 1 );

  m_dTotalCost         = MAX_DOUBLE;
  m_uiTotalDistortion  = 0;
  m_uiTotalBits        = 0;
  m_uiTotalBins        = 0;
  m_uiNumPartition     = pcCU->getTotalNumPart() >> 2;

  Int iSizeInUchar = sizeof( UChar  ) * m_uiNumPartition;
  Int iSizeInBool  = sizeof( Bool   ) * m_uiNumPartition;
  Int sizeInChar = sizeof( SChar  ) * m_uiNumPartition;

  memset( m_phQP,              qp,  sizeInChar );
  memset( m_pbMergeFlag,        0, iSizeInBool  );
  memset( m_puhMergeIndex,      0, iSizeInUchar );
  for (UInt ch=0; ch<MAX_NUM_CHANNEL_TYPE; ch++)
  {
    memset( m_puhIntraDir[ch],  ((ch==0) ? DC_IDX : 0), iSizeInUchar );
  }

  memset( m_puhInterDir,        0, iSizeInUchar );
  memset( m_puhTrIdx,           0, iSizeInUchar );

  for(UInt comp=0; comp<MAX_NUM_COMPONENT; comp++)
  {
    memset( m_crossComponentPredictionAlpha[comp], 0, iSizeInUchar );
    memset( m_puhTransformSkip[comp],              0, iSizeInUchar );
    memset( m_puhCbf[comp],                        0, iSizeInUchar );
    memset( m_explicitRdpcmMode[comp],             NUMBER_OF_RDPCM_MODES, iSizeInUchar );
  }

  memset( m_puhDepth,     uiDepth, iSizeInUchar );
  memset( m_puhWidth,          uhWidth,  iSizeInUchar );
  memset( m_puhHeight,         uhHeight, iSizeInUchar );
  memset( m_pbIPCMFlag,        0, iSizeInBool  );
  for (UInt ui = 0; ui < m_uiNumPartition; ui++)
  {
    m_skipFlag[ui]   = false;
    m_pePartSize[ui] = NUMBER_OF_PART_SIZES;
    m_pePredMode[ui] = NUMBER_OF_PREDICTION_MODES;
    m_CUTransquantBypass[ui] = false;
    m_ChromaQpAdj[ui] = 0;

    for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
    {
      const RefPicList rpl=RefPicList(i);
      m_apiMVPIdx[rpl][ui] = -1;
      m_apiMVPNum[rpl][ui] = -1;
    }
  }

  const UInt numCoeffY    = uhWidth*uhHeight;
  for (UInt ch=0; ch<MAX_NUM_COMPONENT; ch++)
  {
    const UInt componentShift = m_pcPic->getComponentScaleX(ComponentID(ch)) + m_pcPic->getComponentScaleY(ComponentID(ch));
    memset( m_pcTrCoeff[ch],  0, sizeof(TCoeff)*(numCoeffY>>componentShift) );
#if ADAPTIVE_QP_SELECTION
    memset( m_pcArlCoeff[ch], 0, sizeof(TCoeff)*(numCoeffY>>componentShift) );
#endif
    memset( m_pcIPCMSample[ch], 0, sizeof(Pel)* (numCoeffY>>componentShift) );
  }

  for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
  {
    m_acCUMvField[i].clearMvField();
  }

  m_pCtuLeft        = pcCU->getCtuLeft();
  m_pCtuAbove       = pcCU->getCtuAbove();
  m_pCtuAboveLeft   = pcCU->getCtuAboveLeft();
  m_pCtuAboveRight  = pcCU->getCtuAboveRight();
}

Void TComDataCU::setOutsideCUPart( UInt uiAbsPartIdx, UInt uiDepth )
{
  const UInt     uiNumPartition = m_uiNumPartition >> (uiDepth << 1);
  const UInt     uiSizeInUchar  = sizeof( UChar  ) * uiNumPartition;
  const TComSPS &sps            = *(getSlice()->getSPS());
  const UChar    uhWidth        = sps.getMaxCUWidth()  >> uiDepth;
  const UChar    uhHeight       = sps.getMaxCUHeight() >> uiDepth;
  memset( m_puhDepth    + uiAbsPartIdx,     uiDepth,  uiSizeInUchar );
  memset( m_puhWidth    + uiAbsPartIdx,     uhWidth,  uiSizeInUchar );
  memset( m_puhHeight   + uiAbsPartIdx,     uhHeight, uiSizeInUchar );
}

// --------------------------------------------------------------------------------------------------------------------
// Copy
// --------------------------------------------------------------------------------------------------------------------

Void TComDataCU::copySubCU( TComDataCU* pcCU, UInt uiAbsPartIdx )
{
  UInt uiPart = uiAbsPartIdx;

  m_pcPic              = pcCU->getPic();
  m_pcSlice            = pcCU->getSlice();
  m_ctuRsAddr          = pcCU->getCtuRsAddr();
  m_absZIdxInCtu       = uiAbsPartIdx;

  m_uiCUPelX           = pcCU->getCUPelX() + g_auiRasterToPelX[ g_auiZscanToRaster[uiAbsPartIdx] ];
  m_uiCUPelY           = pcCU->getCUPelY() + g_auiRasterToPelY[ g_auiZscanToRaster[uiAbsPartIdx] ];

  m_skipFlag=pcCU->getSkipFlag()          + uiPart;

  m_phQP=pcCU->getQP()                    + uiPart;
  m_ChromaQpAdj = pcCU->getChromaQpAdj()  + uiPart;
  m_pePartSize = pcCU->getPartitionSize() + uiPart;
  m_pePredMode=pcCU->getPredictionMode()  + uiPart;
  m_CUTransquantBypass  = pcCU->getCUTransquantBypass()+uiPart;

  m_pbMergeFlag         = pcCU->getMergeFlag()        + uiPart;
  m_puhMergeIndex       = pcCU->getMergeIndex()       + uiPart;

  for (UInt ch=0; ch<MAX_NUM_CHANNEL_TYPE; ch++)
  {
    m_puhIntraDir[ch]   = pcCU->getIntraDir(ChannelType(ch)) + uiPart;
  }

  m_puhInterDir         = pcCU->getInterDir()         + uiPart;
  m_puhTrIdx            = pcCU->getTransformIdx()     + uiPart;

  for(UInt comp=0; comp<MAX_NUM_COMPONENT; comp++)
  {
    m_crossComponentPredictionAlpha[comp] = pcCU->getCrossComponentPredictionAlpha(ComponentID(comp)) + uiPart;
    m_puhTransformSkip[comp]              = pcCU->getTransformSkip(ComponentID(comp))                 + uiPart;
    m_puhCbf[comp]                        = pcCU->getCbf(ComponentID(comp))                           + uiPart;
    m_explicitRdpcmMode[comp]             = pcCU->getExplicitRdpcmMode(ComponentID(comp))             + uiPart;
  }

  m_puhDepth=pcCU->getDepth()                     + uiPart;
  m_puhWidth=pcCU->getWidth()                     + uiPart;
  m_puhHeight=pcCU->getHeight()                   + uiPart;

  m_pbIPCMFlag         = pcCU->getIPCMFlag()        + uiPart;

  m_pCtuAboveLeft      = pcCU->getCtuAboveLeft();
  m_pCtuAboveRight     = pcCU->getCtuAboveRight();
  m_pCtuAbove          = pcCU->getCtuAbove();
  m_pCtuLeft           = pcCU->getCtuLeft();

  for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
  {
    const RefPicList rpl=RefPicList(i);
    m_apiMVPIdx[rpl]=pcCU->getMVPIdx(rpl)  + uiPart;
    m_apiMVPNum[rpl]=pcCU->getMVPNum(rpl)  + uiPart;
  }

  for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
  {
    const RefPicList rpl=RefPicList(i);
    m_acCUMvField[rpl].linkToWithOffset( pcCU->getCUMvField(rpl), uiPart );
  }

  UInt uiMaxCuWidth=pcCU->getSlice()->getSPS()->getMaxCUWidth();
  UInt uiMaxCuHeight=pcCU->getSlice()->getSPS()->getMaxCUHeight();

  UInt uiCoffOffset = uiMaxCuWidth*uiMaxCuHeight*uiAbsPartIdx/pcCU->getPic()->getNumPartitionsInCtu();

  for (UInt ch=0; ch<MAX_NUM_COMPONENT; ch++)
  {
    const ComponentID component = ComponentID(ch);
    const UInt componentShift   = m_pcPic->getComponentScaleX(component) + m_pcPic->getComponentScaleY(component);
    const UInt offset           = uiCoffOffset >> componentShift;
    m_pcTrCoeff[ch] = pcCU->getCoeff(component) + offset;
#if ADAPTIVE_QP_SELECTION
    m_pcArlCoeff[ch] = pcCU->getArlCoeff(component) + offset;
#endif
    m_pcIPCMSample[ch] = pcCU->getPCMSample(component) + offset;
  }
}

// Copy inter prediction info from the biggest CU
Void TComDataCU::copyInterPredInfoFrom    ( TComDataCU* pcCU, UInt uiAbsPartIdx, RefPicList eRefPicList )
{
  m_pcPic              = pcCU->getPic();
  m_pcSlice            = pcCU->getSlice();
  m_ctuRsAddr          = pcCU->getCtuRsAddr();
  m_absZIdxInCtu       = uiAbsPartIdx;

  Int iRastPartIdx     = g_auiZscanToRaster[uiAbsPartIdx];
  m_uiCUPelX           = pcCU->getCUPelX() + m_pcPic->getMinCUWidth ()*( iRastPartIdx % m_pcPic->getNumPartInCtuWidth() );
  m_uiCUPelY           = pcCU->getCUPelY() + m_pcPic->getMinCUHeight()*( iRastPartIdx / m_pcPic->getNumPartInCtuWidth() );

  m_pCtuAboveLeft      = pcCU->getCtuAboveLeft();
  m_pCtuAboveRight     = pcCU->getCtuAboveRight();
  m_pCtuAbove          = pcCU->getCtuAbove();
  m_pCtuLeft           = pcCU->getCtuLeft();

  m_skipFlag           = pcCU->getSkipFlag ()             + uiAbsPartIdx;

  m_pePartSize         = pcCU->getPartitionSize ()        + uiAbsPartIdx;
  m_pePredMode         = pcCU->getPredictionMode()        + uiAbsPartIdx;
  m_ChromaQpAdj        = pcCU->getChromaQpAdj()           + uiAbsPartIdx;
  m_CUTransquantBypass = pcCU->getCUTransquantBypass()    + uiAbsPartIdx;
  m_puhInterDir        = pcCU->getInterDir      ()        + uiAbsPartIdx;

  m_puhDepth           = pcCU->getDepth ()                + uiAbsPartIdx;
  m_puhWidth           = pcCU->getWidth ()                + uiAbsPartIdx;
  m_puhHeight          = pcCU->getHeight()                + uiAbsPartIdx;

  m_pbMergeFlag        = pcCU->getMergeFlag()             + uiAbsPartIdx;
  m_puhMergeIndex      = pcCU->getMergeIndex()            + uiAbsPartIdx;

  m_apiMVPIdx[eRefPicList] = pcCU->getMVPIdx(eRefPicList) + uiAbsPartIdx;
  m_apiMVPNum[eRefPicList] = pcCU->getMVPNum(eRefPicList) + uiAbsPartIdx;

  m_acCUMvField[ eRefPicList ].linkToWithOffset( pcCU->getCUMvField(eRefPicList), uiAbsPartIdx );
}

// Copy small CU to bigger CU.
// One of quarter parts overwritten by predicted sub part.
Void TComDataCU::copyPartFrom( TComDataCU* pcCU, UInt uiPartUnitIdx, UInt uiDepth )
{
  assert( uiPartUnitIdx<4 );

  m_dTotalCost         += pcCU->getTotalCost();
  m_uiTotalDistortion  += pcCU->getTotalDistortion();
  m_uiTotalBits        += pcCU->getTotalBits();

  UInt uiOffset         = pcCU->getTotalNumPart()*uiPartUnitIdx;
  const UInt numValidComp=pcCU->getPic()->getNumberValidComponents();
  const UInt numValidChan=pcCU->getPic()->getChromaFormat()==CHROMA_400 ? 1:2;

  UInt uiNumPartition = pcCU->getTotalNumPart();
  Int iSizeInUchar  = sizeof( UChar ) * uiNumPartition;
  Int iSizeInBool   = sizeof( Bool  ) * uiNumPartition;

  Int sizeInChar  = sizeof( SChar ) * uiNumPartition;
  memcpy( m_skipFlag   + uiOffset, pcCU->getSkipFlag(),       sizeof( *m_skipFlag )   * uiNumPartition );
  memcpy( m_phQP       + uiOffset, pcCU->getQP(),             sizeInChar                        );
  memcpy( m_pePartSize + uiOffset, pcCU->getPartitionSize(),  sizeof( *m_pePartSize ) * uiNumPartition );
  memcpy( m_pePredMode + uiOffset, pcCU->getPredictionMode(), sizeof( *m_pePredMode ) * uiNumPartition );
  memcpy( m_ChromaQpAdj + uiOffset, pcCU->getChromaQpAdj(),   sizeof( *m_ChromaQpAdj ) * uiNumPartition );
  memcpy( m_CUTransquantBypass + uiOffset, pcCU->getCUTransquantBypass(), sizeof( *m_CUTransquantBypass ) * uiNumPartition );
  memcpy( m_pbMergeFlag         + uiOffset, pcCU->getMergeFlag(),         iSizeInBool  );
  memcpy( m_puhMergeIndex       + uiOffset, pcCU->getMergeIndex(),        iSizeInUchar );

  for (UInt ch=0; ch<numValidChan; ch++)
  {
    memcpy( m_puhIntraDir[ch]   + uiOffset, pcCU->getIntraDir(ChannelType(ch)), iSizeInUchar );
  }

  memcpy( m_puhInterDir         + uiOffset, pcCU->getInterDir(),          iSizeInUchar );
  memcpy( m_puhTrIdx            + uiOffset, pcCU->getTransformIdx(),      iSizeInUchar );

  for(UInt comp=0; comp<numValidComp; comp++)
  {
    memcpy( m_crossComponentPredictionAlpha[comp] + uiOffset, pcCU->getCrossComponentPredictionAlpha(ComponentID(comp)), iSizeInUchar );
    memcpy( m_puhTransformSkip[comp]              + uiOffset, pcCU->getTransformSkip(ComponentID(comp))                , iSizeInUchar );
    memcpy( m_puhCbf[comp]                        + uiOffset, pcCU->getCbf(ComponentID(comp))                          , iSizeInUchar );
    memcpy( m_explicitRdpcmMode[comp]             + uiOffset, pcCU->getExplicitRdpcmMode(ComponentID(comp))            , iSizeInUchar );
  }

  memcpy( m_puhDepth  + uiOffset, pcCU->getDepth(),  iSizeInUchar );
  memcpy( m_puhWidth  + uiOffset, pcCU->getWidth(),  iSizeInUchar );
  memcpy( m_puhHeight + uiOffset, pcCU->getHeight(), iSizeInUchar );

  memcpy( m_pbIPCMFlag + uiOffset, pcCU->getIPCMFlag(), iSizeInBool );

  m_pCtuAboveLeft      = pcCU->getCtuAboveLeft();
  m_pCtuAboveRight     = pcCU->getCtuAboveRight();
  m_pCtuAbove          = pcCU->getCtuAbove();
  m_pCtuLeft           = pcCU->getCtuLeft();

  for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
  {
    const RefPicList rpl=RefPicList(i);
    memcpy( m_apiMVPIdx[rpl] + uiOffset, pcCU->getMVPIdx(rpl), iSizeInUchar );
    memcpy( m_apiMVPNum[rpl] + uiOffset, pcCU->getMVPNum(rpl), iSizeInUchar );
  }

  for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
  {
    const RefPicList rpl=RefPicList(i);
    m_acCUMvField[rpl].copyFrom( pcCU->getCUMvField( rpl ), pcCU->getTotalNumPart(), uiOffset );
  }

  const UInt numCoeffY = (pcCU->getSlice()->getSPS()->getMaxCUWidth()*pcCU->getSlice()->getSPS()->getMaxCUHeight()) >> (uiDepth<<1);
  const UInt offsetY   = uiPartUnitIdx*numCoeffY;
  for (UInt ch=0; ch<numValidComp; ch++)
  {
    const ComponentID component = ComponentID(ch);
    const UInt componentShift   = m_pcPic->getComponentScaleX(component) + m_pcPic->getComponentScaleY(component);
    const UInt offset           = offsetY>>componentShift;
    memcpy( m_pcTrCoeff [ch] + offset, pcCU->getCoeff(component),    sizeof(TCoeff)*(numCoeffY>>componentShift) );
#if ADAPTIVE_QP_SELECTION
    memcpy( m_pcArlCoeff[ch] + offset, pcCU->getArlCoeff(component), sizeof(TCoeff)*(numCoeffY>>componentShift) );
#endif
    memcpy( m_pcIPCMSample[ch] + offset, pcCU->getPCMSample(component), sizeof(Pel)*(numCoeffY>>componentShift) );
  }

  m_uiTotalBins += pcCU->getTotalBins();
}

// Copy current predicted part to a CU in picture.
// It is used to predict for next part
Void TComDataCU::copyToPic( UChar uhDepth )
{
  TComDataCU* pCtu = m_pcPic->getCtu( m_ctuRsAddr );
  const UInt numValidComp=pCtu->getPic()->getNumberValidComponents();
  const UInt numValidChan=pCtu->getPic()->getChromaFormat()==CHROMA_400 ? 1:2;

  pCtu->getTotalCost()       = m_dTotalCost;
  pCtu->getTotalDistortion() = m_uiTotalDistortion;
  pCtu->getTotalBits()       = m_uiTotalBits;

  Int iSizeInUchar  = sizeof( UChar ) * m_uiNumPartition;
  Int iSizeInBool   = sizeof( Bool  ) * m_uiNumPartition;
  Int sizeInChar  = sizeof( SChar ) * m_uiNumPartition;

  memcpy( pCtu->getSkipFlag() + m_absZIdxInCtu, m_skipFlag, sizeof( *m_skipFlag ) * m_uiNumPartition );

  memcpy( pCtu->getQP() + m_absZIdxInCtu, m_phQP, sizeInChar  );

  memcpy( pCtu->getPartitionSize()  + m_absZIdxInCtu, m_pePartSize, sizeof( *m_pePartSize ) * m_uiNumPartition );
  memcpy( pCtu->getPredictionMode() + m_absZIdxInCtu, m_pePredMode, sizeof( *m_pePredMode ) * m_uiNumPartition );
  memcpy( pCtu->getChromaQpAdj() + m_absZIdxInCtu, m_ChromaQpAdj, sizeof( *m_ChromaQpAdj ) * m_uiNumPartition );
  memcpy( pCtu->getCUTransquantBypass()+ m_absZIdxInCtu, m_CUTransquantBypass, sizeof( *m_CUTransquantBypass ) * m_uiNumPartition );
  memcpy( pCtu->getMergeFlag()         + m_absZIdxInCtu, m_pbMergeFlag,         iSizeInBool  );
  memcpy( pCtu->getMergeIndex()        + m_absZIdxInCtu, m_puhMergeIndex,       iSizeInUchar );
  for (UInt ch=0; ch<numValidChan; ch++)
  {
    memcpy( pCtu->getIntraDir(ChannelType(ch)) + m_absZIdxInCtu, m_puhIntraDir[ch], iSizeInUchar);
  }

  memcpy( pCtu->getInterDir()          + m_absZIdxInCtu, m_puhInterDir,         iSizeInUchar );
  memcpy( pCtu->getTransformIdx()      + m_absZIdxInCtu, m_puhTrIdx,            iSizeInUchar );

  for(UInt comp=0; comp<numValidComp; comp++)
  {
    memcpy( pCtu->getCrossComponentPredictionAlpha(ComponentID(comp)) + m_absZIdxInCtu, m_crossComponentPredictionAlpha[comp], iSizeInUchar );
    memcpy( pCtu->getTransformSkip(ComponentID(comp))                 + m_absZIdxInCtu, m_puhTransformSkip[comp],              iSizeInUchar );
    memcpy( pCtu->getCbf(ComponentID(comp))                           + m_absZIdxInCtu, m_puhCbf[comp],                        iSizeInUchar );
    memcpy( pCtu->getExplicitRdpcmMode(ComponentID(comp))             + m_absZIdxInCtu, m_explicitRdpcmMode[comp],             iSizeInUchar );
  }

  memcpy( pCtu->getDepth()  + m_absZIdxInCtu, m_puhDepth,  iSizeInUchar );
  memcpy( pCtu->getWidth()  + m_absZIdxInCtu, m_puhWidth,  iSizeInUchar );
  memcpy( pCtu->getHeight() + m_absZIdxInCtu, m_puhHeight, iSizeInUchar );

  for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
  {
    const RefPicList rpl=RefPicList(i);
    memcpy( pCtu->getMVPIdx(rpl) + m_absZIdxInCtu, m_apiMVPIdx[rpl], iSizeInUchar );
    memcpy( pCtu->getMVPNum(rpl) + m_absZIdxInCtu, m_apiMVPNum[rpl], iSizeInUchar );
  }

  for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
  {
    const RefPicList rpl=RefPicList(i);
    m_acCUMvField[rpl].copyTo( pCtu->getCUMvField( rpl ), m_absZIdxInCtu );
  }

  memcpy( pCtu->getIPCMFlag() + m_absZIdxInCtu, m_pbIPCMFlag,         iSizeInBool  );

  const UInt numCoeffY    = (pCtu->getSlice()->getSPS()->getMaxCUWidth()*pCtu->getSlice()->getSPS()->getMaxCUHeight())>>(uhDepth<<1);
  const UInt offsetY      = m_absZIdxInCtu*m_pcPic->getMinCUWidth()*m_pcPic->getMinCUHeight();
  for (UInt comp=0; comp<numValidComp; comp++)
  {
    const ComponentID component = ComponentID(comp);
    const UInt componentShift   = m_pcPic->getComponentScaleX(component) + m_pcPic->getComponentScaleY(component);
    memcpy( pCtu->getCoeff(component)   + (offsetY>>componentShift), m_pcTrCoeff[component], sizeof(TCoeff)*(numCoeffY>>componentShift) );
#if ADAPTIVE_QP_SELECTION
    memcpy( pCtu->getArlCoeff(component) + (offsetY>>componentShift), m_pcArlCoeff[component], sizeof(TCoeff)*(numCoeffY>>componentShift) );
#endif
    memcpy( pCtu->getPCMSample(component) + (offsetY>>componentShift), m_pcIPCMSample[component], sizeof(Pel)*(numCoeffY>>componentShift) );
  }

  pCtu->getTotalBins() = m_uiTotalBins;
}

// --------------------------------------------------------------------------------------------------------------------
// Other public functions
// --------------------------------------------------------------------------------------------------------------------

const TComDataCU* TComDataCU::getPULeft( UInt& uiLPartUnitIdx,
                                         UInt uiCurrPartUnitIdx,
                                         Bool bEnforceSliceRestriction,
                                         Bool bEnforceTileRestriction ) const
{
  UInt uiAbsPartIdx       = g_auiZscanToRaster[uiCurrPartUnitIdx];
  UInt uiAbsZorderCUIdx   = g_auiZscanToRaster[m_absZIdxInCtu];
  const UInt numPartInCtuWidth = m_pcPic->getNumPartInCtuWidth();

  if ( !RasterAddress::isZeroCol( uiAbsPartIdx, numPartInCtuWidth ) )
  {
    uiLPartUnitIdx = g_auiRasterToZscan[ uiAbsPartIdx - 1 ];
    if ( RasterAddress::isEqualCol( uiAbsPartIdx, uiAbsZorderCUIdx, numPartInCtuWidth ) )
    {
      return m_pcPic->getCtu( getCtuRsAddr() );
    }
    else
    {
      uiLPartUnitIdx -= m_absZIdxInCtu;
      return this;
    }
  }

  uiLPartUnitIdx = g_auiRasterToZscan[ uiAbsPartIdx + numPartInCtuWidth - 1 ];
  if ( (bEnforceSliceRestriction && !CUIsFromSameSlice(m_pCtuLeft)) || (bEnforceTileRestriction && !CUIsFromSameTile(m_pCtuLeft)) )
  {
    return NULL;
  }
  return m_pCtuLeft;
}


const TComDataCU* TComDataCU::getPUAbove( UInt& uiAPartUnitIdx,
                                          UInt uiCurrPartUnitIdx,
                                          Bool bEnforceSliceRestriction,
                                          Bool planarAtCtuBoundary,
                                          Bool bEnforceTileRestriction ) const
{
  UInt uiAbsPartIdx       = g_auiZscanToRaster[uiCurrPartUnitIdx];
  UInt uiAbsZorderCUIdx   = g_auiZscanToRaster[m_absZIdxInCtu];
  const UInt numPartInCtuWidth = m_pcPic->getNumPartInCtuWidth();

  if ( !RasterAddress::isZeroRow( uiAbsPartIdx, numPartInCtuWidth ) )
  {
    uiAPartUnitIdx = g_auiRasterToZscan[ uiAbsPartIdx - numPartInCtuWidth ];
    if ( RasterAddress::isEqualRow( uiAbsPartIdx, uiAbsZorderCUIdx, numPartInCtuWidth ) )
    {
      return m_pcPic->getCtu( getCtuRsAddr() );
    }
    else
    {
      uiAPartUnitIdx -= m_absZIdxInCtu;
      return this;
    }
  }

  if(planarAtCtuBoundary)
  {
    return NULL;
  }

  uiAPartUnitIdx = g_auiRasterToZscan[ uiAbsPartIdx + m_pcPic->getNumPartitionsInCtu() - numPartInCtuWidth ];

  if ( (bEnforceSliceRestriction && !CUIsFromSameSlice(m_pCtuAbove)) || (bEnforceTileRestriction && !CUIsFromSameTile(m_pCtuAbove)) )
  {
    return NULL;
  }
  return m_pCtuAbove;
}

const TComDataCU* TComDataCU::getPUAboveLeft( UInt& uiALPartUnitIdx, UInt uiCurrPartUnitIdx, Bool bEnforceSliceRestriction ) const
{
  UInt uiAbsPartIdx       = g_auiZscanToRaster[uiCurrPartUnitIdx];
  UInt uiAbsZorderCUIdx   = g_auiZscanToRaster[m_absZIdxInCtu];
  const UInt numPartInCtuWidth = m_pcPic->getNumPartInCtuWidth();

  if ( !RasterAddress::isZeroCol( uiAbsPartIdx, numPartInCtuWidth ) )
  {
    if ( !RasterAddress::isZeroRow( uiAbsPartIdx, numPartInCtuWidth ) )
    {
      uiALPartUnitIdx = g_auiRasterToZscan[ uiAbsPartIdx - numPartInCtuWidth - 1 ];
      if ( RasterAddress::isEqualRowOrCol( uiAbsPartIdx, uiAbsZorderCUIdx, numPartInCtuWidth ) )
      {
        return m_pcPic->getCtu( getCtuRsAddr() );
      }
      else
      {
        uiALPartUnitIdx -= m_absZIdxInCtu;
        return this;
      }
    }
    uiALPartUnitIdx = g_auiRasterToZscan[ uiAbsPartIdx + getPic()->getNumPartitionsInCtu() - numPartInCtuWidth - 1 ];
    if ( bEnforceSliceRestriction && !CUIsFromSameSliceAndTile(m_pCtuAbove) )
    {
      return NULL;
    }
    return m_pCtuAbove;
  }

  if ( !RasterAddress::isZeroRow( uiAbsPartIdx, numPartInCtuWidth ) )
  {
    uiALPartUnitIdx = g_auiRasterToZscan[ uiAbsPartIdx - 1 ];
    if ( bEnforceSliceRestriction && !CUIsFromSameSliceAndTile(m_pCtuLeft) )
    {
      return NULL;
    }
    return m_pCtuLeft;
  }

  uiALPartUnitIdx = g_auiRasterToZscan[ m_pcPic->getNumPartitionsInCtu() - 1 ];
  if ( bEnforceSliceRestriction && !CUIsFromSameSliceAndTile(m_pCtuAboveLeft) )
  {
    return NULL;
  }
  return m_pCtuAboveLeft;
}

const TComDataCU* TComDataCU::getPUBelowLeft(UInt& uiBLPartUnitIdx,  UInt uiCurrPartUnitIdx, UInt uiPartUnitOffset, Bool bEnforceSliceRestriction) const
{
  UInt uiAbsPartIdxLB     = g_auiZscanToRaster[uiCurrPartUnitIdx];
  const UInt numPartInCtuWidth = m_pcPic->getNumPartInCtuWidth();
  UInt uiAbsZorderCUIdxLB = g_auiZscanToRaster[ m_absZIdxInCtu ] + ((m_puhHeight[0] / m_pcPic->getMinCUHeight()) - 1)*numPartInCtuWidth;

  if( ( m_pcPic->getCtu(m_ctuRsAddr)->getCUPelY() + g_auiRasterToPelY[uiAbsPartIdxLB] + (m_pcPic->getPicSym()->getMinCUHeight() * uiPartUnitOffset)) >= m_pcSlice->getSPS()->getPicHeightInLumaSamples())
  {
    uiBLPartUnitIdx = MAX_UINT;
    return NULL;
  }

  if ( RasterAddress::lessThanRow( uiAbsPartIdxLB, m_pcPic->getNumPartInCtuHeight() - uiPartUnitOffset, numPartInCtuWidth ) )
  {
    if ( !RasterAddress::isZeroCol( uiAbsPartIdxLB, numPartInCtuWidth ) )
    {
      if ( uiCurrPartUnitIdx > g_auiRasterToZscan[ uiAbsPartIdxLB + uiPartUnitOffset * numPartInCtuWidth - 1 ] )
      {
        uiBLPartUnitIdx = g_auiRasterToZscan[ uiAbsPartIdxLB + uiPartUnitOffset * numPartInCtuWidth - 1 ];
        if ( RasterAddress::isEqualRowOrCol( uiAbsPartIdxLB, uiAbsZorderCUIdxLB, numPartInCtuWidth ) )
        {
          return m_pcPic->getCtu( getCtuRsAddr() );
        }
        else
        {
          uiBLPartUnitIdx -= m_absZIdxInCtu;
          return this;
        }
      }
      uiBLPartUnitIdx = MAX_UINT;
      return NULL;
    }
    uiBLPartUnitIdx = g_auiRasterToZscan[ uiAbsPartIdxLB + (1+uiPartUnitOffset) * numPartInCtuWidth - 1 ];
    if ( bEnforceSliceRestriction && !CUIsFromSameSliceAndTile(m_pCtuLeft) )
    {
      return NULL;
    }
    return m_pCtuLeft;
  }

  uiBLPartUnitIdx = MAX_UINT;
  return NULL;
}

const TComDataCU* TComDataCU::getPUAboveRight(UInt&  uiARPartUnitIdx, UInt uiCurrPartUnitIdx, UInt uiPartUnitOffset, Bool bEnforceSliceRestriction) const
{
  UInt uiAbsPartIdxRT     = g_auiZscanToRaster[uiCurrPartUnitIdx];
  UInt uiAbsZorderCUIdx   = g_auiZscanToRaster[ m_absZIdxInCtu ] + (m_puhWidth[0] / m_pcPic->getMinCUWidth()) - 1;
  const UInt numPartInCtuWidth = m_pcPic->getNumPartInCtuWidth();

  if( ( m_pcPic->getCtu(m_ctuRsAddr)->getCUPelX() + g_auiRasterToPelX[uiAbsPartIdxRT] + (m_pcPic->getPicSym()->getMinCUHeight() * uiPartUnitOffset)) >= m_pcSlice->getSPS()->getPicWidthInLumaSamples() )
  {
    uiARPartUnitIdx = MAX_UINT;
    return NULL;
  }

  if ( RasterAddress::lessThanCol( uiAbsPartIdxRT, numPartInCtuWidth - uiPartUnitOffset, numPartInCtuWidth ) )
  {
    if ( !RasterAddress::isZeroRow( uiAbsPartIdxRT, numPartInCtuWidth ) )
    {
      if ( uiCurrPartUnitIdx > g_auiRasterToZscan[ uiAbsPartIdxRT - numPartInCtuWidth + uiPartUnitOffset ] )
      {
        uiARPartUnitIdx = g_auiRasterToZscan[ uiAbsPartIdxRT - numPartInCtuWidth + uiPartUnitOffset ];
        if ( RasterAddress::isEqualRowOrCol( uiAbsPartIdxRT, uiAbsZorderCUIdx, numPartInCtuWidth ) )
        {
          return m_pcPic->getCtu( getCtuRsAddr() );
        }
        else
        {
          uiARPartUnitIdx -= m_absZIdxInCtu;
          return this;
        }
      }
      uiARPartUnitIdx = MAX_UINT;
      return NULL;
    }

    uiARPartUnitIdx = g_auiRasterToZscan[ uiAbsPartIdxRT + m_pcPic->getNumPartitionsInCtu() - numPartInCtuWidth + uiPartUnitOffset ];
    if ( bEnforceSliceRestriction && !CUIsFromSameSliceAndTile(m_pCtuAbove) )
    {
      return NULL;
    }
    return m_pCtuAbove;
  }

  if ( !RasterAddress::isZeroRow( uiAbsPartIdxRT, numPartInCtuWidth ) )
  {
    uiARPartUnitIdx = MAX_UINT;
    return NULL;
  }

  uiARPartUnitIdx = g_auiRasterToZscan[ m_pcPic->getNumPartitionsInCtu() - numPartInCtuWidth + uiPartUnitOffset-1 ];
  if ( bEnforceSliceRestriction && !CUIsFromSameSliceAndTile(m_pCtuAboveRight) )
  {
    return NULL;
  }
  return m_pCtuAboveRight;
}

/** Get left QpMinCu
*\param   uiLPartUnitIdx
*\param   uiCurrAbsIdxInCtu
*\returns TComDataCU*   point of TComDataCU of left QpMinCu
*/
const TComDataCU* TComDataCU::getQpMinCuLeft( UInt& uiLPartUnitIdx, UInt uiCurrAbsIdxInCtu ) const
{
  const UInt numPartInCtuWidth = m_pcPic->getNumPartInCtuWidth();
  const UInt maxCUDepth        = getSlice()->getSPS()->getMaxTotalCUDepth();
  const UInt maxCuDQPDepth     = getSlice()->getPPS()->getMaxCuDQPDepth();
  const UInt doubleDepthDifference = ((maxCUDepth - maxCuDQPDepth)<<1);
  UInt absZorderQpMinCUIdx = (uiCurrAbsIdxInCtu>>doubleDepthDifference)<<doubleDepthDifference;
  UInt absRorderQpMinCUIdx = g_auiZscanToRaster[absZorderQpMinCUIdx];

  // check for left CTU boundary
  if ( RasterAddress::isZeroCol(absRorderQpMinCUIdx, numPartInCtuWidth) )
  {
    return NULL;
  }

  // get index of left-CU relative to top-left corner of current quantization group
  uiLPartUnitIdx = g_auiRasterToZscan[absRorderQpMinCUIdx - 1];

  // return pointer to current CTU
  return m_pcPic->getCtu( getCtuRsAddr() );
}

/** Get Above QpMinCu
*\param   uiAPartUnitIdx
*\param   uiCurrAbsIdxInCtu
*\returns TComDataCU*   point of TComDataCU of above QpMinCu
*/
const TComDataCU* TComDataCU::getQpMinCuAbove( UInt& uiAPartUnitIdx, UInt uiCurrAbsIdxInCtu ) const
{
  const UInt numPartInCtuWidth = m_pcPic->getNumPartInCtuWidth();
  const UInt maxCUDepth        = getSlice()->getSPS()->getMaxTotalCUDepth();
  const UInt maxCuDQPDepth     = getSlice()->getPPS()->getMaxCuDQPDepth();
  const UInt doubleDepthDifference = ((maxCUDepth - maxCuDQPDepth)<<1);
  UInt absZorderQpMinCUIdx = (uiCurrAbsIdxInCtu>>doubleDepthDifference)<<doubleDepthDifference;
  UInt absRorderQpMinCUIdx = g_auiZscanToRaster[absZorderQpMinCUIdx];

  // check for top CTU boundary
  if ( RasterAddress::isZeroRow( absRorderQpMinCUIdx, numPartInCtuWidth) )
  {
    return NULL;
  }

  // get index of top-CU relative to top-left corner of current quantization group
  uiAPartUnitIdx = g_auiRasterToZscan[absRorderQpMinCUIdx - numPartInCtuWidth];

  // return pointer to current CTU
  return m_pcPic->getCtu( getCtuRsAddr() );
}



/** Get reference QP from left QpMinCu or latest coded QP
*\param   uiCurrAbsIdxInCtu
*\returns SChar   reference QP value
*/
SChar TComDataCU::getRefQP( UInt uiCurrAbsIdxInCtu ) const
{
  UInt lPartIdx = MAX_UINT;
  UInt aPartIdx = MAX_UINT;
  const TComDataCU* cULeft  = getQpMinCuLeft ( lPartIdx, m_absZIdxInCtu + uiCurrAbsIdxInCtu );
  const TComDataCU* cUAbove = getQpMinCuAbove( aPartIdx, m_absZIdxInCtu + uiCurrAbsIdxInCtu );
  return (((cULeft? cULeft->getQP( lPartIdx ): getLastCodedQP( uiCurrAbsIdxInCtu )) + (cUAbove? cUAbove->getQP( aPartIdx ): getLastCodedQP( uiCurrAbsIdxInCtu )) + 1) >> 1);
}

Int TComDataCU::getLastValidPartIdx( Int iAbsPartIdx ) const
{
  Int iLastValidPartIdx = iAbsPartIdx-1;
  while ( iLastValidPartIdx >= 0
       && getPredictionMode( iLastValidPartIdx ) == NUMBER_OF_PREDICTION_MODES )
  {
    UInt uiDepth = getDepth( iLastValidPartIdx );
    iLastValidPartIdx -= m_uiNumPartition>>(uiDepth<<1);
  }
  return iLastValidPartIdx;
}

SChar TComDataCU::getLastCodedQP( UInt uiAbsPartIdx ) const
{
  UInt uiQUPartIdxMask = ~((1<<((getSlice()->getSPS()->getMaxTotalCUDepth() - getSlice()->getPPS()->getMaxCuDQPDepth())<<1))-1);
  Int iLastValidPartIdx = getLastValidPartIdx( uiAbsPartIdx&uiQUPartIdxMask ); // A idx will be invalid if it is off the right or bottom edge of the picture.
  // If this CU is in the first CTU of the slice and there is no valid part before this one, use slice QP
  if ( getPic()->getPicSym()->getCtuTsToRsAddrMap(getSlice()->getSliceCurStartCtuTsAddr()) == getCtuRsAddr() && Int(getZorderIdxInCtu())+iLastValidPartIdx<0)
  {
    return getSlice()->getSliceQp();
  }
  else if ( iLastValidPartIdx >= 0 )
  {
    // If there is a valid part within the current Sub-CU, use it
    return getQP( iLastValidPartIdx );
  }
  else
  {
    if ( getZorderIdxInCtu() > 0 )
    {
      // If this wasn't the first sub-cu within the Ctu, explore the CTU itself.
      return getPic()->getCtu( getCtuRsAddr() )->getLastCodedQP( getZorderIdxInCtu() ); // TODO - remove this recursion
    }
    else if ( getPic()->getPicSym()->getCtuRsToTsAddrMap(getCtuRsAddr()) > 0
      && CUIsFromSameSliceTileAndWavefrontRow(getPic()->getCtu(getPic()->getPicSym()->getCtuTsToRsAddrMap(getPic()->getPicSym()->getCtuRsToTsAddrMap(getCtuRsAddr())-1))) )
    {
      // If this isn't the first Ctu (how can it be due to the first 'if'?), and the previous Ctu is from the same tile, examine the previous Ctu.
      return getPic()->getCtu( getPic()->getPicSym()->getCtuTsToRsAddrMap(getPic()->getPicSym()->getCtuRsToTsAddrMap(getCtuRsAddr())-1) )->getLastCodedQP( getPic()->getNumPartitionsInCtu() );  // TODO - remove this recursion
    }
    else
    {
      // No other options available - use the slice-level QP.
      return getSlice()->getSliceQp();
    }
  }
}


/** Check whether the CU is coded in lossless coding mode.
 * \param   absPartIdx
 * \returns true if the CU is coded in lossless coding mode; false if otherwise
 */
Bool TComDataCU::isLosslessCoded(UInt absPartIdx) const
{
  return (getSlice()->getPPS()->getTransquantBypassEnabledFlag() && getCUTransquantBypass (absPartIdx));
}


/** Get allowed chroma intra modes
*   - fills uiModeList with chroma intra modes
*
*\param   [in]  uiAbsPartIdx
*\param   [out] uiModeList pointer to chroma intra modes array
*/
Void TComDataCU::getAllowedChromaDir( UInt uiAbsPartIdx, UInt uiModeList[NUM_CHROMA_MODE] ) const
{
  uiModeList[0] = PLANAR_IDX;
  uiModeList[1] = VER_IDX;
  uiModeList[2] = HOR_IDX;
  uiModeList[3] = DC_IDX;
  uiModeList[4] = DM_CHROMA_IDX;
  assert(4<NUM_CHROMA_MODE);

  UInt uiLumaMode = getIntraDir( CHANNEL_TYPE_LUMA, uiAbsPartIdx );

  for( Int i = 0; i < NUM_CHROMA_MODE - 1; i++ )
  {
    if( uiLumaMode == uiModeList[i] )
    {
      uiModeList[i] = 34; // VER+8 mode
      break;
    }
  }
}

/** Get most probable intra modes
*\param   uiAbsPartIdx    partition index
*\param   uiIntraDirPred  pointer to the array for MPM storage
*\param   compID          colour component ID
*\param   piMode          it is set with MPM mode in case both MPM are equal. It is used to restrict RD search at encode side.
*\returns Number of MPM
*/
Void TComDataCU::getIntraDirPredictor( UInt uiAbsPartIdx, Int uiIntraDirPred[NUM_MOST_PROBABLE_MODES], const ComponentID compID, Int* piMode ) const
{
  UInt        LeftPartIdx  = MAX_UINT;
  UInt        AbovePartIdx = MAX_UINT;
  Int         iLeftIntraDir, iAboveIntraDir;
  const TComSPS *sps=getSlice()->getSPS();
  const UInt partsPerMinCU = 1<<(2*(sps->getMaxTotalCUDepth() - sps->getLog2DiffMaxMinCodingBlockSize()));

  const ChannelType chType = toChannelType(compID);
  const ChromaFormat chForm = getPic()->getChromaFormat();
  // Get intra direction of left PU
  const TComDataCU *pcCULeft = getPULeft( LeftPartIdx, m_absZIdxInCtu + uiAbsPartIdx );

  if (isChroma(compID))
  {
    LeftPartIdx = getChromasCorrespondingPULumaIdx(LeftPartIdx, chForm, partsPerMinCU);
  }
  iLeftIntraDir  = pcCULeft ? ( pcCULeft->isIntra( LeftPartIdx ) ? pcCULeft->getIntraDir( chType, LeftPartIdx ) : DC_IDX ) : DC_IDX;

  // Get intra direction of above PU
  const TComDataCU *pcCUAbove = getPUAbove( AbovePartIdx, m_absZIdxInCtu + uiAbsPartIdx, true, true );

  if (isChroma(compID))
  {
    AbovePartIdx = getChromasCorrespondingPULumaIdx(AbovePartIdx, chForm, partsPerMinCU);
  }
  iAboveIntraDir = pcCUAbove ? ( pcCUAbove->isIntra( AbovePartIdx ) ? pcCUAbove->getIntraDir( chType, AbovePartIdx ) : DC_IDX ) : DC_IDX;

  if (isChroma(chType))
  {
    if (iLeftIntraDir  == DM_CHROMA_IDX)
    {
      iLeftIntraDir  = pcCULeft-> getIntraDir( CHANNEL_TYPE_LUMA, LeftPartIdx  );
    }
    if (iAboveIntraDir == DM_CHROMA_IDX)
    {
      iAboveIntraDir = pcCUAbove->getIntraDir( CHANNEL_TYPE_LUMA, AbovePartIdx );
    }
  }

  assert (2<NUM_MOST_PROBABLE_MODES);
  if(iLeftIntraDir == iAboveIntraDir)
  {
    if( piMode )
    {
      *piMode = 1;
    }

    if (iLeftIntraDir > 1) // angular modes
    {
      uiIntraDirPred[0] = iLeftIntraDir;
      uiIntraDirPred[1] = ((iLeftIntraDir + 29) % 32) + 2;
      uiIntraDirPred[2] = ((iLeftIntraDir - 1 ) % 32) + 2;
    }
    else //non-angular
    {
      uiIntraDirPred[0] = PLANAR_IDX;
      uiIntraDirPred[1] = DC_IDX;
      uiIntraDirPred[2] = VER_IDX;
    }
  }
  else
  {
    if( piMode )
    {
      *piMode = 2;
    }
    uiIntraDirPred[0] = iLeftIntraDir;
    uiIntraDirPred[1] = iAboveIntraDir;

    if (iLeftIntraDir && iAboveIntraDir ) //both modes are non-planar
    {
      uiIntraDirPred[2] = PLANAR_IDX;
    }
    else
    {
      uiIntraDirPred[2] =  (iLeftIntraDir+iAboveIntraDir)<2? VER_IDX : DC_IDX;
    }
  }
  for (UInt i=0; i<NUM_MOST_PROBABLE_MODES; i++)
  {
    assert(uiIntraDirPred[i] < 35);
  }
}

UInt TComDataCU::getCtxSplitFlag( UInt uiAbsPartIdx, UInt uiDepth ) const
{
  const TComDataCU* pcTempCU;
  UInt              uiTempPartIdx;
  UInt              uiCtx;
  // Get left split flag
  pcTempCU = getPULeft( uiTempPartIdx, m_absZIdxInCtu + uiAbsPartIdx );
  uiCtx  = ( pcTempCU ) ? ( ( pcTempCU->getDepth( uiTempPartIdx ) > uiDepth ) ? 1 : 0 ) : 0;

  // Get above split flag
  pcTempCU = getPUAbove( uiTempPartIdx, m_absZIdxInCtu + uiAbsPartIdx );
  uiCtx += ( pcTempCU ) ? ( ( pcTempCU->getDepth( uiTempPartIdx ) > uiDepth ) ? 1 : 0 ) : 0;

  return uiCtx;
}

UInt TComDataCU::getCtxQtCbf( TComTU &rTu, const ChannelType chType ) const
{
  const UInt transformDepth = rTu.GetTransformDepthRel();

  if (isChroma(chType))
  {
    return transformDepth;
  }
  else
  {
    const UInt uiCtx = ( transformDepth == 0 ? 1 : 0 );
    return uiCtx;
  }
}

UInt TComDataCU::getQuadtreeTULog2MinSizeInCU( UInt absPartIdx ) const
{
  UInt log2CbSize = g_aucConvertToBit[getWidth( absPartIdx )] + 2;
  PartSize  partSize  = getPartitionSize( absPartIdx );
  UInt quadtreeTUMaxDepth = isIntra( absPartIdx ) ? m_pcSlice->getSPS()->getQuadtreeTUMaxDepthIntra() : m_pcSlice->getSPS()->getQuadtreeTUMaxDepthInter();
  Int intraSplitFlag = ( isIntra( absPartIdx ) && partSize == SIZE_NxN ) ? 1 : 0;
  Int interSplitFlag = ((quadtreeTUMaxDepth == 1) && isInter( absPartIdx ) && (partSize != SIZE_2Nx2N) );

  UInt log2MinTUSizeInCU = 0;
  if (log2CbSize < (m_pcSlice->getSPS()->getQuadtreeTULog2MinSize() + quadtreeTUMaxDepth - 1 + interSplitFlag + intraSplitFlag) )
  {
    // when fully making use of signaled TUMaxDepth + inter/intraSplitFlag, resulting luma TB size is < QuadtreeTULog2MinSize
    log2MinTUSizeInCU = m_pcSlice->getSPS()->getQuadtreeTULog2MinSize();
  }
  else
  {
    // when fully making use of signaled TUMaxDepth + inter/intraSplitFlag, resulting luma TB size is still >= QuadtreeTULog2MinSize
    log2MinTUSizeInCU = log2CbSize - ( quadtreeTUMaxDepth - 1 + interSplitFlag + intraSplitFlag); // stop when trafoDepth == hierarchy_depth = splitFlag
    if ( log2MinTUSizeInCU > m_pcSlice->getSPS()->getQuadtreeTULog2MaxSize())
    {
      // when fully making use of signaled TUMaxDepth + inter/intraSplitFlag, resulting luma TB size is still > QuadtreeTULog2MaxSize
      log2MinTUSizeInCU = m_pcSlice->getSPS()->getQuadtreeTULog2MaxSize();
    }
  }
  return log2MinTUSizeInCU;
}

UInt TComDataCU::getCtxSkipFlag( UInt uiAbsPartIdx ) const
{
  const TComDataCU* pcTempCU;
  UInt              uiTempPartIdx;
  UInt              uiCtx = 0;

  // Get BCBP of left PU
  pcTempCU = getPULeft( uiTempPartIdx, m_absZIdxInCtu + uiAbsPartIdx );
  uiCtx    = ( pcTempCU ) ? pcTempCU->isSkipped( uiTempPartIdx ) : 0;

  // Get BCBP of above PU
  pcTempCU = getPUAbove( uiTempPartIdx, m_absZIdxInCtu + uiAbsPartIdx );
  uiCtx   += ( pcTempCU ) ? pcTempCU->isSkipped( uiTempPartIdx ) : 0;

  return uiCtx;
}

UInt TComDataCU::getCtxInterDir( UInt uiAbsPartIdx ) const
{
  return getDepth( uiAbsPartIdx );
}


UChar TComDataCU::getQtRootCbf( UInt uiIdx ) const
{
  const UInt numberValidComponents = getPic()->getNumberValidComponents();
  return getCbf( uiIdx, COMPONENT_Y, 0 )
          || ((numberValidComponents > COMPONENT_Cb) && getCbf( uiIdx, COMPONENT_Cb, 0 ))
          || ((numberValidComponents > COMPONENT_Cr) && getCbf( uiIdx, COMPONENT_Cr, 0 ));
}

Void TComDataCU::setCbfSubParts( const UInt uiCbf[MAX_NUM_COMPONENT], UInt uiAbsPartIdx, UInt uiDepth )
{
  UInt uiCurrPartNumb = m_pcPic->getNumPartitionsInCtu() >> (uiDepth << 1);
  for(UInt comp=0; comp<MAX_NUM_COMPONENT; comp++)
  {
    memset( m_puhCbf[comp] + uiAbsPartIdx, uiCbf[comp], sizeof( UChar ) * uiCurrPartNumb );
  }
}

Void TComDataCU::setCbfSubParts( UInt uiCbf, ComponentID compID, UInt uiAbsPartIdx, UInt uiDepth )
{
  UInt uiCurrPartNumb = m_pcPic->getNumPartitionsInCtu() >> (uiDepth << 1);
  memset( m_puhCbf[compID] + uiAbsPartIdx, uiCbf, sizeof( UChar ) * uiCurrPartNumb );
}

/** Sets a coded block flag for all sub-partitions of a partition
 * \param uiCbf          The value of the coded block flag to be set
 * \param compID
 * \param uiAbsPartIdx
 * \param uiPartIdx
 * \param uiDepth
 */
Void TComDataCU::setCbfSubParts ( UInt uiCbf, ComponentID compID, UInt uiAbsPartIdx, UInt uiPartIdx, UInt uiDepth )
{
  setSubPart<UChar>( uiCbf, m_puhCbf[compID], uiAbsPartIdx, uiDepth, uiPartIdx );
}

Void TComDataCU::setCbfPartRange ( UInt uiCbf, ComponentID compID, UInt uiAbsPartIdx, UInt uiCoveredPartIdxes )
{
  memset((m_puhCbf[compID] + uiAbsPartIdx), uiCbf, (sizeof(UChar) * uiCoveredPartIdxes));
}

Void TComDataCU::bitwiseOrCbfPartRange( UInt uiCbf, ComponentID compID, UInt uiAbsPartIdx, UInt uiCoveredPartIdxes )
{
  const UInt stopAbsPartIdx = uiAbsPartIdx + uiCoveredPartIdxes;

  for (UInt subPartIdx = uiAbsPartIdx; subPartIdx < stopAbsPartIdx; subPartIdx++)
  {
    m_puhCbf[compID][subPartIdx] |= uiCbf;
  }
}

Void TComDataCU::setDepthSubParts( UInt uiDepth, UInt uiAbsPartIdx )
{
  UInt uiCurrPartNumb = m_pcPic->getNumPartitionsInCtu() >> (uiDepth << 1);
  memset( m_puhDepth + uiAbsPartIdx, uiDepth, sizeof(UChar)*uiCurrPartNumb );
}

Bool TComDataCU::isFirstAbsZorderIdxInDepth (UInt uiAbsPartIdx, UInt uiDepth) const
{
  UInt uiPartNumb = m_pcPic->getNumPartitionsInCtu() >> (uiDepth << 1);
  return (((m_absZIdxInCtu + uiAbsPartIdx)% uiPartNumb) == 0);
}

Void TComDataCU::setPartSizeSubParts( PartSize eMode, UInt uiAbsPartIdx, UInt uiDepth )
{
  assert( sizeof( *m_pePartSize) == 1 );
  memset( m_pePartSize + uiAbsPartIdx, eMode, m_pcPic->getNumPartitionsInCtu() >> ( 2 * uiDepth ) );
}

Void TComDataCU::setCUTransquantBypassSubParts( Bool flag, UInt uiAbsPartIdx, UInt uiDepth )
{
  memset( m_CUTransquantBypass + uiAbsPartIdx, flag, m_pcPic->getNumPartitionsInCtu() >> ( 2 * uiDepth ) );
}

Void TComDataCU::setSkipFlagSubParts( Bool skip, UInt absPartIdx, UInt depth )
{
  assert( sizeof( *m_skipFlag) == 1 );
  memset( m_skipFlag + absPartIdx, skip, m_pcPic->getNumPartitionsInCtu() >> ( 2 * depth ) );
}

Void TComDataCU::setPredModeSubParts( PredMode eMode, UInt uiAbsPartIdx, UInt uiDepth )
{
  assert( sizeof( *m_pePredMode) == 1 );
  memset( m_pePredMode + uiAbsPartIdx, eMode, m_pcPic->getNumPartitionsInCtu() >> ( 2 * uiDepth ) );
}

Void TComDataCU::setChromaQpAdjSubParts( UChar val, Int absPartIdx, Int depth )
{
  assert( sizeof(*m_ChromaQpAdj) == 1 );
  memset( m_ChromaQpAdj + absPartIdx, val, m_pcPic->getNumPartitionsInCtu() >> ( 2 * depth ) );
}

Void TComDataCU::setQPSubCUs( Int qp, UInt absPartIdx, UInt depth, Bool &foundNonZeroCbf )
{
  UInt currPartNumb = m_pcPic->getNumPartitionsInCtu() >> (depth << 1);
  UInt currPartNumQ = currPartNumb >> 2;
  const UInt numValidComp = m_pcPic->getNumberValidComponents();

  if(!foundNonZeroCbf)
  {
    if(getDepth(absPartIdx) > depth)
    {
      for ( UInt partUnitIdx = 0; partUnitIdx < 4; partUnitIdx++ )
      {
        setQPSubCUs( qp, absPartIdx+partUnitIdx*currPartNumQ, depth+1, foundNonZeroCbf );
      }
    }
    else
    {
      if(getCbf( absPartIdx, COMPONENT_Y ) || (numValidComp>COMPONENT_Cb && getCbf( absPartIdx, COMPONENT_Cb )) || (numValidComp>COMPONENT_Cr && getCbf( absPartIdx, COMPONENT_Cr) ) )
      {
        foundNonZeroCbf = true;
      }
      else
      {
        setQPSubParts(qp, absPartIdx, depth);
      }
    }
  }
}

Void TComDataCU::setQPSubParts( Int qp, UInt uiAbsPartIdx, UInt uiDepth )
{
  const UInt numPart = m_pcPic->getNumPartitionsInCtu() >> (uiDepth << 1);
  memset(m_phQP+uiAbsPartIdx, qp, numPart);
}

Void TComDataCU::setIntraDirSubParts( const ChannelType channelType, const UInt dir, const UInt absPartIdx, const UInt depth )
{
  UInt numPart = m_pcPic->getNumPartitionsInCtu() >> (depth << 1);
  memset( m_puhIntraDir[channelType] + absPartIdx, dir,sizeof(UChar)*numPart );
}

template<typename T>
Void TComDataCU::setSubPart( T uiParameter, T* puhBaseCtu, UInt uiCUAddr, UInt uiCUDepth, UInt uiPUIdx )
{
  assert( sizeof(T) == 1 ); // Using memset() works only for types of size 1

  UInt uiCurrPartNumQ = (m_pcPic->getNumPartitionsInCtu() >> (2 * uiCUDepth)) >> 2;
  switch ( m_pePartSize[ uiCUAddr ] )
  {
    case SIZE_2Nx2N:
      memset( puhBaseCtu + uiCUAddr, uiParameter, 4 * uiCurrPartNumQ );
      break;
    case SIZE_2NxN:
      memset( puhBaseCtu + uiCUAddr, uiParameter, 2 * uiCurrPartNumQ );
      break;
    case SIZE_Nx2N:
      memset( puhBaseCtu + uiCUAddr, uiParameter, uiCurrPartNumQ );
      memset( puhBaseCtu + uiCUAddr + 2 * uiCurrPartNumQ, uiParameter, uiCurrPartNumQ );
      break;
    case SIZE_NxN:
      memset( puhBaseCtu + uiCUAddr, uiParameter, uiCurrPartNumQ );
      break;
    case SIZE_2NxnU:
      if ( uiPUIdx == 0 )
      {
        memset( puhBaseCtu + uiCUAddr, uiParameter, (uiCurrPartNumQ >> 1) );
        memset( puhBaseCtu + uiCUAddr + uiCurrPartNumQ, uiParameter, (uiCurrPartNumQ >> 1) );
      }
      else if ( uiPUIdx == 1 )
      {
        memset( puhBaseCtu + uiCUAddr, uiParameter, (uiCurrPartNumQ >> 1) );
        memset( puhBaseCtu + uiCUAddr + uiCurrPartNumQ, uiParameter, ((uiCurrPartNumQ >> 1) + (uiCurrPartNumQ << 1)) );
      }
      else
      {
        assert(0);
      }
      break;
    case SIZE_2NxnD:
      if ( uiPUIdx == 0 )
      {
        memset( puhBaseCtu + uiCUAddr, uiParameter, ((uiCurrPartNumQ << 1) + (uiCurrPartNumQ >> 1)) );
        memset( puhBaseCtu + uiCUAddr + (uiCurrPartNumQ << 1) + uiCurrPartNumQ, uiParameter, (uiCurrPartNumQ >> 1) );
      }
      else if ( uiPUIdx == 1 )
      {
        memset( puhBaseCtu + uiCUAddr, uiParameter, (uiCurrPartNumQ >> 1) );
        memset( puhBaseCtu + uiCUAddr + uiCurrPartNumQ, uiParameter, (uiCurrPartNumQ >> 1) );
      }
      else
      {
        assert(0);
      }
      break;
    case SIZE_nLx2N:
      if ( uiPUIdx == 0 )
      {
        memset( puhBaseCtu + uiCUAddr, uiParameter, (uiCurrPartNumQ >> 2) );
        memset( puhBaseCtu + uiCUAddr + (uiCurrPartNumQ >> 1), uiParameter, (uiCurrPartNumQ >> 2) );
        memset( puhBaseCtu + uiCUAddr + (uiCurrPartNumQ << 1), uiParameter, (uiCurrPartNumQ >> 2) );
        memset( puhBaseCtu + uiCUAddr + (uiCurrPartNumQ << 1) + (uiCurrPartNumQ >> 1), uiParameter, (uiCurrPartNumQ >> 2) );
      }
      else if ( uiPUIdx == 1 )
      {
        memset( puhBaseCtu + uiCUAddr, uiParameter, (uiCurrPartNumQ >> 2) );
        memset( puhBaseCtu + uiCUAddr + (uiCurrPartNumQ >> 1), uiParameter, (uiCurrPartNumQ + (uiCurrPartNumQ >> 2)) );
        memset( puhBaseCtu + uiCUAddr + (uiCurrPartNumQ << 1), uiParameter, (uiCurrPartNumQ >> 2) );
        memset( puhBaseCtu + uiCUAddr + (uiCurrPartNumQ << 1) + (uiCurrPartNumQ >> 1), uiParameter, (uiCurrPartNumQ + (uiCurrPartNumQ >> 2)) );
      }
      else
      {
        assert(0);
      }
      break;
    case SIZE_nRx2N:
      if ( uiPUIdx == 0 )
      {
        memset( puhBaseCtu + uiCUAddr, uiParameter, (uiCurrPartNumQ + (uiCurrPartNumQ >> 2)) );
        memset( puhBaseCtu + uiCUAddr + uiCurrPartNumQ + (uiCurrPartNumQ >> 1), uiParameter, (uiCurrPartNumQ >> 2) );
        memset( puhBaseCtu + uiCUAddr + (uiCurrPartNumQ << 1), uiParameter, (uiCurrPartNumQ + (uiCurrPartNumQ >> 2)) );
        memset( puhBaseCtu + uiCUAddr + (uiCurrPartNumQ << 1) + uiCurrPartNumQ + (uiCurrPartNumQ >> 1), uiParameter, (uiCurrPartNumQ >> 2) );
      }
      else if ( uiPUIdx == 1 )
      {
        memset( puhBaseCtu + uiCUAddr, uiParameter, (uiCurrPartNumQ >> 2) );
        memset( puhBaseCtu + uiCUAddr + (uiCurrPartNumQ >> 1), uiParameter, (uiCurrPartNumQ >> 2) );
        memset( puhBaseCtu + uiCUAddr + (uiCurrPartNumQ << 1), uiParameter, (uiCurrPartNumQ >> 2) );
        memset( puhBaseCtu + uiCUAddr + (uiCurrPartNumQ << 1) + (uiCurrPartNumQ >> 1), uiParameter, (uiCurrPartNumQ >> 2) );
      }
      else
      {
        assert(0);
      }
      break;
    default:
      assert( 0 );
      break;
  }
}

Void TComDataCU::setMergeFlagSubParts ( Bool bMergeFlag, UInt uiAbsPartIdx, UInt uiPartIdx, UInt uiDepth )
{
  setSubPart( bMergeFlag, m_pbMergeFlag, uiAbsPartIdx, uiDepth, uiPartIdx );
}

Void TComDataCU::setMergeIndexSubParts ( UInt uiMergeIndex, UInt uiAbsPartIdx, UInt uiPartIdx, UInt uiDepth )
{
  setSubPart<UChar>( uiMergeIndex, m_puhMergeIndex, uiAbsPartIdx, uiDepth, uiPartIdx );
}

Void TComDataCU::setInterDirSubParts( UInt uiDir, UInt uiAbsPartIdx, UInt uiPartIdx, UInt uiDepth )
{
  setSubPart<UChar>( uiDir, m_puhInterDir, uiAbsPartIdx, uiDepth, uiPartIdx );
}

Void TComDataCU::setMVPIdxSubParts( Int iMVPIdx, RefPicList eRefPicList, UInt uiAbsPartIdx, UInt uiPartIdx, UInt uiDepth )
{
  setSubPart<SChar>( iMVPIdx, m_apiMVPIdx[eRefPicList], uiAbsPartIdx, uiDepth, uiPartIdx );
}

Void TComDataCU::setMVPNumSubParts( Int iMVPNum, RefPicList eRefPicList, UInt uiAbsPartIdx, UInt uiPartIdx, UInt uiDepth )
{
  setSubPart<SChar>( iMVPNum, m_apiMVPNum[eRefPicList], uiAbsPartIdx, uiDepth, uiPartIdx );
}


Void TComDataCU::setTrIdxSubParts( UInt uiTrIdx, UInt uiAbsPartIdx, UInt uiDepth )
{
  UInt uiCurrPartNumb = m_pcPic->getNumPartitionsInCtu() >> (uiDepth << 1);

  memset( m_puhTrIdx + uiAbsPartIdx, uiTrIdx, sizeof(UChar)*uiCurrPartNumb );
}

Void TComDataCU::setTransformSkipSubParts( const UInt useTransformSkip[MAX_NUM_COMPONENT], UInt uiAbsPartIdx, UInt uiDepth )
{
  UInt uiCurrPartNumb = m_pcPic->getNumPartitionsInCtu() >> (uiDepth << 1);

  for(UInt i=0; i<MAX_NUM_COMPONENT; i++)
  {
    memset( m_puhTransformSkip[i] + uiAbsPartIdx, useTransformSkip[i], sizeof( UChar ) * uiCurrPartNumb );
  }
}

Void TComDataCU::setTransformSkipSubParts( UInt useTransformSkip, ComponentID compID, UInt uiAbsPartIdx, UInt uiDepth)
{
  UInt uiCurrPartNumb = m_pcPic->getNumPartitionsInCtu() >> (uiDepth << 1);

  memset( m_puhTransformSkip[compID] + uiAbsPartIdx, useTransformSkip, sizeof( UChar ) * uiCurrPartNumb );
}

Void TComDataCU::setTransformSkipPartRange ( UInt useTransformSkip, ComponentID compID, UInt uiAbsPartIdx, UInt uiCoveredPartIdxes )
{
  memset((m_puhTransformSkip[compID] + uiAbsPartIdx), useTransformSkip, (sizeof(UChar) * uiCoveredPartIdxes));
}

Void TComDataCU::setCrossComponentPredictionAlphaPartRange( SChar alphaValue, ComponentID compID, UInt uiAbsPartIdx, UInt uiCoveredPartIdxes )
{
  memset((m_crossComponentPredictionAlpha[compID] + uiAbsPartIdx), alphaValue, (sizeof(SChar) * uiCoveredPartIdxes));
}

Void TComDataCU::setExplicitRdpcmModePartRange ( UInt rdpcmMode, ComponentID compID, UInt uiAbsPartIdx, UInt uiCoveredPartIdxes )
{
  memset((m_explicitRdpcmMode[compID] + uiAbsPartIdx), rdpcmMode, (sizeof(UChar) * uiCoveredPartIdxes));
}

Void TComDataCU::setSizeSubParts( UInt uiWidth, UInt uiHeight, UInt uiAbsPartIdx, UInt uiDepth )
{
  UInt uiCurrPartNumb = m_pcPic->getNumPartitionsInCtu() >> (uiDepth << 1);

  memset( m_puhWidth  + uiAbsPartIdx, uiWidth,  sizeof(UChar)*uiCurrPartNumb );
  memset( m_puhHeight + uiAbsPartIdx, uiHeight, sizeof(UChar)*uiCurrPartNumb );
}

UChar TComDataCU::getNumPartitions(const UInt uiAbsPartIdx) const
{
  UChar iNumPart = 0;

  switch ( m_pePartSize[uiAbsPartIdx] )
  {
    case SIZE_2Nx2N:    iNumPart = 1; break;
    case SIZE_2NxN:     iNumPart = 2; break;
    case SIZE_Nx2N:     iNumPart = 2; break;
    case SIZE_NxN:      iNumPart = 4; break;
    case SIZE_2NxnU:    iNumPart = 2; break;
    case SIZE_2NxnD:    iNumPart = 2; break;
    case SIZE_nLx2N:    iNumPart = 2; break;
    case SIZE_nRx2N:    iNumPart = 2; break;
    default:            assert (0);   break;
  }

  return  iNumPart;
}

// This is for use by a leaf/sub CU object only, with no additional AbsPartIdx
Void TComDataCU::getPartIndexAndSize( UInt uiPartIdx, UInt& ruiPartAddr, Int& riWidth, Int& riHeight ) const
{
  switch ( m_pePartSize[0] )
  {
    case SIZE_2NxN:
      riWidth = getWidth(0);      riHeight = getHeight(0) >> 1; ruiPartAddr = ( uiPartIdx == 0 )? 0 : m_uiNumPartition >> 1;
      break;
    case SIZE_Nx2N:
      riWidth = getWidth(0) >> 1; riHeight = getHeight(0);      ruiPartAddr = ( uiPartIdx == 0 )? 0 : m_uiNumPartition >> 2;
      break;
    case SIZE_NxN:
      riWidth = getWidth(0) >> 1; riHeight = getHeight(0) >> 1; ruiPartAddr = ( m_uiNumPartition >> 2 ) * uiPartIdx;
      break;
    case SIZE_2NxnU:
      riWidth     = getWidth(0);
      riHeight    = ( uiPartIdx == 0 ) ?  getHeight(0) >> 2 : ( getHeight(0) >> 2 ) + ( getHeight(0) >> 1 );
      ruiPartAddr = ( uiPartIdx == 0 ) ? 0 : m_uiNumPartition >> 3;
      break;
    case SIZE_2NxnD:
      riWidth     = getWidth(0);
      riHeight    = ( uiPartIdx == 0 ) ?  ( getHeight(0) >> 2 ) + ( getHeight(0) >> 1 ) : getHeight(0) >> 2;
      ruiPartAddr = ( uiPartIdx == 0 ) ? 0 : (m_uiNumPartition >> 1) + (m_uiNumPartition >> 3);
      break;
    case SIZE_nLx2N:
      riWidth     = ( uiPartIdx == 0 ) ? getWidth(0) >> 2 : ( getWidth(0) >> 2 ) + ( getWidth(0) >> 1 );
      riHeight    = getHeight(0);
      ruiPartAddr = ( uiPartIdx == 0 ) ? 0 : m_uiNumPartition >> 4;
      break;
    case SIZE_nRx2N:
      riWidth     = ( uiPartIdx == 0 ) ? ( getWidth(0) >> 2 ) + ( getWidth(0) >> 1 ) : getWidth(0) >> 2;
      riHeight    = getHeight(0);
      ruiPartAddr = ( uiPartIdx == 0 ) ? 0 : (m_uiNumPartition >> 2) + (m_uiNumPartition >> 4);
      break;
    default:
      assert ( m_pePartSize[0] == SIZE_2Nx2N );
      riWidth = getWidth(0);      riHeight = getHeight(0);      ruiPartAddr = 0;
      break;
  }
}

// static member function
Void TComDataCU::getMvField ( const TComDataCU* pcCU, UInt uiAbsPartIdx, RefPicList eRefPicList, TComMvField& rcMvField )
{
  if ( pcCU == NULL )  // OUT OF BOUNDARY
  {
    TComMv  cZeroMv;
    rcMvField.setMvField( cZeroMv, NOT_VALID );
    return;
  }

  const TComCUMvField*  pcCUMvField = pcCU->getCUMvField( eRefPicList );
  rcMvField.setMvField( pcCUMvField->getMv( uiAbsPartIdx ), pcCUMvField->getRefIdx( uiAbsPartIdx ) );
}

Void TComDataCU::deriveLeftRightTopIdxGeneral ( UInt uiAbsPartIdx, UInt uiPartIdx, UInt& ruiPartIdxLT, UInt& ruiPartIdxRT ) const
{
  ruiPartIdxLT = m_absZIdxInCtu + uiAbsPartIdx;
  UInt uiPUWidth = 0;

  switch ( m_pePartSize[uiAbsPartIdx] )
  {
    case SIZE_2Nx2N: uiPUWidth = m_puhWidth[uiAbsPartIdx];  break;
    case SIZE_2NxN:  uiPUWidth = m_puhWidth[uiAbsPartIdx];   break;
    case SIZE_Nx2N:  uiPUWidth = m_puhWidth[uiAbsPartIdx]  >> 1;  break;
    case SIZE_NxN:   uiPUWidth = m_puhWidth[uiAbsPartIdx]  >> 1; break;
    case SIZE_2NxnU:   uiPUWidth = m_puhWidth[uiAbsPartIdx]; break;
    case SIZE_2NxnD:   uiPUWidth = m_puhWidth[uiAbsPartIdx]; break;
    case SIZE_nLx2N:
      if ( uiPartIdx == 0 )
      {
        uiPUWidth = m_puhWidth[uiAbsPartIdx]  >> 2;
      }
      else if ( uiPartIdx == 1 )
      {
        uiPUWidth = (m_puhWidth[uiAbsPartIdx]  >> 1) + (m_puhWidth[uiAbsPartIdx]  >> 2);
      }
      else
      {
        assert(0);
      }
      break;
    case SIZE_nRx2N:
      if ( uiPartIdx == 0 )
      {
        uiPUWidth = (m_puhWidth[uiAbsPartIdx]  >> 1) + (m_puhWidth[uiAbsPartIdx]  >> 2);
      }
      else if ( uiPartIdx == 1 )
      {
        uiPUWidth = m_puhWidth[uiAbsPartIdx]  >> 2;
      }
      else
      {
        assert(0);
      }
      break;
    default:
      assert (0);
      break;
  }

  ruiPartIdxRT = g_auiRasterToZscan [g_auiZscanToRaster[ ruiPartIdxLT ] + uiPUWidth / m_pcPic->getMinCUWidth() - 1 ];
}

Void TComDataCU::deriveLeftBottomIdxGeneral( UInt uiAbsPartIdx, UInt uiPartIdx, UInt& ruiPartIdxLB ) const
{
  UInt uiPUHeight = 0;
  switch ( m_pePartSize[uiAbsPartIdx] )
  {
    case SIZE_2Nx2N: uiPUHeight = m_puhHeight[uiAbsPartIdx];    break;
    case SIZE_2NxN:  uiPUHeight = m_puhHeight[uiAbsPartIdx] >> 1;    break;
    case SIZE_Nx2N:  uiPUHeight = m_puhHeight[uiAbsPartIdx];  break;
    case SIZE_NxN:   uiPUHeight = m_puhHeight[uiAbsPartIdx] >> 1;    break;
    case SIZE_2NxnU:
      if ( uiPartIdx == 0 )
      {
        uiPUHeight = m_puhHeight[uiAbsPartIdx] >> 2;
      }
      else if ( uiPartIdx == 1 )
      {
        uiPUHeight = (m_puhHeight[uiAbsPartIdx] >> 1) + (m_puhHeight[uiAbsPartIdx] >> 2);
      }
      else
      {
        assert(0);
      }
      break;
    case SIZE_2NxnD:
      if ( uiPartIdx == 0 )
      {
        uiPUHeight = (m_puhHeight[uiAbsPartIdx] >> 1) + (m_puhHeight[uiAbsPartIdx] >> 2);
      }
      else if ( uiPartIdx == 1 )
      {
        uiPUHeight = m_puhHeight[uiAbsPartIdx] >> 2;
      }
      else
      {
        assert(0);
      }
      break;
    case SIZE_nLx2N: uiPUHeight = m_puhHeight[uiAbsPartIdx];  break;
    case SIZE_nRx2N: uiPUHeight = m_puhHeight[uiAbsPartIdx];  break;
    default:
      assert (0);
      break;
  }

  ruiPartIdxLB      = g_auiRasterToZscan [g_auiZscanToRaster[ m_absZIdxInCtu + uiAbsPartIdx ] + ((uiPUHeight / m_pcPic->getMinCUHeight()) - 1)*m_pcPic->getNumPartInCtuWidth()];
}

Void TComDataCU::deriveLeftRightTopIdx ( UInt uiPartIdx, UInt& ruiPartIdxLT, UInt& ruiPartIdxRT ) const
{
  ruiPartIdxLT = m_absZIdxInCtu;
  ruiPartIdxRT = g_auiRasterToZscan [g_auiZscanToRaster[ ruiPartIdxLT ] + m_puhWidth[0] / m_pcPic->getMinCUWidth() - 1 ];

  switch ( m_pePartSize[0] )
  {
    case SIZE_2Nx2N:                                                                                                                                break;
    case SIZE_2NxN:
      ruiPartIdxLT += ( uiPartIdx == 0 )? 0 : m_uiNumPartition >> 1; ruiPartIdxRT += ( uiPartIdx == 0 )? 0 : m_uiNumPartition >> 1;
      break;
    case SIZE_Nx2N:
      ruiPartIdxLT += ( uiPartIdx == 0 )? 0 : m_uiNumPartition >> 2; ruiPartIdxRT -= ( uiPartIdx == 1 )? 0 : m_uiNumPartition >> 2;
      break;
    case SIZE_NxN:
      ruiPartIdxLT += ( m_uiNumPartition >> 2 ) * uiPartIdx;         ruiPartIdxRT +=  ( m_uiNumPartition >> 2 ) * ( uiPartIdx - 1 );
      break;
    case SIZE_2NxnU:
      ruiPartIdxLT += ( uiPartIdx == 0 )? 0 : m_uiNumPartition >> 3;
      ruiPartIdxRT += ( uiPartIdx == 0 )? 0 : m_uiNumPartition >> 3;
      break;
    case SIZE_2NxnD:
      ruiPartIdxLT += ( uiPartIdx == 0 )? 0 : ( m_uiNumPartition >> 1 ) + ( m_uiNumPartition >> 3 );
      ruiPartIdxRT += ( uiPartIdx == 0 )? 0 : ( m_uiNumPartition >> 1 ) + ( m_uiNumPartition >> 3 );
      break;
    case SIZE_nLx2N:
      ruiPartIdxLT += ( uiPartIdx == 0 )? 0 : m_uiNumPartition >> 4;
      ruiPartIdxRT -= ( uiPartIdx == 1 )? 0 : ( m_uiNumPartition >> 2 ) + ( m_uiNumPartition >> 4 );
      break;
    case SIZE_nRx2N:
      ruiPartIdxLT += ( uiPartIdx == 0 )? 0 : ( m_uiNumPartition >> 2 ) + ( m_uiNumPartition >> 4 );
      ruiPartIdxRT -= ( uiPartIdx == 1 )? 0 : m_uiNumPartition >> 4;
      break;
    default:
      assert (0);
      break;
  }

}

Void TComDataCU::deriveLeftBottomIdx( UInt  uiPartIdx,      UInt&      ruiPartIdxLB ) const
{
  ruiPartIdxLB      = g_auiRasterToZscan [g_auiZscanToRaster[ m_absZIdxInCtu ] + ( ((m_puhHeight[0] / m_pcPic->getMinCUHeight())>>1) - 1)*m_pcPic->getNumPartInCtuWidth()];

  switch ( m_pePartSize[0] )
  {
    case SIZE_2Nx2N:
      ruiPartIdxLB += m_uiNumPartition >> 1;
      break;
    case SIZE_2NxN:
      ruiPartIdxLB += ( uiPartIdx == 0 )? 0 : m_uiNumPartition >> 1;
      break;
    case SIZE_Nx2N:
      ruiPartIdxLB += ( uiPartIdx == 0 )? m_uiNumPartition >> 1 : (m_uiNumPartition >> 2)*3;
      break;
    case SIZE_NxN:
      ruiPartIdxLB += ( m_uiNumPartition >> 2 ) * uiPartIdx;
      break;
    case SIZE_2NxnU:
      ruiPartIdxLB += ( uiPartIdx == 0 ) ? -((Int)m_uiNumPartition >> 3) : m_uiNumPartition >> 1;
      break;
    case SIZE_2NxnD:
      ruiPartIdxLB += ( uiPartIdx == 0 ) ? (m_uiNumPartition >> 2) + (m_uiNumPartition >> 3): m_uiNumPartition >> 1;
      break;
    case SIZE_nLx2N:
      ruiPartIdxLB += ( uiPartIdx == 0 ) ? m_uiNumPartition >> 1 : (m_uiNumPartition >> 1) + (m_uiNumPartition >> 4);
      break;
    case SIZE_nRx2N:
      ruiPartIdxLB += ( uiPartIdx == 0 ) ? m_uiNumPartition >> 1 : (m_uiNumPartition >> 1) + (m_uiNumPartition >> 2) + (m_uiNumPartition >> 4);
      break;
    default:
      assert (0);
      break;
  }
}

/** Derive the partition index of neighbouring bottom right block
 * \param [in]  uiPartIdx     current partition index
 * \param [out] ruiPartIdxRB  partition index of neighbouring bottom right block
 */
Void TComDataCU::deriveRightBottomIdx( UInt uiPartIdx, UInt &ruiPartIdxRB ) const
{
  ruiPartIdxRB      = g_auiRasterToZscan [g_auiZscanToRaster[ m_absZIdxInCtu ] + ( ((m_puhHeight[0] / m_pcPic->getMinCUHeight())>>1) - 1)*m_pcPic->getNumPartInCtuWidth() +  m_puhWidth[0] / m_pcPic->getMinCUWidth() - 1];

  switch ( m_pePartSize[0] )
  {
    case SIZE_2Nx2N:
      ruiPartIdxRB += m_uiNumPartition >> 1;
      break;
    case SIZE_2NxN:
      ruiPartIdxRB += ( uiPartIdx == 0 )? 0 : m_uiNumPartition >> 1;
      break;
    case SIZE_Nx2N:
      ruiPartIdxRB += ( uiPartIdx == 0 )? m_uiNumPartition >> 2 : (m_uiNumPartition >> 1);
      break;
    case SIZE_NxN:
      ruiPartIdxRB += ( m_uiNumPartition >> 2 ) * ( uiPartIdx - 1 );
      break;
    case SIZE_2NxnU:
      ruiPartIdxRB += ( uiPartIdx == 0 ) ? -((Int)m_uiNumPartition >> 3) : m_uiNumPartition >> 1;
      break;
    case SIZE_2NxnD:
      ruiPartIdxRB += ( uiPartIdx == 0 ) ? (m_uiNumPartition >> 2) + (m_uiNumPartition >> 3): m_uiNumPartition >> 1;
      break;
    case SIZE_nLx2N:
      ruiPartIdxRB += ( uiPartIdx == 0 ) ? (m_uiNumPartition >> 3) + (m_uiNumPartition >> 4): m_uiNumPartition >> 1;
      break;
    case SIZE_nRx2N:
      ruiPartIdxRB += ( uiPartIdx == 0 ) ? (m_uiNumPartition >> 2) + (m_uiNumPartition >> 3) + (m_uiNumPartition >> 4) : m_uiNumPartition >> 1;
      break;
    default:
      assert (0);
      break;
  }
}

Bool TComDataCU::hasEqualMotion( UInt uiAbsPartIdx, const TComDataCU* pcCandCU, UInt uiCandAbsPartIdx ) const
{
  if ( getInterDir( uiAbsPartIdx ) != pcCandCU->getInterDir( uiCandAbsPartIdx ) )
  {
    return false;
  }

  for ( UInt uiRefListIdx = 0; uiRefListIdx < 2; uiRefListIdx++ )
  {
    if ( getInterDir( uiAbsPartIdx ) & ( 1 << uiRefListIdx ) )
    {
      if ( getCUMvField( RefPicList( uiRefListIdx ) )->getMv( uiAbsPartIdx )     != pcCandCU->getCUMvField( RefPicList( uiRefListIdx ) )->getMv( uiCandAbsPartIdx ) ||
        getCUMvField( RefPicList( uiRefListIdx ) )->getRefIdx( uiAbsPartIdx ) != pcCandCU->getCUMvField( RefPicList( uiRefListIdx ) )->getRefIdx( uiCandAbsPartIdx ) )
      {
        return false;
      }
    }
  }

  return true;
}

//! Construct a list of merging candidates
#if MCTS_ENC_CHECK
Void TComDataCU::getInterMergeCandidates( UInt uiAbsPartIdx, UInt uiPUIdx, TComMvField* pcMvFieldNeighbours, UChar* puhInterDirNeighbours, Int& numValidMergeCand, UInt &numSpatialMergeCandidates, Int mrgCandIdx ) const
#else
Void TComDataCU::getInterMergeCandidates( UInt uiAbsPartIdx, UInt uiPUIdx, TComMvField* pcMvFieldNeighbours, UChar* puhInterDirNeighbours, Int& numValidMergeCand, Int mrgCandIdx ) const
#endif
{
  UInt uiAbsPartAddr = m_absZIdxInCtu + uiAbsPartIdx;
  Bool abCandIsInter[ MRG_MAX_NUM_CANDS ];
  for( UInt ui = 0; ui < getSlice()->getMaxNumMergeCand(); ++ui )
  {
    abCandIsInter[ui] = false;
    pcMvFieldNeighbours[ ( ui << 1 )     ].setRefIdx(NOT_VALID);
    pcMvFieldNeighbours[ ( ui << 1 ) + 1 ].setRefIdx(NOT_VALID);
  }
  numValidMergeCand = getSlice()->getMaxNumMergeCand();
  // compute the location of the current PU
  Int xP, yP, nPSW, nPSH;
  this->getPartPosition(uiPUIdx, xP, yP, nPSW, nPSH);

  Int iCount = 0;

  UInt uiPartIdxLT, uiPartIdxRT, uiPartIdxLB;
  PartSize cCurPS = getPartitionSize( uiAbsPartIdx );
  deriveLeftRightTopIdxGeneral( uiAbsPartIdx, uiPUIdx, uiPartIdxLT, uiPartIdxRT );
  deriveLeftBottomIdxGeneral( uiAbsPartIdx, uiPUIdx, uiPartIdxLB );

  //left
  UInt uiLeftPartIdx = 0;
  const TComDataCU *pcCULeft = getPULeft( uiLeftPartIdx, uiPartIdxLB );

  Bool isAvailableA1 = pcCULeft &&
                       pcCULeft->isDiffMER(xP -1, yP+nPSH-1, xP, yP) &&
                       !( uiPUIdx == 1 && (cCurPS == SIZE_Nx2N || cCurPS == SIZE_nLx2N || cCurPS == SIZE_nRx2N) ) &&
                       pcCULeft->isInter( uiLeftPartIdx ) ;

  if ( isAvailableA1 )
  {
    abCandIsInter[iCount] = true;
    // get Inter Dir
    puhInterDirNeighbours[iCount] = pcCULeft->getInterDir( uiLeftPartIdx );
    // get Mv from Left
    TComDataCU::getMvField( pcCULeft, uiLeftPartIdx, REF_PIC_LIST_0, pcMvFieldNeighbours[iCount<<1] );
    if ( getSlice()->isInterB() )
    {
      TComDataCU::getMvField( pcCULeft, uiLeftPartIdx, REF_PIC_LIST_1, pcMvFieldNeighbours[(iCount<<1)+1] );
    }
    if ( mrgCandIdx == iCount )
    {
#if MCTS_ENC_CHECK
      numSpatialMergeCandidates = iCount + 1;
#endif
      return;
    }
    iCount ++;
  }

  // early termination
  if (iCount == getSlice()->getMaxNumMergeCand())
  {
#if MCTS_ENC_CHECK
    numSpatialMergeCandidates = iCount;
#endif
    return;
  }
  // above
  UInt uiAbovePartIdx = 0;
  const TComDataCU *pcCUAbove = getPUAbove( uiAbovePartIdx, uiPartIdxRT );

  Bool isAvailableB1 = pcCUAbove &&
                       pcCUAbove->isDiffMER(xP+nPSW-1, yP-1, xP, yP) &&
                       !( uiPUIdx == 1 && (cCurPS == SIZE_2NxN || cCurPS == SIZE_2NxnU || cCurPS == SIZE_2NxnD) ) &&
                       pcCUAbove->isInter( uiAbovePartIdx );

  if ( isAvailableB1 && (!isAvailableA1 || !pcCULeft->hasEqualMotion( uiLeftPartIdx, pcCUAbove, uiAbovePartIdx ) ) )
  {
    abCandIsInter[iCount] = true;
    // get Inter Dir
    puhInterDirNeighbours[iCount] = pcCUAbove->getInterDir( uiAbovePartIdx );
    // get Mv from Left
    TComDataCU::getMvField( pcCUAbove, uiAbovePartIdx, REF_PIC_LIST_0, pcMvFieldNeighbours[iCount<<1] );
    if ( getSlice()->isInterB() )
    {
      TComDataCU::getMvField( pcCUAbove, uiAbovePartIdx, REF_PIC_LIST_1, pcMvFieldNeighbours[(iCount<<1)+1] );
    }
    if ( mrgCandIdx == iCount )
    {
#if MCTS_ENC_CHECK
      numSpatialMergeCandidates = iCount + 1;
#endif
      return;
    }
    iCount ++;
  }
  // early termination
  if (iCount == getSlice()->getMaxNumMergeCand())
  {
#if MCTS_ENC_CHECK
    numSpatialMergeCandidates = iCount;
#endif
    return;
  }

  // above right
  UInt uiAboveRightPartIdx = 0;
  const TComDataCU *pcCUAboveRight = getPUAboveRight( uiAboveRightPartIdx, uiPartIdxRT );

  Bool isAvailableB0 = pcCUAboveRight &&
                       pcCUAboveRight->isDiffMER(xP+nPSW, yP-1, xP, yP) &&
                       pcCUAboveRight->isInter( uiAboveRightPartIdx );

  if ( isAvailableB0 && ( !isAvailableB1 || !pcCUAbove->hasEqualMotion( uiAbovePartIdx, pcCUAboveRight, uiAboveRightPartIdx ) ) )
  {
    abCandIsInter[iCount] = true;
    // get Inter Dir
    puhInterDirNeighbours[iCount] = pcCUAboveRight->getInterDir( uiAboveRightPartIdx );
    // get Mv from Left
    TComDataCU::getMvField( pcCUAboveRight, uiAboveRightPartIdx, REF_PIC_LIST_0, pcMvFieldNeighbours[iCount<<1] );
    if ( getSlice()->isInterB() )
    {
      TComDataCU::getMvField( pcCUAboveRight, uiAboveRightPartIdx, REF_PIC_LIST_1, pcMvFieldNeighbours[(iCount<<1)+1] );
    }
    if ( mrgCandIdx == iCount )
    {
#if MCTS_ENC_CHECK
      numSpatialMergeCandidates = iCount + 1;
#endif
      return;
    }
    iCount ++;
  }
  // early termination
  if (iCount == getSlice()->getMaxNumMergeCand())
  {
#if MCTS_ENC_CHECK
    numSpatialMergeCandidates = iCount;
#endif
    return;
  }

  //left bottom
  UInt uiLeftBottomPartIdx = 0;
  const TComDataCU *pcCULeftBottom = this->getPUBelowLeft( uiLeftBottomPartIdx, uiPartIdxLB );

  Bool isAvailableA0 = pcCULeftBottom &&
                       pcCULeftBottom->isDiffMER(xP-1, yP+nPSH, xP, yP) &&
                       pcCULeftBottom->isInter( uiLeftBottomPartIdx ) ;

  if ( isAvailableA0 && ( !isAvailableA1 || !pcCULeft->hasEqualMotion( uiLeftPartIdx, pcCULeftBottom, uiLeftBottomPartIdx ) ) )
  {
    abCandIsInter[iCount] = true;
    // get Inter Dir
    puhInterDirNeighbours[iCount] = pcCULeftBottom->getInterDir( uiLeftBottomPartIdx );
    // get Mv from Left
    TComDataCU::getMvField( pcCULeftBottom, uiLeftBottomPartIdx, REF_PIC_LIST_0, pcMvFieldNeighbours[iCount<<1] );
    if ( getSlice()->isInterB() )
    {
      TComDataCU::getMvField( pcCULeftBottom, uiLeftBottomPartIdx, REF_PIC_LIST_1, pcMvFieldNeighbours[(iCount<<1)+1] );
    }
    if ( mrgCandIdx == iCount )
    {
#if MCTS_ENC_CHECK
      numSpatialMergeCandidates = iCount + 1;
#endif
      return;
    }
    iCount ++;
  }
  // early termination
  if (iCount == getSlice()->getMaxNumMergeCand())
  {
#if MCTS_ENC_CHECK
    numSpatialMergeCandidates = iCount;
#endif
    return;
  }

  // above left
  if( iCount < 4 )
  {
    UInt uiAboveLeftPartIdx = 0;
    const TComDataCU *pcCUAboveLeft = getPUAboveLeft( uiAboveLeftPartIdx, uiAbsPartAddr );

    Bool isAvailableB2 = pcCUAboveLeft &&
                         pcCUAboveLeft->isDiffMER(xP-1, yP-1, xP, yP) &&
                         pcCUAboveLeft->isInter( uiAboveLeftPartIdx );

    if ( isAvailableB2 && ( !isAvailableA1 || !pcCULeft->hasEqualMotion( uiLeftPartIdx, pcCUAboveLeft, uiAboveLeftPartIdx ) )
        && ( !isAvailableB1 || !pcCUAbove->hasEqualMotion( uiAbovePartIdx, pcCUAboveLeft, uiAboveLeftPartIdx ) ) )
    {
      abCandIsInter[iCount] = true;
      // get Inter Dir
      puhInterDirNeighbours[iCount] = pcCUAboveLeft->getInterDir( uiAboveLeftPartIdx );
      // get Mv from Left
      TComDataCU::getMvField( pcCUAboveLeft, uiAboveLeftPartIdx, REF_PIC_LIST_0, pcMvFieldNeighbours[iCount<<1] );
      if ( getSlice()->isInterB() )
      {
        TComDataCU::getMvField( pcCUAboveLeft, uiAboveLeftPartIdx, REF_PIC_LIST_1, pcMvFieldNeighbours[(iCount<<1)+1] );
      }
      if ( mrgCandIdx == iCount )
      {
#if MCTS_ENC_CHECK
        numSpatialMergeCandidates = iCount + 1;
#endif
        return;
      }
      iCount ++;
    }
  }
  // early termination
  if (iCount == getSlice()->getMaxNumMergeCand())
  {
#if MCTS_ENC_CHECK
    numSpatialMergeCandidates = iCount;
#endif
    return;
  }
#if MCTS_ENC_CHECK
  numSpatialMergeCandidates = iCount;
#endif
  if ( getSlice()->getEnableTMVPFlag() )
  {
    //>> MTK colocated-RightBottom
    UInt uiPartIdxRB;

    deriveRightBottomIdx( uiPUIdx, uiPartIdxRB );

    UInt uiAbsPartIdxTmp = g_auiZscanToRaster[uiPartIdxRB];
    const UInt numPartInCtuWidth  = m_pcPic->getNumPartInCtuWidth();
    const UInt numPartInCtuHeight = m_pcPic->getNumPartInCtuHeight();

    TComMv cColMv;
    Int iRefIdx;
    Int ctuRsAddr = -1;

    if (   ( ( m_pcPic->getCtu(m_ctuRsAddr)->getCUPelX() + g_auiRasterToPelX[uiAbsPartIdxTmp] + m_pcPic->getMinCUWidth () ) < m_pcSlice->getSPS()->getPicWidthInLumaSamples () )  // image boundary check
        && ( ( m_pcPic->getCtu(m_ctuRsAddr)->getCUPelY() + g_auiRasterToPelY[uiAbsPartIdxTmp] + m_pcPic->getMinCUHeight() ) < m_pcSlice->getSPS()->getPicHeightInLumaSamples() ) )
    {
      if ( ( uiAbsPartIdxTmp % numPartInCtuWidth < numPartInCtuWidth - 1 ) &&           // is not at the last column of CTU
        ( uiAbsPartIdxTmp / numPartInCtuWidth < numPartInCtuHeight - 1 ) )              // is not at the last row    of CTU
      {
        uiAbsPartAddr = g_auiRasterToZscan[ uiAbsPartIdxTmp + numPartInCtuWidth + 1 ];
        ctuRsAddr = getCtuRsAddr();
      }
      else if ( uiAbsPartIdxTmp % numPartInCtuWidth < numPartInCtuWidth - 1 )           // is not at the last column of CTU But is last row of CTU
      {
        uiAbsPartAddr = g_auiRasterToZscan[ (uiAbsPartIdxTmp + numPartInCtuWidth + 1) % m_pcPic->getNumPartitionsInCtu() ];
      }
      else if ( uiAbsPartIdxTmp / numPartInCtuWidth < numPartInCtuHeight - 1 )          // is not at the last row of CTU But is last column of CTU
      {
        uiAbsPartAddr = g_auiRasterToZscan[ uiAbsPartIdxTmp + 1 ];
        ctuRsAddr = getCtuRsAddr() + 1;
      }
      else //is the right bottom corner of CTU
      {
        uiAbsPartAddr = 0;
      }
    }

    iRefIdx = 0;

    Bool bExistMV = false;
    UInt uiPartIdxCenter;
    Int dir = 0;
    UInt uiArrayAddr = iCount;
    xDeriveCenterIdx( uiPUIdx, uiPartIdxCenter );
    bExistMV = ctuRsAddr >= 0 && xGetColMVP( REF_PIC_LIST_0, ctuRsAddr, uiAbsPartAddr, cColMv, iRefIdx );
    if( bExistMV == false )
    {
      bExistMV = xGetColMVP( REF_PIC_LIST_0, getCtuRsAddr(), uiPartIdxCenter,  cColMv, iRefIdx );
    }
    if( bExistMV )
    {
      dir |= 1;
      pcMvFieldNeighbours[ 2 * uiArrayAddr ].setMvField( cColMv, iRefIdx );
    }

    if ( getSlice()->isInterB() )
    {
      bExistMV = ctuRsAddr >= 0 && xGetColMVP( REF_PIC_LIST_1, ctuRsAddr, uiAbsPartAddr, cColMv, iRefIdx);
      if( bExistMV == false )
      {
        bExistMV = xGetColMVP( REF_PIC_LIST_1, getCtuRsAddr(), uiPartIdxCenter, cColMv, iRefIdx );
      }
      if( bExistMV )
      {
        dir |= 2;
        pcMvFieldNeighbours[ 2 * uiArrayAddr + 1 ].setMvField( cColMv, iRefIdx );
      }
    }

    if (dir != 0)
    {
      puhInterDirNeighbours[uiArrayAddr] = dir;
      abCandIsInter[uiArrayAddr] = true;

      if ( mrgCandIdx == iCount )
      {
        return;
      }
      iCount++;
    }
  }
  // early termination
  if (iCount == getSlice()->getMaxNumMergeCand())
  {
    return;
  }

  UInt uiArrayAddr = iCount;
  UInt uiCutoff = uiArrayAddr;

  if ( getSlice()->isInterB() )
  {
    static const UInt NUM_PRIORITY_LIST=12;
    static const UInt uiPriorityList0[NUM_PRIORITY_LIST] = {0 , 1, 0, 2, 1, 2, 0, 3, 1, 3, 2, 3};
    static const UInt uiPriorityList1[NUM_PRIORITY_LIST] = {1 , 0, 2, 0, 2, 1, 3, 0, 3, 1, 3, 2};

    for (Int idx=0; idx<uiCutoff*(uiCutoff-1) && uiArrayAddr!= getSlice()->getMaxNumMergeCand(); idx++)
    {
      assert(idx<NUM_PRIORITY_LIST);
      Int i = uiPriorityList0[idx];
      Int j = uiPriorityList1[idx];
      if (abCandIsInter[i] && abCandIsInter[j]&& (puhInterDirNeighbours[i]&0x1)&&(puhInterDirNeighbours[j]&0x2))
      {
        abCandIsInter[uiArrayAddr] = true;
        puhInterDirNeighbours[uiArrayAddr] = 3;

        // get Mv from cand[i] and cand[j]
        pcMvFieldNeighbours[uiArrayAddr << 1].setMvField(pcMvFieldNeighbours[i<<1].getMv(), pcMvFieldNeighbours[i<<1].getRefIdx());
        pcMvFieldNeighbours[( uiArrayAddr << 1 ) + 1].setMvField(pcMvFieldNeighbours[(j<<1)+1].getMv(), pcMvFieldNeighbours[(j<<1)+1].getRefIdx());

        Int iRefPOCL0 = m_pcSlice->getRefPOC( REF_PIC_LIST_0, pcMvFieldNeighbours[(uiArrayAddr<<1)].getRefIdx() );
        Int iRefPOCL1 = m_pcSlice->getRefPOC( REF_PIC_LIST_1, pcMvFieldNeighbours[(uiArrayAddr<<1)+1].getRefIdx() );
        if (iRefPOCL0 == iRefPOCL1 && pcMvFieldNeighbours[(uiArrayAddr<<1)].getMv() == pcMvFieldNeighbours[(uiArrayAddr<<1)+1].getMv())
        {
          abCandIsInter[uiArrayAddr] = false;
        }
        else
        {
          uiArrayAddr++;
        }
      }
    }
  }
  // early termination
  if (uiArrayAddr == getSlice()->getMaxNumMergeCand())
  {
    return;
  }

  Int iNumRefIdx = (getSlice()->isInterB()) ? min(m_pcSlice->getNumRefIdx(REF_PIC_LIST_0), m_pcSlice->getNumRefIdx(REF_PIC_LIST_1)) : m_pcSlice->getNumRefIdx(REF_PIC_LIST_0);

  Int r = 0;
  Int refcnt = 0;
  while (uiArrayAddr < getSlice()->getMaxNumMergeCand())
  {
    abCandIsInter[uiArrayAddr] = true;
    puhInterDirNeighbours[uiArrayAddr] = 1;
    pcMvFieldNeighbours[uiArrayAddr << 1].setMvField( TComMv(0, 0), r);

    if ( getSlice()->isInterB() )
    {
      puhInterDirNeighbours[uiArrayAddr] = 3;
      pcMvFieldNeighbours[(uiArrayAddr << 1) + 1].setMvField(TComMv(0, 0), r);
    }
    uiArrayAddr++;

    if ( refcnt == iNumRefIdx - 1 )
    {
      r = 0;
    }
    else
    {
      ++r;
      ++refcnt;
    }
  }
  numValidMergeCand = uiArrayAddr;
}

/** Check whether the current PU and a spatial neighboring PU are in a same ME region.
 * \param xN, yN   location of the upper-left corner pixel of a neighboring PU
 * \param xP, yP   location of the upper-left corner pixel of the current PU
 */
Bool TComDataCU::isDiffMER(Int xN, Int yN, Int xP, Int yP) const
{

  UInt plevel = this->getSlice()->getPPS()->getLog2ParallelMergeLevelMinus2() + 2;
  if ((xN>>plevel)!= (xP>>plevel))
  {
    return true;
  }
  if ((yN>>plevel)!= (yP>>plevel))
  {
    return true;
  }
  return false;
}

/** Calculate the location of upper-left corner pixel and size of the current PU.
 * \param partIdx       PU index within a CU
 * \param xP, yP        location of the upper-left corner pixel of the current PU
 * \param nPSW, nPSH    size of the current PU
 */
Void TComDataCU::getPartPosition( UInt partIdx, Int& xP, Int& yP, Int& nPSW, Int& nPSH) const
{
  UInt col = m_uiCUPelX;
  UInt row = m_uiCUPelY;

  switch ( m_pePartSize[0] )
  {
  case SIZE_2NxN:
    nPSW = getWidth(0);
    nPSH = getHeight(0) >> 1;
    xP   = col;
    yP   = (partIdx ==0)? row: row + nPSH;
    break;
  case SIZE_Nx2N:
    nPSW = getWidth(0) >> 1;
    nPSH = getHeight(0);
    xP   = (partIdx ==0)? col: col + nPSW;
    yP   = row;
    break;
  case SIZE_NxN:
    nPSW = getWidth(0) >> 1;
    nPSH = getHeight(0) >> 1;
    xP   = col + (partIdx&0x1)*nPSW;
    yP   = row + (partIdx>>1)*nPSH;
    break;
  case SIZE_2NxnU:
    nPSW = getWidth(0);
    nPSH = ( partIdx == 0 ) ?  getHeight(0) >> 2 : ( getHeight(0) >> 2 ) + ( getHeight(0) >> 1 );
    xP   = col;
    yP   = (partIdx ==0)? row: row + getHeight(0) - nPSH;

    break;
  case SIZE_2NxnD:
    nPSW = getWidth(0);
    nPSH = ( partIdx == 0 ) ?  ( getHeight(0) >> 2 ) + ( getHeight(0) >> 1 ) : getHeight(0) >> 2;
    xP   = col;
    yP   = (partIdx ==0)? row: row + getHeight(0) - nPSH;
    break;
  case SIZE_nLx2N:
    nPSW = ( partIdx == 0 ) ? getWidth(0) >> 2 : ( getWidth(0) >> 2 ) + ( getWidth(0) >> 1 );
    nPSH = getHeight(0);
    xP   = (partIdx ==0)? col: col + getWidth(0) - nPSW;
    yP   = row;
    break;
  case SIZE_nRx2N:
    nPSW = ( partIdx == 0 ) ? ( getWidth(0) >> 2 ) + ( getWidth(0) >> 1 ) : getWidth(0) >> 2;
    nPSH = getHeight(0);
    xP   = (partIdx ==0)? col: col + getWidth(0) - nPSW;
    yP   = row;
    break;
  default:
    assert ( m_pePartSize[0] == SIZE_2Nx2N );
    nPSW = getWidth(0);
    nPSH = getHeight(0);
    xP   = col ;
    yP   = row ;

    break;
  }
}

/** Constructs a list of candidates for AMVP (See specification, section "Derivation process for motion vector predictor candidates")
 * \param uiPartIdx
 * \param uiPartAddr
 * \param eRefPicList
 * \param iRefIdx
 * \param pInfo
 */
Void TComDataCU::fillMvpCand ( const UInt partIdx, const UInt partAddr, const RefPicList eRefPicList, const Int refIdx, AMVPInfo* pInfo ) const
{
  pInfo->iN = 0;
  if (refIdx < 0)
  {
#if MCTS_ENC_CHECK
    pInfo->numSpatialMVPCandidates = 0;
#endif
    return;
  }

  //-- Get Spatial MV
  UInt partIdxLT, partIdxRT, partIdxLB;
  deriveLeftRightTopIdx( partIdx, partIdxLT, partIdxRT );
  deriveLeftBottomIdx( partIdx, partIdxLB );

  Bool isScaledFlagLX = false; /// variable name from specification; true when the PUs below left or left are available (availableA0 || availableA1).
  {
    UInt idx;
    const TComDataCU* tmpCU = getPUBelowLeft(idx, partIdxLB);
    isScaledFlagLX = (tmpCU != NULL) && (tmpCU->isInter(idx));
    if (!isScaledFlagLX)
    {
      tmpCU = getPULeft(idx, partIdxLB);
      isScaledFlagLX = (tmpCU != NULL) && (tmpCU->isInter(idx));
    }
  }

  // Left predictor search
  if (isScaledFlagLX)
  {
    Bool bAdded = xAddMVPCandUnscaled( *pInfo, eRefPicList, refIdx, partIdxLB, MD_BELOW_LEFT);
    if (!bAdded)
    {
      bAdded = xAddMVPCandUnscaled( *pInfo, eRefPicList, refIdx, partIdxLB, MD_LEFT );
      if(!bAdded)
      {
        bAdded = xAddMVPCandWithScaling( *pInfo, eRefPicList, refIdx, partIdxLB, MD_BELOW_LEFT);
        if (!bAdded)
        {
          xAddMVPCandWithScaling( *pInfo, eRefPicList, refIdx, partIdxLB, MD_LEFT );
        }
      }
    }
  }

  // Above predictor search
  {
    Bool bAdded = xAddMVPCandUnscaled( *pInfo, eRefPicList, refIdx, partIdxRT, MD_ABOVE_RIGHT);
    if (!bAdded)
    {
      bAdded = xAddMVPCandUnscaled( *pInfo, eRefPicList, refIdx, partIdxRT, MD_ABOVE);
      if(!bAdded)
      {
        xAddMVPCandUnscaled( *pInfo, eRefPicList, refIdx, partIdxLT, MD_ABOVE_LEFT);
      }
    }
  }

  if(!isScaledFlagLX)
  {
    Bool bAdded = xAddMVPCandWithScaling( *pInfo, eRefPicList, refIdx, partIdxRT, MD_ABOVE_RIGHT);
    if (!bAdded)
    {
      bAdded = xAddMVPCandWithScaling( *pInfo, eRefPicList, refIdx, partIdxRT, MD_ABOVE);
      if(!bAdded)
      {
        xAddMVPCandWithScaling( *pInfo, eRefPicList, refIdx, partIdxLT, MD_ABOVE_LEFT);
      }
    }
  }

  if ( pInfo->iN == 2 )
  {
    if ( pInfo->m_acMvCand[ 0 ] == pInfo->m_acMvCand[ 1 ] )
    {
      pInfo->iN = 1;
    }
  }
#if MCTS_ENC_CHECK
  pInfo->numSpatialMVPCandidates = pInfo->iN;
#endif
  if (pInfo->iN < AMVP_MAX_NUM_CANDS && getSlice()->getEnableTMVPFlag() )
  {
    // Get Temporal Motion Predictor
    const UInt numPartInCtuWidth  = m_pcPic->getNumPartInCtuWidth();
    const UInt numPartInCtuHeight = m_pcPic->getNumPartInCtuHeight();
    const Int refIdx_Col = refIdx;
    TComMv cColMv;
    UInt partIdxRB;
    UInt absPartIdx;

    deriveRightBottomIdx( partIdx, partIdxRB );
    UInt absPartAddr = m_absZIdxInCtu + partAddr;

    //----  co-located RightBottom Temporal Predictor (H) ---//
    absPartIdx = g_auiZscanToRaster[partIdxRB];
    Int ctuRsAddr = -1;
    if (  ( ( m_pcPic->getCtu(m_ctuRsAddr)->getCUPelX() + g_auiRasterToPelX[absPartIdx] + m_pcPic->getMinCUWidth () ) < m_pcSlice->getSPS()->getPicWidthInLumaSamples () )  // image boundary check
       && ( ( m_pcPic->getCtu(m_ctuRsAddr)->getCUPelY() + g_auiRasterToPelY[absPartIdx] + m_pcPic->getMinCUHeight() ) < m_pcSlice->getSPS()->getPicHeightInLumaSamples() ) )
    {
      if ( ( absPartIdx % numPartInCtuWidth < numPartInCtuWidth - 1 ) &&  // is not at the last column of CTU
           ( absPartIdx / numPartInCtuWidth < numPartInCtuHeight - 1 ) )  // is not at the last row    of CTU
      {
        absPartAddr = g_auiRasterToZscan[ absPartIdx + numPartInCtuWidth + 1 ];
        ctuRsAddr = getCtuRsAddr();
      }
      else if ( absPartIdx % numPartInCtuWidth < numPartInCtuWidth - 1 )  // is not at the last column of CTU But is last row of CTU
      {
        absPartAddr = g_auiRasterToZscan[ (absPartIdx + numPartInCtuWidth + 1) % m_pcPic->getNumPartitionsInCtu() ];
      }
      else if ( absPartIdx / numPartInCtuWidth < numPartInCtuHeight - 1 ) // is not at the last row of CTU But is last column of CTU
      {
        absPartAddr = g_auiRasterToZscan[ absPartIdx + 1 ];
        ctuRsAddr = getCtuRsAddr() + 1;
      }
      else //is the right bottom corner of CTU
      {
        absPartAddr = 0;
      }
    }
    if ( ctuRsAddr >= 0 && xGetColMVP( eRefPicList, ctuRsAddr, absPartAddr, cColMv, refIdx_Col ) )
    {
      pInfo->m_acMvCand[pInfo->iN++] = cColMv;
    }
    else
    {
      UInt uiPartIdxCenter;
      xDeriveCenterIdx( partIdx, uiPartIdxCenter );
      if (xGetColMVP( eRefPicList, getCtuRsAddr(), uiPartIdxCenter,  cColMv, refIdx_Col ))
      {
        pInfo->m_acMvCand[pInfo->iN++] = cColMv;
      }
    }
    //----  co-located RightBottom Temporal Predictor  ---//
  }

  while (pInfo->iN < AMVP_MAX_NUM_CANDS)
  {
    pInfo->m_acMvCand[pInfo->iN].set(0,0);
    pInfo->iN++;
  }
  return ;
}


Bool TComDataCU::isBipredRestriction(UInt puIdx) const
{
  Int width = 0;
  Int height = 0;
  UInt partAddr;

  getPartIndexAndSize( puIdx, partAddr, width, height );
  if ( getWidth(0) == 8 && (width < 8 || height < 8) )
  {
    return true;
  }
  return false;
}


Void TComDataCU::clipMv    (TComMv&  rcMv) const
{
  const TComSPS &sps=*(m_pcSlice->getSPS());
  Int  iMvShift = 2;
  Int iOffset = 8;
  Int iHorMax = ( sps.getPicWidthInLumaSamples() + iOffset - (Int)m_uiCUPelX - 1 ) << iMvShift;
  Int iHorMin = (      -(Int)sps.getMaxCUWidth() - iOffset - (Int)m_uiCUPelX + 1 ) << iMvShift;

  Int iVerMax = ( sps.getPicHeightInLumaSamples() + iOffset - (Int)m_uiCUPelY - 1 ) << iMvShift;
  Int iVerMin = (      -(Int)sps.getMaxCUHeight() - iOffset - (Int)m_uiCUPelY + 1 ) << iMvShift;

  rcMv.setHor( min (iHorMax, max (iHorMin, rcMv.getHor())) );
  rcMv.setVer( min (iVerMax, max (iVerMin, rcMv.getVer())) );
}


UInt TComDataCU::getIntraSizeIdx(UInt uiAbsPartIdx) const
{
  UInt uiShift = ( m_pePartSize[uiAbsPartIdx]==SIZE_NxN ? 1 : 0 );

  UChar uiWidth = m_puhWidth[uiAbsPartIdx]>>uiShift;
  UInt  uiCnt = 0;
  while( uiWidth )
  {
    uiCnt++;
    uiWidth>>=1;
  }
  uiCnt-=2;
  return uiCnt > 6 ? 6 : uiCnt;
}

Void TComDataCU::clearCbf( UInt uiIdx, ComponentID compID, UInt uiNumParts )
{
  memset( &m_puhCbf[compID][uiIdx], 0, sizeof(UChar)*uiNumParts);
}

/** Set a I_PCM flag for all sub-partitions of a partition.
 * \param bIpcmFlag I_PCM flag
 * \param uiAbsPartIdx patition index
 * \param uiDepth CU depth
 * \returns Void
 */
Void TComDataCU::setIPCMFlagSubParts  (Bool bIpcmFlag, UInt uiAbsPartIdx, UInt uiDepth)
{
  UInt uiCurrPartNumb = m_pcPic->getNumPartitionsInCtu() >> (uiDepth << 1);

  memset(m_pbIPCMFlag + uiAbsPartIdx, bIpcmFlag, sizeof(Bool)*uiCurrPartNumb );
}

/** Test whether the block at uiPartIdx is skipped.
 * \param uiPartIdx Partition index
 * \returns true if the current the block is skipped
 */
Bool TComDataCU::isSkipped( UInt uiPartIdx ) const
{
  return ( getSkipFlag( uiPartIdx ) );
}

// ====================================================================================================================
// Protected member functions
// ====================================================================================================================

Bool TComDataCU::xAddMVPCandUnscaled( AMVPInfo &info, const RefPicList eRefPicList, const Int iRefIdx, const UInt uiPartUnitIdx, const MVP_DIR eDir ) const
{
  const TComDataCU* neibCU = NULL;
  UInt neibPUPartIdx;
  switch( eDir )
  {
    case MD_LEFT:
    {
      neibCU = getPULeft(neibPUPartIdx, uiPartUnitIdx);
      break;
    }
    case MD_ABOVE:
    {
      neibCU = getPUAbove(neibPUPartIdx, uiPartUnitIdx);
      break;
    }
    case MD_ABOVE_RIGHT:
    {
      neibCU = getPUAboveRight(neibPUPartIdx, uiPartUnitIdx);
      break;
    }
    case MD_BELOW_LEFT:
    {
      neibCU = getPUBelowLeft(neibPUPartIdx, uiPartUnitIdx);
      break;
    }
    case MD_ABOVE_LEFT:
    {
      neibCU = getPUAboveLeft(neibPUPartIdx, uiPartUnitIdx);
      break;
    }
    default:
    {
      break;
    }
  }

  if ( neibCU == NULL )
  {
    return false;
  }

  const Int        currRefPOC     = m_pcSlice->getRefPic( eRefPicList, iRefIdx)->getPOC();
  const RefPicList eRefPicList2nd = (eRefPicList == REF_PIC_LIST_0) ? REF_PIC_LIST_1 : REF_PIC_LIST_0;

  for(Int predictorSource=0; predictorSource<2; predictorSource++) // examine the indicated reference picture list, then if not available, examine the other list.
  {
    const RefPicList eRefPicListIndex = (predictorSource==0) ? eRefPicList : eRefPicList2nd;
    const Int        neibRefIdx       = neibCU->getCUMvField(eRefPicListIndex)->getRefIdx(neibPUPartIdx);

    if ( neibRefIdx >= 0 && currRefPOC == neibCU->getSlice()->getRefPOC( eRefPicListIndex, neibRefIdx ))
    {
      info.m_acMvCand[info.iN++] = neibCU->getCUMvField(eRefPicListIndex)->getMv(neibPUPartIdx);
      return true;
    }
  }

  return false;
}

/**
 * \param pInfo
 * \param eRefPicList
 * \param iRefIdx
 * \param uiPartUnitIdx
 * \param eDir
 * \returns Bool
 */
Bool TComDataCU::xAddMVPCandWithScaling( AMVPInfo &info, const RefPicList eRefPicList, const Int iRefIdx, const UInt uiPartUnitIdx, const MVP_DIR eDir ) const
{
  const TComDataCU* neibCU = NULL;
  UInt neibPUPartIdx;
  switch( eDir )
  {
  case MD_LEFT:
    {
      neibCU = getPULeft(neibPUPartIdx, uiPartUnitIdx);
      break;
    }
  case MD_ABOVE:
    {
      neibCU = getPUAbove(neibPUPartIdx, uiPartUnitIdx);
      break;
    }
  case MD_ABOVE_RIGHT:
    {
      neibCU = getPUAboveRight(neibPUPartIdx, uiPartUnitIdx);
      break;
    }
  case MD_BELOW_LEFT:
    {
      neibCU = getPUBelowLeft(neibPUPartIdx, uiPartUnitIdx);
      break;
    }
  case MD_ABOVE_LEFT:
    {
      neibCU = getPUAboveLeft(neibPUPartIdx, uiPartUnitIdx);
      break;
    }
  default:
    {
      break;
    }
  }

  if ( neibCU == NULL )
  {
    return false;
  }

  const RefPicList eRefPicList2nd = (eRefPicList == REF_PIC_LIST_0) ? REF_PIC_LIST_1 : REF_PIC_LIST_0;

  const Int  currPOC            = m_pcSlice->getPOC();
  const Int  currRefPOC         = m_pcSlice->getRefPic( eRefPicList, iRefIdx)->getPOC();
  const Bool bIsCurrRefLongTerm = m_pcSlice->getRefPic( eRefPicList, iRefIdx)->getIsLongTerm();
  const Int  neibPOC            = currPOC;

  for(Int predictorSource=0; predictorSource<2; predictorSource++) // examine the indicated reference picture list, then if not available, examine the other list.
  {
    const RefPicList eRefPicListIndex = (predictorSource==0) ? eRefPicList : eRefPicList2nd;
    const Int        neibRefIdx       = neibCU->getCUMvField(eRefPicListIndex)->getRefIdx(neibPUPartIdx);
    if( neibRefIdx >= 0)
    {
      const Bool bIsNeibRefLongTerm = neibCU->getSlice()->getRefPic( eRefPicListIndex, neibRefIdx )->getIsLongTerm();

      if ( bIsCurrRefLongTerm == bIsNeibRefLongTerm )
      {
        const TComMv &cMvPred = neibCU->getCUMvField(eRefPicListIndex)->getMv(neibPUPartIdx);
        TComMv rcMv;
        if ( bIsCurrRefLongTerm /* || bIsNeibRefLongTerm*/ )
        {
          rcMv = cMvPred;
        }
        else
        {
          const Int neibRefPOC = neibCU->getSlice()->getRefPOC( eRefPicListIndex, neibRefIdx );
          const Int scale      = xGetDistScaleFactor( currPOC, currRefPOC, neibPOC, neibRefPOC );
          if ( scale == 4096 )
          {
            rcMv = cMvPred;
          }
          else
          {
            rcMv = cMvPred.scaleMv( scale );
          }
        }

        info.m_acMvCand[info.iN++] = rcMv;
        return true;
      }
    }
  }
  return false;
}

Bool TComDataCU::xGetColMVP( const RefPicList eRefPicList, const Int ctuRsAddr, const Int partUnitIdx, TComMv& rcMv, const Int refIdx ) const
{
  const UInt absPartAddr = partUnitIdx;

  // use coldir.
  const TComPic    * const pColPic = getSlice()->getRefPic( RefPicList(getSlice()->isInterB() ? 1-getSlice()->getColFromL0Flag() : 0), getSlice()->getColRefIdx());
#if REDUCED_ENCODER_MEMORY
  if (!pColPic->getPicSym()->hasDPBPerCtuData())
  {
    return false;
  }
  const TComPicSym::DPBPerCtuData * const pColDpbCtu = &(pColPic->getPicSym()->getDPBPerCtuData(ctuRsAddr));
  const TComSlice * const pColSlice = pColDpbCtu->getSlice();
  if(pColDpbCtu->getPartitionSize(partUnitIdx)==NUMBER_OF_PART_SIZES)
#else
  const TComDataCU * const pColCtu = pColPic->getCtu( ctuRsAddr );
  if(pColCtu->getPic()==0 || pColCtu->getPartitionSize(partUnitIdx)==NUMBER_OF_PART_SIZES)
#endif
  {
    return false;
  }

#if REDUCED_ENCODER_MEMORY
  if (!pColDpbCtu->isInter(absPartAddr))
#else
  if (!pColCtu->isInter(absPartAddr))
#endif
  {
    return false;
  }

  RefPicList eColRefPicList = getSlice()->getCheckLDC() ? eRefPicList : RefPicList(getSlice()->getColFromL0Flag());
#if REDUCED_ENCODER_MEMORY
  Int iColRefIdx            = pColDpbCtu->getCUMvField(RefPicList(eColRefPicList))->getRefIdx(absPartAddr);
#else
  Int iColRefIdx            = pColCtu->getCUMvField(RefPicList(eColRefPicList))->getRefIdx(absPartAddr);
#endif

  if (iColRefIdx < 0 )
  {
    eColRefPicList = RefPicList(1 - eColRefPicList);
#if REDUCED_ENCODER_MEMORY
    iColRefIdx = pColDpbCtu->getCUMvField(RefPicList(eColRefPicList))->getRefIdx(absPartAddr);
#else
    iColRefIdx = pColCtu->getCUMvField(RefPicList(eColRefPicList))->getRefIdx(absPartAddr);
#endif

    if (iColRefIdx < 0 )
    {
      return false;
    }
  }

  const Bool bIsCurrRefLongTerm = m_pcSlice->getRefPic(eRefPicList, refIdx)->getIsLongTerm();
#if REDUCED_ENCODER_MEMORY
  const Bool bIsColRefLongTerm  = pColSlice->getIsUsedAsLongTerm(eColRefPicList, iColRefIdx);
#else
  const Bool bIsColRefLongTerm  = pColCtu->getSlice()->getIsUsedAsLongTerm(eColRefPicList, iColRefIdx);
#endif

  if ( bIsCurrRefLongTerm != bIsColRefLongTerm )
  {
    return false;
  }

  // Scale the vector.
#if REDUCED_ENCODER_MEMORY
  const TComMv &cColMv = pColDpbCtu->getCUMvField(eColRefPicList)->getMv(absPartAddr);
#else
  const TComMv &cColMv = pColCtu->getCUMvField(eColRefPicList)->getMv(absPartAddr);
#endif
  if ( bIsCurrRefLongTerm /*|| bIsColRefLongTerm*/ )
  {
    rcMv = cColMv;
  }
  else
  {
    const Int currPOC    = m_pcSlice->getPOC();
#if REDUCED_ENCODER_MEMORY
    const Int colPOC     = pColSlice->getPOC();
    const Int colRefPOC  = pColSlice->getRefPOC(eColRefPicList, iColRefIdx);
#else
    const Int colPOC     = pColCtu->getSlice()->getPOC();
    const Int colRefPOC  = pColCtu->getSlice()->getRefPOC(eColRefPicList, iColRefIdx);
#endif
    const Int currRefPOC = m_pcSlice->getRefPic(eRefPicList, refIdx)->getPOC();
    const Int scale      = xGetDistScaleFactor(currPOC, currRefPOC, colPOC, colRefPOC);
    if ( scale == 4096 )
    {
      rcMv = cColMv;
    }
    else
    {
      rcMv = cColMv.scaleMv( scale );
    }
  }

  return true;
}

// Static member
Int TComDataCU::xGetDistScaleFactor(Int iCurrPOC, Int iCurrRefPOC, Int iColPOC, Int iColRefPOC)
{
  Int iDiffPocD = iColPOC - iColRefPOC;
  Int iDiffPocB = iCurrPOC - iCurrRefPOC;

  if( iDiffPocD == iDiffPocB )
  {
    return 4096;
  }
  else
  {
    Int iTDB      = Clip3( -128, 127, iDiffPocB );
    Int iTDD      = Clip3( -128, 127, iDiffPocD );
    Int iX        = (0x4000 + abs(iTDD/2)) / iTDD;
    Int iScale    = Clip3( -4096, 4095, (iTDB * iX + 32) >> 6 );
    return iScale;
  }
}

Void TComDataCU::xDeriveCenterIdx( UInt uiPartIdx, UInt& ruiPartIdxCenter ) const
{
  UInt uiPartAddr;
  Int  iPartWidth;
  Int  iPartHeight;
  getPartIndexAndSize( uiPartIdx, uiPartAddr, iPartWidth, iPartHeight);

  ruiPartIdxCenter = m_absZIdxInCtu+uiPartAddr; // partition origin.
  ruiPartIdxCenter = g_auiRasterToZscan[ g_auiZscanToRaster[ ruiPartIdxCenter ]
                                        + ( iPartHeight/m_pcPic->getMinCUHeight()  )/2*m_pcPic->getNumPartInCtuWidth()
                                        + ( iPartWidth/m_pcPic->getMinCUWidth()  )/2];
}

Void TComDataCU::compressMV()
{
#if REDUCED_ENCODER_MEMORY
  const Int scaleFactor = std::max<Int>(1,4 * AMVP_DECIMATION_FACTOR / m_unitSize);
  TComPicSym &picSym=*(getPic()->getPicSym());
  TComPicSym::DPBPerCtuData &dpbForCtu=picSym.getDPBPerCtuData(getCtuRsAddr());

  for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
  {
    dpbForCtu.m_CUMvField[i].compress(dpbForCtu.m_pePredMode, m_pePredMode, scaleFactor,m_acCUMvField[i]);
    memcpy(dpbForCtu.m_pePartSize, m_pePartSize, sizeof(*m_pePartSize)*m_uiNumPartition);
    dpbForCtu.m_pSlice = getSlice();
  }
#else
  Int scaleFactor = 4 * AMVP_DECIMATION_FACTOR / m_unitSize;
  if (scaleFactor > 0)
  {
    for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
    {
      m_acCUMvField[i].compress(m_pePredMode, scaleFactor);
    }
  }
#endif
}

UInt TComDataCU::getCoefScanIdx(const UInt uiAbsPartIdx, const UInt uiWidth, const UInt uiHeight, const ComponentID compID) const
{
  //------------------------------------------------

  //this mechanism is available for intra only

  if (!isIntra(uiAbsPartIdx))
  {
    return SCAN_DIAG;
  }

  //------------------------------------------------

  //check that MDCS can be used for this TU

  const ChromaFormat format = getPic()->getChromaFormat();

  const UInt maximumWidth  = MDCS_MAXIMUM_WIDTH  >> getComponentScaleX(compID, format);
  const UInt maximumHeight = MDCS_MAXIMUM_HEIGHT >> getComponentScaleY(compID, format);

  if ((uiWidth > maximumWidth) || (uiHeight > maximumHeight))
  {
    return SCAN_DIAG;
  }

  //------------------------------------------------

  //otherwise, select the appropriate mode

  UInt uiDirMode  = getIntraDir(toChannelType(compID), uiAbsPartIdx);

  if (uiDirMode==DM_CHROMA_IDX)
  {
    const TComSPS *sps=getSlice()->getSPS();
    const UInt partsPerMinCU = 1<<(2*(sps->getMaxTotalCUDepth() - sps->getLog2DiffMaxMinCodingBlockSize()));
    uiDirMode = getIntraDir(CHANNEL_TYPE_LUMA, getChromasCorrespondingPULumaIdx(uiAbsPartIdx, getPic()->getChromaFormat(), partsPerMinCU));
  }

  if (isChroma(compID) && (format == CHROMA_422))
  {
    uiDirMode = g_chroma422IntraAngleMappingTable[uiDirMode];
  }

  //------------------

  if      (abs((Int)uiDirMode - VER_IDX) <= MDCS_ANGLE_LIMIT)
  {
    return SCAN_HOR;
  }
  else if (abs((Int)uiDirMode - HOR_IDX) <= MDCS_ANGLE_LIMIT)
  {
    return SCAN_VER;
  }
  else
  {
    return SCAN_DIAG;
  }
}

#if MCTS_ENC_CHECK
Bool TComDataCU::isLastColumnCTUInTile() const
{
  UInt      currentTileIdx               = this->getPic()->getPicSym()->getTileIdxMap(this->getCtuRsAddr());
  TComTile *pCurrentTile                 = m_pcPic->getPicSym()->getTComTile(currentTileIdx);
  UInt      frameWidthInCtus             = m_pcPic->getPicSym()->getFrameWidthInCtus();
  UInt      rightEdgeCTUPosInCurrentTile = pCurrentTile->getRightEdgePosInCtus();
  UInt      ctuXPosInCtus                = this->getCtuRsAddr() % frameWidthInCtus;
  
  return (rightEdgeCTUPosInCurrentTile == ctuXPosInCtus);
}
#endif

//! \}
