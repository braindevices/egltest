//
// Created by dracula on 1/26/18.
//

#ifndef EGLTEST_MULTITHREADEGL_HPP
#define EGLTEST_MULTITHREADEGL_HPP
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
typedef boost::chrono::duration<float> fsec;
inline void wait(float seconds) {
    boost::this_thread::sleep_for(fsec{seconds});
}

static const int MAX_DEVICES = 4;

class multithreadegl {
public:
    multithreadegl(int myid, float idelsec, EGLint &configAttribs[], EGLint &pbufferAttribs[]);
    void eglwork();

private:
    int _myid;
    float _idelsec;
    EGLDeviceEXT eglDevs[MAX_DEVICES];
    EGLint numDevices;
};


#endif //EGLTEST_MULTITHREADEGL_HPP
