#include <Windows.h>
#include <sysinfoapi.h>
#include <math.h>
#include "direct.h"
#include "gles_impl.h"
#include "direct/direct.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "glcorearb.h"
#include "WinAudioQueue.h"

// GL 1.2
PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements = NULL;
PFNGLTEXIMAGE3DPROC glTexImage3D = NULL;
PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D = NULL;
PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D = NULL;

// GL 1.3
PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
PFNGLSAMPLECOVERAGEPROC glSampleCoverage = NULL;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = NULL;
PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage = NULL;

// GL 1.4
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = NULL;
PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays = NULL;
PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements = NULL;
PFNGLPOINTPARAMETERFPROC glPointParameterf = NULL;
PFNGLPOINTPARAMETERFVPROC glPointParameterfv = NULL;
PFNGLPOINTPARAMETERIPROC glPointParameteri = NULL;
PFNGLPOINTPARAMETERIVPROC glPointParameteriv = NULL;
PFNGLBLENDCOLORPROC glBlendColor = NULL;
PFNGLBLENDEQUATIONPROC glBlendEquation = NULL;

// GL 1.5
PFNGLGENQUERIESPROC glGenQueries = NULL;
PFNGLDELETEQUERIESPROC glDeleteQueries = NULL;
PFNGLISQUERYPROC glIsQuery = NULL;
PFNGLBEGINQUERYPROC glBeginQuery = NULL;
PFNGLENDQUERYPROC glEndQuery = NULL;
PFNGLGETQUERYIVPROC glGetQueryiv = NULL;
PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv = NULL;
PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLISBUFFERPROC glIsBuffer = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData = NULL;
PFNGLMAPBUFFERPROC glMapBuffer = NULL;
PFNGLUNMAPBUFFERPROC glUnmapBuffer = NULL;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv = NULL;
PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv = NULL;

// GL 2.0
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = NULL;
PFNGLDRAWBUFFERSPROC glDrawBuffers = NULL;
PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate = NULL;
PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate = NULL;
PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLDETACHSHADERPROC glDetachShader = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib = NULL;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform = NULL;
PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders = NULL;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
PFNGLGETSHADERSOURCEPROC glGetShaderSource = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLGETUNIFORMFVPROC glGetUniformfv = NULL;
PFNGLGETUNIFORMIVPROC glGetUniformiv = NULL;
PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv = NULL;
PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv = NULL;
PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv = NULL;
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv = NULL;
PFNGLISPROGRAMPROC glIsProgram = NULL;
PFNGLISSHADERPROC glIsShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLUNIFORM1FPROC glUniform1f = NULL;
PFNGLUNIFORM2FPROC glUniform2f = NULL;
PFNGLUNIFORM3FPROC glUniform3f = NULL;
PFNGLUNIFORM4FPROC glUniform4f = NULL;
PFNGLUNIFORM1IPROC glUniform1i = NULL;
PFNGLUNIFORM2IPROC glUniform2i = NULL;
PFNGLUNIFORM3IPROC glUniform3i = NULL;
PFNGLUNIFORM4IPROC glUniform4i = NULL;
PFNGLUNIFORM1FVPROC glUniform1fv = NULL;
PFNGLUNIFORM2FVPROC glUniform2fv = NULL;
PFNGLUNIFORM3FVPROC glUniform3fv = NULL;
PFNGLUNIFORM4FVPROC glUniform4fv = NULL;
PFNGLUNIFORM1IVPROC glUniform1iv = NULL;
PFNGLUNIFORM2IVPROC glUniform2iv = NULL;
PFNGLUNIFORM3IVPROC glUniform3iv = NULL;
PFNGLUNIFORM4IVPROC glUniform4iv = NULL;
PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv = NULL;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;
PFNGLVALIDATEPROGRAMPROC glValidateProgram = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;

// GL 3.0
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample = NULL;

// GL 3.1
PFNGLISRENDERBUFFERPROC glIsRenderbuffer = NULL;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = NULL;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = NULL;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = NULL;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv = NULL;
PFNGLISFRAMEBUFFERPROC glIsFramebuffer = NULL;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = NULL;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = NULL;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = NULL;
PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = NULL;
PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D = NULL;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv = NULL;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap = NULL;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;

