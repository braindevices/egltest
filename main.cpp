//https://devblogs.nvidia.com/parallelforall/egl-eye-opengl-visualization-without-x-server/
#include <iostream>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EglWorker.hpp>
#include <boost/thread.hpp>
#include <GL/gl.h>
#include <boost/format.hpp>
#include <string>
//https://stackoverflow.com/questions/3227042/maintaining-order-in-a-multi-threaded-pipeline
static const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
};

static const int pbufferWidth = 1920;
static const int pbufferHeight = 1920;

static const EGLint pbufferAttribs[] = {
        EGL_WIDTH, pbufferWidth,
        EGL_HEIGHT, pbufferHeight,
        EGL_NONE,
};

static void checkEglError(const char* op, EGLBoolean returnVal = EGL_TRUE) {
    if (returnVal != EGL_TRUE) {
        fprintf(stderr, "%s() returned %d\n", op, returnVal);
    }

    for (EGLint error = eglGetError(); error != EGL_SUCCESS; error = eglGetError()) {
        fprintf(stderr, "after %s() eglError (0x%x)\n", op, error);
    }
}

#define X(VAL) {VAL, #VAL}
struct {EGLint attribute; const char* name;} egl_cfgattrib_names[] = {
    X(EGL_CONFIG_ID),
    X(EGL_BUFFER_SIZE),
    X(EGL_RED_SIZE),
    X(EGL_GREEN_SIZE),
    X(EGL_BLUE_SIZE),
    X(EGL_ALPHA_SIZE),
    X(EGL_CONFIG_CAVEAT),
    X(EGL_DEPTH_SIZE),
    X(EGL_LEVEL),
    X(EGL_MAX_PBUFFER_WIDTH),
    X(EGL_MAX_PBUFFER_HEIGHT),
    X(EGL_MAX_PBUFFER_PIXELS),
    X(EGL_NATIVE_RENDERABLE),
    X(EGL_NATIVE_VISUAL_ID),
    X(EGL_NATIVE_VISUAL_TYPE),
    // X(EGL_PRESERVED_RESOURCES),
    X(EGL_RENDERABLE_TYPE),
    X(EGL_SAMPLE_BUFFERS),
    X(EGL_SAMPLES),
    // X(EGL_STENCIL_BITS),
    X(EGL_SURFACE_TYPE),
    X(EGL_TRANSPARENT_TYPE),
    X(EGL_TRANSPARENT_RED_VALUE),
    X(EGL_TRANSPARENT_GREEN_VALUE),
    X(EGL_TRANSPARENT_BLUE_VALUE),
    X(EGL_MATCH_NATIVE_PIXMAP),
    X(EGL_CONFORMANT),
    X(EGL_BIND_TO_TEXTURE_RGB),
    X(EGL_BIND_TO_TEXTURE_RGBA),
    X(EGL_ALPHA_MASK_SIZE),
    X(EGL_COLOR_BUFFER_TYPE),
    X(EGL_LUMINANCE_SIZE),
};
#undef X

void printEGLcfg(EGLDisplay dpy, const EGLConfig & config) {
    for (int j = 0; j < sizeof(egl_cfgattrib_names) / sizeof(egl_cfgattrib_names[0]); j++) {
        EGLint value = -1;
        auto returnVal = eglGetConfigAttrib(dpy, config, egl_cfgattrib_names[j].attribute, &value);
        if (returnVal) {
            printf(" %s(%#x): %d (0x%x),", egl_cfgattrib_names[j].name, egl_cfgattrib_names[j].attribute, value, value);
        } else {
            printf(" %s(%#x): fail to obtain,", egl_cfgattrib_names[j].name, egl_cfgattrib_names[j].attribute);
        }
    }
    printf("\n");
}

int printEGLConfigurations(EGLDisplay dpy) {
    EGLint numConfig = 0;
    EGLint returnVal = eglGetConfigs(dpy, NULL, 0, &numConfig);
    checkEglError("eglGetConfigs", returnVal);
    if (!returnVal) {
        return false;
    }

    printf("Number of EGL configuration: %d\n", numConfig);

    EGLConfig* configs = (EGLConfig*) malloc(sizeof(EGLConfig) * numConfig);
    if (! configs) {
        printf("Could not allocate configs.\n");
        return false;
    }

    returnVal = eglGetConfigs(dpy, configs, numConfig, &numConfig);
    checkEglError("eglGetConfigs", returnVal);
    if (!returnVal) {
        free(configs);
        return false;
    }

    for(int i = 0; i < numConfig; i++) {
        printf("Configuration %d\n", i);
        EGLConfig config = configs[i];
        printEGLcfg(dpy, config);
    }

    free(configs);
    return true;
}


int singlethread(bool withsurface)
{
    static const int MAX_DEVICES = 4;
    EGLDeviceEXT eglDevs[MAX_DEVICES];
    EGLint numDevices;
    PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT = (PFNEGLQUERYDEVICESEXTPROC)eglGetProcAddress("eglQueryDevicesEXT");

    eglQueryDevicesEXT(MAX_DEVICES, eglDevs, &numDevices);

    printf("Detected %d devices\n", numDevices);

    // 1. Initialize EGL
//    EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =(PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");

    auto eglDpy = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, eglDevs[0], 0);
    std::cout<<eglDpy<<std::endl;
    EGLint major, minor;
    auto flag_eglinit = eglInitialize(eglDpy, &major, &minor);
    printf("flag_eglinit=%d\n", flag_eglinit);
//    std::cout<<major<<"."<<minor<<std::endl;
//    printEGLConfigurations(eglDpy);
    // 2. Select an appropriate configuration
    EGLint numConfigs;
    EGLConfig eglCfg;

    auto flag_eglcfg = eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);
    printf("flag_eglcfg=%d\n", flag_eglcfg);
    printEGLcfg(eglDpy, eglCfg);
    // 3. Create a surface
    EGLSurface eglSurf;
    if (withsurface) {
        eglSurf = eglCreatePbufferSurface(
                eglDpy, eglCfg, pbufferAttribs
        );
    } else {
        eglSurf = EGL_NO_SURFACE;
    }

    // 4. Bind the API
    eglBindAPI(EGL_OPENGL_API);

    // 5. Create a context and make it current
    EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT,
                                         NULL);

    auto flag_makecurrent = eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);
    printf("flag_makecurrent = %d\n", flag_makecurrent);
//    GLint s[1];
//    glGetIntegerv(GL_MAX_TEXTURE_SIZE, s);
//    printf("eglCtx=&%#lx; GL_MAX_TEXTURE_SIZE=%u\n", eglCtx, s[0]);
    GLint max_texture_size=0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    printf("eglCtx=&%#lx; GL_MAX_TEXTURE_SIZE=%d\n", eglCtx, max_texture_size);
    // from now on use your OpenGL context

    // 6. Terminate EGL when finished
    eglDestroyContext(eglDpy, eglCtx);
    eglTerminate(eglDpy);
    return 0;
}


void multithread_eglworks(int threadnum){
    std::vector<EglWorker> workers;
    std::vector<boost::thread> _threads;
    for (auto _i=0; _i<threadnum; _i++){
        workers.push_back(EglWorker {_i, 0.5, configAttribs, pbufferAttribs} );
        _threads.push_back(boost::thread {&EglWorker::eglwork, workers.back()} );
    }
    for (auto &_i: _threads){
        _i.join();
    }
}

int main(int argc, char *argv[]){
    singlethread(true);
    singlethread(false);
//    multithread_eglworks(3);
    return 0;
}
