#include "DMRGateway.h"
#include "Version.h"
#include "StopWatch.h"
#include "Thread.h"
#include "Timer.h"
#include "Log.h"
#include "DMRLC.h"
#include "Sync.h"
#include "RewriteType.h"
#include "DMRSlotType.h"
#include "RewriteSrc.h"
#include "RewritePC.h"
#include "RewriteSrcId.h"
#include "RewriteDstId.h"
#include "PassAllPC.h"
#include "PassAllTG.h"
#include "DMRFullLC.h"
#include "SimpleIni.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>

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
const unsigned char COLOR_CODE = 3U;

unsigned int rdstId = 0;
unsigned short selnet = 4;
unsigned int storedtg = 0;
// Comment out unused variables to suppress warnings; restore if used in original logic
// static bool net1ok = false;
// static bool net2ok = false;
// static bool net3ok = false;
// static bool net4ok = false;
// static bool net5ok = false;
// static bool net6ok = false;
// static bool rf1ok = false;
// static bool rf2ok = false;
// static bool rf3ok = false;
// static bool rf4ok = false;
// static bool rf5ok = false;
// static bool rf6ok = false;
// static bool ok2tx = false;
// static int locknet = 4;
unsigned short int GWMode = 8;

static bool m_killed = false;
static int m_signal = 0;

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

// Placeholder implementations; replace with original logic if provided
void ClearRFNets() { /* TODO: Restore original implementation */ }
void ClearNetworks() { /* TODO: Restore original implementation */ }
void SetDMR() { /* TODO: Restore original implementation */ }
int Reload() { return 0; /* TODO: Restore original implementation */ }
void SetUserMMDVM() { /* TODO: Restore original implementation */ }
void SetUserRoot() { /* TODO: Restore original implementation */ }

int main(int argc, char** argv)
{
    const char* iniFile = DEFAULT_INI_FILE;

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "--version") {
                ::fprintf(stdout, "DMRGateway version %s\n", VERSION);
                return 0;
            } else if (arg[0] != '-') {
                iniFile = argv[i];
            }
        }
    }

#if !defined(_WIN32) && !defined(_WIN64)
    ::signal(SIGINT,  sigHandler);
    ::signal(SIGTERM, sigHandler);
    ::signal(SIGHUP,  sigHandler);
#endif

    bool ret = ::LogInitialise(false, ".", "DMRGateway", 2U, 2U, false);
    if (!ret) {
        ::fprintf(stderr, "DMRGateway: unable to initialize logging\n");
        return 1;
    }

    ::LogInfo(HEADER1);
    ::LogInfo(HEADER2);
    ::LogInfo(HEADER3);
    ::LogInfo(HEADER4);
    ::LogInfo("Version: %s", VERSION); // Removed GITVERSION due to undeclared error

    CDMRGateway gateway(iniFile);
    int status = gateway.run();

    ::LogFinalise();
    return status;
}

CDMRGateway::CDMRGateway(const std::string& confFile) :
m_conf(confFile),
m_repeater(NULL),
m_dmrNetwork1(NULL),
m_dmrNetwork2(NULL),
m_dmrNetwork3(NULL),
m_dmrNetwork4(NULL),
m_dmrNetwork5(NULL),
m_xlxNetwork(NULL),
m_xlxId(0U),
m_xlxNumber(0U),
m_xlxReflector(0U),
m_xlxRoom(0U),
m_xlxSlot(0U),
m_xlxTG(0U),
m_xlxBase(0U),
m_xlxStartup(0U),
m_xlxConnected(false),
m_xlxRelink(1000U, 60U),
m_xlxUserControl(false),
m_xlxRewrite(NULL),
m_xlxVoice(NULL),
m_status(NULL)
{
    m_status = new DMRGW_STATUS[3U];
    ::memset(m_status, DMRGWS_NONE, 3U * sizeof(DMRGW_STATUS));
}

CDMRGateway::~CDMRGateway()
{
    delete[] m_status;
    delete m_xlxRewrite;
    delete m_xlxVoice;
    delete m_xlxNetwork;
    delete m_dmrNetwork1;
    delete m_dmrNetwork2;
    delete m_dmrNetwork3;
    delete m_dmrNetwork4;
    delete m_dmrNetwork5;
    delete m_repeater;
}