namespace mmapp {

static const int max_modes = 50;

static const int all_keys[] = {
GLFW_KEY_SPACE,              
GLFW_KEY_APOSTROPHE,         
GLFW_KEY_COMMA,              
GLFW_KEY_MINUS,              
GLFW_KEY_PERIOD,             
GLFW_KEY_SLASH,            
GLFW_KEY_0,                  
GLFW_KEY_1,                  
GLFW_KEY_2,                  
GLFW_KEY_3,                  
GLFW_KEY_4,                  
GLFW_KEY_5,                  
GLFW_KEY_6,                  
GLFW_KEY_7,                  
GLFW_KEY_8,                  
GLFW_KEY_9,                  
GLFW_KEY_SEMICOLON,          
GLFW_KEY_EQUAL,              
GLFW_KEY_A,                  
GLFW_KEY_B,                  
GLFW_KEY_C,                  
GLFW_KEY_D,                  
GLFW_KEY_E,                  
GLFW_KEY_F,                  
GLFW_KEY_G,                  
GLFW_KEY_H,                  
GLFW_KEY_I,                  
GLFW_KEY_J,                  
GLFW_KEY_K,                  
GLFW_KEY_L,                  
GLFW_KEY_M,                  
GLFW_KEY_N,                  
GLFW_KEY_O,                  
GLFW_KEY_P,                  
GLFW_KEY_Q,                  
GLFW_KEY_R,                  
GLFW_KEY_S,                  
GLFW_KEY_T,                  
GLFW_KEY_U,                  
GLFW_KEY_V,                  
GLFW_KEY_W,                  
GLFW_KEY_X,                  
GLFW_KEY_Y,                  
GLFW_KEY_Z,                  
GLFW_KEY_LEFT_BRACKET,       
GLFW_KEY_BACKSLASH,          
GLFW_KEY_RIGHT_BRACKET,      
GLFW_KEY_GRAVE_ACCENT,       
GLFW_KEY_WORLD_1,            
GLFW_KEY_WORLD_2,            
GLFW_KEY_ESCAPE,             
GLFW_KEY_ENTER,              
GLFW_KEY_TAB,                
GLFW_KEY_BACKSPACE,          
GLFW_KEY_INSERT,             
GLFW_KEY_DELETE,             
GLFW_KEY_RIGHT,              
GLFW_KEY_LEFT,               
GLFW_KEY_DOWN,               
GLFW_KEY_UP,                 
GLFW_KEY_PAGE_UP,            
GLFW_KEY_PAGE_DOWN,          
GLFW_KEY_HOME,               
GLFW_KEY_END,                
GLFW_KEY_CAPS_LOCK,          
GLFW_KEY_SCROLL_LOCK,        
GLFW_KEY_NUM_LOCK,           
GLFW_KEY_PRINT_SCREEN,       
GLFW_KEY_PAUSE,              
GLFW_KEY_F1,                 
GLFW_KEY_F2,                 
GLFW_KEY_F3,                 
GLFW_KEY_F4,                 
GLFW_KEY_F5,                 
GLFW_KEY_F6,                 
GLFW_KEY_F7,                 
GLFW_KEY_F8,                 
GLFW_KEY_F9,                 
GLFW_KEY_F10,                
GLFW_KEY_F11,                
GLFW_KEY_F12,                
GLFW_KEY_F13,                
GLFW_KEY_F14,                
GLFW_KEY_F15,                
GLFW_KEY_F16,                
GLFW_KEY_F17,                
GLFW_KEY_F18,                
GLFW_KEY_F19,                
GLFW_KEY_F20,                
GLFW_KEY_F21,                
GLFW_KEY_F22,                
GLFW_KEY_F23,                
GLFW_KEY_F24,                
GLFW_KEY_F25,                
GLFW_KEY_KP_0,               
GLFW_KEY_KP_1,               
GLFW_KEY_KP_2,               
GLFW_KEY_KP_3,               
GLFW_KEY_KP_4,               
GLFW_KEY_KP_5,               
GLFW_KEY_KP_6,               
GLFW_KEY_KP_7,               
GLFW_KEY_KP_8,               
GLFW_KEY_KP_9,               
GLFW_KEY_KP_DECIMAL,         
GLFW_KEY_KP_DIVIDE,          
GLFW_KEY_KP_MULTIPLY,        
GLFW_KEY_KP_SUBTRACT,        
GLFW_KEY_KP_ADD,             
GLFW_KEY_KP_ENTER,           
GLFW_KEY_KP_EQUAL,           
GLFW_KEY_LEFT_SHIFT,         
GLFW_KEY_LEFT_CONTROL,       
GLFW_KEY_LEFT_ALT,           
GLFW_KEY_LEFT_SUPER,         
GLFW_KEY_RIGHT_SHIFT,        
GLFW_KEY_RIGHT_CONTROL,      
GLFW_KEY_RIGHT_ALT,          
GLFW_KEY_RIGHT_SUPER,        
GLFW_KEY_MENU};

static const int fifo_len = 8;
static const int key_pressed = 0x80;
static const int key_hit = 0x40;

class DirectImpl : public IMmAppServices
{
public:
    DirectImpl();
    virtual void *get__id() const { return(nullptr); };

    // for each entry_index returns an available full screen video mode. If entry_index is too high returns false
    virtual bool enumerateModes(int32_t entry_index, int32_t *width, int32_t *height) const;

    // the resolution we want to use when in full screen
    virtual bool setFullScreenVideoResolution(int32_t width, int32_t height);
    virtual void getFullScreenVideoResolution(int32_t *width, int32_t *height) const;

    // to actually switch to full screen modes
    virtual bool changeToFromWindowedMode(bool windowed);

    // the resolution and position of the window when in windowed mode
    virtual void getWindowedVideoResolution(int32_t *width, int32_t *height) const;
    virtual void getWindowedWindowPosition(int32_t *xpos, int32_t *ypos) const;

    // the current resolution (depends if we are in windowed or full screen mode)
    virtual void getResolution(int32_t *width, int32_t *height) const;

    // makes the last rendered scene effective
    virtual void swapBuffers();

    virtual bool isMaximized() const;
    virtual bool isWindowed() const;

    virtual void setTitle(const char *title);
    virtual void setIcon(const std::vector<uint8_t> &icon);
    virtual void forceWindowAspectRatio(int32_t width, int32_t height);
    virtual void setWindowSizeLimits(int32_t min_w, int32_t min_h, int32_t max_w, int32_t max_h);

    // for mobile devices
    virtual void enableScreenRotation(int32_t allowed_orientations_mask);
    virtual int32_t getScreenRotation() const;

    // SOUND
    virtual bool startSoundSource(int32_t chunk_samples, int32_t rate, ISoundSynth *callback);
    virtual void shutdownSoundSource();

    // INPUT
    virtual bool isKeyDown(int32_t key_id) const;
    virtual bool isKeyJustPressed(int32_t key_id) const;
    virtual std::string getKeyName(int32_t key_id) const;
    virtual int32_t getCodePoint();
    virtual void flushCodePoints();

    virtual bool isMouseKeyDown(int32_t key_index) const;
    virtual bool isMouseKeyJustPressed(int32_t key_index) const;
    virtual int32_t getMouseWheel();

    virtual void setCursorMode(CursorMode mode);
    virtual void setCursorShape(CursorShape shape);                         
    virtual void getMousePosition(int32_t *xx, int32_t *yy) const;          
    virtual void setMousePosition(int32_t xx, int32_t yy);                  
    virtual void getMouseMovement(int32_t *dx, int32_t *dy) const;                

    virtual bool isJoystickPresent(int32_t idx) const;
    virtual bool isJoystickAGamepad(int32_t idx) const;
    virtual void getJoystickButtons(std::vector<bool> *buttons, int32_t idx) const;
    virtual void getJoystickAxes(std::vector<float> *axis, int32_t idx) const;
    virtual void getJoystickHats(std::vector<int32_t> *hats, int32_t idx) const;
    virtual std::string getJoystickName(int32_t idx) const;

