/*
 *   Copyright (C) 2015-2021 by Jonathan Naylor G4KLX
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
#include <unistd.h>
//#include <sys/types.h>

#include "RewriteType.h"
#include "DMRSlotType.h"
#include "RewriteSrc.h"
#include "DMRGateway.h"
#include "StopWatch.h"
#include "RewritePC.h"
#include "RewriteSrcId.h"
#include "RewriteDstId.h"
#include "PassAllPC.h"
#include "PassAllTG.h"
#include "DMRFullLC.h"
#include "Version.h"
#include "Thread.h"
#include "DMRLC.h"
#include "Sync.h"
#include "Log.h"
#include "GitVersion.h"
#include "./SimpleIni.h"  // for ini

#include <cstdio> 
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <fstream>
#include<iostream>

#include "XLXVoice.h"
#include "DMREMB.h"

#include <cstring>
#include <cstdlib>

#include <sys/stat.h>

const unsigned char SILENCE[] = {0xACU, 0xAAU, 0x40U, 0x20U, 0x00U, 0x44U, 0x40U, 0x80U, 0x80U};

const unsigned char COLOR_CODE = 3U;

const unsigned int SILENCE_LENGTH = 9U;
const unsigned int AMBE_LENGTH = 9U;


CXLXVoice::CXLXVoice(const std::string& directory, const std::string& language, unsigned int id, unsigned int slot, unsigned int tg) :
m_indxFile(),
m_ambeFile(),
m_slot(slot),
m_lc(FLCO_GROUP, id, tg),
m_embeddedLC(),
m_status(XLXVS_NONE),
m_timer(1000U, 1U),
m_stopWatch(),
m_seqNo(0U),
m_streamId(0U),
m_sent(0U),
m_ambe(NULL),
m_positions(),
m_data(),
m_it()
{
        m_embeddedLC.setLC(m_lc);

#if defined(_WIN32) || defined(_WIN64)
        m_indxFile = directory + "\\" + language + ".indx";
        m_ambeFile = directory + "\\" + language + ".ambe";
#else
        m_indxFile = directory + "/" + language + ".indx";
        m_ambeFile = directory + "/" + language + ".ambe";
#endif
}

CXLXVoice::~CXLXVoice()
{
        for (std::vector<CDMRData*>::iterator it = m_data.begin(); it != m_data.end(); ++it)
                delete *it;

        for (std::unordered_map<std::string, CXLXPositions*>::iterator it = m_positions.begin(); it != m_positions.end(); ++it)
                delete it->second;

        m_data.clear();
        m_positions.clear();

        delete[] m_ambe;
}

bool CXLXVoice::open()
{
        FILE* fpindx = ::fopen(m_indxFile.c_str(), "rt");
        if (fpindx == NULL) {
                LogError("Unable to open the index file - %s", m_indxFile.c_str());
                return false;
        }

        struct stat statStruct;
        int ret = ::stat(m_ambeFile.c_str(), &statStruct);
        if (ret != 0) {
                LogError("Unable to stat the AMBE file - %s", m_ambeFile.c_str());
                ::fclose(fpindx);
                return false;
        }

        FILE* fpambe = ::fopen(m_ambeFile.c_str(), "rb");
        if (fpambe == NULL) {
                LogError("Unable to open the AMBE file - %s", m_ambeFile.c_str());
                ::fclose(fpindx);
                return false;
        }

        m_ambe = new unsigned char[statStruct.st_size];

        size_t sizeRead = ::fread(m_ambe, 1U, statStruct.st_size, fpambe);
        if (sizeRead != 0U) {
                char buffer[80U];
                while (::fgets(buffer, 80, fpindx) != NULL) {
                        char* p1 = ::strtok(buffer, "\t\r\n");
                        char* p2 = ::strtok(NULL, "\t\r\n");
                        char* p3 = ::strtok(NULL, "\t\r\n");

                        if (p1 != NULL && p2 != NULL && p3 != NULL) {
                                std::string symbol  = std::string(p1);
                                unsigned int start  = ::atoi(p2) * AMBE_LENGTH;
                                unsigned int length = ::atoi(p3) * AMBE_LENGTH;

                                CXLXPositions* pos = new CXLXPositions;
                                pos->m_start = start;
                                pos->m_length = length;

                                m_positions[symbol] = pos;
                        }
                }
        }

        ::fclose(fpindx);
        ::fclose(fpambe);

        return true;
}

void CXLXVoice::linkedTo(unsigned int number, unsigned int room)
{
        char letters[10U];
        ::sprintf(letters, "%03u", number);

        std::vector<std::string> words;
        if (m_positions.count("linkedto") == 0U) {
                words.push_back("linked");
                words.push_back("2");
        } else {
                words.push_back("linkedto");
        }
        words.push_back("X");
        words.push_back("L");
        words.push_back("X");
        words.push_back(std::string(1U, letters[0U]));
        words.push_back(std::string(1U, letters[1U]));
        words.push_back(std::string(1U, letters[2U]));

        // 4001 => 1 => A, 4002 => 2 => B, etc.
        room %= 100U;

        if (room >= 1U && room <= 26U)
                words.push_back(std::string(1U, 'A' + room - 1U));

        createVoice(words);
}

void CXLXVoice::unlinked()
{
        std::vector<std::string> words;
        words.push_back("notlinked");

        createVoice(words);
}

void CXLXVoice::announceTG(unsigned int tg)
{
        char letters[10U];
        ::sprintf(letters, "%u", tg); // Convert TG to string

        std::vector<std::string> words;
        words.push_back("talkgroup"); // Optional: Announce "talk group"

        // Convert each digit to a word (e.g., "3" -> "three")
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

void CXLXVoice::createVoice(const std::vector<std::string>& words)
{
        unsigned int ambeLength = 0U;
        for (std::vector<std::string>::const_iterator it = words.begin(); it != words.end(); ++it) {
                if (m_positions.count(*it) > 0U) {
                        CXLXPositions* position = m_positions.at(*it);
                        ambeLength += position->m_length;
                } else {
                        LogWarning("Unable to find character/phrase \"%s\" in the index", (*it).c_str());
                }
        }

        // Ensure that the AMBE is an integer number of DMR frames
        if ((ambeLength % (3U * AMBE_LENGTH)) != 0U) {
                unsigned int frames = ambeLength / (3U * AMBE_LENGTH);
                frames++;
                ambeLength = frames * (3U * AMBE_LENGTH);
        }

        // Add space for silence before and after the voice
        ambeLength += SILENCE_LENGTH * AMBE_LENGTH;
        ambeLength += SILENCE_LENGTH * AMBE_LENGTH;

        unsigned char* ambeData = new unsigned char[ambeLength];

        // Fill the AMBE data with silence
        for (unsigned int i = 0U; i < ambeLength; i += AMBE_LENGTH)
                ::memcpy(ambeData + i, SILENCE, AMBE_LENGTH);

        // Put offset in for silence at the beginning
        unsigned int pos = SILENCE_LENGTH * AMBE_LENGTH;
        for (std::vector<std::string>::const_iterator it = words.begin(); it != words.end(); ++it) {
                if (m_positions.count(*it) > 0U) {
                        CXLXPositions* position = m_positions.at(*it);
                        unsigned int start = position->m_start;
                        unsigned int length = position->m_length;
                        ::memcpy(ambeData + pos, m_ambe + start, length);
                        pos += length;
                }
        }

        for (std::vector<CDMRData*>::iterator it = m_data.begin(); it != m_data.end(); ++it)
                delete *it;

        m_data.clear();

        m_streamId = ::rand() + 1U;
        m_seqNo = 0U;

        createHeaderTerminator(DT_VOICE_LC_HEADER);
        createHeaderTerminator(DT_VOICE_LC_HEADER);
        createHeaderTerminator(DT_VOICE_LC_HEADER);

        unsigned char buffer[DMR_FRAME_LENGTH_BYTES];

        unsigned int n = 0U;
        for (unsigned int i = 0U; i < ambeLength; i += (3U * AMBE_LENGTH)) {
                unsigned char* p = ambeData + i;

                CDMRData* data = new CDMRData;

                data->setSlotNo(m_slot);
                data->setFLCO(FLCO_GROUP);
                data->setSrcId(m_lc.getSrcId());
                data->setDstId(m_lc.getDstId());
                data->setN(n);
                data->setSeqNo(m_seqNo++);
                data->setStreamId(m_streamId);

                ::memcpy(buffer + 0U, p + 0U, AMBE_LENGTH);
                ::memcpy(buffer + 9U, p + 9U, AMBE_LENGTH);
                ::memcpy(buffer + 15U, p + 9U, AMBE_LENGTH);
                ::memcpy(buffer + 24U, p + 18U, AMBE_LENGTH);

                if (n == 0U) {
                        CSync::addDMRAudioSync(buffer, true);
                        data->setDataType(DT_VOICE_SYNC);
                } else {
                        unsigned char lcss = m_embeddedLC.getData(buffer, n);

                        CDMREMB emb;
                        emb.setColorCode(COLOR_CODE);
                        emb.setPI(false);
                        emb.setLCSS(lcss);
                        emb.getData(buffer);

                        data->setDataType(DT_VOICE);
                }

                n++;
                if (n >= 6U)
                        n = 0U;

                data->setData(buffer);

                m_data.push_back(data);
        }

        createHeaderTerminator(DT_TERMINATOR_WITH_LC);
        createHeaderTerminator(DT_TERMINATOR_WITH_LC);

        delete[] ambeData;

        m_status = XLXVS_WAITING;
        m_timer.start();
}

void CXLXVoice::reset()
{
        for (std::vector<CDMRData*>::iterator it = m_data.begin(); it != m_data.end(); ++it)
                delete *it;

        m_timer.stop();
        m_status = XLXVS_NONE;
        m_data.clear();
        m_seqNo = 0U;
        m_streamId = 0U;
        m_sent = 0U;
}

bool CXLXVoice::read(CDMRData& data)
{
        if (m_status != XLXVS_SENDING)
                return false;

        unsigned int count = m_stopWatch.elapsed() / DMR_SLOT_TIME;

        if (m_sent < count) {
                data = *(*m_it);

                ++m_sent;
                ++m_it;

                if (m_it == m_data.end()) {
                        for (std::vector<CDMRData*>::iterator it = m_data.begin(); it != m_data.end(); ++it)
                                delete *it;
                        m_data.clear();
                        m_timer.stop();
                        m_status = XLXVS_NONE;
                }

                return true;
        }

        return false;
}

void CXLXVoice::clock(unsigned int ms)
{
        m_timer.clock(ms);
        if (m_timer.isRunning() && m_timer.hasExpired()) {
                if (m_status == XLXVS_WAITING) {
                        m_stopWatch.start();
                        m_status = XLXVS_SENDING;
                        m_it = m_data.begin();
                        m_sent = 0U;
                }
        }
}

void CXLXVoice::createHeaderTerminator(unsigned char type)
{
        CDMRData* data = new CDMRData;

        data->setSlotNo(m_slot);
        data->setFLCO(FLCO_GROUP);
        data->setSrcId(m_lc.getSrcId());
        data->setDstId(m_lc.getDstId());
        data->setDataType(type);
        data->setN(0U);
        data->setSeqNo(m_seqNo++);
        data->setStreamId(m_streamId);

        unsigned char buffer[DMR_FRAME_LENGTH_BYTES];

        CDMRFullLC fullLC;
        fullLC.encode(m_lc, buffer, type);

        CDMRSlotType slotType;
        slotType.setColorCode(COLOR_CODE);
        slotType.setDataType(type);
        slotType.getData(buffer);

        CSync::addDMRDataSync(buffer, true);

        data->setData(buffer);

        m_data.push_back(data);
}




#if !defined(_WIN32) && !defined(_WIN64)
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <pwd.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
const char* DEFAULT_INI_FILE = "DMRGateway.ini";
#else
const char* DEFAULT_INI_FILE = "/etc/DMRGateway.ini";
#endif


const unsigned int XLX_SLOT = 2U;
const unsigned int XLX_TG   = 9U;


unsigned int rdstId = 0;

unsigned short selnet = 4;

unsigned int  storedtg =0;
static bool net1ok = false;
static bool net2ok = false;
static bool net3ok = false;
static bool net4ok = false;
static bool net5ok = false;
static bool net6ok = false;
//static bool allnetok = false;

static bool rf1ok = false;
static bool rf2ok = false;
static bool rf3ok = false;
static bool rf4ok = false;
static bool rf5ok = false;
static bool rf6ok = false;
static bool ok2tx = false;
static int  locknet = 4;


void ClearRFNets();
void ClearNetworks();
void SetDMR();
int Reload();
void SetUserMMDVM();
void SetUserRoot();

 unsigned short int GWMode = 8; 

static bool m_killed = false;
static int  m_signal = 0;

#if !defined(_WIN32) && !defined(_WIN64)
static void sigHandler(int signum)
{
	m_killed = true;
	m_signal = signum;
}
#endif



const char* HEADER1 = "This software is licenced under the GPL v2 and is primarily";
const char* HEADER2 = "intended for amateur and educational use.";
const char* HEADER3 = "Created by Jonathan Naylor, G4KLX and others";
const char* HEADER4 = "Modified by Phil Thompson VE3RD";

int main(int argc, char** argv)
{
	const char* iniFile = DEFAULT_INI_FILE;


	if (argc > 1) {
		for (int currentArg = 1; currentArg < argc; ++currentArg) {
			std::string arg = argv[currentArg];
			if ((arg == "-v") || (arg == "--version")) {
				::fprintf(stdout, "DMRGateway version %s git #%.7s\n", VERSION, gitversion);
				return 0;
			} else if (arg.substr(0,1) == "-") {
				::fprintf(stderr, "Usage: DMRGateway [-v|--version] [filename]\n");
				return 1;
			} else {
				iniFile = argv[currentArg];
			}
		}
	}

#if !defined(_WIN32) && !defined(_WIN64)
	::signal(SIGINT,  sigHandler);
	::signal(SIGTERM, sigHandler);
	::signal(SIGHUP,  sigHandler);
#endif

	int ret = 0;


	do {
		m_signal = 0;
	

		CDMRGateway* host = new CDMRGateway(std::string(iniFile));
		ret = host->run();

		delete host;

		if (m_signal == 2)
			::LogInfo("DMRGateway-%s exited on receipt of SIGINT", VERSION);

		if (m_signal == 15)
			::LogInfo("DMRGateway-%s exited on receipt of SIGTERM", VERSION);

		if (m_signal == 1)
			::LogInfo("DMRGateway-%s restarted on receipt of SIGHUP", VERSION);

	} while (m_signal == 1);

	::LogFinalise();


	return ret;
}


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

#if !defined(DMRGateway_H)
#define DMRGateway_H

#include "RemoteControl.h"
#include "RewriteDynTGNet.h"
#include "RewriteDynTGRF.h"
#include "MMDVMNetwork.h"
#include "DMRNetwork.h"
#include "APRSWriter.h"
#include "Reflectors.h"
#include "XLXVoice.h"
#include "UDPSocket.h"
#include "RewriteTG.h"
#include "DynVoice.h"
#include "Rewrite.h"
#include "Timer.h"
#include "Conf.h"
#include "GPSD.h"

#include <string>

enum DMRGW_STATUS {
        DMRGWS_NONE,
        DMRGWS_DMRNETWORK1,
        DMRGWS_DMRNETWORK2,
        DMRGWS_DMRNETWORK3,
        DMRGWS_DMRNETWORK4,
        DMRGWS_DMRNETWORK5,
        DMRGWS_DMRNETWORK6,
        DMRGWS_XLXREFLECTOR
};

class CDMRGateway {
public:
    CDMRGateway(const std::string& confFile);
    ~CDMRGateway();

    int run();

    void buildNetworkStatusString(std::string& str);

private:
    CConf               m_conf;
    DMRGW_STATUS*       m_status;
    CMMDVMNetwork*      m_repeater;
    unsigned char*      m_config;
    unsigned int        m_configLen;
    CDMRNetwork*        m_dmrNetwork1;
    std::string         m_dmr1Name;
    CDMRNetwork*        m_dmrNetwork2;
    std::string         m_dmr2Name;
    CDMRNetwork*        m_dmrNetwork3;
    std::string         m_dmr3Name;
    CDMRNetwork*        m_dmrNetwork4;
    std::string         m_dmr4Name;
    CDMRNetwork*        m_dmrNetwork5;
    std::string         m_dmr5Name;
    CDMRNetwork*        m_dmrNetwork6;
    std::string         m_dmr6Name;
    bool                m_network1Enabled;
    bool                m_network2Enabled;
    bool                m_network3Enabled;
    bool                m_network4Enabled;
    bool                m_network5Enabled;
    bool                m_network6Enabled;
    bool                m_dmrNetwork1Connected; // Connection status
    bool                m_dmrNetwork2Connected;
    bool                m_dmrNetwork3Connected;
    bool                m_dmrNetwork4Connected;
    bool                m_dmrNetwork5Connected;
    bool                m_dmrNetwork6Connected;
    CXLXVoice*          m_dmr1Voice; // Voice for DMR Network 1
    CXLXVoice*          m_dmr2Voice; // Voice for DMR Network 2
    CXLXVoice*          m_dmr3Voice; // Voice for DMR Network 3
    CXLXVoice*          m_dmr4Voice; // Voice for DMR Network 4
    CXLXVoice*          m_dmr5Voice; // Voice for DMR Network 5
    CXLXVoice*          m_dmr6Voice; // Voice for DMR Network 6
    CReflectors*        m_xlxReflectors;
    CDMRNetwork*        m_xlxNetwork;
    unsigned int        m_xlxId;
    unsigned int        m_xlxNumber;
    unsigned int        m_xlxReflector;
    unsigned int        m_xlxSlot;
    unsigned int        m_xlxTG;
    unsigned int        m_xlxBase;
    unsigned short      m_xlxLocal;
    unsigned short      m_xlxPort;
    std::string         m_xlxPassword;
    unsigned int        m_xlxStartup;
    unsigned int        m_xlxRoom;
    CTimer              m_xlxRelink;
    bool                m_xlxConnected;
    bool                m_xlxDebug;
    bool                m_xlxUserControl;
    char                m_xlxModule;
    CRewriteTG*         m_rptRewrite;
    CRewriteTG*         m_xlxRewrite;
    CXLXVoice*          m_xlxVoice;
    std::vector<CRewrite*> m_dmr1NetRewrites;
    std::vector<CRewrite*> m_dmr1RFRewrites;
    std::vector<CRewrite*> m_dmr1SrcRewrites;
    std::vector<CRewrite*> m_dmr2NetRewrites;
    std::vector<CRewrite*> m_dmr2RFRewrites;
    std::vector<CRewrite*> m_dmr2SrcRewrites;
    std::vector<CRewrite*> m_dmr3NetRewrites;
    std::vector<CRewrite*> m_dmr3RFRewrites;
    std::vector<CRewrite*> m_dmr3SrcRewrites;
    std::vector<CRewrite*> m_dmr4NetRewrites;
    std::vector<CRewrite*> m_dmr4RFRewrites;
    std::vector<CRewrite*> m_dmr4SrcRewrites;
    std::vector<CRewrite*> m_dmr5NetRewrites;
    std::vector<CRewrite*> m_dmr5RFRewrites;
    std::vector<CRewrite*> m_dmr5SrcRewrites;
    std::vector<CRewrite*> m_dmr6NetRewrites;
    std::vector<CRewrite*> m_dmr6RFRewrites;
    std::vector<CRewrite*> m_dmr6SrcRewrites;
    std::vector<CRewrite*> m_dmr1Passalls;
    std::vector<CRewrite*> m_dmr2Passalls;
    std::vector<CRewrite*> m_dmr3Passalls;
    std::vector<CRewrite*> m_dmr4Passalls;
    std::vector<CRewrite*> m_dmr5Passalls;
    std::vector<CRewrite*> m_dmr6Passalls;
    std::vector<CDynVoice*> m_dynVoices;
    std::vector<CRewriteDynTGRF*> m_dynRF;
    CUDPSocket*         m_socket;
    CAPRSWriter*        m_writer;
    std::string         m_callsign;
    unsigned int        m_txFrequency;
    unsigned int        m_rxFrequency;
#if defined(USE_GPSD)
    CGPSD*              m_gpsd;
#endif
    bool                m_networkXlxEnabled;
    CRemoteControl*     m_remoteControl;

    bool createMMDVM();
    bool createDMRNetwork1();
    bool createDMRNetwork2();
    bool createDMRNetwork3();
    bool createDMRNetwork4();
    bool createDMRNetwork5();
    bool createDMRNetwork6();
    bool createXLXNetwork();
    bool createDynamicTGControl();

    bool linkXLX(unsigned int number);
    void unlinkXLX();
    void writeXLXLink(unsigned int srcId, unsigned int dstId, CDMRNetwork* network);

    bool rewrite(std::vector<CRewrite*>& rewrites, CDMRData& data, bool trace);

    unsigned int getConfig(const std::string& name, unsigned char* buffer);

    void processRadioPosition();
    void processTalkerAlias();
    void createAPRS();
    void processDynamicTGControl();
    void remoteControl();
    void processEnableCommand(CDMRNetwork* network, const std::string& name, bool& mode, bool enabled);
    void buildNetworkStatusNetworkString(std::string& str, const std::string& name, CDMRNetwork* network, bool enabled);
};

#endif


void SetUserRoot()
{
		// If we are currently root...
////		if (getuid() == 0) {
			struct passwd* user = ::getpwnam("root");
			if (user == NULL) {
				::fprintf(stderr, "Could not det root user");
				LogInfo("Could not get the root user\n");
			}
			
			uid_t root_uid = user->pw_uid;
			gid_t root_gid = user->pw_gid;
	

			// Set user and group ID's to mmdvm:mmdvm
			if (setgid(root_gid) != 0) {
				::fprintf(stderr, "Could not set root GID");
				LogInfo("Could not set root GID");
			}

			if (setuid(root_uid) != 0) {				::fprintf(stderr, "Could not set root UID\n");
				LogInfo("Could not set root UID\n");
			}

	    
////		}


}

void SetUserMMDVM()
{
		// If we are currently root...
		if (getuid() == 0) {
			struct passwd* user = ::getpwnam("mmdvm");
			if (user == NULL) {
				::fprintf(stderr, "Could not get the mmdvm user\n");
				LogInfo("Could not get the mmdvm user\n");
			}
			
			uid_t mmdvm_uid = user->pw_uid;
			gid_t mmdvm_gid = user->pw_gid;
	

			// Set user and group ID's to mmdvm:mmdvm
			if (setgid(mmdvm_gid) != 0) {
				::fprintf(stderr, "Could not set mmdvm GID\n");
				LogInfo("Could not set mmdvm GID\n");
			}

			if (setuid(mmdvm_uid) != 0) {
				::fprintf(stderr, "Could not set mmdvm UID\n");
				LogInfo("Could not set mmdvm UID\n");
			}

	    
			// Double check it worked (AKA Paranoia) 
			if (setuid(0) != -1) {
				::fprintf(stderr, "It's possible to regain root - something is wrong!\n");
				LogInfo("It's possible to regain root - something is wrong!\n");
			}
		}




}
CDMRGateway::CDMRGateway(const std::string& confFile) :
m_conf(confFile),
m_status(NULL),
m_repeater(NULL),
m_config(NULL),
m_configLen(0U),
m_dmrNetwork1(NULL),
m_dmr1Name(),
m_dmrNetwork2(NULL),
m_dmr2Name(),
m_dmrNetwork3(NULL),
m_dmr3Name(),
m_dmrNetwork4(NULL),
m_dmr4Name(),
m_dmrNetwork5(NULL),
m_dmr5Name(),
m_dmrNetwork6(NULL),
m_dmr6Name(),
m_network1Enabled(false),
m_network2Enabled(false),
m_network3Enabled(false),
m_network4Enabled(false),
m_network5Enabled(false),
m_network6Enabled(false),
m_dmrNetwork1Connected(false),
m_dmrNetwork2Connected(false),
m_dmrNetwork3Connected(false),
m_dmrNetwork4Connected(false),
m_dmrNetwork5Connected(false),
m_dmrNetwork6Connected(false),
m_dmr1Voice(NULL),
m_dmr2Voice(NULL),
m_dmr3Voice(NULL),
m_dmr4Voice(NULL),
m_dmr5Voice(NULL),
m_dmr6Voice(NULL),
m_xlxReflectors(NULL),
m_xlxNetwork(NULL),
m_xlxId(0U),
m_xlxNumber(0U),
m_xlxReflector(0U),
m_xlxSlot(0U),
m_xlxTG(0U),
m_xlxBase(0U),
m_xlxLocal(0U),
m_xlxPort(0U),
m_xlxPassword(),
m_xlxStartup(0U),
m_xlxRoom(0U),
m_xlxRelink(0U, 0U, 0U),
m_xlxConnected(false),
m_xlxDebug(false),
m_xlxUserControl(false),
m_xlxModule(0),
m_rptRewrite(NULL),
m_xlxRewrite(NULL),
m_xlxVoice(NULL),
m_dmr1NetRewrites(),
m_dmr1RFRewrites(),
m_dmr1SrcRewrites(),
m_dmr2NetRewrites(),
m_dmr2RFRewrites(),
m_dmr2SrcRewrites(),
m_dmr3NetRewrites(),
m_dmr3RFRewrites(),
m_dmr3SrcRewrites(),
m_dmr4NetRewrites(),
m_dmr4RFRewrites(),
m_dmr4SrcRewrites(),
m_dmr5NetRewrites(),
m_dmr5RFRewrites(),
m_dmr5SrcRewrites(),
m_dmr6NetRewrites(),
m_dmr6RFRewrites(),
m_dmr6SrcRewrites(),
m_dmr1Passalls(),
m_dmr2Passalls(),
m_dmr3Passalls(),
m_dmr4Passalls(),
m_dmr5Passalls(),
m_dmr6Passalls(),
m_dynVoices(),
m_dynRF(),
m_socket(NULL),
m_writer(NULL),
m_callsign(),
m_txFrequency(0U),
m_rxFrequency(0U),
#if defined(USE_GPSD)
m_gpsd(NULL),
#endif
m_networkXlxEnabled(false),
m_remoteControl(NULL)
{
}



int CDMRGateway::run()
{
	// Initialize voice objects for XLX and DMR1-DMR6 networks
	if (m_conf.getVoiceEnabled()) {
		// Use unique IDs and appropriate slots/talk groups for each network
		m_xlxVoice = new CXLXVoice(m_conf.getVoiceDirectory(), m_conf.getVoiceLanguage(), m_conf.getXLXNetworkId(), 1U, 6U); // XLX: slot 1, TG 6
		if (!m_xlxVoice->open()) {
			delete m_xlxVoice;
			m_xlxVoice = NULL;
			LogError("Failed to open XLX voice");
		}

		m_dmr1Voice = new CXLXVoice(m_conf.getVoiceDirectory(), m_conf.getVoiceLanguage(), m_conf.getDMRNetwork1Id(), 2U, 9U); // DMR1: slot 2, TG 9
		if (!m_dmr1Voice->open()) {
			delete m_dmr1Voice;
			m_dmr1Voice = NULL;
			LogError("Failed to open DMR1 voice");
		}

		m_dmr2Voice = new CXLXVoice(m_conf.getVoiceDirectory(), m_conf.getVoiceLanguage(), m_conf.getDMRNetwork2Id(), 2U, 8U); // DMR2: slot 2, TG 8
		if (!m_dmr2Voice->open()) {
			delete m_dmr2Voice;
			m_dmr2Voice = NULL;
			LogError("Failed to open DMR2 voice");
		}

		m_dmr3Voice = new CXLXVoice(m_conf.getVoiceDirectory(), m_conf.getVoiceLanguage(), m_conf.getDMRNetwork3Id(), 2U, 11U); // DMR3: slot 2, TG 11
		if (!m_dmr3Voice->open()) {
			delete m_dmr3Voice;
			m_dmr3Voice = NULL;
		 LogError("Failed to open DMR3 voice");
		}

		m_dmr4Voice = new CXLXVoice(m_conf.getVoiceDirectory(), m_conf.getVoiceLanguage(), m_conf.getDMRNetwork4Id(), 2U, 9990U); // DMR4: slot 2, TG 9990
		if (!m_dmr4Voice->open()) {
			delete m_dmr4Voice;
			m_dmr4Voice = NULL;
			LogError("Failed to open DMR4 voice");
		}

		m_dmr5Voice = new CXLXVoice(m_conf.getVoiceDirectory(), m_conf.getVoiceLanguage(), m_conf.getDMRNetwork5Id(), 2U, 9990U); // DMR5: slot 2, TG 9990
		if (!m_dmr5Voice->open()) {
			delete m_dmr5Voice;
			m_dmr5Voice = NULL;
			LogError("Failed to open DMR5 voice");
		}

		m_dmr6Voice = new CXLXVoice(m_conf.getVoiceDirectory(), m_conf.getVoiceLanguage(), m_conf.getDMRNetwork6Id(), 2U, 9990U); // DMR6: slot 2, TG 9990
		if (!m_dmr6Voice->open()) {
			delete m_dmr6Voice;
			m_dmr6Voice = NULL;
			LogError("Failed to open DMR6 voice");
		}
	}

	// Initialize networks (unchanged)
	bool ret = createXLXNetwork();
	if (!ret)
		return 1;

	ret = createDMRNetwork1();
	if (!ret)
		return 1;

	ret = createDMRNetwork2();
	if (!ret)
		return 1;

	ret = createDMRNetwork3();
	if (!ret)
		return 1;

	ret = createDMRNetwork4();
	if (!ret)
		return 1;

	ret = createDMRNetwork5();
	if (!ret)
		return 1;

	ret = createDMRNetwork6();
	if (!ret)
		return 1;

	CStopWatch stopWatch;
	stopWatch.start();

	CTimer statusTimer(1000U, 5U * 60U);
	CTimer aprsTimer(1000U, 20U);
	CTimer voiceTimer(1000U, 10U); // Timer to prevent frequent voice announcements
	voiceTimer.start();

	std::map<unsigned int, unsigned int> lastTG; // Track last TG per network to avoid repeat announcements

	LogMessage("Starting DMRGateway-%s", VERSION);

	while (true) {
		// Process XLX network
		if (m_xlxNetwork != NULL) {
			CDMRData data;
			while (m_xlxNetwork->read(data)) {
				if (data.getFLCO() == FLCO_GROUP && m_conf.getVoiceEnabled() && m_xlxVoice != NULL && voiceTimer.hasExpired()) {
					unsigned int tg = data.getDstId();
					if (lastTG[0] != tg) { // New TG for XLX (network 0)
						m_xlxVoice->announceTG(tg);
						lastTG[0] = tg;
						voiceTimer.start(); // Reset timer to prevent immediate repeat
					}
				}
				// Existing XLX data processing (unchanged)
			}
		}

		// Process DMR1 network
		if (m_dmrNetwork1 != NULL) {
			CDMRData data;
			while (m_dmrNetwork1->read(data)) {
				if (data.getFLCO() == FLCO_GROUP && m_conf.getVoiceEnabled() && m_dmr1Voice != NULL && voiceTimer.hasExpired()) {
					unsigned int tg = data.getDstId();
					if (lastTG[1] != tg) { // New TG for DMR1 (network 1)
						m_dmr1Voice->announceTG(tg);
						lastTG[1] = tg;
						voiceTimer.start();
					}
				}
				// Existing DMR1 data processing (unchanged)
			}
		}

		// Process DMR2 network
		if (m_dmrNetwork2 != NULL) {
			CDMRData data;
			while (m_dmrNetwork2->read(data)) {
				if (data.getFLCO() == FLCO_GROUP && m_conf.getVoiceEnabled() && m_dmr2Voice != NULL && voiceTimer.hasExpired()) {
					unsigned int tg = data.getDstId();
					if (lastTG[2] != tg) { // New TG for DMR2 (network 2)
						m_dmr2Voice->announceTG(tg);
						lastTG[2] = tg;
						voiceTimer.start();
					}
				}
				// Existing DMR2 data processing (unchanged)
			}
		}

		// Process DMR3 network
		if (m_dmrNetwork3 != NULL) {
			CDMRData data;
			while (m_dmrNetwork3->read(data)) {
				if (data.getFLCO() == FLCO_GROUP && m_conf.getVoiceEnabled() && m_dmr3Voice != NULL && voiceTimer.hasExpired()) {
					unsigned int tg = data.getDstId();
					if (lastTG[3] != tg) { // New TG for DMR3 (network 3)
						m_dmr3Voice->announceTG(tg);
						lastTG[3] = tg;
						voiceTimer.start();
					}
				}
				// Existing DMR3 data processing (unchanged)
			}
		}

		// Process DMR4 network
		if (m_dmrNetwork4 != NULL) {
			CDMRData data;
			while (m_dmrNetwork4->read(data)) {
				if (data.getFLCO() == FLCO_GROUP && m_conf.getVoiceEnabled() && m_dmr4Voice != NULL && voiceTimer.hasExpired()) {
					unsigned int tg = data.getDstId();
					if (lastTG[4] != tg) { // New TG for DMR4 (network 4)
						m_dmr4Voice->announceTG(tg);
						lastTG[4] = tg;
						voiceTimer.start();
					}
				}
				// Existing DMR4 data processing (unchanged)
			}
		}

		// Process DMR5 network
		if (m_dmrNetwork5 != NULL) {
			CDMRData data;
			while (m_dmrNetwork5->read(data)) {
				if (data.getFLCO() == FLCO_GROUP && m_conf.getVoiceEnabled() && m_dmr5Voice != NULL && voiceTimer.hasExpired()) {
					unsigned int tg = data.getDstId();
					if (lastTG[5] != tg) { // New TG for DMR5 (network 5)
						m_dmr5Voice->announceTG(tg);
						lastTG[5] = tg;
						voiceTimer.start();
					}
				}
				// Existing DMR5 data processing (unchanged)
			}
		}

		// Process DMR6 network
		if (m_dmrNetwork6 != NULL) {
			CDMRData data;
			while (m_dmrNetwork6->read(data)) {
				if (data.getFLCO() == FLCO_GROUP && m_conf.getVoiceEnabled() && m_dmr6Voice != NULL && voiceTimer.hasExpired()) {
					unsigned int tg = data.getDstId();
					if (lastTG[6] != tg) { // New TG for DMR6 (network 6)
						m_dmr6Voice->announceTG(tg);
						lastTG[6] = tg;
						voiceTimer.start();
					}
				}
				// Existing DMR6 data processing (unchanged)
			}
		}

		// Voice processing for all networks
		if (m_conf.getVoiceEnabled()) {
			CDMRData voiceData;
			if (m_xlxVoice != NULL && m_xlxVoice->read(voiceData)) {
				if (m_xlxNetwork != NULL)
					m_xlxNetwork->write(voiceData);
			}
			if (m_dmr1Voice != NULL && m_dmr1Voice->read(voiceData)) {
				if (m_dmrNetwork1 != NULL)
					m_dmrNetwork1->write(voiceData);
			}
			if (m_dmr2Voice != NULL && m_dmr2Voice->read(voiceData)) {
				if (m_dmrNetwork2 != NULL)
					m_dmrNetwork2->write(voiceData);
			}
			if (m_dmr3Voice != NULL && m_dmr3Voice->read(voiceData)) {
				if (m_dmrNetwork3 != NULL)
					m_dmrNetwork3->write(voiceData);
			}
			if (m_dmr4Voice != NULL && m_dmr4Voice->read(voiceData)) {
				if (m_dmrNetwork4 != NULL)
					m_dmrNetwork4->write(voiceData);
			}
			if (m_dmr5Voice != NULL && m_dmr5Voice->read(voiceData)) {
				if (m_dmrNetwork5 != NULL)
					m_dmrNetwork5->write(voiceData);
			}
			if (m_dmr6Voice != NULL && m_dmr6Voice->read(voiceData)) {
				if (m_dmrNetwork6 != NULL)
					m_dmrNetwork6->write(voiceData);
			}
		}

		// Clock all voice timers
		unsigned int ms = stopWatch.elapsed();
		stopWatch.start();

		if (m_conf.getVoiceEnabled()) {
			if (m_xlxVoice != NULL)
				m_xlxVoice->clock(ms);
			if (m_dmr1Voice != NULL)
				m_dmr1Voice->clock(ms);
			if (m_dmr2Voice != NULL)
				m_dmr2Voice->clock(ms);
			if (m_dmr3Voice != NULL)
				m_dmr3Voice->clock(ms);
			if (m_dmr4Voice != NULL)
				m_dmr4Voice->clock(ms);
			if (m_dmr5Voice != NULL)
				m_dmr5Voice->clock(ms);
			if (m_dmr6Voice != NULL)
				m_dmr6Voice->clock(ms);
			voiceTimer.clock(ms);
		}

		// Existing timer and cleanup code (unchanged)
		// ...

		usleep(1000U); // Prevent tight loop
	}

	return 0;
}
bool CDMRGateway::createMMDVM()
{
	std::string rptAddress   = m_conf.getRptAddress();
	unsigned short rptPort   = m_conf.getRptPort();
	std::string localAddress = m_conf.getLocalAddress();
	unsigned short localPort = m_conf.getLocalPort();
	unsigned short startNet = m_conf.getStartNet();
	unsigned short rawNet = m_conf.getRawNet();
	bool debug               = m_conf.getDebug();

	selnet=startNet;

	LogInfo("MMDVM Network Parameters");
	LogInfo("    Rpt Address: %s", rptAddress.c_str());
	LogInfo("    Rpt Port: %hu", rptPort);
	LogInfo("    Local Address: %s", localAddress.c_str());
	LogInfo("    Local Port: %hu", localPort);
	LogInfo("    Start Net: %hu", startNet);
	LogInfo("    Raw Net: %hu", rawNet);

	m_repeater = new CMMDVMNetwork(rptAddress, rptPort, localAddress, localPort, startNet, debug);

	bool ret = m_repeater->open();
	if (!ret) {
		delete m_repeater;
		m_repeater = NULL;
		return false;
	}

	return true;
}

bool CDMRGateway::createDMRNetwork1()
{
	std::string address  = m_conf.getDMRNetwork1Address();
	unsigned short port  = m_conf.getDMRNetwork1Port();
	unsigned short local = m_conf.getDMRNetwork1Local();
	unsigned int id      = m_conf.getDMRNetwork1Id();
	std::string password = m_conf.getDMRNetwork1Password();
	bool location        = m_conf.getDMRNetwork1Location();
	bool debug           = m_conf.getDMRNetwork1Debug();
	m_dmr1Name           = m_conf.getDMRNetwork1Name();

	if (id == 0U)
		id = m_repeater->getId();

	LogInfo("DMR Network 1 Parameters");
	LogInfo("    Name: %s", m_dmr1Name.c_str());
	LogInfo("    Id: %u", id);
	LogInfo("    Address: %s", address.c_str());
	LogInfo("    Port: %hu", port);
	if (local > 0U)
		LogInfo("    Local: %hu", local);
	else
		LogInfo("    Local: random");
	LogInfo("    Location Data: %s", location ? "yes" : "no");

	m_dmrNetwork1 = new CDMRNetwork(address, port, local, id, password, m_dmr1Name, location, debug);

	std::string options = m_conf.getDMRNetwork1Options();

	if (!options.empty()) {
		LogInfo("    Options: %s", options.c_str());
		m_dmrNetwork1->setOptions(options);
	}

	unsigned char config[400U];
	unsigned int len = getConfig(m_dmr1Name, config);
	m_dmrNetwork1->setConfig(config, len);

	bool ret = m_dmrNetwork1->open();
	if (!ret) {
		delete m_dmrNetwork1;
		m_dmrNetwork1 = NULL;
		return false;
	}

#if defined(USE_GPSD)
	if (location && (m_gpsd != NULL))
		m_gpsd->addNetwork(m_dmrNetwork1);
#endif

	std::vector<CTGRewriteStruct> tgRewrites = m_conf.getDMRNetwork1TGRewrites();
	for (std::vector<CTGRewriteStruct>::const_iterator it = tgRewrites.begin(); it != tgRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:TG%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toTG);
		else
			LogInfo("    Rewrite RF: %u:TG%u-TG%u -> %u:TG%u-TG%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toTG, (*it).m_toTG + (*it).m_range - 1U);
		if ((*it).m_range == 1)
			LogInfo("    Rewrite Net: %u:TG%u -> %u:TG%u", (*it).m_toSlot, (*it).m_toTG, (*it).m_fromSlot, (*it).m_fromTG);
		else
			LogInfo("    Rewrite Net: %u:TG%u-TG%u -> %u:TG%u-TG%u", (*it).m_toSlot, (*it).m_toTG, (*it).m_toTG + (*it).m_range - 1U, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U);

		CRewriteTG* rfRewrite  = new CRewriteTG(m_dmr1Name, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toTG, (*it).m_range);
		CRewriteTG* netRewrite = new CRewriteTG(m_dmr1Name, (*it).m_toSlot, (*it).m_toTG, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_range);

		m_dmr1RFRewrites.push_back(rfRewrite);
		m_dmr1NetRewrites.push_back(netRewrite);
	}

	std::vector<CPCRewriteStruct> pcRewrites = m_conf.getDMRNetwork1PCRewrites();
	for (std::vector<CPCRewriteStruct>::const_iterator it = pcRewrites.begin(); it != pcRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:%u -> %u:%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toId);
		else
			LogInfo("    Rewrite RF: %u:%u-%u -> %u:%u-%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_fromId + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toId, (*it).m_toId + (*it).m_range - 1U);

		CRewritePC* rewrite = new CRewritePC(m_dmr1Name, (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toId, (*it).m_range);

		m_dmr1RFRewrites.push_back(rewrite);
	}

	std::vector<CTypeRewriteStruct> typeRewrites = m_conf.getDMRNetwork1TypeRewrites();
	for (std::vector<CTypeRewriteStruct>::const_iterator it = typeRewrites.begin(); it != typeRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:TG%u -> %u:%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toId);
		else
			LogInfo("    Rewrite RF: %u:TG%u-%u -> %u:%u-%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toId, (*it).m_toId + (*it).m_range - 1U);

		CRewriteType* rewrite = new CRewriteType(m_dmr1Name, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toId, (*it).m_range);

		m_dmr1RFRewrites.push_back(rewrite);
	}

	std::vector<CSrcRewriteStruct> srcRewrites = m_conf.getDMRNetwork1SrcRewrites();
	for (std::vector<CSrcRewriteStruct>::const_iterator it = srcRewrites.begin(); it != srcRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite Net: %u:%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toTG);
		else
			LogInfo("    Rewrite Net: %u:%u-%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_fromId + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toTG);

		CRewriteSrc* rewrite = new CRewriteSrc(m_dmr1Name, (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toTG, (*it).m_range);

		m_dmr1NetRewrites.push_back(rewrite);
	}

	std::vector<CTGDynRewriteStruct> dynRewrites = m_conf.getDMRNetwork1TGDynRewrites();
	for (std::vector<CTGDynRewriteStruct>::const_iterator it = dynRewrites.begin(); it != dynRewrites.end(); ++it) {
		LogInfo("    Dyn Rewrite: %u:TG%u-%u:TG%u <-> %u:TG%u (disc %u:%u) (status %u:%u) (%u exclusions)", (*it).m_slot, (*it).m_fromTG, (*it).m_slot, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_slot, (*it).m_toTG, (*it).m_slot, (*it).m_discPC, (*it).m_slot, (*it).m_statusPC, (*it).m_exclTGs.size());

		CDynVoice* voice = NULL;
		if (m_conf.getVoiceEnabled()) {
			std::string language  = m_conf.getVoiceLanguage();
			std::string directory = m_conf.getVoiceDirectory();

			voice = new CDynVoice(directory, language, m_repeater->getId(), (*it).m_slot, (*it).m_toTG);
			bool ret = voice->open();
			if (!ret) {
				delete voice;
				voice = NULL;
			} else {
				m_dynVoices.push_back(voice);
			}
		}

		CRewriteDynTGNet* netRewriteDynTG = new CRewriteDynTGNet(m_dmr1Name, (*it).m_slot, (*it).m_toTG);
		CRewriteDynTGRF* rfRewriteDynTG = new CRewriteDynTGRF(m_dmr1Name, (*it).m_slot, (*it).m_fromTG, (*it).m_toTG, (*it).m_discPC, (*it).m_statusPC, (*it).m_range, (*it).m_exclTGs, netRewriteDynTG, voice);

		m_dmr1RFRewrites.push_back(rfRewriteDynTG);
		m_dmr1NetRewrites.push_back(netRewriteDynTG);
		m_dynRF.push_back(rfRewriteDynTG);
	}

	std::vector<CIdRewriteStruct> idRewrites = m_conf.getDMRNetwork1IdRewrites();
	for (std::vector<CIdRewriteStruct>::const_iterator it = idRewrites.begin(); it != idRewrites.end(); ++it) {
		LogInfo("    Rewrite Id: %u <-> %u", (*it).m_rfId, (*it).m_netId);

		CRewriteSrcId* rewriteSrcId = new CRewriteSrcId(m_dmr1Name, (*it).m_rfId, (*it).m_netId);
		CRewriteDstId* rewriteDstId = new CRewriteDstId(m_dmr1Name, (*it).m_netId, (*it).m_rfId);

		m_dmr1SrcRewrites.push_back(rewriteSrcId);
		m_dmr1NetRewrites.push_back(rewriteDstId);
	}

	std::vector<unsigned int> tgPassAll = m_conf.getDMRNetwork1PassAllTG();
	for (std::vector<unsigned int>::const_iterator it = tgPassAll.begin(); it != tgPassAll.end(); ++it) {
		LogInfo("    Pass All TG: %u", *it);

		CPassAllTG* rfPassAllTG  = new CPassAllTG(m_dmr1Name, *it);
		CPassAllTG* netPassAllTG = new CPassAllTG(m_dmr1Name, *it);

		m_dmr1Passalls.push_back(rfPassAllTG);
		m_dmr1NetRewrites.push_back(netPassAllTG);
	}

	std::vector<unsigned int> pcPassAll = m_conf.getDMRNetwork1PassAllPC();
	for (std::vector<unsigned int>::const_iterator it = pcPassAll.begin(); it != pcPassAll.end(); ++it) {
		LogInfo("    Pass All PC: %u", *it);

		CPassAllPC* rfPassAllPC  = new CPassAllPC(m_dmr1Name, *it);
		CPassAllPC* netPassAllPC = new CPassAllPC(m_dmr1Name, *it);

		m_dmr1Passalls.push_back(rfPassAllPC);
		m_dmr1NetRewrites.push_back(netPassAllPC);
	}

	m_dmrNetwork1->enable(true);

	return true;
}

bool CDMRGateway::createDMRNetwork2()
{
	std::string address  = m_conf.getDMRNetwork2Address();
	unsigned short port  = m_conf.getDMRNetwork2Port();
	unsigned short local = m_conf.getDMRNetwork2Local();
	unsigned int id      = m_conf.getDMRNetwork2Id();
	std::string password = m_conf.getDMRNetwork2Password();
	bool location        = m_conf.getDMRNetwork2Location();
	bool debug           = m_conf.getDMRNetwork2Debug();
	m_dmr2Name           = m_conf.getDMRNetwork2Name();

	if (id == 0U)
		id = m_repeater->getId();

	LogInfo("DMR Network 2 Parameters");
	LogInfo("    Name: %s", m_dmr2Name.c_str());
	LogInfo("    Id: %u", id);
	LogInfo("    Address: %s", address.c_str());
	LogInfo("    Port: %hu", port);
	if (local > 0U)
		LogInfo("    Local: %hu", local);
	else
		LogInfo("    Local: random");
	LogInfo("    Location Data: %s", location ? "yes" : "no");

	m_dmrNetwork2 = new CDMRNetwork(address, port, local, id, password, m_dmr2Name, location, debug);

	std::string options = m_conf.getDMRNetwork2Options();

	if (!options.empty()) {
		LogInfo("    Options: %s", options.c_str());
		m_dmrNetwork2->setOptions(options);
	}

	unsigned char config[400U];
	unsigned int len = getConfig(m_dmr2Name, config);
	m_dmrNetwork2->setConfig(config, len);

	bool ret = m_dmrNetwork2->open();
	if (!ret) {
		delete m_dmrNetwork2;
		m_dmrNetwork2 = NULL;
		return false;
	}

#if defined(USE_GPSD)
	if (location && (m_gpsd != NULL))
		m_gpsd->addNetwork(m_dmrNetwork2);
#endif

	std::vector<CTGRewriteStruct> tgRewrites = m_conf.getDMRNetwork2TGRewrites();
	for (std::vector<CTGRewriteStruct>::const_iterator it = tgRewrites.begin(); it != tgRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:TG%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toTG);
		else
			LogInfo("    Rewrite RF: %u:TG%u-TG%u -> %u:TG%u-TG%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toTG, (*it).m_toTG + (*it).m_range - 1U);
		if ((*it).m_range == 1)
			LogInfo("    Rewrite Net: %u:TG%u -> %u:TG%u", (*it).m_toSlot, (*it).m_toTG, (*it).m_fromSlot, (*it).m_fromTG);
		else
			LogInfo("    Rewrite Net: %u:TG%u-TG%u -> %u:TG%u-TG%u", (*it).m_toSlot, (*it).m_toTG, (*it).m_toTG + (*it).m_range - 1U, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U);

		CRewriteTG* rfRewrite  = new CRewriteTG(m_dmr2Name, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toTG, (*it).m_range);
		CRewriteTG* netRewrite = new CRewriteTG(m_dmr2Name, (*it).m_toSlot, (*it).m_toTG, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_range);

		m_dmr2RFRewrites.push_back(rfRewrite);
		m_dmr2NetRewrites.push_back(netRewrite);
	}

	std::vector<CPCRewriteStruct> pcRewrites = m_conf.getDMRNetwork2PCRewrites();
	for (std::vector<CPCRewriteStruct>::const_iterator it = pcRewrites.begin(); it != pcRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:%u -> %u:%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toId);
		else
			LogInfo("    Rewrite RF: %u:%u-%u -> %u:%u-%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_fromId + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toId, (*it).m_toId + (*it).m_range - 1U);

		CRewritePC* rewrite = new CRewritePC(m_dmr2Name, (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toId, (*it).m_range);

		m_dmr2RFRewrites.push_back(rewrite);
	}

	std::vector<CTypeRewriteStruct> typeRewrites = m_conf.getDMRNetwork2TypeRewrites();
	for (std::vector<CTypeRewriteStruct>::const_iterator it = typeRewrites.begin(); it != typeRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:TG%u -> %u:%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toId);
		else
			LogInfo("    Rewrite RF: %u:TG%u-%u -> %u:%u-%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toId, (*it).m_toId + (*it).m_range - 1U);

		CRewriteType* rewrite = new CRewriteType(m_dmr2Name, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toId, (*it).m_range);

		m_dmr2RFRewrites.push_back(rewrite);
	}

	std::vector<CSrcRewriteStruct> srcRewrites = m_conf.getDMRNetwork2SrcRewrites();
	for (std::vector<CSrcRewriteStruct>::const_iterator it = srcRewrites.begin(); it != srcRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite Net: %u:%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toTG);
		else
			LogInfo("    Rewrite Net: %u:%u-%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_fromId + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toTG);

		CRewriteSrc* rewrite = new CRewriteSrc(m_dmr2Name, (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toTG, (*it).m_range);

		m_dmr2NetRewrites.push_back(rewrite);
	}

	std::vector<CTGDynRewriteStruct> dynRewrites = m_conf.getDMRNetwork2TGDynRewrites();
	for (std::vector<CTGDynRewriteStruct>::const_iterator it = dynRewrites.begin(); it != dynRewrites.end(); ++it) {
		LogInfo("    Dyn Rewrite: %u:TG%u-%u:TG%u <-> %u:TG%u (disc %u:%u) (status %u:%u) (%u exclusions)", (*it).m_slot, (*it).m_fromTG, (*it).m_slot, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_slot, (*it).m_toTG, (*it).m_slot, (*it).m_discPC, (*it).m_slot, (*it).m_statusPC, (*it).m_exclTGs.size());

		CDynVoice* voice = NULL;
		if (m_conf.getVoiceEnabled()) {
			std::string language = m_conf.getVoiceLanguage();
			std::string directory = m_conf.getVoiceDirectory();

			voice = new CDynVoice(directory, language, m_repeater->getId(), (*it).m_slot, (*it).m_toTG);
			bool ret = voice->open();
			if (!ret) {
				delete voice;
				voice = NULL;
			} else {
				m_dynVoices.push_back(voice);
			}
		}

		CRewriteDynTGNet* netRewriteDynTG = new CRewriteDynTGNet(m_dmr2Name, (*it).m_slot, (*it).m_toTG);
		CRewriteDynTGRF* rfRewriteDynTG = new CRewriteDynTGRF(m_dmr2Name, (*it).m_slot, (*it).m_fromTG, (*it).m_toTG, (*it).m_discPC, (*it).m_statusPC, (*it).m_range, (*it).m_exclTGs, netRewriteDynTG, voice);

		m_dmr2RFRewrites.push_back(rfRewriteDynTG);
		m_dmr2NetRewrites.push_back(netRewriteDynTG);
		m_dynRF.push_back(rfRewriteDynTG);
	}

	std::vector<CIdRewriteStruct> idRewrites = m_conf.getDMRNetwork2IdRewrites();
	for (std::vector<CIdRewriteStruct>::const_iterator it = idRewrites.begin(); it != idRewrites.end(); ++it) {
		LogInfo("    Rewrite Id: %u <-> %u", (*it).m_rfId, (*it).m_netId);

		CRewriteSrcId* rewriteSrcId = new CRewriteSrcId(m_dmr2Name, (*it).m_rfId, (*it).m_netId);
		CRewriteDstId* rewriteDstId = new CRewriteDstId(m_dmr2Name, (*it).m_netId, (*it).m_rfId);

		m_dmr2SrcRewrites.push_back(rewriteSrcId);
		m_dmr2NetRewrites.push_back(rewriteDstId);
	}

	std::vector<unsigned int> tgPassAll = m_conf.getDMRNetwork2PassAllTG();
	for (std::vector<unsigned int>::const_iterator it = tgPassAll.begin(); it != tgPassAll.end(); ++it) {
		LogInfo("    Pass All TG: %u", *it);

		CPassAllTG* rfPassAllTG  = new CPassAllTG(m_dmr2Name, *it);
		CPassAllTG* netPassAllTG = new CPassAllTG(m_dmr2Name, *it);

		m_dmr2Passalls.push_back(rfPassAllTG);
		m_dmr2NetRewrites.push_back(netPassAllTG);
	}

	std::vector<unsigned int> pcPassAll = m_conf.getDMRNetwork2PassAllPC();
	for (std::vector<unsigned int>::const_iterator it = pcPassAll.begin(); it != pcPassAll.end(); ++it) {
		LogInfo("    Pass All PC: %u", *it);

		CPassAllPC* rfPassAllPC  = new CPassAllPC(m_dmr2Name, *it);
		CPassAllPC* netPassAllPC = new CPassAllPC(m_dmr2Name, *it);

		m_dmr2Passalls.push_back(rfPassAllPC);
		m_dmr2NetRewrites.push_back(netPassAllPC);
	}

	m_dmrNetwork2->enable(true);

	return true;
}

bool CDMRGateway::createDMRNetwork3()
{
	std::string address  = m_conf.getDMRNetwork3Address();
	unsigned short port  = m_conf.getDMRNetwork3Port();
	unsigned short local = m_conf.getDMRNetwork3Local();
	unsigned int id      = m_conf.getDMRNetwork3Id();
	std::string password = m_conf.getDMRNetwork3Password();
	bool location        = m_conf.getDMRNetwork3Location();
	bool debug           = m_conf.getDMRNetwork3Debug();
	m_dmr3Name           = m_conf.getDMRNetwork3Name();

	if (id == 0U)
		id = m_repeater->getId();

	LogInfo("DMR Network 3 Parameters");
	LogInfo("    Name: %s", m_dmr3Name.c_str());
	LogInfo("    Id: %u", id);
	LogInfo("    Address: %s", address.c_str());
	LogInfo("    Port: %hu", port);
	if (local > 0U)
		LogInfo("    Local: %hu", local);
	else
		LogInfo("    Local: random");
	LogInfo("    Location Data: %s", location ? "yes" : "no");

	m_dmrNetwork3 = new CDMRNetwork(address, port, local, id, password, m_dmr3Name, location, debug);

	std::string options = m_conf.getDMRNetwork3Options();

	if (!options.empty()) {
		LogInfo("    Options: %s", options.c_str());
		m_dmrNetwork3->setOptions(options);
	}

	unsigned char config[400U];
	unsigned int len = getConfig(m_dmr3Name, config);
	m_dmrNetwork3->setConfig(config, len);

	bool ret = m_dmrNetwork3->open();
	if (!ret) {
		delete m_dmrNetwork3;
		m_dmrNetwork3 = NULL;
		return false;
	}

#if defined(USE_GPSD)
	if (location && (m_gpsd != NULL))
		m_gpsd->addNetwork(m_dmrNetwork3);
#endif

	std::vector<CTGRewriteStruct> tgRewrites = m_conf.getDMRNetwork3TGRewrites();
	for (std::vector<CTGRewriteStruct>::const_iterator it = tgRewrites.begin(); it != tgRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:TG%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toTG);
		else
			LogInfo("    Rewrite RF: %u:TG%u-TG%u -> %u:TG%u-TG%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toTG, (*it).m_toTG + (*it).m_range - 1U);
		if ((*it).m_range == 1)
			LogInfo("    Rewrite Net: %u:TG%u -> %u:TG%u", (*it).m_toSlot, (*it).m_toTG, (*it).m_fromSlot, (*it).m_fromTG);
		else
			LogInfo("    Rewrite Net: %u:TG%u-TG%u -> %u:TG%u-TG%u", (*it).m_toSlot, (*it).m_toTG, (*it).m_toTG + (*it).m_range - 1U, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U);

		CRewriteTG* rfRewrite = new CRewriteTG(m_dmr3Name, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toTG, (*it).m_range);
		CRewriteTG* netRewrite = new CRewriteTG(m_dmr3Name, (*it).m_toSlot, (*it).m_toTG, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_range);

		m_dmr3RFRewrites.push_back(rfRewrite);
		m_dmr3NetRewrites.push_back(netRewrite);
	}

	std::vector<CPCRewriteStruct> pcRewrites = m_conf.getDMRNetwork3PCRewrites();
	for (std::vector<CPCRewriteStruct>::const_iterator it = pcRewrites.begin(); it != pcRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:%u -> %u:%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toId);
		else
			LogInfo("    Rewrite RF: %u:%u-%u -> %u:%u-%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_fromId + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toId, (*it).m_toId + (*it).m_range - 1U);

		CRewritePC* rewrite = new CRewritePC(m_dmr3Name, (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toId, (*it).m_range);

		m_dmr3RFRewrites.push_back(rewrite);
	}

	std::vector<CTypeRewriteStruct> typeRewrites = m_conf.getDMRNetwork3TypeRewrites();
	for (std::vector<CTypeRewriteStruct>::const_iterator it = typeRewrites.begin(); it != typeRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:TG%u -> %u:%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toId);
		else
			LogInfo("    Rewrite RF: %u:TG%u-%u -> %u:%u-%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toId, (*it).m_toId + (*it).m_range - 1U);

		CRewriteType* rewrite = new CRewriteType(m_dmr3Name, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toId, (*it).m_range);

		m_dmr3RFRewrites.push_back(rewrite);
	}

	std::vector<CSrcRewriteStruct> srcRewrites = m_conf.getDMRNetwork3SrcRewrites();
	for (std::vector<CSrcRewriteStruct>::const_iterator it = srcRewrites.begin(); it != srcRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite Net: %u:%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toTG);
		else
			LogInfo("    Rewrite Net: %u:%u-%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_fromId + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toTG);

		CRewriteSrc* rewrite = new CRewriteSrc(m_dmr3Name, (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toTG, (*it).m_range);

		m_dmr3NetRewrites.push_back(rewrite);
	}

	std::vector<CTGDynRewriteStruct> dynRewrites = m_conf.getDMRNetwork3TGDynRewrites();
	for (std::vector<CTGDynRewriteStruct>::const_iterator it = dynRewrites.begin(); it != dynRewrites.end(); ++it) {
		LogInfo("    Dyn Rewrite: %u:TG%u-%u:TG%u <-> %u:TG%u (disc %u:%u) (status %u:%u) (%u exclusions)", (*it).m_slot, (*it).m_fromTG, (*it).m_slot, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_slot, (*it).m_toTG, (*it).m_slot, (*it).m_discPC, (*it).m_slot, (*it).m_statusPC, (*it).m_exclTGs.size());

		CDynVoice* voice = NULL;
		if (m_conf.getVoiceEnabled()) {
			std::string language = m_conf.getVoiceLanguage();
			std::string directory = m_conf.getVoiceDirectory();

			voice = new CDynVoice(directory, language, m_repeater->getId(), (*it).m_slot, (*it).m_toTG);
			bool ret = voice->open();
			if (!ret) {
				delete voice;
				voice = NULL;
			} else {
				m_dynVoices.push_back(voice);
			}
		}

		CRewriteDynTGNet* netRewriteDynTG = new CRewriteDynTGNet(m_dmr3Name, (*it).m_slot, (*it).m_toTG);
		CRewriteDynTGRF* rfRewriteDynTG = new CRewriteDynTGRF(m_dmr3Name, (*it).m_slot, (*it).m_fromTG, (*it).m_toTG, (*it).m_discPC, (*it).m_statusPC, (*it).m_range, (*it).m_exclTGs, netRewriteDynTG, voice);

		m_dmr3RFRewrites.push_back(rfRewriteDynTG);
		m_dmr3NetRewrites.push_back(netRewriteDynTG);
		m_dynRF.push_back(rfRewriteDynTG);
	}

	std::vector<CIdRewriteStruct> idRewrites = m_conf.getDMRNetwork3IdRewrites();
	for (std::vector<CIdRewriteStruct>::const_iterator it = idRewrites.begin(); it != idRewrites.end(); ++it) {
		LogInfo("    Rewrite Id: %u <-> %u", (*it).m_rfId, (*it).m_netId);

		CRewriteSrcId* rewriteSrcId = new CRewriteSrcId(m_dmr3Name, (*it).m_rfId, (*it).m_netId);
		CRewriteDstId* rewriteDstId = new CRewriteDstId(m_dmr3Name, (*it).m_netId, (*it).m_rfId);

		m_dmr3SrcRewrites.push_back(rewriteSrcId);
		m_dmr3NetRewrites.push_back(rewriteDstId);
	}

	std::vector<unsigned int> tgPassAll = m_conf.getDMRNetwork3PassAllTG();
	for (std::vector<unsigned int>::const_iterator it = tgPassAll.begin(); it != tgPassAll.end(); ++it) {
		LogInfo("    Pass All TG: %u", *it);

		CPassAllTG* rfPassAllTG = new CPassAllTG(m_dmr3Name, *it);
		CPassAllTG* netPassAllTG = new CPassAllTG(m_dmr3Name, *it);

		m_dmr3Passalls.push_back(rfPassAllTG);
		m_dmr3NetRewrites.push_back(netPassAllTG);
	}

	std::vector<unsigned int> pcPassAll = m_conf.getDMRNetwork3PassAllPC();
	for (std::vector<unsigned int>::const_iterator it = pcPassAll.begin(); it != pcPassAll.end(); ++it) {
		LogInfo("    Pass All PC: %u", *it);

		CPassAllPC* rfPassAllPC = new CPassAllPC(m_dmr3Name, *it);
		CPassAllPC* netPassAllPC = new CPassAllPC(m_dmr3Name, *it);

		m_dmr3Passalls.push_back(rfPassAllPC);
		m_dmr3NetRewrites.push_back(netPassAllPC);
	}

	m_dmrNetwork3->enable(true);

	return true;
}

bool CDMRGateway::createDMRNetwork4()
{
	std::string address  = m_conf.getDMRNetwork4Address();
	unsigned short port  = m_conf.getDMRNetwork4Port();
	unsigned short local = m_conf.getDMRNetwork4Local();
	unsigned int id      = m_conf.getDMRNetwork4Id();
	std::string password = m_conf.getDMRNetwork4Password();
	bool location        = m_conf.getDMRNetwork4Location();
	bool debug           = m_conf.getDMRNetwork4Debug();
	m_dmr4Name           = m_conf.getDMRNetwork4Name();

	if (id == 0U)
		id = m_repeater->getId();

	LogInfo("DMR Network 4 Parameters");
	LogInfo("    Name: %s", m_dmr4Name.c_str());
	LogInfo("    Id: %u", id);
	LogInfo("    Address: %s", address.c_str());
	LogInfo("    Port: %hu", port);
	if (local > 0U)
		LogInfo("    Local: %hu", local);
	else
		LogInfo("    Local: random");
	LogInfo("    Location Data: %s", location ? "yes" : "no");

	m_dmrNetwork4 = new CDMRNetwork(address, port, local, id, password, m_dmr4Name, location, debug);

	std::string options = m_conf.getDMRNetwork4Options();

	if (!options.empty()) {
		LogInfo("    Options: %s", options.c_str());
		m_dmrNetwork4->setOptions(options);
	}

	unsigned char config[400U];
	unsigned int len = getConfig(m_dmr4Name, config);
	m_dmrNetwork4->setConfig(config, len);

	bool ret = m_dmrNetwork4->open();
	if (!ret) {
		delete m_dmrNetwork4;
		m_dmrNetwork4 = NULL;
		return false;
	}

#if defined(USE_GPSD)
	if (location && (m_gpsd != NULL))
		m_gpsd->addNetwork(m_dmrNetwork4);
#endif

	std::vector<CTGRewriteStruct> tgRewrites = m_conf.getDMRNetwork4TGRewrites();
	for (std::vector<CTGRewriteStruct>::const_iterator it = tgRewrites.begin(); it != tgRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:TG%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toTG);
		else
			LogInfo("    Rewrite RF: %u:TG%u-TG%u -> %u:TG%u-TG%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toTG, (*it).m_toTG + (*it).m_range - 1U);
		if ((*it).m_range == 1)
			LogInfo("    Rewrite Net: %u:TG%u -> %u:TG%u", (*it).m_toSlot, (*it).m_toTG, (*it).m_fromSlot, (*it).m_fromTG);
		else
			LogInfo("    Rewrite Net: %u:TG%u-TG%u -> %u:TG%u-TG%u", (*it).m_toSlot, (*it).m_toTG, (*it).m_toTG + (*it).m_range - 1U, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U);

		CRewriteTG* rfRewrite = new CRewriteTG(m_dmr4Name, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toTG, (*it).m_range);
		CRewriteTG* netRewrite = new CRewriteTG(m_dmr4Name, (*it).m_toSlot, (*it).m_toTG, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_range);

		m_dmr4RFRewrites.push_back(rfRewrite);
		m_dmr4NetRewrites.push_back(netRewrite);
	}

	std::vector<CPCRewriteStruct> pcRewrites = m_conf.getDMRNetwork4PCRewrites();
	for (std::vector<CPCRewriteStruct>::const_iterator it = pcRewrites.begin(); it != pcRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:%u -> %u:%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toId);
		else
			LogInfo("    Rewrite RF: %u:%u-%u -> %u:%u-%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_fromId + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toId, (*it).m_toId + (*it).m_range - 1U);

		CRewritePC* rewrite = new CRewritePC(m_dmr4Name, (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toId, (*it).m_range);

		m_dmr4RFRewrites.push_back(rewrite);
	}

	std::vector<CTypeRewriteStruct> typeRewrites = m_conf.getDMRNetwork4TypeRewrites();
	for (std::vector<CTypeRewriteStruct>::const_iterator it = typeRewrites.begin(); it != typeRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:TG%u -> %u:%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toId);
		else
			LogInfo("    Rewrite RF: %u:TG%u-%u -> %u:%u-%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toId, (*it).m_toId + (*it).m_range - 1U);

		CRewriteType* rewrite = new CRewriteType(m_dmr4Name, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toId, (*it).m_range);

		m_dmr4RFRewrites.push_back(rewrite);
	}

	std::vector<CSrcRewriteStruct> srcRewrites = m_conf.getDMRNetwork4SrcRewrites();
	for (std::vector<CSrcRewriteStruct>::const_iterator it = srcRewrites.begin(); it != srcRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite Net: %u:%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toTG);
		else
			LogInfo("    Rewrite Net: %u:%u-%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_fromId + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toTG);

		CRewriteSrc* rewrite = new CRewriteSrc(m_dmr4Name, (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toTG, (*it).m_range);

		m_dmr4NetRewrites.push_back(rewrite);
	}

	std::vector<CTGDynRewriteStruct> dynRewrites = m_conf.getDMRNetwork4TGDynRewrites();
	for (std::vector<CTGDynRewriteStruct>::const_iterator it = dynRewrites.begin(); it != dynRewrites.end(); ++it) {
		LogInfo("    Dyn Rewrite: %u:TG%u-%u:TG%u <-> %u:TG%u (disc %u:%u) (status %u:%u) (%u exclusions)", (*it).m_slot, (*it).m_fromTG, (*it).m_slot, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_slot, (*it).m_toTG, (*it).m_slot, (*it).m_discPC, (*it).m_slot, (*it).m_statusPC, (*it).m_exclTGs.size());

		CDynVoice* voice = NULL;
		if (m_conf.getVoiceEnabled()) {
			std::string language = m_conf.getVoiceLanguage();
			std::string directory = m_conf.getVoiceDirectory();

			voice = new CDynVoice(directory, language, m_repeater->getId(), (*it).m_slot, (*it).m_toTG);
			bool ret = voice->open();
			if (!ret) {
				delete voice;
				voice = NULL;
			} else {
				m_dynVoices.push_back(voice);
			}
		}

		CRewriteDynTGNet* netRewriteDynTG = new CRewriteDynTGNet(m_dmr4Name, (*it).m_slot, (*it).m_toTG);
		CRewriteDynTGRF* rfRewriteDynTG = new CRewriteDynTGRF(m_dmr4Name, (*it).m_slot, (*it).m_fromTG, (*it).m_toTG, (*it).m_discPC, (*it).m_statusPC, (*it).m_range, (*it).m_exclTGs, netRewriteDynTG, voice);

		m_dmr4RFRewrites.push_back(rfRewriteDynTG);
		m_dmr4NetRewrites.push_back(netRewriteDynTG);
		m_dynRF.push_back(rfRewriteDynTG);
	}

	std::vector<CIdRewriteStruct> idRewrites = m_conf.getDMRNetwork4IdRewrites();
	for (std::vector<CIdRewriteStruct>::const_iterator it = idRewrites.begin(); it != idRewrites.end(); ++it) {
		LogInfo("    Rewrite Id: %u <-> %u", (*it).m_rfId, (*it).m_netId);

		CRewriteSrcId* rewriteSrcId = new CRewriteSrcId(m_dmr4Name, (*it).m_rfId, (*it).m_netId);
		CRewriteDstId* rewriteDstId = new CRewriteDstId(m_dmr4Name, (*it).m_netId, (*it).m_rfId);

		m_dmr4SrcRewrites.push_back(rewriteSrcId);
		m_dmr4NetRewrites.push_back(rewriteDstId);
	}

	std::vector<unsigned int> tgPassAll = m_conf.getDMRNetwork4PassAllTG();
	for (std::vector<unsigned int>::const_iterator it = tgPassAll.begin(); it != tgPassAll.end(); ++it) {
		LogInfo("    Pass All TG: %u", *it);

		CPassAllTG* rfPassAllTG = new CPassAllTG(m_dmr4Name, *it);
		CPassAllTG* netPassAllTG = new CPassAllTG(m_dmr4Name, *it);

		m_dmr4Passalls.push_back(rfPassAllTG);
		m_dmr4NetRewrites.push_back(netPassAllTG);
	}

	std::vector<unsigned int> pcPassAll = m_conf.getDMRNetwork4PassAllPC();
	for (std::vector<unsigned int>::const_iterator it = pcPassAll.begin(); it != pcPassAll.end(); ++it) {
		LogInfo("    Pass All PC: %u", *it);

		CPassAllPC* rfPassAllPC = new CPassAllPC(m_dmr4Name, *it);
		CPassAllPC* netPassAllPC = new CPassAllPC(m_dmr4Name, *it);

		m_dmr4Passalls.push_back(rfPassAllPC);
		m_dmr4NetRewrites.push_back(netPassAllPC);
	}

	m_dmrNetwork4->enable(true);

	return true;
}

bool CDMRGateway::createDMRNetwork5()
{
	std::string address  = m_conf.getDMRNetwork5Address();
	unsigned short port  = m_conf.getDMRNetwork5Port();
	unsigned short local = m_conf.getDMRNetwork5Local();
	unsigned int id      = m_conf.getDMRNetwork5Id();
	std::string password = m_conf.getDMRNetwork5Password();
	bool location        = m_conf.getDMRNetwork5Location();
	bool debug           = m_conf.getDMRNetwork5Debug();
	m_dmr5Name           = m_conf.getDMRNetwork5Name();

	if (id == 0U)
		id = m_repeater->getId();

	LogInfo("DMR Network 5 Parameters");
	LogInfo("    Name: %s", m_dmr5Name.c_str());
	LogInfo("    Id: %u", id);
	LogInfo("    Address: %s", address.c_str());
	LogInfo("    Port: %hu", port);
	if (local > 0U)
		LogInfo("    Local: %hu", local);
	else
		LogInfo("    Local: random");
	LogInfo("    Location Data: %s", location ? "yes" : "no");

	m_dmrNetwork5 = new CDMRNetwork(address, port, local, id, password, m_dmr5Name, location, debug);

	std::string options = m_conf.getDMRNetwork5Options();

	if (!options.empty()) {
		LogInfo("    Options: %s", options.c_str());
		m_dmrNetwork5->setOptions(options);
	}

	unsigned char config[400U];
	unsigned int len = getConfig(m_dmr5Name, config);
	m_dmrNetwork5->setConfig(config, len);

	bool ret = m_dmrNetwork5->open();
	if (!ret) {
		delete m_dmrNetwork5;
		m_dmrNetwork5 = NULL;
		return false;
	}

#if defined(USE_GPSD)
	if (location && (m_gpsd != NULL))
		m_gpsd->addNetwork(m_dmrNetwork5);
#endif

	std::vector<CTGRewriteStruct> tgRewrites = m_conf.getDMRNetwork5TGRewrites();
	for (std::vector<CTGRewriteStruct>::const_iterator it = tgRewrites.begin(); it != tgRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:TG%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toTG);
		else
			LogInfo("    Rewrite RF: %u:TG%u-TG%u -> %u:TG%u-TG%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toTG, (*it).m_toTG + (*it).m_range - 1U);
		if ((*it).m_range == 1)
			LogInfo("    Rewrite Net: %u:TG%u -> %u:TG%u", (*it).m_toSlot, (*it).m_toTG, (*it).m_fromSlot, (*it).m_fromTG);
		else
			LogInfo("    Rewrite Net: %u:TG%u-TG%u -> %u:TG%u-TG%u", (*it).m_toSlot, (*it).m_toTG, (*it).m_toTG + (*it).m_range - 1U, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U);

		CRewriteTG* rfRewrite = new CRewriteTG(m_dmr5Name, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toTG, (*it).m_range);
		CRewriteTG* netRewrite = new CRewriteTG(m_dmr5Name, (*it).m_toSlot, (*it).m_toTG, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_range);

		m_dmr5RFRewrites.push_back(rfRewrite);
		m_dmr5NetRewrites.push_back(netRewrite);
	}

	std::vector<CPCRewriteStruct> pcRewrites = m_conf.getDMRNetwork5PCRewrites();
	for (std::vector<CPCRewriteStruct>::const_iterator it = pcRewrites.begin(); it != pcRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:%u -> %u:%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toId);
		else
			LogInfo("    Rewrite RF: %u:%u-%u -> %u:%u-%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_fromId + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toId, (*it).m_toId + (*it).m_range - 1U);

		CRewritePC* rewrite = new CRewritePC(m_dmr5Name, (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toId, (*it).m_range);

		m_dmr5RFRewrites.push_back(rewrite);
	}

	std::vector<CTypeRewriteStruct> typeRewrites = m_conf.getDMRNetwork5TypeRewrites();
	for (std::vector<CTypeRewriteStruct>::const_iterator it = typeRewrites.begin(); it != typeRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:TG%u -> %u:%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toId);
		else
			LogInfo("    Rewrite RF: %u:TG%u-%u -> %u:%u-%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toId, (*it).m_toId + (*it).m_range - 1U);

		CRewriteType* rewrite = new CRewriteType(m_dmr5Name, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toId, (*it).m_range);

		m_dmr5RFRewrites.push_back(rewrite);
	}

	std::vector<CSrcRewriteStruct> srcRewrites = m_conf.getDMRNetwork5SrcRewrites();
	for (std::vector<CSrcRewriteStruct>::const_iterator it = srcRewrites.begin(); it != srcRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite Net: %u:%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toTG);
		else
			LogInfo("    Rewrite Net: %u:%u-%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_fromId + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toTG);

		CRewriteSrc* rewrite = new CRewriteSrc(m_dmr5Name, (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toTG, (*it).m_range);

		m_dmr5NetRewrites.push_back(rewrite);
	}

	std::vector<CTGDynRewriteStruct> dynRewrites = m_conf.getDMRNetwork5TGDynRewrites();
	for (std::vector<CTGDynRewriteStruct>::const_iterator it = dynRewrites.begin(); it != dynRewrites.end(); ++it) {
		LogInfo("    Dyn Rewrite: %u:TG%u-%u:TG%u <-> %u:TG%u (disc %u:%u) (status %u:%u) (%u exclusions)", (*it).m_slot, (*it).m_fromTG, (*it).m_slot, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_slot, (*it).m_toTG, (*it).m_slot, (*it).m_discPC, (*it).m_slot, (*it).m_statusPC, (*it).m_exclTGs.size());

		CDynVoice* voice = NULL;
		if (m_conf.getVoiceEnabled()) {
			std::string language = m_conf.getVoiceLanguage();
			std::string directory = m_conf.getVoiceDirectory();

			voice = new CDynVoice(directory, language, m_repeater->getId(), (*it).m_slot, (*it).m_toTG);
			bool ret = voice->open();
			if (!ret) {
				delete voice;
				voice = NULL;
			} else {
				m_dynVoices.push_back(voice);
			}
		}

		CRewriteDynTGNet* netRewriteDynTG = new CRewriteDynTGNet(m_dmr5Name, (*it).m_slot, (*it).m_toTG);
		CRewriteDynTGRF* rfRewriteDynTG = new CRewriteDynTGRF(m_dmr5Name, (*it).m_slot, (*it).m_fromTG, (*it).m_toTG, (*it).m_discPC, (*it).m_statusPC, (*it).m_range, (*it).m_exclTGs, netRewriteDynTG, voice);

		m_dmr5RFRewrites.push_back(rfRewriteDynTG);
		m_dmr5NetRewrites.push_back(netRewriteDynTG);
		m_dynRF.push_back(rfRewriteDynTG);
	}

	std::vector<CIdRewriteStruct> idRewrites = m_conf.getDMRNetwork5IdRewrites();
	for (std::vector<CIdRewriteStruct>::const_iterator it = idRewrites.begin(); it != idRewrites.end(); ++it) {
		LogInfo("    Rewrite Id: %u <-> %u", (*it).m_rfId, (*it).m_netId);

		CRewriteSrcId* rewriteSrcId = new CRewriteSrcId(m_dmr5Name, (*it).m_rfId, (*it).m_netId);
		CRewriteDstId* rewriteDstId = new CRewriteDstId(m_dmr5Name, (*it).m_netId, (*it).m_rfId);

		m_dmr5SrcRewrites.push_back(rewriteSrcId);
		m_dmr5NetRewrites.push_back(rewriteDstId);
	}

	std::vector<unsigned int> tgPassAll = m_conf.getDMRNetwork5PassAllTG();
	for (std::vector<unsigned int>::const_iterator it = tgPassAll.begin(); it != tgPassAll.end(); ++it) {
		LogInfo("    Pass All TG: %u", *it);

		CPassAllTG* rfPassAllTG = new CPassAllTG(m_dmr5Name, *it);
		CPassAllTG* netPassAllTG = new CPassAllTG(m_dmr5Name, *it);

		m_dmr5Passalls.push_back(rfPassAllTG);
		m_dmr5NetRewrites.push_back(netPassAllTG);
	}

	std::vector<unsigned int> pcPassAll = m_conf.getDMRNetwork5PassAllPC();
	for (std::vector<unsigned int>::const_iterator it = pcPassAll.begin(); it != pcPassAll.end(); ++it) {
		LogInfo("    Pass All PC: %u", *it);

		CPassAllPC* rfPassAllPC = new CPassAllPC(m_dmr5Name, *it);
		CPassAllPC* netPassAllPC = new CPassAllPC(m_dmr5Name, *it);

		m_dmr5Passalls.push_back(rfPassAllPC);
		m_dmr5NetRewrites.push_back(netPassAllPC);
	}

	m_dmrNetwork5->enable(true);

	return true;
}


bool CDMRGateway::createDMRNetwork6()
{
	std::string address  = m_conf.getDMRNetwork6Address();
	unsigned short port  = m_conf.getDMRNetwork6Port();
	unsigned short local = m_conf.getDMRNetwork6Local();
	unsigned int id      = m_conf.getDMRNetwork6Id();
	std::string password = m_conf.getDMRNetwork6Password();
	bool location        = m_conf.getDMRNetwork6Location();
	bool debug           = m_conf.getDMRNetwork6Debug();
	m_dmr6Name           = m_conf.getDMRNetwork6Name();

	if (id == 0U)
		id = m_repeater->getId();

	LogInfo("DMR Network 6 Parameters");
	LogInfo("    Name: %s", m_dmr6Name.c_str());
	LogInfo("    Id: %u", id);
	LogInfo("    Address: %s", address.c_str());
	LogInfo("    Port: %hu", port);
	if (local > 0U)
		LogInfo("    Local: %hu", local);
	else
		LogInfo("    Local: random");
	LogInfo("    Location Data: %s", location ? "yes" : "no");

	m_dmrNetwork6 = new CDMRNetwork(address, port, local, id, password, m_dmr6Name, location, debug);

	std::string options = m_conf.getDMRNetwork6Options();

	if (!options.empty()) {
		LogInfo("    Options: %s", options.c_str());
		m_dmrNetwork6->setOptions(options);
	}

	unsigned char config[400U];
	unsigned int len = getConfig(m_dmr6Name, config);
	m_dmrNetwork6->setConfig(config, len);

	bool ret = m_dmrNetwork6->open();
	if (!ret) {
		delete m_dmrNetwork6;
		m_dmrNetwork6 = NULL;
		return false;
	}

#if defined(USE_GPSD)
	if (location && (m_gpsd != NULL))
		m_gpsd->addNetwork(m_dmrNetwork6);
#endif

	std::vector<CTGRewriteStruct> tgRewrites = m_conf.getDMRNetwork6TGRewrites();
	for (std::vector<CTGRewriteStruct>::const_iterator it = tgRewrites.begin(); it != tgRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:TG%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toTG);
		else
			LogInfo("    Rewrite RF: %u:TG%u-TG%u -> %u:TG%u-TG%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toTG, (*it).m_toTG + (*it).m_range - 1U);
		if ((*it).m_range == 1)
			LogInfo("    Rewrite Net: %u:TG%u -> %u:TG%u", (*it).m_toSlot, (*it).m_toTG, (*it).m_fromSlot, (*it).m_fromTG);
		else
			LogInfo("    Rewrite Net: %u:TG%u-TG%u -> %u:TG%u-TG%u", (*it).m_toSlot, (*it).m_toTG, (*it).m_toTG + (*it).m_range - 1U, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U);

		CRewriteTG* rfRewrite = new CRewriteTG(m_dmr6Name, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toTG, (*it).m_range);
		CRewriteTG* netRewrite = new CRewriteTG(m_dmr6Name, (*it).m_toSlot, (*it).m_toTG, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_range);

		m_dmr6RFRewrites.push_back(rfRewrite);
		m_dmr6NetRewrites.push_back(netRewrite);
	}

	std::vector<CPCRewriteStruct> pcRewrites = m_conf.getDMRNetwork6PCRewrites();
	for (std::vector<CPCRewriteStruct>::const_iterator it = pcRewrites.begin(); it != pcRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:%u -> %u:%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toId);
		else
			LogInfo("    Rewrite RF: %u:%u-%u -> %u:%u-%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_fromId + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toId, (*it).m_toId + (*it).m_range - 1U);

		CRewritePC* rewrite = new CRewritePC(m_dmr6Name, (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toId, (*it).m_range);

		m_dmr6RFRewrites.push_back(rewrite);
	}

	std::vector<CTypeRewriteStruct> typeRewrites = m_conf.getDMRNetwork6TypeRewrites();
	for (std::vector<CTypeRewriteStruct>::const_iterator it = typeRewrites.begin(); it != typeRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite RF: %u:TG%u -> %u:%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toId);
		else
			LogInfo("    Rewrite RF: %u:TG%u-%u -> %u:%u-%u", (*it).m_fromSlot, (*it).m_fromTG, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toId, (*it).m_toId + (*it).m_range - 1U);

		CRewriteType* rewrite = new CRewriteType(m_dmr6Name, (*it).m_fromSlot, (*it).m_fromTG, (*it).m_toSlot, (*it).m_toId, (*it).m_range);

		m_dmr6RFRewrites.push_back(rewrite);
	}

	std::vector<CSrcRewriteStruct> srcRewrites = m_conf.getDMRNetwork6SrcRewrites();
	for (std::vector<CSrcRewriteStruct>::const_iterator it = srcRewrites.begin(); it != srcRewrites.end(); ++it) {
		if ((*it).m_range == 1)
			LogInfo("    Rewrite Net: %u:%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toTG);
		else
			LogInfo("    Rewrite Net: %u:%u-%u -> %u:TG%u", (*it).m_fromSlot, (*it).m_fromId, (*it).m_fromId + (*it).m_range - 1U, (*it).m_toSlot, (*it).m_toTG);

		CRewriteSrc* rewrite = new CRewriteSrc(m_dmr6Name, (*it).m_fromSlot, (*it).m_fromId, (*it).m_toSlot, (*it).m_toTG, (*it).m_range);

		m_dmr6NetRewrites.push_back(rewrite);
	}

	std::vector<CTGDynRewriteStruct> dynRewrites = m_conf.getDMRNetwork6TGDynRewrites();
	for (std::vector<CTGDynRewriteStruct>::const_iterator it = dynRewrites.begin(); it != dynRewrites.end(); ++it) {
		LogInfo("    Dyn Rewrite: %u:TG%u-%u:TG%u <-> %u:TG%u (disc %u:%u) (status %u:%u) (%u exclusions)", (*it).m_slot, (*it).m_fromTG, (*it).m_slot, (*it).m_fromTG + (*it).m_range - 1U, (*it).m_slot, (*it).m_toTG, (*it).m_slot, (*it).m_discPC, (*it).m_slot, (*it).m_statusPC, (*it).m_exclTGs.size());

		CDynVoice* voice = NULL;
		if (m_conf.getVoiceEnabled()) {
			std::string language = m_conf.getVoiceLanguage();
			std::string directory = m_conf.getVoiceDirectory();

			voice = new CDynVoice(directory, language, m_repeater->getId(), (*it).m_slot, (*it).m_toTG);
			bool ret = voice->open();
			if (!ret) {
				delete voice;
				voice = NULL;
			} else {
				m_dynVoices.push_back(voice);
			}
		}

		CRewriteDynTGNet* netRewriteDynTG = new CRewriteDynTGNet(m_dmr6Name, (*it).m_slot, (*it).m_toTG);
		CRewriteDynTGRF* rfRewriteDynTG = new CRewriteDynTGRF(m_dmr6Name, (*it).m_slot, (*it).m_fromTG, (*it).m_toTG, (*it).m_discPC, (*it).m_statusPC, (*it).m_range, (*it).m_exclTGs, netRewriteDynTG, voice);

		m_dmr6RFRewrites.push_back(rfRewriteDynTG);
		m_dmr6NetRewrites.push_back(netRewriteDynTG);
		m_dynRF.push_back(rfRewriteDynTG);
	}

	std::vector<CIdRewriteStruct> idRewrites = m_conf.getDMRNetwork6IdRewrites();
	for (std::vector<CIdRewriteStruct>::const_iterator it = idRewrites.begin(); it != idRewrites.end(); ++it) {
		LogInfo("    Rewrite Id: %u <-> %u", (*it).m_rfId, (*it).m_netId);

		CRewriteSrcId* rewriteSrcId = new CRewriteSrcId(m_dmr6Name, (*it).m_rfId, (*it).m_netId);
		CRewriteDstId* rewriteDstId = new CRewriteDstId(m_dmr6Name, (*it).m_netId, (*it).m_rfId);

		m_dmr6SrcRewrites.push_back(rewriteSrcId);
		m_dmr6NetRewrites.push_back(rewriteDstId);
	}

	std::vector<unsigned int> tgPassAll = m_conf.getDMRNetwork6PassAllTG();
	for (std::vector<unsigned int>::const_iterator it = tgPassAll.begin(); it != tgPassAll.end(); ++it) {
		LogInfo("    Pass All TG: %u", *it);

		CPassAllTG* rfPassAllTG = new CPassAllTG(m_dmr6Name, *it);
		CPassAllTG* netPassAllTG = new CPassAllTG(m_dmr6Name, *it);

		m_dmr6Passalls.push_back(rfPassAllTG);
		m_dmr6NetRewrites.push_back(netPassAllTG);
	}

	std::vector<unsigned int> pcPassAll = m_conf.getDMRNetwork6PassAllPC();
	for (std::vector<unsigned int>::const_iterator it = pcPassAll.begin(); it != pcPassAll.end(); ++it) {
		LogInfo("    Pass All PC: %u", *it);

		CPassAllPC* rfPassAllPC = new CPassAllPC(m_dmr6Name, *it);
		CPassAllPC* netPassAllPC = new CPassAllPC(m_dmr6Name, *it);

		m_dmr6Passalls.push_back(rfPassAllPC);
		m_dmr6NetRewrites.push_back(netPassAllPC);
	}

	m_dmrNetwork6->enable(true);

	return true;
}

bool CDMRGateway::createXLXNetwork()
{
	std::string fileName    = m_conf.getXLXNetworkFile();
	unsigned int reloadTime = m_conf.getXLXNetworkReloadTime();

	m_xlxReflectors = new CReflectors(fileName, reloadTime);

	bool ret = m_xlxReflectors->load();
	if (!ret) {
		delete m_xlxReflectors;
		return false;
	}

	m_xlxLocal         = m_conf.getXLXNetworkLocal();
	m_xlxPort          = m_conf.getXLXNetworkPort();
	m_xlxPassword      = m_conf.getXLXNetworkPassword();
	m_xlxId            = m_conf.getXLXNetworkId();
	m_xlxDebug         = m_conf.getXLXNetworkDebug();
	m_xlxUserControl   = m_conf.getXLXNetworkUserControl();

	if (m_xlxId == 0U)
		m_xlxId = m_repeater->getId();

	m_xlxSlot    = m_conf.getXLXNetworkSlot();
	m_xlxTG      = m_conf.getXLXNetworkTG();
	m_xlxBase    = m_conf.getXLXNetworkBase();
	m_xlxStartup = m_conf.getXLXNetworkStartup();
	m_xlxModule  = m_conf.getXLXNetworkModule();

	unsigned int xlxRelink  = m_conf.getXLXNetworkRelink();

	LogInfo("XLX Network Parameters");
	LogInfo("    Id: %u", m_xlxId);
	LogInfo("    Hosts file: %s", fileName.c_str());
	LogInfo("    Reload time: %u minutes", reloadTime);
	if (m_xlxLocal > 0U)
		LogInfo("    Local: %hu", m_xlxLocal);
	else
		LogInfo("    Local: random");
	LogInfo("    Port: %hu", m_xlxPort);
	LogInfo("    Slot: %u", m_xlxSlot);
	LogInfo("    TG: %u", m_xlxTG);
	LogInfo("    Base: %u", m_xlxBase);

	if (m_xlxStartup > 0U)
		LogInfo("    Startup: XLX%03u", m_xlxStartup);

	if (xlxRelink > 0U) {
		m_xlxRelink.setTimeout(xlxRelink * 60U);
		LogInfo("    Relink: %u minutes", xlxRelink);
	} else {
		LogInfo("    Relink: disabled");
	}

	if (m_xlxUserControl)
		LogInfo("    User Control: enabled");
	else
		LogInfo("    User Control: disabled");

	if (m_xlxModule != 0U)
		LogInfo("     Module: %c", m_xlxModule);

	if (m_xlxStartup > 0U)
		linkXLX(m_xlxStartup);

	m_rptRewrite = new CRewriteTG("XLX", XLX_SLOT, XLX_TG, m_xlxSlot, m_xlxTG, 1U);
	m_xlxRewrite = new CRewriteTG("XLX", m_xlxSlot, m_xlxTG, XLX_SLOT, XLX_TG, 1U);

	return true;
}

bool CDMRGateway::createDynamicTGControl()
{
	unsigned short port = m_conf.getDynamicTGControlPort();

	m_socket = new CUDPSocket(port);

	bool ret = m_socket->open();
	if (!ret) {
		delete m_socket;
		m_socket = NULL;
		return false;
	}

	return true;
}

bool CDMRGateway::linkXLX(unsigned int number)
{
	CReflector* reflector = m_xlxReflectors->find(number);
	if (reflector == NULL)
		return false;

	if (m_xlxNetwork != NULL) {
		LogMessage("XLX, Disconnecting from XLX%03u", m_xlxNumber);
		m_xlxNetwork->close(true);
		delete m_xlxNetwork;
	}

	m_xlxConnected = false;
	m_xlxRelink.stop();

	m_xlxNetwork = new CDMRNetwork(reflector->m_address, m_xlxPort, m_xlxLocal, m_xlxId, m_xlxPassword, "XLX", false, m_xlxDebug);

	unsigned char config[400U];
	unsigned int len = getConfig("XLX", config);
	m_xlxNetwork->setConfig(config, len);

	bool ret = m_xlxNetwork->open();
	if (!ret) {
		delete m_xlxNetwork;
		m_xlxNetwork = NULL;
		return false;
	}

	m_xlxNumber = number;
	if (m_xlxModule != 0U)
		m_xlxRoom  = ((int(m_xlxModule) - 64U) + 4000U);
	else
		m_xlxRoom  = reflector->m_startup;
	m_xlxReflector = 4000U;

	LogMessage("XLX, Connecting to XLX%03u", m_xlxNumber);

	m_xlxNetwork->enable(m_networkXlxEnabled);

	return true;
}

void CDMRGateway::unlinkXLX()
{
	if (m_xlxNetwork != NULL) {
		m_xlxNetwork->close(true);
		delete m_xlxNetwork;
		m_xlxNetwork = NULL;
	}

	m_xlxConnected = false;
	m_xlxRelink.stop();
}

void CDMRGateway::writeXLXLink(unsigned int srcId, unsigned int dstId, CDMRNetwork* network)
{
	assert(network != NULL);

	unsigned int streamId = ::rand() + 1U;

	CDMRData data;

	data.setSlotNo(XLX_SLOT);
	data.setFLCO(FLCO_USER_USER);
	data.setSrcId(srcId);
	data.setDstId(dstId);
	data.setDataType(DT_VOICE_LC_HEADER);
	data.setN(0U);
	data.setStreamId(streamId);

	unsigned char buffer[DMR_FRAME_LENGTH_BYTES];

	CDMRLC lc;
	lc.setSrcId(srcId);
	lc.setDstId(dstId);
	lc.setFLCO(FLCO_USER_USER);

	CDMRFullLC fullLC;
	fullLC.encode(lc, buffer, DT_VOICE_LC_HEADER);

	CDMRSlotType slotType;
	slotType.setColorCode(COLOR_CODE);
	slotType.setDataType(DT_VOICE_LC_HEADER);
	slotType.getData(buffer);

	CSync::addDMRDataSync(buffer, true);

	data.setData(buffer);

	for (unsigned int i = 0U; i < 3U; i++) {
		data.setSeqNo(i);
		network->write(data);
	}

	data.setDataType(DT_TERMINATOR_WITH_LC);

	fullLC.encode(lc, buffer, DT_TERMINATOR_WITH_LC);

	slotType.setDataType(DT_TERMINATOR_WITH_LC);
	slotType.getData(buffer);

	data.setData(buffer);

	for (unsigned int i = 0U; i < 2U; i++) {
		data.setSeqNo(i + 3U);
		network->write(data);
	}
}

bool CDMRGateway::rewrite(std::vector<CRewrite*>& rewrites, CDMRData & data, bool trace)
{
	for (std::vector<CRewrite*>::iterator it = rewrites.begin(); it != rewrites.end(); ++it)
		if ((*it)->process(data, trace))
			return true;
	return false;
}

unsigned int CDMRGateway::getConfig(const std::string& name, unsigned char* buffer)
{
	assert(buffer != NULL);

	float lat = m_conf.getInfoLatitude();
	if ((lat > 90) || (lat < -90))
		lat = 0;
	float lon = m_conf.getInfoLongitude();
	if ((lon > 180) || (lon < -180))
		lon = 0;

	int height = m_conf.getInfoHeight();
	if (height > 999)
		height = 999;
	else if (height < 0)
		height = 0;

	std::string location    = m_conf.getInfoLocation();
	std::string description = m_conf.getInfoDescription();
	std::string url         = m_conf.getInfoURL();

	::sprintf((char*)buffer, "%8.8s%9.9s%9.9s%2.2s%2.2s%+08.4f%+09.4f%03d%-20.20s%-19.19s%c%-124.124s%40.40s%40.40s",
		m_config + 0U, m_config + 8U, m_config + 17U, m_config + 26U, m_config + 28U,
		lat, lon, height, location.c_str(),
		description.c_str(), m_config[30U], url.c_str(), m_config + 31U, m_config + 71U);

	m_callsign = std::string((char*)m_config + 0U, 8U);
	size_t n = m_callsign.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	if (n != std::string::npos)
		m_callsign.erase(n);

	char frequency[10U];
	::memset(frequency, 0x00U, 10U);
	::memcpy(frequency, m_config + 8U, 9U);
	m_rxFrequency = (unsigned int)::atoi(frequency);

	::memset(frequency, 0x00U, 10U);
	::memcpy(frequency, m_config + 17U, 9U);
	m_txFrequency = (unsigned int)::atoi(frequency);

	LogInfo("%s: configuration message: %s", name.c_str(), buffer);

	return (unsigned int)::strlen((char*)buffer);
}

void CDMRGateway::processRadioPosition()
{
	unsigned char buffer[50U];
	unsigned int length;
	bool ret = m_repeater->readRadioPosition(buffer, length);
	if (!ret)
		return;

	if (m_network1Enabled && (m_dmrNetwork1 != NULL) && (m_status[1U] == DMRGWS_DMRNETWORK1 || m_status[2U] == DMRGWS_DMRNETWORK1))
		m_dmrNetwork1->writeRadioPosition(buffer, length);

	if (m_network2Enabled && (m_dmrNetwork2 != NULL) && (m_status[1U] == DMRGWS_DMRNETWORK2 || m_status[2U] == DMRGWS_DMRNETWORK2))
		m_dmrNetwork2->writeRadioPosition(buffer, length);

	if (m_network3Enabled && (m_dmrNetwork3 != NULL) && (m_status[1U] == DMRGWS_DMRNETWORK3 || m_status[2U] == DMRGWS_DMRNETWORK3))
		m_dmrNetwork3->writeRadioPosition(buffer, length);

	if (m_network4Enabled && (m_dmrNetwork4 != NULL) && (m_status[1U] == DMRGWS_DMRNETWORK4 || m_status[2U] == DMRGWS_DMRNETWORK4))
		m_dmrNetwork4->writeRadioPosition(buffer, length);

	if (m_network5Enabled && (m_dmrNetwork5 != NULL) && (m_status[1U] == DMRGWS_DMRNETWORK5 || m_status[2U] == DMRGWS_DMRNETWORK5))
		m_dmrNetwork5->writeRadioPosition(buffer, length);

	if (m_network6Enabled && (m_dmrNetwork6 != NULL) && (m_status[1U] == DMRGWS_DMRNETWORK6 || m_status[2U] == DMRGWS_DMRNETWORK6))
		m_dmrNetwork6->writeRadioPosition(buffer, length);
}

void CDMRGateway::processTalkerAlias()
{
	unsigned char buffer[50U];
	unsigned int length;
	bool ret = m_repeater->readTalkerAlias(buffer, length);
	if (!ret)
		return;

	if (m_network1Enabled && (m_dmrNetwork1 != NULL) && (m_status[1U] == DMRGWS_DMRNETWORK1 || m_status[2U] == DMRGWS_DMRNETWORK1))
		m_dmrNetwork1->writeTalkerAlias(buffer, length);

	if (m_network2Enabled && (m_dmrNetwork2 != NULL) && (m_status[1U] == DMRGWS_DMRNETWORK2 || m_status[2U] == DMRGWS_DMRNETWORK2))
		m_dmrNetwork2->writeTalkerAlias(buffer, length);

	if (m_network3Enabled && (m_dmrNetwork3 != NULL) && (m_status[1U] == DMRGWS_DMRNETWORK3 || m_status[2U] == DMRGWS_DMRNETWORK3))
		m_dmrNetwork3->writeTalkerAlias(buffer, length);

	if (m_network4Enabled && (m_dmrNetwork4 != NULL) && (m_status[1U] == DMRGWS_DMRNETWORK4 || m_status[2U] == DMRGWS_DMRNETWORK4))
		m_dmrNetwork4->writeTalkerAlias(buffer, length);

	if (m_network5Enabled && (m_dmrNetwork5 != NULL) && (m_status[1U] == DMRGWS_DMRNETWORK5 || m_status[2U] == DMRGWS_DMRNETWORK5))
		m_dmrNetwork5->writeTalkerAlias(buffer, length);

	if (m_network6Enabled && (m_dmrNetwork6 != NULL) && (m_status[1U] == DMRGWS_DMRNETWORK6 || m_status[2U] == DMRGWS_DMRNETWORK6))
		m_dmrNetwork6->writeTalkerAlias(buffer, length);
}

void CDMRGateway::createAPRS()
{
	if (!m_conf.getAPRSEnabled())
		return;

	std::string address = m_conf.getAPRSAddress();
	unsigned short port = m_conf.getAPRSPort();
	std::string suffix  = m_conf.getAPRSSuffix();
	bool debug          = m_conf.getDebug();

	m_writer = new CAPRSWriter(m_callsign, suffix, address, port, debug);

	std::string desc    = m_conf.getAPRSDescription();

	m_writer->setInfo(m_txFrequency, m_rxFrequency, desc);

	float latitude  = m_conf.getInfoLatitude();
	float longitude = m_conf.getInfoLongitude();
	int height      = m_conf.getInfoHeight();

	m_writer->setLocation(latitude, longitude, height);

	bool ret = m_writer->open();
	if (!ret) {
		delete m_writer;
		m_writer = NULL;
		return;
	}

#if defined(USE_GPSD)
	if (m_gpsd != NULL)
		m_gpsd->setAPRS(m_writer);
#endif
}

void CDMRGateway::processDynamicTGControl()
{
	unsigned char buffer[100U];
	sockaddr_storage address;
	unsigned int addrlen;
	int len = m_socket->read(buffer, 100U, address, addrlen);
	if (len <= 0)
		return;

	buffer[len] = '\0';

	if (::memcmp(buffer + 0U, "DynTG", 5U) == 0) {
		char* pSlot = ::strtok((char*)(buffer + 5U), ", \r\n");
		char* pTG   = ::strtok(NULL, ", \r\n");

		if (pSlot == NULL || pTG == NULL) {
			LogWarning("Malformed dynamic TG control message");
			return;
		}

		unsigned int slot = (unsigned int)::atoi(pSlot);
		unsigned int tg   = (unsigned int)::atoi(pTG);

		for (std::vector<CRewriteDynTGRF*>::iterator it = m_dynRF.begin(); it != m_dynRF.end(); ++it)
			(*it)->tgChange(slot, tg);
	} else {
		LogWarning("Unknown dynamic TG control message: %s", buffer);
	}
}

void CDMRGateway::remoteControl()
{
	if (m_remoteControl == NULL)
		return;

	REMOTE_COMMAND command = m_remoteControl->getCommand();
	switch (command) {
		case RCD_ENABLE_NETWORK1:
			processEnableCommand(m_dmrNetwork1, "DMR Network 1", m_network1Enabled, true);
			break;
		case RCD_ENABLE_NETWORK2:
			processEnableCommand(m_dmrNetwork2, "DMR Network 2", m_network2Enabled, true);
			break;
		case RCD_ENABLE_NETWORK3:
			processEnableCommand(m_dmrNetwork3, "DMR Network 3", m_network3Enabled, true);
			break;
		case RCD_ENABLE_NETWORK4:
			processEnableCommand(m_dmrNetwork4, "DMR Network 4", m_network4Enabled, true);
			break;
		case RCD_ENABLE_NETWORK5:
			processEnableCommand(m_dmrNetwork5, "DMR Network 5", m_network5Enabled, true);
			break;
		case RCD_ENABLE_XLX:
			if (m_xlxVoice != NULL) {
				m_xlxVoice->reset();
			}
			processEnableCommand(m_xlxNetwork, "XLX Network", m_networkXlxEnabled, true);
			break;
		case RCD_DISABLE_NETWORK1:
			processEnableCommand(m_dmrNetwork1, "DMR Network 1", m_network1Enabled, false);
			break;
		case RCD_DISABLE_NETWORK2:
			processEnableCommand(m_dmrNetwork2, "DMR Network 2", m_network2Enabled, false);
			break;
		case RCD_DISABLE_NETWORK3:
			processEnableCommand(m_dmrNetwork3, "DMR Network 3", m_network3Enabled, false);
			break;
		case RCD_DISABLE_NETWORK4:
			processEnableCommand(m_dmrNetwork4, "DMR Network 4", m_network4Enabled, false);
			break;
		case RCD_DISABLE_NETWORK5:
			processEnableCommand(m_dmrNetwork5, "DMR Network 5", m_network5Enabled, false);
			break;
		case RCD_DISABLE_XLX:
			processEnableCommand(m_xlxNetwork, "XLX Network", m_networkXlxEnabled, false);
			break;
		default:
			break;
	}
}

void CDMRGateway::processEnableCommand(CDMRNetwork* network, const std::string& name, bool& mode, bool enabled)
{
	LogDebug("Setting '%s' mode current=%s new=%s", name.c_str(), mode ? "true" : "false", enabled ? "true" : "false");

	if (network != NULL) {
		mode = enabled;
		network->enable(enabled);
	}
}

void CDMRGateway::buildNetworkStatusString(std::string &str)
{
	str = "";
	buildNetworkStatusNetworkString(str, "xlx", m_xlxNetwork, m_networkXlxEnabled);
	str += " ";
	buildNetworkStatusNetworkString(str, "net1", m_dmrNetwork1, m_network1Enabled);
	str += " ";
	buildNetworkStatusNetworkString(str, "net2", m_dmrNetwork2, m_network2Enabled);
	str += " ";
	buildNetworkStatusNetworkString(str, "net3", m_dmrNetwork3, m_network3Enabled);
	str += " ";
	buildNetworkStatusNetworkString(str, "net4", m_dmrNetwork4, m_network4Enabled);
	str += " ";
	buildNetworkStatusNetworkString(str, "net5", m_dmrNetwork5, m_network5Enabled);
	str += " ";
	buildNetworkStatusNetworkString(str, "net6", m_dmrNetwork6, m_network6Enabled);
}

void CDMRGateway::buildNetworkStatusNetworkString(std::string &str, const std::string& name, CDMRNetwork* network, bool enabled)
{
	str += name + ":"+ (((network == NULL) || (enabled == false)) ? "n/a" : (network->isConnected() ? "conn" : "disc"));
}


void ClearNetworks()
{
net1ok=false;
net2ok=false;
net3ok=false;
net4ok=false;
net5ok=false;
net6ok=false;
}
void ClearRFNets()
{
rf1ok=false;
rf2ok=false;
rf3ok=false;
rf4ok=false;
rf5ok=false;
rf6ok=false;
}


int Reload()
{
	const char* iniFile = DEFAULT_INI_FILE;

	using namespace std;
	ifstream myfile8;
	ifstream myfile7;
	ifstream myfile1;

	myfile8.open("/etc/dmrgateway8");
	if ( myfile8 ) {
        	const char* iniFile8 = "/etc/dmrgateway8";
		if ( GWMode == 8 ) { 
			iniFile = iniFile8;
			LogInfo("..... Loading /etc/dmrgateway8!");
		}
	} else {
		cout<<"/etc/dmrgateway8 file does not exist";
		LogInfo("..... /etc/dmrgateway8 file does not exist!");
		LogInfo("..... Unable to switch modes - Run GWConfig.sh");
		return -1;  
	 }


	myfile7.open("/etc/dmrgateway7");
	if ( myfile7 ) {
	        const char* iniFile7 = "/etc/dmrgateway7";
		if ( GWMode == 7 ) {
			iniFile = iniFile7;
			LogInfo("..... Loading /etc/dmrgateway7!");
		}
	} else {
		cout<<"/etc/dmrgateway1 file does not exist";
		LogInfo("..... /etc/dmrgateway7 file does not exist!");
		LogInfo("..... Unable to switch modes - Run GWConfig.sh");
  	 	return -1;
		}

	myfile1.open("/etc/dmrgateway1");
	if ( myfile1 ) {
        	const char* iniFile1 = "/etc/dmrgateway1";
		if ( GWMode == 1 ) {
			iniFile = iniFile1;
			LogInfo("..... Loading /etc/dmrgateway1!");
		}
	} else {
		cout<<"/etc/dmrgateway1 file does not exist";
		LogInfo("..... /etc/dmrgateway8 file does not exist!");
		LogInfo("..... Unable to switch modes - Run GWConfig.sh");
               return -1;
  	 }


 

        int ret = 0;

        do {
                m_signal = 0;

		CDMRGateway* host = new CDMRGateway(std::string(iniFile));

                ret = host->run();

                delete host;

                if (m_signal == 2)
                        ::LogInfo("DMRGateway-%s exited on receipt of SIGINT", VERSION);

                if (m_signal == 15)
                        ::LogInfo("DMRGateway-%s exited on receipt of SIGTERM", VERSION);

                if (m_signal == 1)
                        ::LogInfo("DMRGateway-%s restarted on receipt of SIGHUP", VERSION);
        } while (m_signal == 1);

        ::LogFinalise();

        return ret;


}


void SetDMR()
{

return;
// this function to be deleted

static bool ok2save = false;
SetUserRoot();

	system("/bin/mount -o remount,rw /");

        CSimpleIniA ini;
        ini.SetUnicode();
        SI_Error rc = ini.LoadFile("/etc/dmrgateway");

   if (GWMode == 8) 
   {
        rc = ini.SetValue("General", "GWMode", "8");
        rc = ini.SetValue("DMR Network 1", "TGRewrite0", "2,11000001,2,1,999999");
        rc = ini.SetValue("DMR Network 1", "PCRewrite0", "2,11000001,2,1,999999");
        rc = ini.SetValue("DMR Network 2", "TGRewrite0", "2,12000001,2,1,999999");
        rc = ini.SetValue("DMR Network 2", "PCRewrite0", "2,12000001,2,1,999999");
        rc = ini.SetValue("DMR Network 3", "TGRewrite0", "2,13000001,2,1,999999");
        rc = ini.SetValue("DMR Network 3", "PCRewrite0", "2,13000001,2,1,999999");
        rc = ini.SetValue("DMR Network 4", "TGRewrite0", "2,14000001,2,1,999999");
        rc = ini.SetValue("DMR Network 4", "PCRewrite0", "2,14000001,2,1,999999");
        rc = ini.SetValue("DMR Network 5", "TGRewrite0", "2,15000001,2,1,999999");
        rc = ini.SetValue("DMR Network 5", "PCRewrite0", "2,15000001,2,1,999999");
        rc = ini.SetValue("DMR Network 6", "TGRewrite0", "2,16000001,2,1,999999");
        rc = ini.SetValue("DMR Network 6", "PCRewrite0", "2,16000001,2,1,999999");

        rc = ini.SetValue("DMR Network 1", "TGRewrite1", "2,1000001,2,1000001,5599999");
        rc = ini.SetValue("DMR Network 2", "TGRewrite1", "2,1000001,2,1000001,5599999");
        rc = ini.SetValue("DMR Network 3", "TGRewrite1", "2,1000001,2,1000001,5599999");
        rc = ini.SetValue("DMR Network 4", "TGRewrite1", "2,1000001,2,1000001,5599999");
        rc = ini.SetValue("DMR Network 5", "TGRewrite1", "2,1000001,2,1000001,5599999");
        rc = ini.SetValue("DMR Network 6", "TGRewrite1", "2,1000001,2,1000001,5599999");

        if (rc < 0) {
                LogInfo("Setting DMR Mode 8 Failed.....");
        }else{
               // LogInfo("Setting DMR Mode 8 Completed - Saving Changes");
         
		rc = ini.SaveFile("/etc/dmrgateway");
                if (rc < 0) {
                        LogInfo("Saving DMR Mode 8 Failed.....");
			ok2save=false;
               }else{
			 LogInfo("DMR Mode 8 Change Completed");
			ok2save=true;
                }
	system("/bin/mount -o remount,ro /");

        }

   }

   if (GWMode == 7) 
    {
        rc = ini.SetValue("General", "GWMode", "7");
        rc = ini.SetValue("DMR Network 1", "TGRewrite0", "2,1000001,2,1,999999");
        rc = ini.SetValue("DMR Network 1", "TGRewrite1", "2,1000001,2,1,999999");
        rc = ini.SetValue("DMR Network 1", "PCRewrite0", "2,1000001,2,1,999999");
        rc = ini.SetValue("DMR Network 2", "TGRewrite0", "2,2000001,2,1,999999");
        rc = ini.SetValue("DMR Network 2", "TGRewrite1", "2,2000001,2,1,999999");
        rc = ini.SetValue("DMR Network 2", "PCRewrite0", "2,2000001,2,1,999999");
        rc = ini.SetValue("DMR Network 3", "TGRewrite0", "2,3000001,2,1,999999");
        rc = ini.SetValue("DMR Network 3", "TGRewrite1", "2,3000001,2,1,999999");
        rc = ini.SetValue("DMR Network 3", "PCRewrite0", "2,3000001,2,1,999999");
        rc = ini.SetValue("DMR Network 4", "TGRewrite0", "2,4000001,2,1,999999");
        rc = ini.SetValue("DMR Network 4", "TGRewrite1", "2,4000001,2,1,999999");
        rc = ini.SetValue("DMR Network 4", "PCRewrite0", "2,4000001,2,1,999999");
        rc = ini.SetValue("DMR Network 5", "TGRewrite0", "2,5000001,2,1,999999");
        rc = ini.SetValue("DMR Network 5", "TGRewrite1", "2,5000001,2,1,999999");
        rc = ini.SetValue("DMR Network 5", "PCRewrite0", "2,5000001,2,1,999999");
        rc = ini.SetValue("DMR Network 6", "TGRewrite0", "2,6000001,2,1,999999");
        rc = ini.SetValue("DMR Network 6", "TGRewrite1", "2,6000001,2,1,999999");
        rc = ini.SetValue("DMR Network 6", "PCRewrite0", "2,6000001,2,1,999999");

        if (rc < 0) {
                LogInfo("Setting DMR Mode 7 Failed.....");
        }else{
                LogInfo("Setting DMR Mode 7 Completed - Saving Changes");
                rc = ini.SaveFile("/etc/dmrgateway");
                if (rc < 0) {
                        LogInfo("Saving DMR Mode 7 Failed.....");
			ok2save=false;

               }else{
			 LogInfo("DMR Mode 7 Change Completed");
			ok2save=true;
                }
	system("/bin/mount -o remount,ro /");

	}
   }
  if (GWMode == 1)
    {
        rc = ini.SetValue("General", "GWMode", "1");

        rc = ini.SetValue("DMR Network 1", "TGRewrite0", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 1", "TGRewrite1", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 1", "PCRewrite0", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 2", "TGRewrite0", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 2", "TGRewrite1", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 2", "PCRewrite0", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 3", "TGRewrite0", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 3", "TGRewrite1", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 3", "PCRewrite0", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 4", "TGRewrite0", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 4", "TGRewrite1", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 4", "PCRewrite0", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 5", "TGRewrite0", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 5", "TGRewrite1", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 5", "PCRewrite0", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 6", "TGRewrite0", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 6", "TGRewrite1", "2,1,2,1,9999999");
        rc = ini.SetValue("DMR Network 6", "PCRewrite0", "2,1,2,1,9999999");

        if (rc < 0) {
                LogInfo("Setting DMR Mode 1 Failed.....");
        }else{
                LogInfo("Setting DMR Mode 1 Completed - Saving Changes");
                rc = ini.SaveFile("/etc/dmrgateway");
               if (rc < 0) {
                        LogInfo("Saving DMR Mode 1 Failed.....");
			ok2save=false;
               }else{
			 LogInfo("DMR Mode 1 Change Completed");
			ok2save=true;
                }
	system("/bin/mount -o remount,ro /");
        }
SetUserMMDVM();   
}


	if ( ok2save == true ) {
		Reload();
	}


}









