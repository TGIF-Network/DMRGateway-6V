/*
*   Copyright (C) 2017,2020 by Jonathan Naylor G4KLX
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

#ifndef XLXVoice_H
#define XLXVoice_H

#include "DMRData.h"
#include "DMREmbeddedData.h"
#include "Timer.h"

#include <string>

class CXLXVoice {
public:
    CXLXVoice(const std::string& directory, const std::string& language, unsigned int id, unsigned int slot, unsigned int tg);
    ~CXLXVoice();
    bool open();
    bool linked(CDMRData& data, const std::string& callsign, unsigned int number, unsigned int reflector);
    bool unlinked(CDMRData& data);
    void announce(const std::string& mode, const std::string& network, unsigned int tg);
    void clock(unsigned int ms);
private:
    std::string      m_directory;
    std::string      m_language;
    unsigned int     m_id;
    unsigned int     m_slot;
    unsigned int     m_tg;
    CDMRData         m_dmr;
    CDMREmbeddedData m_embedded;
    CTimer           m_timer;
    unsigned char*   m_wavData;
    unsigned int     m_wavLength;
    unsigned int     m_pos;
    unsigned int     m_seqNo;
    unsigned int     m_dstId;
    bool             m_sent;
    void addData(unsigned char dataType);
};

#endif
