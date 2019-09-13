#pragma once
#include "Engine/Input/XboxController.hpp"

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/Stopwatch.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"

#include <array>
#include <bitset>

class Window;

enum class KeyCode : int {
    FirstMouseButton_ /* Internal use only. */
    ,LButton = FirstMouseButton_ /* Left Mouse Button */
    ,RButton /* Right Mouse Button */
    ,Cancel /* Control-break processing */
    ,MButton /* Middle Mouse Button */
    ,XButton1 /* Xtra Mouse Button 1 */
    ,XButton2 /* Xtra Mouse Button 2 */
    ,LastMouseButton_ /* Internal use only. */
    ,First_ /* Internal use only. */
    ,Back = First_  /* Also Backspace */
    ,Backspace = Back /* Also Back */
    ,Tab
    ,Clear
    ,Return  /* Also Enter */
    ,Enter = Return
    ,Shift /* either RShift or LShift */
    ,Ctrl  /* either RCtrl or LCtrl */
    ,Menu  /* either RMenu or LMenu, Also Alt */
    ,Alt = Menu /* either RAlt or LAlt, Also Menu */
    ,Pause
    ,Capital  /* Also CapsLock */
    ,CapsLock = Capital
    ,Kana
    ,Hangul  /* Also Hangeul */
    ,Hangeul = Hangul /* Also Hangul */
    ,Junja
    ,Final
    ,Hanja
    ,Kanji
    ,Escape  /* Also Esc */
    ,Esc = Escape
    ,Convert
    ,NonConvert
    ,Accept
    ,ModeChange
    ,Space  /* Also Spacebar */
    ,Spacebar = Space
    ,Prior  /* Also PageUp */
    ,PageUp = Prior
    ,Next  /* Also PageDown and PageDn */
    ,PageDown = Next /* Also PageDn or Next */
    ,PageDn = Next /* Also PageDown or Next */
    ,End
    ,Home
    ,Left
    ,Up
    ,Right
    ,Down
    ,Select
    ,Print
    ,Execute
    ,Snapshot  /* Also PrintScreen */
    ,PrintScreen = Snapshot /* Also Snapshot */
    ,Insert
    ,Delete  /* Also Del */
    ,Del = Delete /* Also Delete */
    ,Help
    ,Numeric0 /* Number key above keyboard */
    ,Numeric1 /* Number key above keyboard */
    ,Numeric2 /* Number key above keyboard */
    ,Numeric3 /* Number key above keyboard */
    ,Numeric4 /* Number key above keyboard */
    ,Numeric5 /* Number key above keyboard */
    ,Numeric6 /* Number key above keyboard */
    ,Numeric7 /* Number key above keyboard */
    ,Numeric8 /* Number key above keyboard */
    ,Numeric9 /* Number key above keyboard */
    ,A
    ,B
    ,C
    ,D
    ,E
    ,F
    ,G
    ,H
    ,I
    ,J
    ,K
    ,L
    ,M
    ,N
    ,O
    ,P
    ,Q
    ,R
    ,S
    ,T
    ,U
    ,V
    ,W
    ,X
    ,Y
    ,Z
    ,LWin
    ,RWin
    ,Apps
    ,Sleep
    ,NumPad0
    ,NumPad1
    ,NumPad2
    ,NumPad3
    ,NumPad4
    ,NumPad5
    ,NumPad6
    ,NumPad7
    ,NumPad8
    ,NumPad9
    ,Multiply
    ,Add
    ,Separator  /* Also NumPadEnter */
    ,NumPadEnter = Separator /* Also Separator */
    ,Subtract
    ,Decimal
    ,Divide
    ,F1
    ,F2
    ,F3
    ,F4
    ,F5
    ,F6
    ,F7
    ,F8
    ,F9
    ,F10
    ,F11
    ,F12
    ,F13
    ,F14
    ,F15
    ,F16
    ,F17
    ,F18
    ,F19
    ,F20
    ,F21
    ,F22
    ,F23
    ,F24
    ,NumLock
    ,Scroll  /* Also ScrollLock */
    ,ScrollLock = Scroll /* Also Scroll */
    ,Oem_Nec_Equal
    ,Oem_Fj_Jisho
    ,Oem_Fj_Masshou
    ,Oem_Fj_Touroku
    ,Oem_Fj_Loya
    ,Oem_Fj_Roya
    ,LShift
    ,RShift
    ,LControl  /* Also LCtrl */
    ,LCtrl = LControl /* Also LControl */
    ,RControl  /* Also RCtrl */
    ,RCtrl = RControl /* Also RControl */
    ,RMenu  /* Also RAlt */
    ,RAlt = RMenu /* Also RMenu */
    ,LMenu  /* Also LAlt */
    ,LAlt = LMenu /* Also LMenu */
    ,Browser_Back
    ,Browser_Forward
    ,Browser_Refresh
    ,Browser_Stop
    ,Browser_Search
    ,Browser_Favorites
    ,Browser_Home
    ,Volume_Mute
    ,Volume_Down
    ,Volume_Up
    ,Media_Next_Track
    ,Media_Prev_Track
    ,Media_Stop
    ,Media_Play_Pause
    ,Launch_Mail
    ,Launch_Media_Select
    ,Launch_App1
    ,Launch_App2
    ,Oem_1  /* Key ;: */
    ,Semicolon = Oem_1 /* Key ;: */
    ,Oem_Plus  /* Key =+ */
    ,Equals = Oem_Plus /* Key =+ */
    ,Oem_Comma  /* Key ,< */
    ,Comma = Oem_Comma /* Key ,< */
    ,Oem_Minus  /* Key -_ */
    ,Minus = Oem_Minus /* Key -_ */
    ,Oem_Period /* Key .> */
    ,Period = Oem_Period /* Key .> */
    ,Oem_2 /* Key /? */
    ,ForwardSlash = Oem_2 /* Key /?, Also FSlash */
    ,FSlash = Oem_2 /* Key /?, Also ForwardSlash */
    ,Oem_3 /* Key `~ */
    ,Backquote = Oem_3 /* Key `~, Also Tilde */
    ,Tilde = Oem_3 /* Key `~, Also Backquote */
    ,Gamepad_First_ /* Internal use only. */
    ,Gamepad_A = Gamepad_First_
    ,Gamepad_B
    ,Gamepad_X
    ,Gamepad_Y
    ,Gamepad_Right_Shoulder
    ,Gamepad_Left_Shoulder
    ,Gamepad_Left_Trigger
    ,Gamepad_Right_Trigger
    ,Gamepad_DPad_Up
    ,Gamepad_DPad_Down
    ,Gamepad_DPad_Left
    ,Gamepad_DPad_Right
    ,Gamepad_Menu
    ,Gamepad_View
    ,Gamepad_Left_Thumbstick_Button
    ,Gamepad_Right_Thumbstick_Button
    ,Gamepad_Left_Thumbstick_Up
    ,Gamepad_Left_Thumbstick_Down
    ,Gamepad_Left_Thumbstick_Right
    ,Gamepad_Left_Thumbstick_Left
    ,Gamepad_Right_Thumbstick_Up
    ,Gamepad_Right_Thumbstick_Down
    ,Gamepad_Right_Thumbstick_Right
    ,Gamepad_Right_Thumbstick_Left
    ,Gamepad_Last_ /* Internal use only. */
    ,Oem_4 /* Key [{ */
    ,LeftBracket = Oem_4 /* Key [{, Also LBracket */
    ,LBracket = Oem_4 /* Key [{, Also LeftBracket */
    ,Oem_5 /* Key \|, Also Backslash */
    ,Backslash = Oem_5 /* Key \| */
    ,Oem_6 /* Key ]} */
    ,RightBracket = Oem_6 /* Key ]}, Also RBracket */
    ,RBracket = Oem_6 /* Key ]}, Also RightBracket */
    ,Oem_7 /* Key '" */
    ,Apostrophe = Oem_7 /* Key '", Also Apostrophe */
    ,SingleQuote = Oem_7 /* Key '", Also SingleQuote */
    ,Oem_8  /* misc. unknown */
    ,Oem_Ax
    ,Oem_102  /* RT 102's "<>" or "\|" */
    ,Ico_Help  /* Help key on ICO keyboard */
    ,Ico_00  /* 00 key on ICO keyboard */
    ,ProcessKey
    ,Ico_Clear  /* Clear key on ICO keyboard */
    ,Packet  /* Key is packet of data */
    ,Oem_Reset
    ,Oem_Jump
    ,Oem_Pa1
    ,Oem_Pa2
    ,Oem_Pa3
    ,Oem_WsCtrl
    ,Oem_CuSel
    ,Oem_Attn
    ,Oem_Finish
    ,Oem_Copy
    ,Oem_Auto
    ,Oem_EnlW
    ,Oem_BackTab
    ,Attn
    ,CrSel
    ,ExSel
    ,ErEof
    ,Play
    ,Zoom
    ,NoName
    ,Pa1
    ,Oem_Clear
    ,Last_ /* Internal use only */
    ,Unknown = 0xFF /* A manufacturer-specific key was pressed. */
    ,Max /* Internal use only */
};

