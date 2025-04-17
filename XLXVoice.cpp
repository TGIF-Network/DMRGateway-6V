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
#include "XLXVoice.h"
#include "Log.h"
#include "DMRDefines.h"
#include "DMRData.h"

// Temporary definitions if DMRDefines.h lacks them
#ifndef DMR_FRAME_TYPE_HEADER
#define DMR_FRAME_TYPE_HEADER     0x00U
#define DMR_FRAME_TYPE_TERMINATOR 0x01U
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

CXLXVoice::CXLXVoice(const std::string& directory, const std::string& language, unsigned int id, unsigned int slot, unsigned int tg) :
m_directory(directory),
m_language(language),
m_id(id),
m_slot(slot),
m_tg(tg),
m_queue(),
m_queueLength(0U),
m_timer(1000U, 2U),
m_busy(false),
m_header(NULL),
m_terminator(NULL)
{
    assert(!directory.empty());
    assert(!language.empty());
}

CXLXVoice::~CXLXVoice()
{
    for (std::vector<unsigned char*>::iterator it = m_queue.begin(); it != m_queue.end(); ++it)
        delete[] *it;
    m_queue.clear();

    delete[] m_header;
    delete[] m_terminator;
}

bool CXLXVoice::open()
{
    // Placeholder for voice file loading, assuming .ambe files in m_directory
    LogInfo("CXLXVoice: initialized for directory=%s, language=%s, id=%u, slot=%u, tg=%u", m_directory.c_str(), m_language.c_str(), m_id, m_slot, m_tg);
    createHeaderTerminator(DMR_FRAME_TYPE_HEADER);
    return true;
}

void CXLXVoice::linkedTo(unsigned int number, unsigned int room)
{
    std::vector<std::string> words;
    words.push_back("linked");
    words.push_back("to");

    char numberStr[10U];
    ::sprintf(numberStr, "%u", number);
    for (unsigned int i = 0U; numberStr[i] != '\0'; i++) {
        switch (numberStr[i]) {
            case '0': words.push_back("zero"); break;
            case '1': words.push_back("one"); break;
            case '2': words.push_back("two"); break;
            case '3': words.push_back("three"); break;
            case '4': words.push_back("four"); break;
            case '5': words.push_back("five"); break;
            case '6': words.push_back("six"); break;
            case '7': words.push_back("seven"); break;
            case '8': words.push_back("eight"); break;
            case '9': words.push_back("nine"); break;
            default: break;
        }
    }

    words.push_back("module");
    char roomStr[10U];
    ::sprintf(roomStr, "%u", room);
    for (unsigned int i = 0U; roomStr[i] != '\0'; i++) {
        switch (roomStr[i]) {
            case '0': words.push_back("zero"); break;
            case '1': words.push_back("one"); break;
            case '2': words.push_back("two"); break;
            case '3': words.push_back("three"); break;
            case '4': words.push_back("four"); break;
            case '5': words.push_back("five"); break;
            case '6': words.push_back("six"); break;
            case '7': words.push_back("seven"); break;
            case '8': words.push_back("eight"); break;
            case '9': words.push_back("nine"); break;
            default: break;
        }
    }

    createVoice(words);
}
//void CXLXVoice::linkedToNetwork(unsigned int number, unsigned int room)
bool CXLXVoice::linkedToNetwork(const std::string& networkName, unsigned int tg, CDMRData& data)
{
    std::vector<std::string> words;
    words.push_back("linked");
    words.push_back("to");

    char numberStr[10U];
    ::sprintf(numberStr, "%u", number);
    for (unsigned int i = 0U; numberStr[i] != '\0'; i++) {
        switch (numberStr[i]) {
            case '0': words.push_back("zero"); break;
            case '1': words.push_back("one"); break;
            case '2': words.push_back("two"); break;
            case '3': words.push_back("three"); break;
            case '4': words.push_back("four"); break;
            case '5': words.push_back("five"); break;
            case '6': words.push_back("six"); break;
            case '7': words.push_back("seven"); break;
            case '8': words.push_back("eight"); break;
            case '9': words.push_back("nine"); break;
            default: break;
        }
    }

    words.push_back("module");
    char roomStr[10U];
    ::sprintf(roomStr, "%u", room);
    for (unsigned int i = 0U; roomStr[i] != '\0'; i++) {
        switch (roomStr[i]) {
            case '0': words.push_back("zero"); break;
            case '1': words.push_back("one"); break;
            case '2': words.push_back("two"); break;
            case '3': words.push_back("three"); break;
            case '4': words.push_back("four"); break;
            case '5': words.push_back("five"); break;
            case '6': words.push_back("six"); break;
            case '7': words.push_back("seven"); break;
            case '8': words.push_back("eight"); break;
            case '9': words.push_back("nine"); break;
            default: break;
        }
    }

    createVoice(words);
}