    virtual void getTouchScreenContacts(sing::array<TouchContact, max_contacts> *contacts);
    virtual void flushTouchScreenContacts();

    virtual void getAccelerometerVector(float *xx, float *yy, float *zz) const;

    // clipboard and dragging
    virtual std::string getClipboardString() const;
    virtual void setClipboardString(const char *str);
    virtual void getDroppedPaths(std::vector<std::string> *paths);

    // MIX
    virtual void setScheduleMode(ScheduleMode mode, float tout);
    virtual void postEmptyEvent();
    virtual bool didLooseFocus();                       // games need to know so they switch to a pause screen.
    virtual bool didLooseContext();
    virtual int32_t getAvailableMemory() const;         // Mbytes, returns 0 if unsupported on the platform.

    void run(IMmApp *the_app, const char *win_title, int32_t win_flags, int32_t win_width, int32_t win_height);
    void onMonitorDisconnect(GLFWmonitor *monitor);
    void onCodepoint(int32_t codepoint);
    void onScroll(float xx, float yy);
    void onDrop(int count, const char** paths);
    void onFocusLost(void);
    void onSoundBufferRequest(uint8_t *buffer);

private:

    // window and context
    GLFWwindow  *window_;
    GLFWmonitor *primary_;
    int32_t     num_modes_;
    GLFWvidmode modes_[max_modes];
    bool        focus_lost_;
    CGlEs       gles_;

    // window sizing
    int32_t     fs_width_;
    int32_t     fs_height_;
    int32_t     window_xpos_;
    int32_t     window_ypos_;
    int32_t     windowed_width_;
    int32_t     windowed_height_;

    // sound
    ISoundSynth             *sound_source_;
    WinAudioQueue           audio_queue_;
    std::vector<int16_t>    samples_;
    int                     samples_count_;

    // keyboard
    uint8_t     keys_[GLFW_KEY_LAST + 1];
    int32_t     fifo_[fifo_len];
    int32_t     fifo_in_;
    int32_t     fifo_out_;
    int32_t     fifo_cnt_;

    // mouse
    CursorMode  cursor_mode_;
    GLFWcursor* curr_cursor_;
    CursorShape curr_shape_;
    uint8_t     mouse_keys_[mouse_keycount];
    float       scroll_;
    double      xcurr_;
    double      ycurr_;
    double      xprev_;
    double      yprev_;
    bool        mouse_reinit_pos_;

    // mix
    std::vector<std::string> paths_;
    ScheduleMode mode_;
    float        tout_;

    void doDisplayModesEnumeration(void);
    void sampleInputs(void);
    void updateWindowPosAndSize(void);
    bool initOpenGLFunctionPointers(void);
};

DirectImpl implementor;

void runMmApp(IMmApp *the_app, const char *win_title, int32_t win_flags, int32_t win_width, int32_t win_height)
{
    implementor.run(the_app, win_title, win_flags, win_width, win_height);
}

// Callbacks
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void monitor_callback(GLFWmonitor* monitor, int event)
{
    if (event == GLFW_DISCONNECTED) {
        implementor.onMonitorDisconnect(monitor);
    }
}

void char_callback(GLFWwindow* window, unsigned int codepoint)
{
    implementor.onCodepoint((int32_t)codepoint);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    implementor.onScroll((float)xoffset, (float)yoffset);
}

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
    implementor.onDrop(count, paths);
}

void focus_callback(GLFWwindow* window, int focused)
{
    if (focused == GLFW_FALSE) {
        implementor.onFocusLost();
    }
}

void sound_callback(uint8_t *buffer)
{
    implementor.onSoundBufferRequest(buffer);
}

// DirectImpl implementation
DirectImpl::DirectImpl()
{
    // window and cursor
    window_ = nullptr;
    primary_ = nullptr;
    num_modes_ = 0;
    focus_lost_ = false;

    // window sizing
    fs_width_ = 640;
    fs_height_ = 480;
    window_xpos_ = 0;
    window_ypos_ = 0;
    windowed_width_ = 640;
    windowed_height_ = 480;

    // sound
    sound_source_ = nullptr;

    // keyboard
    fifo_in_ = fifo_out_ = fifo_cnt_ = 0;
    for (auto value : keys_) value = 0;

    // mouse
    curr_cursor_ = nullptr;
    curr_shape_ = CursorShape::arrow;
    scroll_ = 0;
    mouse_reinit_pos_ = true;
    for (auto value : mouse_keys_) value = 0;

    // mix
    mode_ = ScheduleMode::loop_fast;
}

// for each entry_index returns an available full screen video mode. If entry_index is too high returns false
bool DirectImpl::enumerateModes(int32_t entry_index, int32_t *width, int32_t *height) const
{
    if (entry_index < (int32_t)num_modes_) {
        *width = (int32_t)modes_[entry_index].width;
        *height = (int32_t)modes_[entry_index].height;
        return(true);
    }
    return(false);
}

// the resolution we want to use when in full screen
bool DirectImpl::setFullScreenVideoResolution(int32_t width, int32_t height)
{
    if (glfwGetWindowMonitor(window_) != nullptr) {
        glfwSetWindowSize(window_, width, height);
    }
    fs_width_ = width;
    fs_height_ = height;
    return(true);
}

void DirectImpl::getFullScreenVideoResolution(int32_t *width, int32_t *height) const
{
    *width = fs_width_;
    *height = fs_height_;
}

// to actually switch to full screen modes
bool DirectImpl::changeToFromWindowedMode(bool windowed)
{
    bool currently_windowed = glfwGetWindowMonitor(window_) == nullptr;
    if (windowed && !currently_windowed) {
        glfwSetWindowMonitor(window_, nullptr, window_xpos_, window_ypos_, windowed_width_, windowed_height_, GLFW_DONT_CARE);
    } else if (!windowed && currently_windowed) {
        if (primary_ == nullptr) return(false);
        glfwSetWindowMonitor(window_, primary_, 0, 0, fs_width_, fs_height_, GLFW_DONT_CARE);
    }
    return(true);
}

