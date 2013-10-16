#ifndef moduleinterface_hpp
#define moduleinterface_hpp

#define BUFSIZE 32

extern "C" struct resource {
    size_t size;
};

class ModuleInterface {
    public:
        virtual const void *getResource(void *buffer = NULL) {return NULL;};
};

#endif