int CDMRGateway::run()
{
    bool ret = m_conf.read();
    if (!ret) {
        ::LogError("Cannot read the configuration file");
        return 1;
    }

#if !defined(_WIN32) && !defined(_WIN64)
    bool m_daemon = m_conf.getDaemon();
    if (m_daemon) {
        pid_t pid = ::fork();
        if (pid == -1) {
            ::LogError("Couldn't fork() , exiting");
            return -1;
        } else if (pid != 0) {
            exit(0);
        }

        if (::setsid() == -1) {
            ::LogError("Couldn't setsid() , exiting");
            return -1;
        }

        pid = ::fork();
        if (pid == -1) {
            ::LogError("Couldn't fork() , exiting");
            return -1;
        } else if (pid != 0) {
            exit(0);
        }
    }
#endif

    ret = ::LogInitialise(m_conf.getDaemon(), m_conf.getLogFilePath(), m_conf.getLogFileRoot(), m_conf.getLogFileLevel(), m_conf.getLogDisplayLevel(), m_conf.getLogFileRotate());
    if (!ret) {
        ::LogError("Unable to initialize the logging");
        return 1;
    }

    std::string rptAddress   = m_conf.getRptAddress();
    unsigned short rptPort   = m_conf.getRptPort();
    std::string localAddress = m_conf.getLocalAddress();
    unsigned short localPort = m_conf.getLocalPort();
    bool debug               = m_conf.getDebug();

    m_repeater = new CMMDVMNetwork(rptAddress, rptPort, localAddress, localPort, 0U, debug);
    ret = m_repeater->open();
    if (!ret) {
        ::LogError("Cannot open the MMDVM repeater network");
        delete m_repeater;
        ::LogFinalise();
        return 1;
    }

    if (m_conf.getDMRNetwork1Enabled()) {
        std::string address  = m_conf.getDMRNetwork1Address();
        unsigned short port  = m_conf.getDMRNetwork1Port();
        unsigned short local = m_conf.getDMRNetwork1Local();
        std::string password = m_conf.getDMRNetwork1Password();
        bool location        = m_conf.getDMRNetwork1Location();
        bool debug           = m_conf.getDMRNetwork1Debug();

        m_dmrNetwork1 = new CDMRNetwork(address, port, local, m_conf.getDMRNetwork1Id(), password, "DMR1", VERSION, debug);
        m_dmrNetwork1->enable(location);

        ret = m_dmrNetwork1->open();
        if (!ret) {
            ::LogError("Cannot open DMR Network 1");
            delete m_dmrNetwork1;
            m_dmrNetwork1 = NULL;
        }
    }

    if (m_conf.getDMRNetwork2Enabled()) {
        std::string address  = m_conf.getDMRNetwork2Address();
        unsigned short port  = m_conf.getDMRNetwork2Port();
        unsigned short local = m_conf.getDMRNetwork2Local();
        std::string password = m_conf.getDMRNetwork2Password();
        bool location        = m_conf.getDMRNetwork2Location();
        bool debug           = m_conf.getDMRNetwork2Debug();

        m_dmrNetwork2 = new CDMRNetwork(address, port, local, m_conf.getDMRNetwork2Id(), password, "DMR2", VERSION, debug);
        m_dmrNetwork2->enable(location);

        ret = m_dmrNetwork2->open();
        if (!ret) {
            ::LogError("Cannot open DMR Network 2");
            delete m_dmrNetwork2;
            m_dmrNetwork2 = NULL;
        }
    }

    if (m_conf.getDMRNetwork3Enabled()) {
        std::string address  = m_conf.getDMRNetwork3Address();
        unsigned short port  = m_conf.getDMRNetwork3Port();
        unsigned short local = m_conf.getDMRNetwork3Local();
        std::string password = m_conf.getDMRNetwork3Password();
        bool location        = m_conf.getDMRNetwork3Location();
        bool debug           = m_conf.getDMRNetwork3Debug();

        m_dmrNetwork3 = new CDMRNetwork(address, port, local, m_conf.getDMRNetwork3Id(), password, "DMR3", VERSION, debug);
        m_dmrNetwork3->enable(location);

        ret = m_dmrNetwork3->open();
        if (!ret) {
            ::LogError("Cannot open DMR Network 3");
            delete m_dmrNetwork3;
            m_dmrNetwork3 = NULL;
        }
    }

    if (m_conf.getDMRNetwork4Enabled()) {
        std::string address  = m_conf.getDMRNetwork4Address();
        unsigned short port  = m_conf.getDMRNetwork4Port();
        unsigned short local = m_conf.getDMRNetwork4Local();
        std::string password = m_conf.getDMRNetwork4Password();
        bool location        = m_conf.getDMRNetwork4Location();
        bool debug           = m_conf.getDMRNetwork4Debug();

        m_dmrNetwork4 = new CDMRNetwork(address, port, local, m_conf.getDMRNetwork4Id(), password, "DMR4", VERSION, debug);
        m_dmrNetwork4->enable(location);

        ret = m_dmrNetwork4->open();
        if (!ret) {
            ::LogError("Cannot open DMR Network 4");
            delete m_dmrNetwork4;
            m_dmrNetwork4 = NULL;
        }
    }

    if (m_conf.getDMRNetwork5Enabled()) {
        std::string address  = m_conf.getDMRNetwork5Address();
        unsigned short port  = m_conf.getDMRNetwork5Port();
        unsigned short local = m_conf.getDMRNetwork5Local();
        std::string password = m_conf.getDMRNetwork5Password();
        bool location        = m_conf.getDMRNetwork5Location();
        bool debug           = m_conf.getDMRNetwork5Debug();

        m_dmrNetwork5 = new CDMRNetwork(address, port, local, m_conf.getDMRNetwork5Id(), password, "DMR5", VERSION, debug);
        m_dmrNetwork5->enable(location);

        ret = m_dmrNetwork5->open();
        if (!ret) {
            ::LogError("Cannot open DMR Network 5");
            delete m_dmrNetwork5;
            m_dmrNetwork5 = NULL;
        }
    }

    if (m_conf.getXLXNetworkEnabled()) {
        std::string file      = m_conf.getXLXNetworkFile();
        unsigned short port   = m_conf.getXLXNetworkPort();
        std::string password  = m_conf.getXLXNetworkPassword();
        unsigned short local  = m_conf.getXLXNetworkLocal();
        bool debug            = m_conf.getXLXNetworkDebug();

        m_xlxNetwork = new CDMRNetwork(file, port, local, m_conf.getDMRNetwork1Id(), password, "XLX", VERSION, debug);

        ret = m_xlxNetwork->open();
        if (!ret) {
            ::LogError("Cannot open XLX Network");
            delete m_xlxNetwork;
            m_xlxNetwork = NULL;
        } else {
            m_xlxRewrite = new CRewriteTG("XLX", m_conf.getXLXNetworkSlot(), m_conf.getXLXNetworkTG(), m_conf.getXLXNetworkSlot(), m_conf.getXLXNetworkTG(), 1U);
        }
    }

    if (m_dmrNetwork1)
        m_dmrNetwork1->enable(true);
    if (m_dmrNetwork2)
        m_dmrNetwork2->enable(true);
    if (m_dmrNetwork3)
        m_dmrNetwork3->enable(true);
    if (m_dmrNetwork4)
        m_dmrNetwork4->enable(true);
    if (m_dmrNetwork5)
        m_dmrNetwork5->enable(true);

    m_xlxId        = m_conf.getDMRNetwork1Id();
    m_xlxSlot      = m_conf.getXLXNetworkSlot();
    m_xlxTG        = m_conf.getXLXNetworkTG();
    m_xlxBase      = m_conf.getXLXNetworkBase();
    m_xlxRoom      = m_conf.getXLXNetworkStartup();
    m_xlxUserControl = m_conf.getXLXNetworkUserControl();
    m_xlxStartup   = m_conf.getXLXNetworkStartup();
    if (m_xlxRoom != 0U)
        m_xlxReflector = m_xlxRoom;

    bool voiceEnabled = m_conf.getVoiceEnabled();
    if (voiceEnabled) {
        std::string language  = m_conf.getVoiceLanguage();
        std::string directory = m_conf.getVoiceDirectory();

        m_xlxVoice = new CXLXVoice(directory, language, m_conf.getDMRNetwork1Id(), m_xlxSlot, m_xlxTG);
        ret = m_xlxVoice->open();
        if (!ret) {
            ::LogError("Cannot open the XLX Voice");
            delete m_xlxVoice;
            m_xlxVoice = NULL;
        }
    }

    unsigned int netTimeout = m_conf.getNetTimeout();

    CTimer* timer[3U];
    timer[1U] = new CTimer(1000U, 0U, 0U);
    timer[2U] = new CTimer(1000U, 0U, 0U);

    unsigned int rfSrcId[3U];
    unsigned int rfDstId[3U];
    rfSrcId[1U] = rfSrcId[2U] = rfDstId[1U] = rfDstId[2U] = 0U;

    unsigned int dmr1SrcId[3U];
    unsigned int dmr1DstId[3U];
    dmr1SrcId[1U] = dmr1SrcId[2U] = dmr1DstId[1U] = dmr1DstId[2U] = 0U;

    unsigned int dmr2SrcId[3U];
    unsigned int dmr2DstId[3U];
    dmr2SrcId[1U] = dmr2SrcId[2U] = dmr2DstId[1U] = dmr2DstId[2U] = 0U;

    unsigned int dmr3SrcId[3U];
    unsigned int dmr3DstId[3U];
    dmr3SrcId[1U] = dmr3SrcId[2U] = dmr3DstId[1U] = dmr3DstId[2U] = 0U;

    unsigned int dmr4SrcId[3U];
    unsigned int dmr4DstId[3U];
    dmr4SrcId[1U] = dmr4SrcId[2U] = dmr4DstId[1U] = dmr4DstId[2U] = 0U;

    unsigned int dmr5SrcId[3U];
    unsigned int dmr5DstId[3U];
    dmr5SrcId[1U] = dmr5SrcId[2U] = dmr5DstId[1U] = dmr5DstId[2U] = 0U;

    CStopWatch stopWatch;
    stopWatch.start();

    LogMessage("DMRGateway-%s is running", VERSION);

    while (!m_killed) {
        if (m_xlxNetwork) {
            bool connected = m_xlxNetwork->isConnected();
            if (connected && !m_xlxConnected) {
                if (m_xlxReflector >= 4001U && m_xlxReflector <= 4026U) {
                    writeXLXLink(m_xlxId, m_xlxReflector, m_xlxNetwork);
                    char c = ('A' + (m_xlxReflector % 100U)) - 1U;
                    LogMessage("XLX, Linking to reflector XLX%u %c", m_xlxNumber, c);
                    if (m_xlxVoice != NULL) {
                        CDMRData data;
                        m_xlxVoice->linked(data, m_conf.getDMRNetwork1Name(), m_xlxNumber, m_xlxReflector);
                        m_repeater->write(data);
                    }
                } else if (m_xlxRoom >= 4001U && m_xlxRoom <= 4026U) {
                    writeXLXLink(m_xlxId, m_xlxRoom, m_xlxNetwork);
                    char c = ('A' + (m_xlxRoom % 100U)) - 1U;
                    LogMessage("XLX, Linking to reflector XLX%u %c", m_xlxNumber, c);
                    if (m_xlxVoice != NULL) {
                        CDMRData data;
                        m_xlxVoice->linked(data, m_conf.getDMRNetwork1Name(), m_xlxNumber, m_xlxRoom);
                        m_repeater->write(data);
                    }
                    m_xlxReflector = m_xlxRoom;
                } else {
                    if (m_xlxVoice != NULL) {
                        CDMRData data;
                        m_xlxVoice->linked(data, m_conf.getDMRNetwork1Name(), m_xlxNumber, 0U);
                        m_repeater->write(data);
                    }
                }

                m_xlxConnected = true;

                if (m_xlxNumber == m_xlxStartup && m_xlxRoom == m_xlxReflector)
                    m_xlxRelink.stop();
                else
                    m_xlxRelink.start();
            } else if (!connected && m_xlxConnected) {
                LogMessage("XLX, Unlinking from XLX%u due to loss of connection", m_xlxNumber);

                if (m_xlxVoice != NULL) {
                    CDMRData data;
                    m_xlxVoice->unlinked(data);
                    m_repeater->write(data);
                }

                m_xlxConnected = false;
                m_xlxRelink.stop();
            } else if (connected && m_xlxRelink.isRunning() && m_xlxRelink.hasExpired()) {
                m_xlxRelink.stop();

                if (m_xlxNumber != m_xlxStartup) {
                    if (m_xlxStartup != 4000U) {
                        m_xlxReflector = 4000U;
                        char c = ('A' + (m_xlxRoom % 100U)) - 1U;
                        LogMessage("XLX, Re-linking to startup reflector XLX%u %c due to RF inactivity timeout", m_xlxNumber, c);
                        linkXLX(m_xlxStartup);
                    } else {
                        LogMessage("XLX, Unlinking from XLX%u due to RF inactivity timeout", m_xlxNumber);
                        unlinkXLX();
                    }
                } else {
                    if (m_xlxReflector >= 4001U && m_xlxReflector <= 4026U)
                        writeXLXLink(m_xlxId, 4000U, m_xlxNetwork);

                    if (m_xlxRoom >= 4001U && m_xlxRoom <= 4026U) {
                        writeXLXLink(m_xlxId, m_xlxRoom, m_xlxNetwork);
                        char c = ('A' + (m_xlxRoom % 100U)) - 1U;
                        LogMessage("XLX, Re-linking to startup reflector XLX%u %c due to RF inactivity timeout", m_xlxNumber, c);
                    } else if (m_xlxReflector >= 4001U && m_xlxReflector <= 4026U) {
                        char c = ('A' + (m_xlxReflector % 100U)) - 1U;
                        LogMessage("XLX, Unlinking from reflector XLX%u %c due to RF inactivity timeout", m_xlxNumber, c);
                    }

                    m_xlxReflector = m_xlxRoom;
                    if (m_xlxVoice != NULL) {
                        CDMRData data;
                        if (m_xlxReflector < 4001U || m_xlxReflector > 4026U)
                            m_xlxVoice->linked(data, m_conf.getDMRNetwork1Name(), m_xlxNumber, 0U);
                        else
                            m_xlxVoice->linked(data, m_conf.getDMRNetwork1Name(), m_xlxNumber, m_xlxReflector);
                        m_repeater->write(data);
                    }
                }
            }
        }

        CDMRData data;

        bool ret = m_repeater->read(data);
if (ret) {
    unsigned int slotNo = data.getSlotNo();
    LogDebug("RF DMR: slot=%u, src=%u, dst=%u, flco=%d, status=%d", slotNo, data.getSrcId(), data.getDstId(), data.getFLCO(), m_status[slotNo]);
} else {
    LogDebug("No RF data received");
}

        if (ret) {
            unsigned int slotNo = data.getSlotNo();
            unsigned int srcId = data.getSrcId();
            unsigned int dstId = data.getDstId();
            FLCO flco = data.getFLCO();

            std::string mode;
            unsigned int network = 0U;
            unsigned int tg = dstId;

            if (dstId >= 9001U && dstId <= 9007U) {
                mode = "Raw";
                if (dstId == 9007U) {
                    network = 0U; // XLX
                } else {
                    network = dstId - 9000U; // DMR Network 1-5
                    switch (network) {
                        case 1U: tg = m_conf.getDMRNetwork1Id(); break;
                        case 2U: tg = m_conf.getDMRNetwork2Id(); break;
                        case 3U: tg = m_conf.getDMRNetwork3Id(); break;
                        case 4U: tg = m_conf.getDMRNetwork4Id(); break;
                        case 5U: tg = m_conf.getDMRNetwork5Id(); break;
                        default: break;
                    }
                }
            } else if (dstId >= 1000000U && dstId <= 9999999U) {
                mode = "Seven Digit";
                unsigned int prefix = dstId / 1000000U;
                if (prefix >= 1U && prefix <= 5U) {
                    network = prefix;
                    tg = dstId % 1000000U;
                }
            } else if (dstId >= 11000000U && dstId <= 15999999U) {
                mode = "Eight Digit";
                unsigned int prefix = dstId / 1000000U;
                if (prefix >= 11U && prefix <= 15U) {
                    network = prefix - 10U;
                    tg = dstId % 1000000U;
                }
            }

            if (m_xlxVoice != NULL && m_conf.getVoiceEnabled() && network > 0U && network <= 5U) {
                std::string networkName;
                switch (network) {
                    case 1U: networkName = m_conf.getDMRNetwork1Name(); break;
                    case 2U: networkName = m_conf.getDMRNetwork2Name(); break;
                    case 3U: networkName = m_conf.getDMRNetwork3Name(); break;
                    case 4U: networkName = m_conf.getDMRNetwork4Name(); break;
                    case 5U: networkName = m_conf.getDMRNetwork5Name(); break;
                    default: break;
                }
                m_xlxVoice->announce(mode, networkName, tg);
            } else if (m_xlxVoice != NULL && m_conf.getVoiceEnabled() && network == 0U && dstId == 9007U) {
                m_xlxVoice->announce(mode, "XLX", tg);
            }

            if (flco == FLCO_GROUP && slotNo == m_xlxSlot && dstId == m_xlxTG) {
                if (m_xlxReflector != m_xlxRoom || m_xlxNumber != m_xlxStartup)
                    m_xlxRelink.start();

                m_xlxRewrite->process(data, false);
                if (m_xlxNetwork)
                    m_xlxNetwork->write(data);
                m_status[slotNo] = DMRGWS_XLXREFLECTOR;
                timer[slotNo]->setTimeout(m_conf.getRFTimeout());
                timer[slotNo]->start();
            } else if ((dstId <= (m_xlxBase + 26U) || dstId == (m_xlxBase + 1000U)) && flco == FLCO_USER_USER && slotNo == m_xlxSlot && dstId >= m_xlxBase && m_xlxUserControl) {
                dstId += 4000U;
                dstId -= m_xlxBase;

                if (dstId != m_xlxReflector) {
                    if (dstId == 4000U) {
                        writeXLXLink(srcId, 4000U, m_xlxNetwork);
                        m_xlxReflector = 4000U;
                        char c = ('A' + (m_xlxRoom % 100U)) - 1U;
                        LogMessage("XLX, Unlinking from reflector XLX%u %c", m_xlxNumber, c);
                    } else if (dstId != 5000U) {
                        if (m_xlxReflector != 4000U)
                            writeXLXLink(srcId, 4000U, m_xlxNetwork);
                        writeXLXLink(srcId, dstId, m_xlxNetwork);
                        m_xlxReflector = dstId;
                        char c = ('A' + (m_xlxReflector % 100U)) - 1U;
                        LogMessage("XLX, Linking to reflector XLX%u %c", m_xlxNumber, c);
                    }

                    if (m_xlxVoice != NULL) {
                        CDMRData voiceData;
                        if (dstId < 4001U || dstId > 4026U)
                            m_xlxVoice->linked(voiceData, m_conf.getDMRNetwork1Name(), m_xlxNumber, 0U);
                        else
                            m_xlxVoice->linked(voiceData, m_conf.getDMRNetwork1Name(), m_xlxNumber, dstId);
                        m_repeater->write(voiceData);
                    }
                }
            } else {
                if (m_dmrNetwork1) {
                    if (m_status[slotNo] == DMRGWS_NONE || m_status[slotNo] == DMRGWS_DMRNETWORK1) {
                        m_dmrNetwork1->write(data);
                        timer[slotNo]->setTimeout(m_conf.getRFTimeout());
                        timer[slotNo]->start();
                        m_status[slotNo] = DMRGWS_DMRNETWORK1;

                        if (srcId != dmr1SrcId[slotNo] || dstId != dmr1DstId[slotNo]) {
                            LogMessage("DMR Slot %u: RF -> DMR Network 1: %u -> %u", slotNo, srcId, dstId);
                            dmr1SrcId[slotNo] = srcId;
                            dmr1DstId[slotNo] = dstId;
                        }
                    }
                }

                if (m_dmrNetwork2) {
                    if (m_status[slotNo] == DMRGWS_NONE || m_status[slotNo] == DMRGWS_DMRNETWORK2) {
                        m_dmrNetwork2->write(data);
                        timer[slotNo]->setTimeout(m_conf.getRFTimeout());
                        timer[slotNo]->start();
                        m_status[slotNo] = DMRGWS_DMRNETWORK2;

                        if (srcId != dmr2SrcId[slotNo] || dstId != dmr2DstId[slotNo]) {
                            LogMessage("DMR Slot %u: RF -> DMR Network 2: %u -> %u", slotNo, srcId, dstId);
                            dmr2SrcId[slotNo] = srcId;
                            dmr2DstId[slotNo] = dstId;
                        }
                    }
                }

                if (m_dmrNetwork3) {
                    if (m_status[slotNo] == DMRGWS_NONE || m_status[slotNo] == DMRGWS_DMRNETWORK3) {
                        m_dmrNetwork3->write(data);
                        timer[slotNo]->setTimeout(m_conf.getRFTimeout());
                        timer[slotNo]->start();
                        m_status[slotNo] = DMRGWS_DMRNETWORK3;

                        if (srcId != dmr3SrcId[slotNo] || dstId != dmr3DstId[slotNo]) {
                            LogMessage("DMR Slot %u: RF -> DMR Network 3: %u -> %u", slotNo, srcId, dstId);
                            dmr3SrcId[slotNo] = srcId;
                            dmr3DstId[slotNo] = dstId;
                        }
                    }
                }

                if (m_dmrNetwork4) {
                    if (m_status[slotNo] == DMRGWS_NONE || m_status[slotNo] == DMRGWS_DMRNETWORK4) {
                        m_dmrNetwork4->write(data);
                        timer[slotNo]->setTimeout(m_conf.getRFTimeout());
                        timer[slotNo]->start();
                        m_status[slotNo] = DMRGWS_DMRNETWORK4;

                        if (srcId != dmr4SrcId[slotNo] || dstId != dmr4DstId[slotNo]) {
                            LogMessage("DMR Slot %u: RF -> DMR Network 4: %u -> %u", slotNo, srcId, dstId);
                            dmr4SrcId[slotNo] = srcId;
                            dmr4DstId[slotNo] = dstId;
                        }
                    }
                }

                if (m_dmrNetwork5) {
                    if (m_status[slotNo] == DMRGWS_NONE || m_status[slotNo] == DMRGWS_DMRNETWORK5) {
                        m_dmrNetwork5->write(data);
                        timer[slotNo]->setTimeout(m_conf.getRFTimeout());
                        timer[slotNo]->start();
                        m_status[slotNo] = DMRGWS_DMRNETWORK5;

                        if (srcId != dmr5SrcId[slotNo] || dstId != dmr5DstId[slotNo]) {
                            LogMessage("DMR Slot %u: RF -> DMR Network 5: %u -> %u", slotNo, srcId, dstId);
                            dmr5SrcId[slotNo] = srcId;
                            dmr5DstId[slotNo] = dstId;
                        }
                    }
                }
            }
        }

        if (m_dmrNetwork1) {
            while (m_dmrNetwork1->read(data)) {
                unsigned int slotNo = data.getSlotNo();
                unsigned int srcId = data.getSrcId();
                unsigned int dstId = data.getDstId();

                if (m_status[slotNo] == DMRGWS_NONE || m_status[slotNo] == DMRGWS_DMRNETWORK1) {
                    m_repeater->write(data);
                    timer[slotNo]->setTimeout(netTimeout);
                    timer[slotNo]->start();
                    m_status[slotNo] = DMRGWS_DMRNETWORK1;

                    if (srcId != dmr1SrcId[slotNo] || dstId != dmr1DstId[slotNo]) {
                        LogMessage("DMR Slot %u: DMR Network 1 -> RF: %u -> %u", slotNo, srcId, dstId);
                        dmr1SrcId[slotNo] = srcId;
                        dmr1DstId[slotNo] = dstId;
                    }
                }
            }
        }

        if (m_dmrNetwork2) {
            while (m_dmrNetwork2->read(data)) {
                unsigned int slotNo = data.getSlotNo();
                unsigned int srcId = data.getSrcId();
                unsigned int dstId = data.getDstId();

                if (m_status[slotNo] == DMRGWS_NONE || m_status[slotNo] == DMRGWS_DMRNETWORK2) {
                    m_repeater->write(data);
                    timer[slotNo]->setTimeout(netTimeout);
                    timer[slotNo]->start();
                    m_status[slotNo] = DMRGWS_DMRNETWORK2;

                    if (srcId != dmr2SrcId[slotNo] || dstId != dmr2DstId[slotNo]) {
                        LogMessage("DMR Slot %u: DMR Network 2 -> RF: %u -> %u", slotNo, srcId, dstId);
                        dmr2SrcId[slotNo] = srcId;
                        dmr2DstId[slotNo] = dstId;
                    }
                }
            }
        }

        if (m_dmrNetwork3) {
            while (m_dmrNetwork3->read(data)) {
                unsigned int slotNo = data.getSlotNo();
                unsigned int srcId = data.getSrcId();
                unsigned int dstId = data.getDstId();

                if (m_status[slotNo] == DMRGWS_NONE || m_status[slotNo] == DMRGWS_DMRNETWORK3) {
                    m_repeater->write(data);
                    timer[slotNo]->setTimeout(netTimeout);
                    timer[slotNo]->start();
                    m_status[slotNo] = DMRGWS_DMRNETWORK3;

                    if (srcId != dmr3SrcId[slotNo] || dstId != dmr3DstId[slotNo]) {
                        LogMessage("DMR Slot %u: DMR Network 3 -> RF: %u -> %u", slotNo, srcId, dstId);
                        dmr3SrcId[slotNo] = srcId;
                        dmr3DstId[slotNo] = dstId;
                    }
                }
            }
        }

        if (m_dmrNetwork4) {
            while (m_dmrNetwork4->read(data)) {
                unsigned int slotNo = data.getSlotNo();
                unsigned int srcId = data.getSrcId();
                unsigned int dstId = data.getDstId();

                if (m_status[slotNo] == DMRGWS_NONE || m_status[slotNo] == DMRGWS_DMRNETWORK4) {
                    m_repeater->write(data);
                    timer[slotNo]->setTimeout(netTimeout);
                    timer[slotNo]->start();
                    m_status[slotNo] = DMRGWS_DMRNETWORK4;

                    if (srcId != dmr4SrcId[slotNo] || dstId != dmr4DstId[slotNo]) {
                        LogMessage("DMR Slot %u: DMR Network 4 -> RF: %u -> %u", slotNo, srcId, dstId);
                        dmr4SrcId[slotNo] = srcId;
                        dmr4DstId[slotNo] = dstId;
                    }
                }
            }
        }

        if (m_dmrNetwork5) {
            while (m_dmrNetwork5->read(data)) {
                unsigned int slotNo = data.getSlotNo();
                unsigned int srcId = data.getSrcId();
                unsigned int dstId = data.getDstId();

                if (m_status[slotNo] == DMRGWS_NONE || m_status[slotNo] == DMRGWS_DMRNETWORK5) {
                    m_repeater->write(data);
                    timer[slotNo]->setTimeout(netTimeout);
                    timer[slotNo]->start();
                    m_status[slotNo] = DMRGWS_DMRNETWORK5;

                    if (srcId != dmr5SrcId[slotNo] || dstId != dmr5DstId[slotNo]) {
                        LogMessage("DMR Slot %u: DMR Network 5 -> RF: %u -> %u", slotNo, srcId, dstId);
                        dmr5SrcId[slotNo] = srcId;
                        dmr5DstId[slotNo] = dstId;
                    }
                }
            }
        }

        if (m_xlxNetwork) {
            while (m_xlxNetwork->read(data)) {
                unsigned int slotNo = data.getSlotNo();
                unsigned int srcId = data.getSrcId();
                unsigned int dstId = data.getDstId();

                if (m_status[slotNo] == DMRGWS_NONE || m_status[slotNo] == DMRGWS_XLXREFLECTOR) {
                    m_xlxRewrite->process(data, true);
                    m_repeater->write(data);
                    timer[slotNo]->setTimeout(netTimeout);
                    timer[slotNo]->start();
                    m_status[slotNo] = DMRGWS_XLXREFLECTOR;

                    LogMessage("DMR Slot %u: XLX -> RF: %u -> %u", slotNo, srcId, dstId);
                }
            }
        }

        for (unsigned int i = 1U; i <= 2U; i++) {
            if (timer[i]->isRunning() && timer[i]->hasExpired()) {
                timer[i]->stop();
                m_status[i] = DMRGWS_NONE;
            }
        }

        unsigned int ms = stopWatch.elapsed();
        if (ms > 10U) {
            stopWatch.start();

            if (m_dmrNetwork1)
                m_dmrNetwork1->clock(ms);
            if (m_dmrNetwork2)
                m_dmrNetwork2->clock(ms);
            if (m_dmrNetwork3)
                m_dmrNetwork3->clock(ms);
            if (m_dmrNetwork4)
                m_dmrNetwork4->clock(ms);
            if (m_dmrNetwork5)
                m_dmrNetwork5->clock(ms);
            if (m_xlxNetwork)
                m_xlxNetwork->clock(ms);

            for (unsigned int i = 1U; i <= 2U; i++)
                timer[i]->clock(ms);

            m_xlxRelink.clock(ms);
        }

        if (m_xlxVoice != NULL)
            m_xlxVoice->clock(ms);

        CThread::sleep(10U);
    }

    ::LogInfo("DMRGateway is exiting on receipt of signal %d", m_signal);

    if (m_dmrNetwork1) {
        m_dmrNetwork1->close(true);
        delete m_dmrNetwork1;
        m_dmrNetwork1 = NULL;
    }
    if (m_dmrNetwork2) {
        m_dmrNetwork2->close(true);
        delete m_dmrNetwork2;
        m_dmrNetwork2 = NULL;
    }
    if (m_dmrNetwork3) {
        m_dmrNetwork3->close(true);
        delete m_dmrNetwork3;
        m_dmrNetwork3 = NULL;
    }
    if (m_dmrNetwork4) {
        m_dmrNetwork4->close(true);
        delete m_dmrNetwork4;
        m_dmrNetwork4 = NULL;
    }
    if (m_dmrNetwork5) {
        m_dmrNetwork5->close(true);
        delete m_dmrNetwork5;
        m_dmrNetwork5 = NULL;
    }
    if (m_xlxNetwork) {
        m_xlxNetwork->close(true);
        delete m_xlxNetwork;
        m_xlxNetwork = NULL;
    }
    if (m_repeater) {
        m_repeater->close();
        delete m_repeater;
        m_repeater = NULL;
    }

    return 0;
}

