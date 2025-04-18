/*
 *   Copyright (C) 2015,2016,2017,2019,2020 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef DMRGateway_H
   #define DMRGateway_H

   #include "MMDVMNetwork.h"
   #include "DMRNetwork.h"
   #include "Conf.h"
   #include "XLXVoice.h"
   #include "RewriteTG.h"
   #include "Timer.h"

   #include <string>

   enum DMRGW_STATUS {
       DMRGWS_NONE,
       DMRGWS_DMRNETWORK1,
       DMRGWS_DMRNETWORK2,
       DMRGWS_DMRNETWORK3,
       DMRGWS_DMRNETWORK4,
       DMRGWS_DMRNETWORK5,
       DMRGWS_XLXREFLECTOR
   };

   class CDMRGateway {
   public:
       CDMRGateway(const std::string& confFile);
       ~CDMRGateway();
       int run();
       void buildNetworkStatusString(std::string& replyStr);
   private:
       CConf           m_conf;
       CMMDVMNetwork*  m_repeater;
       CDMRNetwork*    m_dmrNetwork1;
       CDMRNetwork*    m_dmrNetwork2;
       CDMRNetwork*    m_dmrNetwork3;
       CDMRNetwork*    m_dmrNetwork4;
       CDMRNetwork*    m_dmrNetwork5;
       CDMRNetwork*    m_xlxNetwork;
       unsigned int    m_xlxId;
       unsigned int    m_xlxNumber;
       unsigned int    m_xlxReflector;
       unsigned int    m_xlxRoom;
       unsigned int    m_xlxSlot;
       unsigned int    m_xlxTG;
       unsigned int    m_xlxBase;
       unsigned int    m_xlxStartup;
       bool            m_xlxConnected;
       CTimer          m_xlxRelink;
       bool            m_xlxUserControl;
       CRewriteTG*     m_xlxRewrite;
       CXLXVoice*      m_xlxVoice;
       DMRGW_STATUS*   m_status;

       bool linkXLX(unsigned int number);
       void unlinkXLX();
       void writeXLXLink(unsigned int sourceId, unsigned int destId, CDMRNetwork* network);
   };

   #endif
