#include "CamReaderWrapper.hpp"

#define MASTER "129.242.157.214" // CAKE
#define PIX_FORMAT NorthLight::NL_PIX_FMT_YUYV

CamReaderWrapper::CamReaderWrapper() {
    initialized = false;
}

CamReaderWrapper::~CamReaderWrapper() {
    if (!initialized) return;

    delete reader;
    delete frameInfo;
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

        bufferSize = width * height * 2; // Allocate enough space for two cameras

        frameInfo = reader.initSlave(cams, width, height, frameRate, PIX_FORMAT, MASTER, port);
    } else { // CAKE
        cams.push_back(std::string("gige://192.168.1.2/00305314BC7A"));
        cams.push_back(std::string("gige://192.168.1.2/003053138FBA"));
        cams.push_back(std::string("gige://192.168.1.2/00305314BC75"));

        bufferSize = width * height * 3; // Allocate enough space for two cameras

        frameInfo = reader.initSlave(cams, width, height, frameRate, PIX_FORMAT, dur, exposure, port);
    }

    reader.startStreaming();

    initialized = true;

    return true;
}

const void *CamReaderWrapper::getResource(void *buffer) {
    if (!initialized) return NULL;

    // If no buffer is given, allocate a new one
    if (buffer == NULL) buffer = malloc(bufferSize);

    // Malloc failed
    if (buffer == NULL) return NULL;

    // Read frame to buffer
    bool s = reader.readFrames((unsigned char *)buffer, true);
    if (!s) return NULL;

    // Wait untill the frame is ready
    s = reader.getFrames(timestamp);
    if (!s) return NULL; // TODO Dropped frame

    return (const void *)buffer;
}