bool CDMRGateway::linkXLX(unsigned int number)
{
    m_xlxNumber = number;

    if (m_xlxVoice != NULL) {
        CDMRData data;
        m_xlxVoice->linked(data, m_conf.getDMRNetwork1Name(), m_xlxNumber, 0U);
        m_repeater->write(data);
    }

    if (m_xlxReflector != 4000U)
        writeXLXLink(m_xlxId, 4000U, m_xlxNetwork);

    return true;
}

void CDMRGateway::unlinkXLX()
{
    if (m_xlxVoice != NULL) {
        CDMRData data;
        m_xlxVoice->unlinked(data);
        m_repeater->write(data);
    }

    if (m_xlxReflector != 4000U)
        writeXLXLink(m_xlxId, 4000U, m_xlxNetwork);

    m_xlxReflector = 4000U;
}

void CDMRGateway::writeXLXLink(unsigned int sourceId, unsigned int destId, CDMRNetwork* network)
{
    CDMRData data;

    data.setSrcId(sourceId);
    data.setDstId(destId);
    data.setFLCO(FLCO_USER_USER);
    data.setSlotNo(m_xlxSlot);
    data.setDataType(DT_VOICE_LC_HEADER);

    unsigned char buffer[DMR_FRAME_LENGTH_BYTES];
    data.getData(buffer);
    network->write(data);

    data.setDataType(DT_VOICE);
    for (unsigned int i = 0U; i < 5U; i++) {
        data.getData(buffer);
        network->write(data);
    }

    data.setDataType(DT_TERMINATOR_WITH_LC);
    data.getData(buffer);
    network->write(data);
}

