#ifndef camreaderwrapper_hpp
#define camreaderwrapper_hpp

#include <stdint.h>

#include "../interface.hpp"
#include "../MultiMachineReader/MultiMachineReader.hpp"

class CamReaderWrapper : public Interface {
    public:
        CamReaderWrapper();
        ~CamReaderWrapper();
        bool setup(bool slave, uint32_t port, uint32_t seconds, double exposure);
        const void *getResource(void *buffer = NULL);

    private:
        bool initialized;

        uint32_t frameRate;
        uint32_t height;
        uint32_t width

        size_t bufferSize;

        std::string timestamp;

        MultiMachineReader *reader;
        FrameSetInfo *frameInfo;
};

#endif