// the resolution and position of the window when in windowed mode
void DirectImpl::getWindowedVideoResolution(int32_t *width, int32_t *height) const
{
    *width = windowed_width_;
    *height = windowed_height_;
}

void DirectImpl::getWindowedWindowPosition(int32_t *xpos, int32_t *ypos) const
{
    *xpos = window_xpos_;
    *ypos = window_ypos_;
}

// the current resolution (depends if we are in windowed or full screen mode)
void DirectImpl::getResolution(int32_t *width, int32_t *height) const
{
    *width = 0;
    *height = 0;
    int ww, hh;
    glfwGetFramebufferSize(window_, &ww, &hh);
    *width = ww;
    *height = hh;
}

void DirectImpl::swapBuffers()
{
    glfwSwapBuffers(window_);
}

bool DirectImpl::isMaximized() const
{
    return(glfwGetWindowAttrib(window_, GLFW_MAXIMIZED) == GLFW_TRUE);
}

bool DirectImpl::isWindowed() const
{
    return(glfwGetWindowMonitor(window_) == nullptr);
}

void DirectImpl::setTitle(const char *title)
{ 
    glfwSetWindowTitle(window_, title); 
}

void DirectImpl::setIcon(const std::vector<uint8_t> &icon)
{
    GLFWimage image;
    int32_t size = (int32_t)sqrt(icon.size());
    if (size < 16 || size * size != icon.size()) {
        image.width = image.height = 0;
        glfwSetWindowIcon(window_, 0, &image);
    } else {
        image.height = image.width = size;
        image.pixels = (unsigned char*)icon.data();
        glfwSetWindowIcon(window_, 1, &image);
    }
}

void DirectImpl::forceWindowAspectRatio(int32_t width, int32_t height)
{
    glfwSetWindowAspectRatio(window_, width, height);
}

void DirectImpl::setWindowSizeLimits(int32_t min_w, int32_t min_h, int32_t max_w, int32_t max_h)
{
    glfwSetWindowSizeLimits(window_, min_w, min_h, max_w, max_h);
}

// for mobile devices
void DirectImpl::enableScreenRotation(int32_t allowed_orientations_mask)
{
}

int32_t DirectImpl::getScreenRotation() const
{
    return(0);
}

// SOUND
bool DirectImpl::startSoundSource(int32_t chunk_samples, int32_t rate, ISoundSynth *callback)
{
    if (chunk_samples < 128 || chunk_samples > 100000 || rate < 1024 || rate > 100000 || callback == nullptr) return(false);
    if (sound_source_ != nullptr) shutdownSoundSource();
    sound_source_ = callback;
    samples_count_ = chunk_samples;
    if (audio_queue_.Init(glfwGetWin32Window(window_), chunk_samples * 2, rate, sound_callback) == FAIL) {
        sound_source_ = nullptr;
        return(false);
    }
    return(true);
}

void DirectImpl::shutdownSoundSource()
{
    audio_queue_.Shutdown();
    sound_source_ = nullptr;
}

// INPUT
bool DirectImpl::isKeyDown(int32_t key_id) const
{
    if (window_ == nullptr || key_id < 0 || key_id > GLFW_KEY_LAST) {
        return(false);
    }
    return((keys_[key_id] & key_pressed) != 0);
}

bool DirectImpl::isKeyJustPressed(int32_t key_id) const
{
    if (window_ == nullptr || key_id < 0 || key_id > GLFW_KEY_LAST) {
        return(false);
    }
    return((keys_[key_id] & key_hit) != 0);
}

std::string DirectImpl::getKeyName(int32_t key_id) const
{
    const char *name = glfwGetKeyName(key_id, 0);
    if (name == nullptr) return("");
    return(name);
}

int32_t DirectImpl::getCodePoint()
{
    if (fifo_cnt_ > 0) {
        int32_t cp = fifo_[fifo_out_++];
        if (fifo_out_ == fifo_len) fifo_out_ = 0;
        --fifo_cnt_;
        return(cp);
    }
    return(0);
}

void DirectImpl::flushCodePoints()
{
    fifo_in_ = fifo_out_ = fifo_cnt_ = 0;
}

bool DirectImpl::isMouseKeyDown(int32_t key_id) const
{
    if (window_ == nullptr || key_id < 0 || key_id >= mouse_keycount) {
        return(false);
    }
    return((mouse_keys_[key_id] & key_pressed) != 0);
}

bool DirectImpl::isMouseKeyJustPressed(int32_t key_id) const
{
    if (window_ == nullptr || key_id < 0 || key_id >= mouse_keycount) {
        return(false);
    }
    return((mouse_keys_[key_id] & key_hit) != 0);
}

int32_t DirectImpl::getMouseWheel()
{
    float retval = scroll_;
    scroll_ = 0;
    return((int32_t)retval);
}

void DirectImpl::setCursorMode(CursorMode mode)
{
    if (mode == CursorMode::standard) {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported()) {
            glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
        mouse_reinit_pos_ = true;
    }
}

void DirectImpl::setCursorShape(CursorShape shape)
{
    if (shape == curr_shape_) return;
    GLFWcursor* cursor = nullptr;
    switch (shape) {
    case CursorShape::arrow:
        break;
    case CursorShape::ibeam:
        cursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        break;
    case CursorShape::xair:
        cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
        break;
    case CursorShape::hand:
        cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        break;
    case CursorShape::h_resize:
        cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        break;
    case CursorShape::v_resize:
        cursor = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
        break;
    }
    glfwSetCursor(window_, cursor);
    if (curr_cursor_ != nullptr) glfwDestroyCursor(curr_cursor_);
    curr_cursor_ = cursor;
    curr_shape_ = shape;
}

void DirectImpl::getMousePosition(int32_t *xx, int32_t *yy) const
{
    double xpos = 0;
    double ypos = 0;
    if (window_ != nullptr) {
        glfwGetCursorPos(window_, &xpos, &ypos);
    }
    *xx = (int32_t)xpos;
    *yy = (int32_t)ypos;
}

