//
// Created by dracula on 1/26/18.
//

#include "EglWorker.hpp"
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#define GL_LUMINANCE GL_RED
#define GL_LUMINANCE_ALPHA GL_RG

#include <boost/format.hpp>
#include <opencv2/imgproc/imgproc.hpp>

EglWorker::EglWorker(
        int myid,
        float idelsec,
        const EGLint * configAttribs,
        const EGLint * pbufferAttribs
) :
        _myid{myid},
        _idelsec{idelsec},
        _configAttribs{configAttribs}
//        _pbufferAttribs{pbufferAttribs}

{
    PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT = (PFNEGLQUERYDEVICESEXTPROC)eglGetProcAddress("eglQueryDevicesEXT");
    eglQueryDevicesEXT(MAX_DEVICES, eglDevs, &numDevices);
    printf("Detected %d devices\n", numDevices);
};

void check_complete()
{
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::string message;
        if (status == GL_FRAMEBUFFER_UNSUPPORTED)
            message = "framebuffer unsupported (after attaching texture)";
        else
            message = (boost::format{"incomplete framebuffer after attaching texture: %1%"} % status).str();
        throw std::runtime_error{message};
    }
}

cv::Mat3b read_rgb(GLenum format)
{
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    // position (vp[0], vp[1]), Size2D(vp[2], vp[3])};
    printf("vp=position(%d, %d) size (%d, %d)", vp[0], vp[1], vp[2], vp[3]);
    cv::Mat3b data(vp[3], vp[2]);
    glViewport(vp[0], vp[1], vp[2], vp[3]);
    glReadPixels
            (
                    vp[0],
                    vp[1],
                    vp[2],
                    vp[3],
                    format,
                    GL_UNSIGNED_BYTE,
                    data.data
            );
    return data;
}

void EglWorker::eglwork() {
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
    eglChooseConfig(eglDpy, _configAttribs, &eglCfg, 1, &numConfigs);
    // 3. Create a surface
//    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg,
//                                                 _pbufferAttribs);
    //we can do surface free context

    // 4. Bind the API
    eglBindAPI(EGL_OPENGL_API);
    // 5. Create a context and make it current
    EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT,
                                         NULL);
    eglMakeCurrent(eglDpy, NULL, NULL, eglCtx);
//    GLint s[1];
//    glGetIntegerv(GL_MAX_TEXTURE_SIZE, s);
//    printf("GL_MAX_TEXTURE_SIZE=%u\n", s[0]);
//    GLuint renderedTexture;
//    glGenTextures(1, &renderedTexture);
//    GLenum _target = GL_TEXTURE_2D;
//    GLenum internal_format = GL_RGB8;
//    glBindTexture(_target, renderedTexture);
//    GLsizei _width{1920}, _height{1080};
//    glTexStorage2D(_target, 1, internal_format, _width, _height);
//
//    GLuint FramebufferName;
//    glGenFramebuffers(1, &FramebufferName);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _target, renderedTexture, 0);
//    check_complete();
//
//    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
//    glViewport(0, 0, _width, _height);
//    glClear(GL_COLOR_BUFFER_BIT);
//    glEnable(GL_BLEND);



    for (auto _i=0; _i<10; _i++) {
        wait(_idelsec);
        printf("woerker%02d: work at %d\n", _myid, _i);
    }
    // from now on use your OpenGL context
    // 6. Terminate EGL when finished
    eglTerminate(eglDpy);
}
