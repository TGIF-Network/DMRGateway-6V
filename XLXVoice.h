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

#include <string>
#include <vector>

#include "DMRData.h"
#include "Timer.h"

class CXLXVoice {
public:
    CXLXVoice(const std::string& directory, const std::string& language, unsigned int id, unsigned int slot, unsigned int tg);
    virtual ~CXLXVoice();
    bool open();
//    void linkedTo(unsigned int number, unsigned int room);
//    void unlinked();
    void announceTG(unsigned int tg);
    void reset();
    bool read(CDMRData& data);
    void clock(unsigned int ms);
    bool linkedToNetwork(const std::string& networkName, unsigned int tg, CDMRData& data);
    bool unlinkedNetwork(const std::string& networkName, CDMRData& data);

bool isValid() const;
    bool linkedTo(const std::string& number, unsigned int reflector, CDMRData& data);
    bool unlinked(CDMRData& data);


private:
    std::string m_directory;
    std::string m_language;
    unsigned int m_id;
    unsigned int m_slot;
    unsigned int m_tg;
    std::vector<unsigned char*> m_queue;
    unsigned int m_queueLength;
    CTimer m_timer;
    bool m_busy;
    unsigned char* m_header;
    unsigned char* m_terminator;

    void createHeaderTerminator(unsigned char type);
    void createVoice(const std::vector<std::string>& words);
};

#endif