void DirectImpl::setMousePosition(int32_t xx, int32_t yy)
{
    if (window_ != nullptr) {
        glfwSetCursorPos(window_, xx, yy);
    }
}

void DirectImpl::getMouseMovement(int32_t *dx, int32_t *dy) const
{
    *dx = (int32_t)(xcurr_ - xprev_);
    *dy = (int32_t)(ycurr_ - yprev_);
}

bool DirectImpl::isJoystickPresent(int32_t idx) const
{
    if (idx < 0 || idx > GLFW_JOYSTICK_LAST) return(false);
    return(glfwJoystickPresent(idx) == GLFW_TRUE);
}

bool DirectImpl::isJoystickAGamepad(int32_t idx) const
{
    if (idx < 0 || idx > GLFW_JOYSTICK_LAST) return(false);
    return(glfwJoystickIsGamepad(idx) == GLFW_TRUE);
}

void DirectImpl::getJoystickButtons(std::vector<bool> *buttons, int32_t idx) const
{
    buttons->clear();
    if (idx < 0 || idx > GLFW_JOYSTICK_LAST) return;
    int count;
    const unsigned char *but_array = glfwGetJoystickButtons(idx, &count);
    int ii;
    for (ii = 0; ii < count; ++ii) {
        buttons->push_back(but_array[ii] == GLFW_PRESS);
    }
    for (; ii <= gamepad_button_dpad_left; ++ii) {
        buttons->push_back(false);
    }
}

void DirectImpl::getJoystickAxes(std::vector<float> *axis, int32_t idx) const
{
    axis->clear();
    if (idx < 0 || idx > GLFW_JOYSTICK_LAST) return;
    int count;
    const float *axis_array = glfwGetJoystickAxes(idx, &count);
    int ii;
    for (ii = 0; ii < count; ++ii) {
        axis->push_back(axis_array[ii]);
    }
    for (; ii <= gamepad_axis_right_trigger; ++ii) {
        axis->push_back(0);
    }
}

void DirectImpl::getJoystickHats(std::vector<int32_t> *hats, int32_t idx) const
{
    hats->clear();
    if (idx < 0 || idx > GLFW_JOYSTICK_LAST) return;
    int count;
    const unsigned char *hats_array = glfwGetJoystickHats(idx, &count);
    int ii;
    for (ii = 0; ii < count; ++ii) {
        hats->push_back((int32_t)hats_array[ii]);
    }
}

std::string DirectImpl::getJoystickName(int32_t idx) const
{
    if (idx < 0 || idx > GLFW_JOYSTICK_LAST) return("");
    const char *name = glfwGetJoystickName(idx);
    if (name == nullptr) return("");
    return(name);
}

void DirectImpl::getTouchScreenContacts(sing::array<TouchContact, max_contacts> *contacts)
{
}

void DirectImpl::flushTouchScreenContacts()
{
}

void DirectImpl::getAccelerometerVector(float *xx, float *yy, float *zz) const
{
}

// clipboard and dragging
std::string DirectImpl::getClipboardString() const
{
    const char *value = glfwGetClipboardString(nullptr);
    if (value == nullptr) return("");
    return(value);
}

void DirectImpl::setClipboardString(const char *str)
{
    glfwSetClipboardString(nullptr, str);
}

void DirectImpl::getDroppedPaths(std::vector<std::string> *paths)
{
    *paths = paths_;
    paths_.clear();
}

// MIX
void DirectImpl::setScheduleMode(ScheduleMode mode, float tout)
{
    mode_ = mode;
    tout_ = tout;
}

void DirectImpl::postEmptyEvent()
{
    glfwPostEmptyEvent();
}

bool DirectImpl::didLooseFocus()                       // games need to know so they switch to a pause screen.
{
    bool value = focus_lost_;
    focus_lost_ = false;
    return(value);
}

bool DirectImpl::didLooseContext()
{
    return(false);
}

int32_t DirectImpl::getAvailableMemory() const         // Mbytes, returns 0 if unsupported on the platform.
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    return((int32_t)(statex.ullTotalPhys / (1024*1024)));
}