KeyCode& operator++(KeyCode& keycode) noexcept;
KeyCode operator++(KeyCode& keycode, int) noexcept;

class InputSystem : public EngineSubsystem {
public:
    InputSystem() noexcept = default;
    InputSystem(const InputSystem& other) noexcept = default;
    InputSystem(InputSystem&& r_other) noexcept = default;
    InputSystem& operator=(const InputSystem& rhs) noexcept = default;
    InputSystem& operator=(InputSystem&& rhs) noexcept = default;
    virtual ~InputSystem() noexcept = default;

    void RegisterKeyDown(unsigned char keyIndex) noexcept;
    void RegisterKeyUp(unsigned char keyIndex) noexcept;
    
    virtual bool ProcessSystemMessage(const EngineMessage& msg) noexcept override;
    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update([[maybe_unused]]TimeUtils::FPSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;

    bool WasAnyKeyPressed() const noexcept;
    bool IsKeyUp(const KeyCode& key) const noexcept;
    bool WasKeyJustPressed(const KeyCode& key) const noexcept;
    bool IsKeyDown(const KeyCode& key) const noexcept;
    bool IsAnyKeyDown() const noexcept;
    bool WasKeyJustReleased(const KeyCode& key) const noexcept;
    bool WasMouseWheelJustScrolledUp() const noexcept;
    bool WasMouseWheelJustScrolledDown() const noexcept;
    bool WasMouseWheelJustScrolledLeft() const noexcept;
    bool WasMouseWheelJustScrolledRight() const noexcept;

    std::size_t GetConnectedControllerCount() const noexcept;
    bool IsAnyControllerConnected() const noexcept;
    const XboxController& GetXboxController(const std::size_t& controllerIndex) const noexcept;
    XboxController& GetXboxController(const std::size_t& controllerIndex) noexcept;

    static unsigned char ConvertKeyCodeToWinVK(const KeyCode& code) noexcept;
    static KeyCode ConvertWinVKToKeyCode(unsigned char winVK) noexcept;

    void ToggleMouseCursorVisibility() noexcept;
    void HideMouseCursor() noexcept;
    void ShowMouseCursor() noexcept;

    void SetCursorScreenPosition(const Vector2& screen_pos) noexcept;
    Vector2 GetCursorScreenPosition() const noexcept;

    void SetCursorWindowPosition(const Window& window, const Vector2& window_pos) noexcept;
    Vector2 GetCursorWindowPosition(const Window& window_ref) const noexcept;

    void SetCursorToScreenCenter() noexcept;
    void SetCursorToWindowCenter(const Window& window_ref) noexcept;
    const Vector2& GetMouseCoords() const noexcept;

    int GetMouseWheelPosition() const noexcept;
    int GetMouseWheelPositionNormalized() const noexcept;

    int GetMouseWheelHorizontalPosition() const noexcept;
    int GetMouseWheelHorizontalPositionNormalized() const noexcept;

    IntVector2 GetMouseWheelPositionAsIntVector2() const noexcept;

protected:
private:

    void UpdateXboxConnectedState() noexcept;

    Vector2 GetScreenCenter() const noexcept;
    Vector2 GetWindowCenter(const Window& window) const noexcept;

    std::array<XboxController, 4> _xboxControllers{};
    std::bitset<(std::size_t)KeyCode::Max> _previousKeys{};
    std::bitset<(std::size_t)KeyCode::Max> _currentKeys{};
    Vector2 _mouseCoords = Vector2::ZERO;
    Vector2 _mouseDelta = Vector2::ZERO;
    Stopwatch _connection_poll = Stopwatch(TimeUtils::FPSeconds{ 1.0f });
    int _mouseWheelPosition = 0;
    int _mouseWheelHPosition = 0;
    int _connected_controller_count = 0;
    bool _cursor_visible = true;
};