void CXLXVoice::unlinked()
{
    std::vector<std::string> words;
    words.push_back("not");
    words.push_back("linked");
    createVoice(words);
}

void CXLXVoice::announceTG(unsigned int tg)
{
    char letters[10U];
    ::sprintf(letters, "%u", tg);

    std::vector<std::string> words;
    words.push_back("talkgroup");

    for (unsigned int i = 0U; letters[i] != '\0'; i++) {
        switch (letters[i]) {
            case '0': words.push_back("zero"); break;
            case '1': words.push_back("one"); break;
            case '2': words.push_back("two"); break;
            case '3': words.push_back("three"); break;
            case '4': words.push_back("four"); break;
            case '5': words.push_back("five"); break;
            case '6': words.push_back("six"); break;
            case '7': words.push_back("seven"); break;
            case '8': words.push_back("eight"); break;
            case '9': words.push_back("nine"); break;
            default: break;
        }
    }

    createVoice(words);
}

void CXLXVoice::reset()
{
    m_busy = false;
    m_timer.stop();

    for (std::vector<unsigned char*>::iterator it = m_queue.begin(); it != m_queue.end(); ++it)
        delete[] *it;
    m_queue.clear();
    m_queueLength = 0U;
}

bool CXLXVoice::read(CDMRData& data)
{
    if (!m_busy)
        return false;

    if (m_queue.empty()) {
        m_busy = false;
        createHeaderTerminator(DMR_FRAME_TYPE_TERMINATOR);
        data.setData(m_terminator);
        data.setSlotNo(m_slot);
        data.setDstId(m_tg);
        data.setSrcId(m_id);
        data.setFLCO(FLCO_GROUP);
        return true;
    }

    unsigned char* buffer = m_queue.front();
    m_queue.erase(m_queue.begin());
    m_queueLength--;

    data.setData(buffer);
    data.setSlotNo(m_slot);
    data.setDstId(m_tg);
    data.setSrcId(m_id);
    data.setFLCO(FLCO_GROUP);

    delete[] buffer;
    return true;
}

void CXLXVoice::clock(unsigned int ms)
{
    if (m_busy)
        m_timer.clock(ms);
}

void CXLXVoice::createHeaderTerminator(unsigned char type)
{
    unsigned char* buffer = new unsigned char[DMR_FRAME_LENGTH_BYTES];
    ::memset(buffer, 0U, DMR_FRAME_LENGTH_BYTES);

    if (type == DMR_FRAME_TYPE_HEADER) {
        // Simplified header creation (adjust as needed)
        buffer[0U] = DMR_FRAME_TYPE_HEADER;
        m_header = buffer;
    } else {
        // Simplified terminator creation
        buffer[0U] = DMR_FRAME_TYPE_TERMINATOR;
        m_terminator = buffer;
    }
}

void CXLXVoice::createVoice(const std::vector<std::string>& words)
{
    if (m_busy)
        return;

    m_busy = true;
    m_timer.start();

    // Placeholder for voice frame creation
    for (std::vector<std::string>::const_iterator it = words.begin(); it != words.end(); ++it) {
        std::string word = *it;
        std::string filename = m_directory + "/" + m_language + "/" + word + ".ambe";
        // Simulate reading .ambe file (implementation depends on actual file handling)
        unsigned char* buffer = new unsigned char[DMR_FRAME_LENGTH_BYTES];
        ::memset(buffer, 0U, DMR_FRAME_LENGTH_BYTES);
        // Fill buffer with dummy data for now
        buffer[0U] = 0xAA; // Example data
        m_queue.push_back(buffer);
        m_queueLength++;
    }

    if (m_header != NULL) {
        unsigned char* buffer = new unsigned char[DMR_FRAME_LENGTH_BYTES];
        ::memcpy(buffer, m_header, DMR_FRAME_LENGTH_BYTES);
        m_queue.insert(m_queue.begin(), buffer);
        m_queueLength++;
    }
}
