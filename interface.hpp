#ifndef interface_hpp
#define interface_hpp

#define BUFSIZE 32

extern "C" struct resource {
    size_t size;
};

class Interface {
    public:
        virtual const void *getResource() {};
};

#endif
