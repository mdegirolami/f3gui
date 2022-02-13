#pragma once

#include <sing.h>
#include "direct/gles.h"

namespace mmapp {

class ISoundSynth {
public:
    virtual ~ISoundSynth() {}
    virtual void *get__id() const = 0;
    virtual void fillSamples(int32_t samples_count, std::vector<int16_t> *samples) = 0;
};

enum class CursorMode {standard, disabled};
enum class CursorShape {arrow, ibeam, xair, hand, h_resize, v_resize};

enum class ScheduleMode {loop_fast, wake_on_event};

static const int32_t win_resizable = 1;
static const int32_t win_decorated = 2;
static const int32_t win_maximized = 4;
static const int32_t win_exit_on_esc = 8;

static const int32_t mouse_keycount = 8;
static const int32_t mouse_mbr = 0;
static const int32_t mouse_mbl = 1;
static const int32_t mouse_mbc = 2;

static const int32_t joy_axis_x = 0;
static const int32_t joy_axis_y = 1;
static const int32_t joy_axis_z = 2;
static const int32_t joy_axis_rx = 3;
static const int32_t joy_axis_ry = 4;
static const int32_t joy_axis_rz = 5;
static const int32_t joy_axis_slider0 = 6;
static const int32_t joy_axis_slider1 = 7;

static const int32_t hat_up = 1;
static const int32_t hat_right = 2;
static const int32_t hat_down = 4;
static const int32_t hat_left = 8;

static const int32_t gamepad_axis_left_x = 0;
static const int32_t gamepad_axis_left_y = 1;
static const int32_t gamepad_axis_right_x = 2;
static const int32_t gamepad_axis_right_y = 3;
static const int32_t gamepad_axis_left_trigger = 4;
static const int32_t gamepad_axis_right_trigger = 5;

static const int32_t gamepad_button_a = 0;
static const int32_t gamepad_button_b = 1;
static const int32_t gamepad_button_x = 2;
static const int32_t gamepad_button_y = 3;
static const int32_t gamepad_button_left_bumper = 4;
static const int32_t gamepad_button_right_bumper = 5;
static const int32_t gamepad_button_back = 6;
static const int32_t gamepad_button_start = 7;
static const int32_t gamepad_button_guide = 8;
static const int32_t gamepad_button_left_thumb = 9;
static const int32_t gamepad_button_right_thumb = 10;
static const int32_t gamepad_button_dpad_up = 11;
static const int32_t gamepad_button_dpad_right = 12;
static const int32_t gamepad_button_dpad_down = 13;
static const int32_t gamepad_button_dpad_left = 14;

static const int32_t max_contacts = 5;

//
// begin: is returned just once, makes the slot active - The TouchContact entries are valid
// active: the contact is active - The TouchContact entries are valid
// ended: end of the contact - The TouchContact entries are valid 
// cancelled: end of the contact because of an external event (es: lost focus of the app). Ignore the gesture if you can.
// free_slot: unused record in the returned vector.
//
enum class ContactPhase {begin, active, ended, cancelled, free_slot};

class TouchContact final {
public:
    TouchContact();
    ContactPhase status_;
    int32_t tapcounts_;                 // single or double
    float startx_;                      // very first contact position (when status was 'begin')     
    float starty_;
    float prevx_;                       // previously known location (previous read)
    float prevy_;
    float endx_;    // current location
    float endy_;
};

class IMmAppServices {
public:
    virtual ~IMmAppServices() {}
    virtual void *get__id() const = 0;

    // for each entry_index returns an available full screen video mode. If entry_index is too high returns false
    virtual bool enumerateModes(int32_t entry_index, int32_t *width, int32_t *height) const = 0;

    // the resolution we want to use when in full screen
    virtual bool setFullScreenVideoResolution(int32_t width, int32_t height) = 0;
    virtual void getFullScreenVideoResolution(int32_t *width, int32_t *height) const = 0;

    // to actually switch to/from full screen modes
    virtual bool changeToFromWindowedMode(bool windowed) = 0;

    // the resolution and position of the window when in windowed mode
    virtual void getWindowedVideoResolution(int32_t *width, int32_t *height) const = 0;
    virtual void getWindowedWindowPosition(int32_t *xpos, int32_t *ypos) const = 0;

