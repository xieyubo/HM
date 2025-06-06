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

/**
 \file     NALread.cpp
 \brief    reading functionality for NAL units
 */


#include <vector>
#include <algorithm>
#include <ostream>

#include "NALread.h"
#include "TLibCommon/NAL.h"
#include "TLibCommon/TComBitStream.h"
#if RExt__DECODER_DEBUG_BIT_STATISTICS
#include "TLibCommon/TComCodingStatistics.h"
#endif
#if ENC_DEC_TRACE && DEC_NUH_TRACE
#include "TLibCommon/TComRom.h"
#endif

using namespace std;

//! \ingroup TLibDecoder
//! \{
static Void convertPayloadToRBSP(vector<uint8_t>& nalUnitBuf, TComInputBitstream *bitstream, Bool isVclNalUnit)
{
  UInt zeroCount = 0;
  vector<uint8_t>::iterator it_read, it_write;

  UInt pos = 0;
  bitstream->clearEmulationPreventionByteLocation();
  for (it_read = it_write = nalUnitBuf.begin(); it_read != nalUnitBuf.end(); it_read++, it_write++, pos++)
  {
    assert(zeroCount < 2 || *it_read >= 0x03);
    if (zeroCount == 2 && *it_read == 0x03)
    {
      bitstream->pushEmulationPreventionByteLocation( pos );
      pos++;
      it_read++;
      zeroCount = 0;
#if RExt__DECODER_DEBUG_BIT_STATISTICS
      TComCodingStatistics::IncrementStatisticEP(STATS__EMULATION_PREVENTION_3_BYTES, 8, 0);
#endif
      if (it_read == nalUnitBuf.end())
      {
        break;
      }
      assert(*it_read <= 0x03);
    }
    zeroCount = (*it_read == 0x00) ? zeroCount+1 : 0;
    *it_write = *it_read;
  }
  assert(zeroCount == 0);

  if (isVclNalUnit)
  {
    // Remove cabac_zero_word from payload if present
    Int n = 0;

    while (it_write[-1] == 0x00)
    {
      it_write--;
      n++;
    }

    if (n > 0)
    {
      printf("\nDetected %d instances of cabac_zero_word\n", n/2);
    }
  }

  nalUnitBuf.resize(it_write - nalUnitBuf.begin());
}

#if ENC_DEC_TRACE && DEC_NUH_TRACE
void xTraceNalUnitHeader(InputNALUnit& nalu)
{
  fprintf( g_hTrace, "*********** NAL UNIT (%s) ***********\n", nalUnitTypeToString(nalu.m_nalUnitType) );

  fprintf( g_hTrace, "%8lld  ", g_nSymbolCounter++ );
  fprintf( g_hTrace, "%-50s u(%d)  : %u\n", "forbidden_zero_bit", 1, 0 ); 

  fprintf( g_hTrace, "%8lld  ", g_nSymbolCounter++ );
  fprintf( g_hTrace, "%-50s u(%d)  : %u\n", "nal_unit_type", 6, nalu.m_nalUnitType ); 

  fprintf( g_hTrace, "%8lld  ", g_nSymbolCounter++ );
  fprintf( g_hTrace, "%-50s u(%d)  : %u\n", "nuh_layer_id", 6, nalu.m_nuhLayerId );

  fprintf( g_hTrace, "%8lld  ", g_nSymbolCounter++ );
  fprintf( g_hTrace, "%-50s u(%d)  : %u\n", "nuh_temporal_id_plus1", 3, nalu.m_temporalId + 1 );

  fflush ( g_hTrace );
}
#endif

Void readNalUnitHeader(InputNALUnit& nalu)
{
  TComInputBitstream& bs = nalu.getBitstream();

  Bool forbidden_zero_bit = bs.read(1);           // forbidden_zero_bit
  assert(forbidden_zero_bit == 0);
  nalu.m_nalUnitType = (NalUnitType) bs.read(6);  // nal_unit_type
  nalu.m_nuhLayerId = bs.read(6);                 // nuh_layer_id
  nalu.m_temporalId = bs.read(3) - 1;             // nuh_temporal_id_plus1
#if RExt__DECODER_DEBUG_BIT_STATISTICS
  TComCodingStatistics::IncrementStatisticEP(STATS__NAL_UNIT_HEADER_BITS, 1+6+6+3, 0);
#endif

#if ENC_DEC_TRACE && DEC_NUH_TRACE
  xTraceNalUnitHeader(nalu);
#endif

  // only check these rules for base layer
  if (nalu.m_nuhLayerId == 0)
  {
    if ( nalu.m_temporalId )
    {
      assert( nalu.m_nalUnitType != NAL_UNIT_CODED_SLICE_BLA_W_LP
           && nalu.m_nalUnitType != NAL_UNIT_CODED_SLICE_BLA_W_RADL
           && nalu.m_nalUnitType != NAL_UNIT_CODED_SLICE_BLA_N_LP
           && nalu.m_nalUnitType != NAL_UNIT_CODED_SLICE_IDR_W_RADL
           && nalu.m_nalUnitType != NAL_UNIT_CODED_SLICE_IDR_N_LP
           && nalu.m_nalUnitType != NAL_UNIT_CODED_SLICE_CRA
           && nalu.m_nalUnitType != NAL_UNIT_VPS
           && nalu.m_nalUnitType != NAL_UNIT_SPS
           && nalu.m_nalUnitType != NAL_UNIT_EOS
           && nalu.m_nalUnitType != NAL_UNIT_EOB );
    }
    else
    {
      assert( nalu.m_nalUnitType != NAL_UNIT_CODED_SLICE_TSA_R
           && nalu.m_nalUnitType != NAL_UNIT_CODED_SLICE_TSA_N
           && nalu.m_nalUnitType != NAL_UNIT_CODED_SLICE_STSA_R
           && nalu.m_nalUnitType != NAL_UNIT_CODED_SLICE_STSA_N );
    }
  }
}
/**
 * create a NALunit structure with given header values and storage for
 * a bitstream
 */
Void read(InputNALUnit& nalu)
{
  TComInputBitstream &bitstream = nalu.getBitstream();
  vector<uint8_t>& nalUnitBuf=bitstream.getFifo();
  // perform anti-emulation prevention
  const bool isVCLNalUnit = ((nalUnitBuf[0] & 64) == 0);
#if JVET_AK0194_DSC_SEI
  const NalUnitType nut = (NalUnitType)((nalUnitBuf[0] & 127) >> 1);
  if (nut == NAL_UNIT_VPS || nut == NAL_UNIT_SPS || nut == NAL_UNIT_PPS || isVCLNalUnit )
  {
    bitstream.copyToOrigFifo();
  }
#endif
  convertPayloadToRBSP(nalUnitBuf, &bitstream, isVCLNalUnit);
  bitstream.resetToStart();
  readNalUnitHeader(nalu);
}
//! \}