void DirectImpl::run(IMmApp *the_app, const char *win_title, int32_t win_flags, int32_t win_width, int32_t win_height)
{
    // window creation
    glfwSetErrorCallback(error_callback);
    if (!glfwInit() || the_app == nullptr) {
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_RESIZABLE, ((win_flags & win_resizable) == 0) ? GLFW_FALSE : GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, ((win_flags & win_decorated) == 0) ? GLFW_FALSE : GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, ((win_flags & win_maximized) == 0) ? GLFW_FALSE : GLFW_TRUE);
    windowed_width_ = win_width;
    windowed_height_ = win_height;
    window_ = glfwCreateWindow(windowed_width_, windowed_height_, win_title, NULL, NULL);
    if (window_ == nullptr) {
        // Window or OpenGL context creation failed
        glfwTerminate();
        return;
    }
    updateWindowPosAndSize();

    // callbacks setup
    if ((win_flags & win_exit_on_esc) != 0) {
        glfwSetKeyCallback(window_, key_callback);
    }
    glfwSetMonitorCallback(monitor_callback);
    glfwSetCharCallback(window_, char_callback);
    glfwSetScrollCallback(window_, scroll_callback);
    glfwSetDropCallback(window_, drop_callback);
    glfwSetWindowFocusCallback(window_, focus_callback);

    // input init
    glfwSetInputMode(window_, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(window_, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    // graphics stuff
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
    initOpenGLFunctionPointers();
    doDisplayModesEnumeration();

    int ww, hh;
    glfwGetFramebufferSize(window_, &ww, &hh);
    gles_.init((int32_t)ww, (int32_t)hh);

    if (the_app->initApplication(this)) {
        while (true) {
            audio_queue_.RestoreLostBuffers();
            if (glfwWindowShouldClose(window_)) {
                if (the_app->canExit()) break;
                glfwSetWindowShouldClose(window_, GLFW_FALSE);
            }
            if (mode_ == ScheduleMode::loop_fast) {
                glfwPollEvents();
            } else if (tout_ == 0) {
                glfwWaitEvents();
            } else {
                glfwWaitEventsTimeout(tout_);
            }
            sampleInputs();
            if (glfwGetWindowMonitor(window_) == nullptr) {
                updateWindowPosAndSize();
            }
            glfwGetFramebufferSize(window_, &ww, &hh);
            gles_.update_bkbuf_size((int32_t)ww, (int32_t)hh);
            if (!the_app->stepApplication(this, &gles_)) {
                break;
            }
        }
        the_app->quitApplication();
    }
    shutdownSoundSource();
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void DirectImpl::onMonitorDisconnect(GLFWmonitor *monitor)
{
    if (monitor == primary_) {
        doDisplayModesEnumeration();
    }
}

void DirectImpl::onCodepoint(int32_t codepoint)
{
    if (fifo_cnt_ < fifo_len) {
        fifo_[fifo_in_++] = codepoint;
        if (fifo_in_ == fifo_len) fifo_in_ = 0;
        ++fifo_cnt_;
    }
}

void DirectImpl::onScroll(float xx, float yy)
{
    scroll_ += yy;
}

void DirectImpl::onDrop(int count, const char** paths)
{
    paths_.clear();
    for (int ii = 0; ii < count; ii++) {
        paths_.push_back(paths[ii]);
    }
}

void DirectImpl::onFocusLost(void)
{
    focus_lost_ = true;
}

void DirectImpl::onSoundBufferRequest(uint8_t *buffer)
{
    if (sound_source_ != nullptr) {
        samples_.clear();
        sound_source_->fillSamples(samples_count_, &samples_);
        memcpy(buffer, samples_.data(), samples_count_ * 2);
    }
}

void DirectImpl::doDisplayModesEnumeration(void)
{
    num_modes_ = 0;
    primary_ = glfwGetPrimaryMonitor();
    if (primary_ == nullptr) return;
    int count;
    const GLFWvidmode* src_modes = glfwGetVideoModes(primary_, &count);

    int maxpixels = 0;
    int maxmode = -1;
    for (int ii = 0; ii < count && num_modes_ < max_modes; ++ii) {
        if (src_modes[ii].blueBits == 8 && src_modes[ii].greenBits == 8 && src_modes[ii].redBits == 8) {

            // already seen ?
            int scan;
            for (scan = 0; scan < num_modes_; ++scan) {
                if (modes_[scan].width == src_modes[ii].width &&
                    modes_[scan].height == src_modes[ii].height) {
                    break;
                }
            }

            if (scan != num_modes_) {

                // already seen - replace if the new one has an higher refresh rate
                if (src_modes[ii].refreshRate > modes_[scan].refreshRate) {
                    modes_[scan] = src_modes[ii];
                }
            } else {

                // new
                modes_[scan] = src_modes[ii];
                int totpixels = src_modes[ii].width * src_modes[ii].height;
                if (totpixels > maxpixels) {
                    maxpixels = totpixels;
                    maxmode = scan;
                }
                ++num_modes_;
            }
        }
    }
}

void DirectImpl::sampleInputs(void)
{
    for (auto pressed : keys_) {
        pressed = 0;
    }
    for (auto key : all_keys) {
        if (glfwGetKey(window_, key) == GLFW_PRESS) {
            keys_[key] = (keys_[key] == 0) ? (key_hit + key_pressed) : key_pressed;
        } else {
            keys_[key] = 0;
        }
    }
    for (int ii = 0; ii < mouse_keycount; ++ii) {
        if (glfwGetMouseButton(window_, ii) == GLFW_PRESS) {
            mouse_keys_[ii] = (mouse_keys_[ii] == 0) ? (key_hit + key_pressed) : key_pressed;
        } else {
            mouse_keys_[ii] = 0;
        }
    }
    if (mouse_reinit_pos_) {
        glfwGetCursorPos(window_, &xcurr_, &ycurr_);
        xprev_ = xcurr_;
        yprev_ = ycurr_;
        mouse_reinit_pos_ = false;
    } else {
        xprev_ = xcurr_;
        yprev_ = ycurr_;
        glfwGetCursorPos(window_, &xcurr_, &ycurr_);
    }
}

void DirectImpl::updateWindowPosAndSize(void)
{
    int xpos, ypos;
    glfwGetWindowPos(window_, &xpos, &ypos);
    window_xpos_ = xpos;
    window_ypos_ = ypos;
    
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    windowed_width_ = width;
    windowed_height_ = height;
}

bool DirectImpl::initOpenGLFunctionPointers(void)
{
    bool result = true;

    // GL 1.2
    result &= (glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)glfwGetProcAddress("glDrawRangeElements")) != NULL;
    result &= (glTexImage3D = (PFNGLTEXIMAGE3DPROC)glfwGetProcAddress("glTexImage3D")) != NULL;
    result &= (glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)glfwGetProcAddress("glTexSubImage3D")) != NULL;
    result &= (glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC)glfwGetProcAddress("glCopyTexSubImage3D")) != NULL;

    // GL 1.3
    result &= (glActiveTexture = (PFNGLACTIVETEXTUREPROC)glfwGetProcAddress("glActiveTexture")) != NULL;
    result &= (glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC)glfwGetProcAddress("glSampleCoverage")) != NULL;
    result &= (glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)glfwGetProcAddress("glCompressedTexImage3D")) != NULL;
    result &= (glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)glfwGetProcAddress("glCompressedTexImage2D")) != NULL;
    result &= (glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC)glfwGetProcAddress("glCompressedTexImage1D")) != NULL;
    result &= (glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)glfwGetProcAddress("glCompressedTexSubImage3D")) != NULL;
    result &= (glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)glfwGetProcAddress("glCompressedTexSubImage2D")) != NULL;
    result &= (glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)glfwGetProcAddress("glCompressedTexSubImage1D")) != NULL;
    result &= (glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)glfwGetProcAddress("glGetCompressedTexImage")) != NULL;

    // GL 1.4
    result &= (glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)glfwGetProcAddress("glBlendFuncSeparate")) != NULL;
    result &= (glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC)glfwGetProcAddress("glMultiDrawArrays")) != NULL;
    result &= (glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC)glfwGetProcAddress("glMultiDrawElements")) != NULL;
    result &= (glPointParameterf = (PFNGLPOINTPARAMETERFPROC)glfwGetProcAddress("glPointParameterf")) != NULL;
    result &= (glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)glfwGetProcAddress("glPointParameterfv")) != NULL;
    result &= (glPointParameteri = (PFNGLPOINTPARAMETERIPROC)glfwGetProcAddress("glPointParameteri")) != NULL;
    result &= (glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC)glfwGetProcAddress("glPointParameteriv")) != NULL;
    result &= (glBlendColor = (PFNGLBLENDCOLORPROC)glfwGetProcAddress("glBlendColor")) != NULL;
    result &= (glBlendEquation = (PFNGLBLENDEQUATIONPROC)glfwGetProcAddress("glBlendEquation")) != NULL;

    // GL 1.5
    result &= (glGenQueries = (PFNGLGENQUERIESPROC)glfwGetProcAddress("glGenQueries")) != NULL;
    result &= (glDeleteQueries = (PFNGLDELETEQUERIESPROC)glfwGetProcAddress("glDeleteQueries")) != NULL;
    result &= (glIsQuery = (PFNGLISQUERYPROC)glfwGetProcAddress("glIsQuery")) != NULL;
    result &= (glBeginQuery = (PFNGLBEGINQUERYPROC)glfwGetProcAddress("glBeginQuery")) != NULL;
    result &= (glEndQuery = (PFNGLENDQUERYPROC)glfwGetProcAddress("glEndQuery")) != NULL;
    result &= (glGetQueryiv = (PFNGLGETQUERYIVPROC)glfwGetProcAddress("glGetQueryiv")) != NULL;
    result &= (glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)glfwGetProcAddress("glGetQueryObjectiv")) != NULL;
    result &= (glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)glfwGetProcAddress("glGetQueryObjectuiv")) != NULL;
    result &= (glBindBuffer = (PFNGLBINDBUFFERPROC)glfwGetProcAddress("glBindBuffer")) != NULL;
    result &= (glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glfwGetProcAddress("glDeleteBuffers")) != NULL;
    result &= (glGenBuffers = (PFNGLGENBUFFERSPROC)glfwGetProcAddress("glGenBuffers")) != NULL;
    result &= (glIsBuffer = (PFNGLISBUFFERPROC)glfwGetProcAddress("glIsBuffer")) != NULL;
    result &= (glBufferData = (PFNGLBUFFERDATAPROC)glfwGetProcAddress("glBufferData")) != NULL;
    result &= (glBufferSubData = (PFNGLBUFFERSUBDATAPROC)glfwGetProcAddress("glBufferSubData")) != NULL;
    result &= (glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)glfwGetProcAddress("glGetBufferSubData")) != NULL;
    result &= (glMapBuffer = (PFNGLMAPBUFFERPROC)glfwGetProcAddress("glMapBuffer")) != NULL;
    result &= (glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)glfwGetProcAddress("glUnmapBuffer")) != NULL;
    result &= (glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)glfwGetProcAddress("glGetBufferParameteriv")) != NULL;
    result &= (glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)glfwGetProcAddress("glGetBufferPointerv")) != NULL;

    // GL 2.0
    result &= (glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)glfwGetProcAddress("glBlendEquationSeparate")) != NULL;
    result &= (glDrawBuffers = (PFNGLDRAWBUFFERSPROC)glfwGetProcAddress("glDrawBuffers")) != NULL;
    result &= (glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)glfwGetProcAddress("glStencilOpSeparate")) != NULL;
    result &= (glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)glfwGetProcAddress("glStencilFuncSeparate")) != NULL;
    result &= (glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)glfwGetProcAddress("glStencilMaskSeparate")) != NULL;
    result &= (glAttachShader = (PFNGLATTACHSHADERPROC)glfwGetProcAddress("glAttachShader")) != NULL;
    result &= (glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)glfwGetProcAddress("glBindAttribLocation")) != NULL;
    result &= (glCompileShader = (PFNGLCOMPILESHADERPROC)glfwGetProcAddress("glCompileShader")) != NULL;
    result &= (glCreateProgram = (PFNGLCREATEPROGRAMPROC)glfwGetProcAddress("glCreateProgram")) != NULL;
    result &= (glCreateShader = (PFNGLCREATESHADERPROC)glfwGetProcAddress("glCreateShader")) != NULL;
    result &= (glDeleteProgram = (PFNGLDELETEPROGRAMPROC)glfwGetProcAddress("glDeleteProgram")) != NULL;
    result &= (glDeleteShader = (PFNGLDELETESHADERPROC)glfwGetProcAddress("glDeleteShader")) != NULL;
    result &= (glDetachShader = (PFNGLDETACHSHADERPROC)glfwGetProcAddress("glDetachShader")) != NULL;
    result &= (glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)glfwGetProcAddress("glDisableVertexAttribArray")) != NULL;
    result &= (glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glfwGetProcAddress("glEnableVertexAttribArray")) != NULL;
    result &= (glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)glfwGetProcAddress("glGetActiveAttrib")) != NULL;
    result &= (glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)glfwGetProcAddress("glGetActiveUniform")) != NULL;
    result &= (glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)glfwGetProcAddress("glGetAttachedShaders")) != NULL;
    result &= (glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)glfwGetProcAddress("glGetAttribLocation")) != NULL;
    result &= (glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glfwGetProcAddress("glGetProgramiv")) != NULL;
    result &= (glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glfwGetProcAddress("glGetProgramInfoLog")) != NULL;
    result &= (glGetShaderiv = (PFNGLGETSHADERIVPROC)glfwGetProcAddress("glGetShaderiv")) != NULL;
    result &= (glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glfwGetProcAddress("glGetShaderInfoLog")) != NULL;
    result &= (glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)glfwGetProcAddress("glGetShaderSource")) != NULL;
    result &= (glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glfwGetProcAddress("glGetUniformLocation")) != NULL;
    result &= (glGetUniformfv = (PFNGLGETUNIFORMFVPROC)glfwGetProcAddress("glGetUniformfv")) != NULL;
    result &= (glGetUniformiv = (PFNGLGETUNIFORMIVPROC)glfwGetProcAddress("glGetUniformiv")) != NULL;
    result &= (glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)glfwGetProcAddress("glGetVertexAttribdv")) != NULL;
    result &= (glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)glfwGetProcAddress("glGetVertexAttribfv")) != NULL;
    result &= (glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)glfwGetProcAddress("glGetVertexAttribiv")) != NULL;
    result &= (glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)glfwGetProcAddress("glGetVertexAttribPointerv")) != NULL;
    result &= (glIsProgram = (PFNGLISPROGRAMPROC)glfwGetProcAddress("glIsProgram")) != NULL;
    result &= (glIsShader = (PFNGLISSHADERPROC)glfwGetProcAddress("glIsShader")) != NULL;
    result &= (glLinkProgram = (PFNGLLINKPROGRAMPROC)glfwGetProcAddress("glLinkProgram")) != NULL;
    result &= (glShaderSource = (PFNGLSHADERSOURCEPROC)glfwGetProcAddress("glShaderSource")) != NULL;
    result &= (glUseProgram = (PFNGLUSEPROGRAMPROC)glfwGetProcAddress("glUseProgram")) != NULL;
    result &= (glUniform1f = (PFNGLUNIFORM1FPROC)glfwGetProcAddress("glUniform1f")) != NULL;
    result &= (glUniform2f = (PFNGLUNIFORM2FPROC)glfwGetProcAddress("glUniform2f")) != NULL;
    result &= (glUniform3f = (PFNGLUNIFORM3FPROC)glfwGetProcAddress("glUniform3f")) != NULL;
    result &= (glUniform4f = (PFNGLUNIFORM4FPROC)glfwGetProcAddress("glUniform4f")) != NULL;
    result &= (glUniform1i = (PFNGLUNIFORM1IPROC)glfwGetProcAddress("glUniform1i")) != NULL;
    result &= (glUniform2i = (PFNGLUNIFORM2IPROC)glfwGetProcAddress("glUniform2i")) != NULL;
    result &= (glUniform3i = (PFNGLUNIFORM3IPROC)glfwGetProcAddress("glUniform3i")) != NULL;
    result &= (glUniform4i = (PFNGLUNIFORM4IPROC)glfwGetProcAddress("glUniform4i")) != NULL;
    result &= (glUniform1fv = (PFNGLUNIFORM1FVPROC)glfwGetProcAddress("glUniform1fv")) != NULL;
    result &= (glUniform2fv = (PFNGLUNIFORM2FVPROC)glfwGetProcAddress("glUniform2fv")) != NULL;
    result &= (glUniform3fv = (PFNGLUNIFORM3FVPROC)glfwGetProcAddress("glUniform3fv")) != NULL;
    result &= (glUniform4fv = (PFNGLUNIFORM4FVPROC)glfwGetProcAddress("glUniform4fv")) != NULL;
    result &= (glUniform1iv = (PFNGLUNIFORM1IVPROC)glfwGetProcAddress("glUniform1iv")) != NULL;
    result &= (glUniform2iv = (PFNGLUNIFORM2IVPROC)glfwGetProcAddress("glUniform2iv")) != NULL;
    result &= (glUniform3iv = (PFNGLUNIFORM3IVPROC)glfwGetProcAddress("glUniform3iv")) != NULL;
    result &= (glUniform4iv = (PFNGLUNIFORM4IVPROC)glfwGetProcAddress("glUniform4iv")) != NULL;
    result &= (glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)glfwGetProcAddress("glUniformMatrix2fv")) != NULL;
    result &= (glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)glfwGetProcAddress("glUniformMatrix3fv")) != NULL;
    result &= (glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)glfwGetProcAddress("glUniformMatrix4fv")) != NULL;
    result &= (glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)glfwGetProcAddress("glValidateProgram")) != NULL;
    result &= (glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glfwGetProcAddress("glVertexAttribPointer")) != NULL;

    // GL 3.0
    result &= (glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)glfwGetProcAddress("glRenderbufferStorageMultisample")) != NULL;

    // GL 3.1 
    result &= (glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)glfwGetProcAddress("glIsRenderbuffer")) != NULL;
    result &= (glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)glfwGetProcAddress("glBindRenderbuffer")) != NULL;
    result &= (glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)glfwGetProcAddress("glDeleteRenderbuffers")) != NULL;
    result &= (glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)glfwGetProcAddress("glGenRenderbuffers")) != NULL;
    result &= (glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)glfwGetProcAddress("glRenderbufferStorage")) != NULL;
    result &= (glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)glfwGetProcAddress("glGetRenderbufferParameteriv")) != NULL;
    result &= (glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)glfwGetProcAddress("glIsFramebuffer")) != NULL;
    result &= (glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer")) != NULL;
    result &= (glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers")) != NULL;
    result &= (glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers")) != NULL;
    result &= (glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatus")) != NULL;
    result &= (glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)glfwGetProcAddress("glFramebufferTexture1D")) != NULL;
    result &= (glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glfwGetProcAddress("glFramebufferTexture2D")) != NULL;
    result &= (glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)glfwGetProcAddress("glFramebufferTexture3D")) != NULL;
    result &= (glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)glfwGetProcAddress("glFramebufferRenderbuffer")) != NULL;
    result &= (glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)glfwGetProcAddress("glGetFramebufferAttachmentParameteriv")) != NULL;
    result &= (glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)glfwGetProcAddress("glGenerateMipmap")) != NULL;
    result &= (glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays")) != NULL;
    result &= (glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray")) != NULL;
    return result;
}

} // namespace