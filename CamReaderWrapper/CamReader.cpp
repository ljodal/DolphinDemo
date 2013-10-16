#include "CamReader.hpp"

#define MASTER "129.242.157.214" // CAKE
#define PIX_FORMAT NorthLight::NL_PIX_FMT_YUYV

CamReaderWrapper::CamReaderWrapper() {
    initialized = false;
}

bool CamReaderWrapper::setup(bool slave, uint32_t port, uint32_t seconds, double exposure) {
    reader = new MultiMachineReader();
    std::vector<std::string> cams;

    frameRate = 25;
    width = 1920;
    height = 1080;
    dur = seconds * frameRate;

    if (slave) { // TOPPING
        cams.push_back(std::string("gige://alfheim04.cs.uit.no/00305314BC77"));
        cams.push_back(std::string("gige://alfheim04.cs.uit.no/003053151E46"));

        frameInfo = reader.initSlave(cams, width, height, frameRate, PIX_FORMAT, MASTER, port);
    } else { // CAKE
        cams.push_back(std::string("gige://192.168.1.2/00305314BC7A"));
        cams.push_back(std::string("gige://192.168.1.2/003053138FBA"));
        cams.push_back(std::string("gige://192.168.1.2/00305314BC75"));

        frameInfo = reader.initSlave(cams, width, height, frameRate, PIX_FORMAT, dur, exposure, port);
    }

    initialized = true;

    return true;
}

const void *CamReaderWrapper::getResource(void *buffer) {
    if (!initialized) return NULL;
}

CamReaderWrapper::~CamReaderWrapper() {
    if (!initialized) return;

    delete reader;
    delete frameInfo;
}