void CDMRGateway::buildNetworkStatusString(std::string& replyStr)
{
    replyStr.clear();
    if (m_dmrNetwork1) {
        replyStr += "DMR Network 1: " + std::string(m_dmrNetwork1->isConnected() ? "Connected" : "Disconnected") + "\n";
    }
    if (m_dmrNetwork2) {
        replyStr += "DMR Network 2: " + std::string(m_dmrNetwork2->isConnected() ? "Connected" : "Disconnected") + "\n";
    }
    if (m_dmrNetwork3) {
        replyStr += "DMR Network 3: " + std::string(m_dmrNetwork3->isConnected() ? "Connected" : "Disconnected") + "\n";
    }
    if (m_dmrNetwork4) {
        replyStr += "DMR Network 4: " + std::string(m_dmrNetwork4->isConnected() ? "Connected" : "Disconnected") + "\n";
    }
    if (m_dmrNetwork5) {
        replyStr += "DMR Network 5: " + std::string(m_dmrNetwork5->isConnected() ? "Connected" : "Disconnected") + "\n";
    }
    if (m_xlxNetwork) {
        replyStr += "XLX Network: ";
        if (m_xlxConnected) {
            if (m_xlxReflector >= 4001U && m_xlxReflector <= 4026U) {
                char c = ('A' + (m_xlxReflector % 100U)) - 1U;
                replyStr += "Connected to XLX" + std::to_string(m_xlxNumber) + " " + c;
            } else {
                replyStr += "Connected to XLX" + std::to_string(m_xlxNumber);
            }
        } else {
            replyStr += "Disconnected";
        }
        replyStr += "\n";
    }
}
