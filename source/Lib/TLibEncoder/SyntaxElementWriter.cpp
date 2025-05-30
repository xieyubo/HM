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

/** \file     SyntaxElementWriter.cpp
    \brief    CAVLC encoder class
*/

#include "TLibCommon/CommonDef.h"
#include "SyntaxElementWriter.h"

//! \ingroup TLibEncoder
//! \{

#if ENC_DEC_TRACE

Void  SyntaxElementWriter::xWriteSCodeTr (Int value, UInt  length, const TChar *pSymbolName)
{
  xWriteSCode (value,length);
  if( g_HLSTraceEnable )
  {
    fprintf( g_hTrace, "%8lld  ", g_nSymbolCounter++ );
    if( length<10 )
    {
      fprintf( g_hTrace, "%-50s u(%d)  : %d\n", pSymbolName, length, value );
    }
    else
    {
      fprintf( g_hTrace, "%-50s u(%d) : %d\n", pSymbolName, length, value );
    }
  }
}

Void  SyntaxElementWriter::xWriteCodeTr (UInt value, UInt  length, const TChar *pSymbolName)
{
  xWriteCode (value,length);
  if( g_HLSTraceEnable )
  {
    fprintf( g_hTrace, "%8lld  ", g_nSymbolCounter++ );
    if( length<10 )
    {
      fprintf( g_hTrace, "%-50s u(%d)  : %d\n", pSymbolName, length, value );
    }
    else
    {
      fprintf( g_hTrace, "%-50s u(%d) : %d\n", pSymbolName, length, value );
    }
  }
}

Void  SyntaxElementWriter::xWriteUvlcTr (UInt value, const TChar *pSymbolName)
{
  xWriteUvlc (value);
  if( g_HLSTraceEnable )
  {
    fprintf( g_hTrace, "%8lld  ", g_nSymbolCounter++ );
    fprintf( g_hTrace, "%-50s ue(v) : %d\n", pSymbolName, value );
  }
}

Void  SyntaxElementWriter::xWriteSvlcTr (Int value, const TChar *pSymbolName)
{
  xWriteSvlc(value);
  if( g_HLSTraceEnable )
  {
    fprintf( g_hTrace, "%8lld  ", g_nSymbolCounter++ );
    fprintf( g_hTrace, "%-50s se(v) : %d\n", pSymbolName, value );
  }
}

Void  SyntaxElementWriter::xWriteFlagTr(UInt value, const TChar *pSymbolName)
{
  xWriteFlag(value);
  if( g_HLSTraceEnable )
  {
    fprintf( g_hTrace, "%8lld  ", g_nSymbolCounter++ );
    fprintf( g_hTrace, "%-50s u(1)  : %d\n", pSymbolName, value );
  }
}

void  SyntaxElementWriter::xWriteStringTr( const std::string &value, const char *symbolName )
{
  xWriteString(value);
  if (g_HLSTraceEnable)
  {
    fprintf( g_hTrace, "%8lld  ", g_nSymbolCounter++ );
    fprintf( g_hTrace, "%-50s st(v)  : %s\n", symbolName, value.c_str() );
  }
}
#endif

Void SyntaxElementWriter::xWriteSCode    ( Int iCode, UInt uiLength )
{
  assert ( uiLength > 0 && uiLength<=32 );
  assert( uiLength==32 || (iCode>=-(1<<(uiLength-1)) && iCode<(1<<(uiLength-1))) );
  m_pcBitIf->write( uiLength==32 ? UInt(iCode) : ( UInt(iCode)&((1<<uiLength)-1) ), uiLength );
}

Void SyntaxElementWriter::xWriteCode     ( UInt uiCode, UInt uiLength )
{
  assert ( uiLength > 0 );
  m_pcBitIf->write( uiCode, uiLength );
}

Void SyntaxElementWriter::xWriteUvlc     ( UInt uiCode )
{
  UInt uiLength = 1;
  UInt uiTemp = ++uiCode;

  assert ( uiTemp );

  while( 1 != uiTemp )
  {
    uiTemp >>= 1;
    uiLength += 2;
  }
  // Take care of cases where uiLength > 32
  m_pcBitIf->write( 0, uiLength >> 1);
  m_pcBitIf->write( uiCode, (uiLength+1) >> 1);
}

Void SyntaxElementWriter::xWriteSvlc     ( Int iCode )
{
  UInt uiCode;

  uiCode = xConvertToUInt( iCode );
  xWriteUvlc( uiCode );
}

Void SyntaxElementWriter::xWriteFlag( UInt uiCode )
{
  m_pcBitIf->write( uiCode, 1 );
}

void  SyntaxElementWriter::xWriteString( const std::string &value )
{
  for (int i = 0; i < value.length(); ++i)
  {
    m_pcBitIf->write(value[i], 8);
  }
  m_pcBitIf->write('\0', 8);
}

Void SyntaxElementWriter::xWriteRbspTrailingBits()
{
  WRITE_FLAG( 1, "rbsp_stop_one_bit");
  Int cnt = 0;
  while (m_pcBitIf->getNumBitsUntilByteAligned())
  {
    WRITE_FLAG( 0, "rbsp_alignment_zero_bit");
    cnt++;
  }
  assert(cnt<8);
}

//! \}
