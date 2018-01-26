//
// Created by dracula on 1/26/18.
//

#include "multithreadegl.hpp"

multithreadegl::multithreadegl(
        int myid,
        float idelsec,
        EGLint &configAttribs[],
        EGLint &pbufferAttribs[]
) :
        _myid{_myid},
        _idelsec{_idelsec}
{
    PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT = (PFNEGLQUERYDEVICESEXTPROC)eglGetProcAddress("eglQueryDevicesEXT");
    eglQueryDevicesEXT(MAX_DEVICES, eglDevs, &numDevices);
    printf("Detected %d devices\n", numDevices);
};

void multithreadegl::eglwork() {
    // 1. Initialize EGL
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =(PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");

    auto eglDpy = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, eglDevs[0], 0);
    printf("worker%02d: eglDpy=&%#lx\n", _myid, size_t(eglDpy));
    EGLint major, minor;
    eglInitialize(eglDpy, &major, &minor);
    printf("woerker%02d: egl version:%d.%d\n", _myid, major, minor);
    // 2. Select an appropriate configuration
    EGLint numConfigs;
    EGLConfig eglCfg;
    eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);
    // 3. Create a surface
    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg,
                                                 pbufferAttribs);
    // 4. Bind the API
    eglBindAPI(EGL_OPENGL_API);
    // 5. Create a context and make it current
    EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT,
                                         NULL);
    eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);
    for (auto _i=0; _i<10; _i++) {
        wait(_idelsec);
        printf("woerker%02d: work at %d\n", _myid, _i);
    }
    // from now on use your OpenGL context
    // 6. Terminate EGL when finished
    eglTerminate(eglDpy);
}
