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

/** \file     TAppEncTop.h
    \brief    Encoder application class (header)
*/

#ifndef __TAPPENCTOP__
#define __TAPPENCTOP__

#include <list>
#include <ostream>

#include "TLibEncoder/TEncTop.h"
#include "Utilities/TVideoIOYuv.h"
#include "TLibCommon/AccessUnit.h"
#include "TAppEncCfg.h"

//! \ingroup TAppEncoder
//! \{

// ====================================================================================================================
// Class definition
// ====================================================================================================================

/// encoder application class
class TAppEncTop : public TAppEncCfg
{
private:
  // class interface
  TEncTop                    m_cTEncTop;                    ///< encoder class
  TVideoIOYuv                m_cTVideoIOYuvInputFile;       ///< input YUV file
  TVideoIOYuv                m_cTVideoIOYuvReconFile;       ///< output reconstruction file
#if SHUTTER_INTERVAL_SEI_PROCESSING
  TVideoIOYuv                m_cTVideoIOYuvSIIPreFile;      ///< output pre-filtered file
#endif

  TComList<TComPicYuv*>      m_cListPicYuvRec;              ///< list of reconstruction YUV files

  Int                        m_iFrameRcvd;                  ///< number of received frames

  UInt m_essentialBytes;
  UInt m_totalBytes;

protected:
  // initialization
  Void  xCreateLib        ();                               ///< create files & encoder class
  Void  xInitLibCfg       ();                               ///< initialize internal variables
  Void  xInitLib          (Bool isFieldCoding);             ///< initialize encoder class
  Void  xDestroyLib       ();                               ///< destroy encoder class

  /// obtain required buffers
  Void xGetBuffer(TComPicYuv*& rpcPicYuvRec);

  /// delete allocated buffers
  Void  xDeleteBuffer     ();

  // file I/O
  Void xWriteOutput(std::ostream& bitstreamFile, Int iNumEncoded, const std::list<AccessUnit>& accessUnits); ///< write bitstream to file
  Void rateStatsAccum(const AccessUnit& au, const std::vector<UInt>& stats);
  Void printRateSummary();
  Void printChromaFormat();

public:
  TAppEncTop();
  virtual ~TAppEncTop();

  Void        encode      ();                               ///< main encoding function
  TEncTop&    getTEncTop  ()   { return  m_cTEncTop; }      ///< return encoder class pointer reference

};// END CLASS DEFINITION TAppEncTop

//! \}

#endif // __TAPPENCTOP__

