#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <termios.h>

#include <bcm_host.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

EGLDisplay eglDisplay;
EGLSurface eglSurface;
EGL_DISPMANX_WINDOW_T nativewindow;
struct termios origTermAattr;
struct termios newTermAttr;
bool termInitialized = false;
volatile bool caughtSignal = false;

void HandleSignal(int signum)
{
    caughtSignal = true;
}

bool ShouldTerminate()
{
    return caughtSignal;
}

void InitKeyboard() 
{
    // set the terminal to raw mode, non-blocking, no echo
    tcgetattr(fileno(stdin), &origTermAattr);
    memcpy(&newTermAttr, &origTermAattr, sizeof(struct termios));
    newTermAttr.c_lflag &= ~(ECHO|ICANON);
    newTermAttr.c_cc[VTIME] = 0;
    newTermAttr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &newTermAttr); 
    termInitialized = true;
}
    
int32_t GetKey() 
{
    // read a character from the stdin stream without blocking
    // returns EOF (-1) if no character is available
    return fgetc(stdin);
}

void ResetKeyboard()
{
    // restore the original terminal attributes
    if (termInitialized)
    {
        origTermAattr.c_lflag |= ECHO|ICANON;
        tcsetattr(fileno(stdin), TCSANOW, &origTermAattr);
    }
}

bool InitDisplay(uint32_t* fbWidth, uint32_t* fbHeight, bool multisampling)
{
     // get the display screen dimensions
    uint32_t display_width, display_height;
    int s = graphics_get_display_size(0 /* LCD */, &display_width, &display_height);
    assert(s >= 0);

    if (*fbWidth == 0 || *fbHeight == 0)
    {
        *fbWidth = display_width;
        *fbHeight = display_height;
    }

    // initialize EGL
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(eglDisplay, NULL, NULL);
    assert(eglGetError() == EGL_SUCCESS);

    // get an EGL config for 32 bit color, 16 bit depth framebuffer
    static const EGLint defaultConfigAttributes[] = 
    {
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_ALPHA_SIZE,      8,
        EGL_DEPTH_SIZE,      16, 
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT, 
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, 
        EGL_NONE
    };

    // get an EGL config for 32 bit color, 16 bit depth framebuffer, 4x multisampling
    static const EGLint multisamplingConfigAttributes[] = 
    {
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_ALPHA_SIZE,      8,
        EGL_DEPTH_SIZE,      16, 
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT, 
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, 
        EGL_SAMPLE_BUFFERS,  1,
        EGL_SAMPLES,         4,
        EGL_NONE
    };

    int numConfigs;
    EGLConfig eglConfig;
    EGLBoolean success = eglChooseConfig(eglDisplay, 
        multisampling ? multisamplingConfigAttributes : defaultConfigAttributes, 
        &eglConfig, 
        1, 
        &numConfigs);
    assert(success && numConfigs > 0);

    // create an EGL rendering context for OpenGL ES 2.x
    static const EGLint contextAttributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE }; 
    EGLContext eglContext = eglCreateContext(
        eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttributes); 
    assert (eglGetError() == EGL_SUCCESS && eglContext != EGL_NO_CONTEXT);

    // create source and destination rects for the hardware overlay
    VC_RECT_T dst_rect;
    VC_RECT_T src_rect;

    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width = display_width;
    dst_rect.height = display_height;

    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.width = *fbWidth << 16;
    src_rect.height = *fbHeight << 16;

    // configure the display manager to show our native window in the overlay
    DISPMANX_DISPLAY_HANDLE_T dispman_display = vc_dispmanx_display_open(0 /* LCD */);
    DISPMANX_UPDATE_HANDLE_T dispman_update = vc_dispmanx_update_start(0);

    DISPMANX_ELEMENT_HANDLE_T dispman_element = vc_dispmanx_element_add(
        dispman_update, dispman_display, 1 /*layer*/, &dst_rect, 0 /*src*/,
        &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0 /*clamp*/,
        (DISPMANX_TRANSFORM_T)0 /*transform*/);
    
    nativewindow.element = dispman_element;
    nativewindow.width = *fbWidth;
    nativewindow.height = *fbHeight;
    vc_dispmanx_update_submit_sync(dispman_update);

    // create the native window surface, and make it the currently active surface
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, &nativewindow, NULL);
    assert(eglGetError() == EGL_SUCCESS && eglSurface != EGL_NO_SURFACE);
   
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    assert(eglGetError() == EGL_SUCCESS);

    eglSwapInterval(eglDisplay, 0);

    printf("Vendor  : %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version : %s\n", glGetString(GL_VERSION));
    printf("GLSL ver: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void InitPlatform(uint32_t* fbWidth, uint32_t* fbHeight, bool multisampling) 
{
    bcm_host_init();

    // install handler to gracefully clean up upon a SIGTERM or SIGINT 
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = HandleSignal;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);

    InitDisplay(fbWidth, fbHeight, multisampling);

    InitKeyboard();
}

void ReInitDisplay(uint32_t* fbWidth, uint32_t* fbHeight, bool multisampling)
{
    // close the old display
    eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    assert(eglGetError() == EGL_SUCCESS);
    eglTerminate(eglDisplay);
    assert(eglGetError() == EGL_SUCCESS);
    eglReleaseThread();

    InitDisplay(fbWidth, fbHeight, multisampling);
}

void SaveScreenshot()
{
    // A simple demo using dispmanx to display get screenshot
    DISPMANX_DISPLAY_HANDLE_T   display;
    DISPMANX_MODEINFO_T         info;
    DISPMANX_RESOURCE_HANDLE_T  resource;
    VC_IMAGE_TYPE_T type = VC_IMAGE_RGB888;
    VC_IMAGE_TRANSFORM_T   transform = (VC_IMAGE_TRANSFORM_T)0;
    VC_RECT_T         rect;

    void                       *image;
    uint32_t                    vc_image_ptr;

    int                   ret;

    uint32_t        screen = 0;

    bcm_host_init();

    printf("Open display[%i]...\n", screen );
    display = vc_dispmanx_display_open( screen );

    ret = vc_dispmanx_display_get_info(display, &info);
    assert(ret == 0);
    printf( "Display is %d x %d\n", info.width, info.height );

    image = calloc( 1, info.width * 3 * info.height );

    assert(image);

    resource = vc_dispmanx_resource_create( type,
                                                  info.width,
                                                  info.height,
                                                  &vc_image_ptr );

    vc_dispmanx_snapshot(display, resource, (DISPMANX_TRANSFORM_T)transform);

    vc_dispmanx_rect_set(&rect, 0, 0, info.width, info.height);
    vc_dispmanx_resource_read_data(resource, &rect, image, info.width*3); 

    FILE *fp = fopen("screenshot.ppm", "wb");
    fprintf(fp, "P6\n%d %d\n255\n", info.width, info.height);
    fwrite(image, info.width*3*info.height, 1, fp);
    fclose(fp);

    ret = vc_dispmanx_resource_delete( resource );
    assert( ret == 0 );
    ret = vc_dispmanx_display_close(display );
    assert( ret == 0 );

    return;
}

void SwapBuffers() 
{ 
    eglSwapBuffers(eglDisplay, eglSurface); 
}


void DeinitPlatform() 
{
    ResetKeyboard();

    eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    assert(eglGetError() == EGL_SUCCESS);
    eglTerminate(eglDisplay);
    assert(eglGetError() == EGL_SUCCESS);
    eglReleaseThread();
}