    // the current resolution (depends if we are in windowed or full screen mode)
    virtual void getResolution(int32_t *width, int32_t *height) const = 0;

    // makes the last rendered scene effective
    virtual void swapBuffers() = 0;

    virtual bool isMaximized() const = 0;
    virtual bool isWindowed() const = 0;

    virtual void setTitle(const char *title) = 0;
    virtual void setIcon(const std::vector<uint8_t> &icon) = 0;
    virtual void forceWindowAspectRatio(int32_t width, int32_t height) = 0;
    virtual void setWindowSizeLimits(int32_t min_w, int32_t min_h, int32_t max_w, int32_t max_h) = 0;

    // for mobile devices
    virtual void enableScreenRotation(int32_t allowed_orientations_mask) = 0;
    virtual int32_t getScreenRotation() const = 0;

    // SOUND
    virtual bool startSoundSource(int32_t chunk_samples, int32_t rate, ISoundSynth *callback) = 0;
    virtual void shutdownSoundSource() = 0;

    // INPUT
    virtual bool isKeyDown(int32_t key_id) const = 0;
    virtual bool isKeyJustPressed(int32_t key_id) const = 0;
    virtual std::string getKeyName(int32_t key_id) const = 0;
    virtual int32_t getCodePoint() = 0;
    virtual void flushCodePoints() = 0;

    virtual bool isMouseKeyDown(int32_t key_index) const = 0;
    virtual bool isMouseKeyJustPressed(int32_t key_index) const = 0;
    virtual int32_t getMouseWheel() = 0;

    virtual void setCursorMode(CursorMode mode) = 0;
    virtual void setCursorShape(CursorShape shape) = 0;
    virtual void getMousePosition(int32_t *xx, int32_t *yy) const = 0;
    virtual void setMousePosition(int32_t xx, int32_t yy) = 0;
    virtual void getMouseMovement(int32_t *dx, int32_t *dy) const = 0;

    virtual bool isJoystickPresent(int32_t idx = 0) const = 0;
    virtual bool isJoystickAGamepad(int32_t idx = 0) const = 0;
    virtual void getJoystickButtons(std::vector<bool> *buttons, int32_t idx = 0) const = 0;
    virtual void getJoystickAxes(std::vector<float> *axis, int32_t idx = 0) const = 0;
    virtual void getJoystickHats(std::vector<int32_t> *hats, int32_t idx = 0) const = 0;
    virtual std::string getJoystickName(int32_t idx = 0) const = 0;

    virtual void getTouchScreenContacts(sing::array<TouchContact, max_contacts> *contacts) = 0;
    virtual void flushTouchScreenContacts() = 0;

    virtual void getAccelerometerVector(float *xx, float *yy, float *zz) const = 0;

    // clipboard and dragging
    virtual std::string getClipboardString() const = 0;
    virtual void setClipboardString(const char *str) = 0;
    virtual void getDroppedPaths(std::vector<std::string> *paths) = 0;

    // MIX
    virtual void setScheduleMode(ScheduleMode mode, float tout) = 0;
    virtual void postEmptyEvent() = 0;
    virtual bool didLooseFocus() = 0;                       // games need to know so they switch to a pause screen.
    virtual bool didLooseContext() = 0;
    virtual int32_t getAvailableMemory() const = 0;         // Mbytes, returns 0 if unsupported on the platform.
};

class IMmApp {
public:
    virtual ~IMmApp() {}
    virtual void *get__id() const = 0;

    // invoked once at startup
    virtual bool initApplication(IMmAppServices *mm) = 0;

    // invoked periodically
    virtual bool stepApplication(IMmAppServices *mm, IGlEs *gles) = 0;

    // invoked when the user press the close button on the window bar. Return false to prevent the app from closing
    virtual bool canExit() = 0;

    // invoked once on exit
    virtual bool quitApplication() = 0;
};

// exits when the application exits
void runMmApp(IMmApp *the_app, const char *win_title, int32_t win_flags, int32_t win_width, int32_t win_height);

}   // namespace
