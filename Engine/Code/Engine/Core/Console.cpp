#include "Engine/Core/Console.hpp"

#include "Engine/Core/ArgumentParser.hpp"
#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/Clipboard.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileLogger.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Win.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/RHI/RHIOutput.hpp"
#include "Engine/Renderer/Camera2D.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Window.hpp"

#include <iterator>
#include <sstream>
#include <string>
#include <utility>

namespace a2de {

    constexpr const uint16_t IDM_COPY = 0;
    constexpr const uint16_t IDM_PASTE = 1;
    constexpr const uint16_t IDM_CUT = 2;
    constexpr const uint16_t IDM_SELECTALL = 3;
    HACCEL hAcceleratorTable{};

    void* Console::GetAcceleratorTable() const noexcept {
        return static_cast<void*>(hAcceleratorTable);
    }

    Console::Console(FileLogger& fileLogger, Renderer& renderer) noexcept
    : EngineSubsystem()
    , _fileLogger(fileLogger)
    , _renderer(renderer)
    , _show_cursor(false)
    , _is_open(false)
    , _highlight_mode(false)
    , _skip_nonwhitespace_mode(false)
    , _dirty_text(false)
    , _non_rendering_char(false)
    , _entryline_changed(false)
    , _output_changed(false)

    {
        ACCEL copy{};
        copy.fVirt = FCONTROL | FVIRTKEY;
        copy.key = InputSystem::ConvertKeyCodeToWinVK(KeyCode::C);
        copy.cmd = IDM_COPY;

        ACCEL paste{};
        paste.fVirt = FCONTROL | FVIRTKEY;
        paste.key = InputSystem::ConvertKeyCodeToWinVK(KeyCode::V);
        paste.cmd = IDM_PASTE;

        ACCEL cut{};
        cut.fVirt = FCONTROL | FVIRTKEY;
        cut.key = InputSystem::ConvertKeyCodeToWinVK(KeyCode::X);
        cut.cmd = IDM_CUT;

        ACCEL select_all{};
        select_all.fVirt = FCONTROL | FVIRTKEY;
        select_all.key = InputSystem::ConvertKeyCodeToWinVK(KeyCode::A);
        select_all.cmd = IDM_SELECTALL;

        std::vector<ACCEL> accelerators = {copy, paste, cut, select_all};
        hAcceleratorTable = ::CreateAcceleratorTableA(accelerators.data(), static_cast<int>(accelerators.size()));
    }

    Console::~Console() noexcept {
        ::DestroyAcceleratorTable(hAcceleratorTable);
        _commands.clear();
    }

    bool Console::ProcessSystemMessage(const EngineMessage& msg) noexcept {
        LPARAM lp = msg.lparam;
        WPARAM wp = msg.wparam;
        switch(msg.wmMessageCode) {
        case WindowsSystemMessage::Menu_SysCommand:
        case WindowsSystemMessage::Menu_Command: {
            bool is_lp_not_valid = lp ? true : false;
            bool is_closed = IsClosed();
            bool is_not_from_accelerator = HIWORD(wp) == 0;
            bool wont_handle = is_closed || is_lp_not_valid || is_not_from_accelerator;
            if(wont_handle) {
                return false;
            }
            auto id = LOWORD(wp);
            switch(id) {
            case IDM_COPY:
                (void)HandleClipboardCopy(); //Return value only useful for Cut operation
                break;
            case IDM_PASTE:
                HandleClipboardPaste();
                break;
            case IDM_CUT:
                HandleClipboardCut();
                break;
            case IDM_SELECTALL:
                HandleSelectAll();
                break;
            }
            return true;
        }
        case WindowsSystemMessage::Keyboard_SysKeyDown: {
            _non_rendering_char = false;
            auto key = static_cast<unsigned char>(wp);
            auto lpBits = static_cast<uint32_t>(lp & 0xFFFFFFFFu);
            //0bTPXRRRRESSSSSSSSCCCCCCCCCCCCCCCC
            //C: repeat count
            //S: scan code
            //E: extended key flag
            //R: reserved
            //X: context code: 1 if the ALT key is down;
            //   0 if the WM_SYSKEYDOWN message is posted to the active window because no window has the keyboard focus.
            //P: previous state: 1 for already down
            //T: transition state: 0 for KEYDOWN
            constexpr uint32_t repeat_count_mask = 0b0000'0000'0000'0000'1111'1111'1111'1111;     //0x0000FFFF;
            constexpr uint32_t scan_code_mask = 0b0000'0000'1111'1111'0000'0000'0000'0000;        //0x00FF0000;
            constexpr uint32_t extended_key_mask = 0b0000'0001'0000'0000'0000'0000'0000'0000;     //0x01000000;
            constexpr uint32_t reserved_mask = 0b0001'1110'0000'0000'0000'0000'0000'0000;         //0x1E000000;
            constexpr uint32_t context_code_mask = 0b0010'0000'0000'0000'0000'0000'0000'0000;     //0x20000000;
            constexpr uint32_t previous_state_mask = 0b0100'0000'0000'0000'0000'0000'0000'0000;   //0x40000000;
            constexpr uint32_t transition_state_mask = 0b1000'0000'0000'0000'0000'0000'0000'0000; //0x80000000;
            bool is_extended_key = (lpBits & extended_key_mask) != 0;
            if(key < 32 || key == 127) { //Control and Del chars
                _non_rendering_char = true;
            }
            auto my_key = InputSystem::ConvertWinVKToKeyCode(key);
            if(is_extended_key) {
                if(IsClosed()) {
                    return false;
                }
                switch(my_key) {
                case KeyCode::Alt: DebuggerPrintf("Pressing Alt.\n"); return true;
                case KeyCode::LAlt: DebuggerPrintf("Pressing LAlt.\n"); return true;
                case KeyCode::RAlt: DebuggerPrintf("Pressing RAlt.\n"); return true;
                default: return false;
                }
            }
            if(!_non_rendering_char) {
                return true;
            }
            return false;
        }
        case WindowsSystemMessage::Keyboard_KeyDown: {
            _non_rendering_char = false;
            auto key = static_cast<unsigned char>(wp);
            auto lpBits = static_cast<uint32_t>(lp & 0xFFFFFFFFu);
            //0bTPXRRRRESSSSSSSSCCCCCCCCCCCCCCCC
            //C: repeat count
            //S: scan code
            //E: extended key flag
            //R: reserved
            //X: context code: 0 for KEYDOWN
            //P: previous state: 1 for already down
            //T: transition state: 0 for KEYDOWN
            constexpr uint32_t repeat_count_mask = 0b0000'0000'0000'0000'1111'1111'1111'1111;     //0x0000FFFF;
            constexpr uint32_t scan_code_mask = 0b0000'0000'1111'1111'0000'0000'0000'0000;        //0x00FF0000;
            constexpr uint32_t extended_key_mask = 0b0000'0001'0000'0000'0000'0000'0000'0000;     //0x01000000;
            constexpr uint32_t reserved_mask = 0b0001'1110'0000'0000'0000'0000'0000'0000;         //0x1E000000;
            constexpr uint32_t context_code_mask = 0b0010'0000'0000'0000'0000'0000'0000'0000;     //0x20000000;
            constexpr uint32_t previous_state_mask = 0b0100'0000'0000'0000'0000'0000'0000'0000;   //0x40000000;
            constexpr uint32_t transition_state_mask = 0b1000'0000'0000'0000'0000'0000'0000'0000; //0x80000000;
            bool is_extended_key = (lpBits & extended_key_mask) != 0;
            if(key < 32 || key == 127) { //Control and Del chars
                _non_rendering_char = true;
            }
            auto my_key = InputSystem::ConvertWinVKToKeyCode(key);
            if(is_extended_key) {
                if(IsClosed()) {
                    return false;
                }
                switch(my_key) {
                case KeyCode::Alt: return true;
                case KeyCode::Ctrl: SetSkipNonWhitespaceMode(true); return true;
                //On Num Pad
                case KeyCode::Return: return HandleReturnKey();
                case KeyCode::LWin: return true;
                case KeyCode::RWin: return true;
                case KeyCode::Shift: SetHighlightMode(true); return true;
                //Left Arrow in cluster on 108-key keyboards
                case KeyCode::Left: return HandleLeftKey();
                //Right Arrow in cluster on 108-key keyboards
                case KeyCode::Right: return HandleRightKey();
                //Up Arrow in cluster on 108-key keyboards
                case KeyCode::Up: return HandleUpKey();
                //Down Arrow in cluster on 108-key keyboards
                case KeyCode::Down: return HandleDownKey();
                //Delete key in cluster on 108-key keyboards
                case KeyCode::Del: return HandleDelKey();
                //Nav cluster on 108-key keyboards
                case KeyCode::Home: return HandleHomeKey();
                //Nav cluster in 108-key keyboards
                case KeyCode::End: return HandleEndKey();
                default: return false;
                }
            }
            if(my_key == KeyCode::Tilde)
                return HandleTildeKey();
            if(IsClosed())
                return false;
            switch(my_key) {
            case KeyCode::Return: return HandleReturnKey();
            case KeyCode::Backspace: return HandleBackspaceKey();
            case KeyCode::Del: return HandleDelKey();
            case KeyCode::Escape: return HandleEscapeKey();
            //On Num Pad
            case KeyCode::Up: return HandleUpKey();
            //On Num Pad
            case KeyCode::Down: return HandleDownKey();
            //On Num Pad
            case KeyCode::Left: return HandleLeftKey();
            case KeyCode::Right: return HandleRightKey();
            //on Num Pad
            case KeyCode::Home: return HandleHomeKey();
            //on Num Pad
            case KeyCode::End: return HandleEndKey();
            case KeyCode::Ctrl: SetSkipNonWhitespaceMode(true); return true;
            case KeyCode::Shift: SetHighlightMode(true); return true;
            case KeyCode::Tab: return HandleTabKey();
            case KeyCode::F1: RunCommand(std::string("help ") + _entryline); return true;
            default: {
                if(!_non_rendering_char) {
                    return true;
                }
                return false;
            }
            }
        }
        case WindowsSystemMessage::Keyboard_Char: {
            if(IsClosed() || _non_rendering_char) {
                return false;
            }
            _entryline_changed = false;
            auto char_code = static_cast<unsigned char>(wp);
            //uint32_t lpBits = lp;
            //0bTPXRRRRESSSSSSSSCCCCCCCCCCCCCCCC
            //C: repeat count
            //S: scan code
            //E: extended key flag
            //R: reserved
            //X: context code: 1 if Alt is already down, 0 otherwise
            //P: previous state: 1 for already down
            //T: transition state: 1 if being released, 0 if being pressed
            //constexpr uint32_t repeat_count_mask     = 0b0000'0000'0000'0000'1111'1111'1111'1111; //0x0000FFFF;
            //constexpr uint32_t scan_code_mask        = 0b0000'0000'1111'1111'0000'0000'0000'0000; //0x00FF0000;
            //constexpr uint32_t extended_key_mask     = 0b0000'0001'0000'0000'0000'0000'0000'0000; //0x01000000;
            //constexpr uint32_t reserved_mask         = 0b0001'1110'0000'0000'0000'0000'0000'0000; //0x1E000000;
            //constexpr uint32_t context_code_mask     = 0b0010'0000'0000'0000'0000'0000'0000'0000; //0x20000000;
            //constexpr uint32_t previous_state_mask   = 0b0100'0000'0000'0000'0000'0000'0000'0000; //0x40000000;
            //constexpr uint32_t transition_state_mask = 0b1000'0000'0000'0000'0000'0000'0000'0000; //0x80000000;
            //bool is_extended_key = (lpBits & extended_key_mask) != 0;
            //bool is_alt_down = (lpBits & context_code_mask) != 0;
            //auto my_key = InputSystem::ConvertWinVKToKeyCode(char_code);
            InsertCharInEntryLine(char_code);
            return true;
        }
        case WindowsSystemMessage::Keyboard_SysKeyUp:
        case WindowsSystemMessage::Keyboard_KeyUp: {
            if(IsClosed()) {
                return false;
            }
            _entryline_changed = false;
            auto char_code = static_cast<unsigned char>(wp);
            auto lpBits = static_cast<uint32_t>(lp & 0xFFFFFFFFu);
            //0bTPXRRRRESSSSSSSSCCCCCCCCCCCCCCCC
            //C: repeat count
            //S: scan code
            //E: extended key flag
            //R: reserved
            //X: context code: 0 for KEYUP
            //P: previous state: 1 for KEYUP
            //T: transition state: 1 for KEYUP
            constexpr uint32_t repeat_count_mask = 0b0000'0000'0000'0000'1111'1111'1111'1111;     //0x0000FFFF;
            constexpr uint32_t scan_code_mask = 0b0000'0000'1111'1111'0000'0000'0000'0000;        //0x00FF0000;
            constexpr uint32_t extended_key_mask = 0b0000'0001'0000'0000'0000'0000'0000'0000;     //0x01000000;
            constexpr uint32_t reserved_mask = 0b0001'1110'0000'0000'0000'0000'0000'0000;         //0x1E000000;
            constexpr uint32_t context_code_mask = 0b0010'0000'0000'0000'0000'0000'0000'0000;     //0x20000000;
            constexpr uint32_t previous_state_mask = 0b0100'0000'0000'0000'0000'0000'0000'0000;   //0x40000000;
            constexpr uint32_t transition_state_mask = 0b1000'0000'0000'0000'0000'0000'0000'0000; //0x80000000;
            bool is_extended_key = (lpBits & extended_key_mask) != 0;
            //bool is_alt_down = (lpBits & context_code_mask) != 0;
            auto my_key = InputSystem::ConvertWinVKToKeyCode(char_code);
            if(is_extended_key) {
                switch(my_key) {
                case KeyCode::Ctrl: SetSkipNonWhitespaceMode(false); return true;
                case KeyCode::Shift: SetHighlightMode(false); return true;
                //Numpad Enter
                case KeyCode::Return: SetOutputChanged(true); return true;
                default: return false;
                }
            }
            switch(my_key) {
            case KeyCode::Shift: SetHighlightMode(false); return true;
            case KeyCode::Ctrl: SetSkipNonWhitespaceMode(false); return true;
            case KeyCode::Return: SetOutputChanged(true); return true;
            default: return false;
            }
        }
        case WindowsSystemMessage::Mouse_MouseWheel: {
            if(IsClosed()) {
                return false;
            }
            constexpr uint16_t wheeldelta_mask = 0b1111'1111'0000'0000;    //FF00
            constexpr uint16_t lbutton_mask = 0b0000'0000'0000'0001;       //0x0001
            constexpr uint16_t rbutton_mask = 0b0000'0000'0000'0010;       //0x0002
            constexpr uint16_t shift_mask = 0b0000'0000'0000'0100;         //0x0004
            constexpr uint16_t ctrl_mask = 0b0000'0000'0000'1000;          //0x0008
            constexpr uint16_t mbutton_mask = 0b0000'0000'0001'0000;       //0x0010
            constexpr uint16_t xbutton1_down_mask = 0b0000'0000'0010'0000; //0x0020
            constexpr uint16_t xbutton2_down_mask = 0b0000'0000'0100'0000; //0x0040
            _mouseWheelPosition = GET_WHEEL_DELTA_WPARAM(wp);
            return true;
        }
        case WindowsSystemMessage::Window_Size: {

        }
        default: {
            return false;
        }
        }
    }

    bool Console::HandleClipboardCopy() const noexcept {
        bool did_copy = false;
        if(Clipboard::HasText()) {
            auto hwnd = static_cast<HWND>(_renderer.GetOutput()->GetWindow()->GetWindowHandle());
            Clipboard c{hwnd};
            if(_cursor_position != _selection_position) {
                std::string copied_text = CopyText(_cursor_position, _selection_position);
                did_copy = c.Copy(copied_text);
            } else {
                did_copy = c.Copy(_entryline);
            }
        }
        return did_copy;
    }

    void Console::HandleClipboardPaste() noexcept {
        if(Clipboard::HasText()) {
            auto hwnd = static_cast<HWND>(_renderer.GetOutput()->GetWindow()->GetWindowHandle());
            Clipboard c{hwnd};
            auto string_to_paste = c.Paste();
            PasteText(string_to_paste, _cursor_position);
        }
    }

    void Console::HandleClipboardCut() noexcept {
        if(HandleClipboardCopy()) {
            RemoveText(_cursor_position, _selection_position);
        }
    }

    void Console::HandleSelectAll() noexcept {
        _cursor_position = _entryline.end();
        _selection_position = _entryline.begin();
    }

    bool Console::HandleEscapeKey() noexcept {
        _entryline.empty() ? Close() : ClearEntryLine();
        return true;
    }

    bool Console::HandleTabKey() noexcept {
        AutoCompleteEntryline();
        return true;
    }

    void Console::AutoCompleteEntryline() noexcept {
        if(_entryline.empty()) {
            return;
        }
        for(const auto& entry : _commands) {
            if(StringUtils::StartsWith(entry.first, _entryline)) {
                _entryline = entry.first;
                MoveCursorToEnd();
            }
        }
    }

    bool Console::HandleBackspaceKey() noexcept {
        if(_cursor_position != _selection_position) {
            RemoveText(_cursor_position, _selection_position);
        } else {
            RemoveTextBehindCaret();
        }
        return true;
    }

    bool Console::HandleUpKey() noexcept {
        HistoryUp();
        return true;
    }

    bool Console::HandleDownKey() noexcept {
        HistoryDown();
        return true;
    }

    bool Console::HandleReturnKey() noexcept {
        PostEntryLine();
        return true;
    }

    bool Console::HandleTildeKey() noexcept {
        ToggleConsole();
        if(IsOpen()) {
            _mouseWheelPosition = 0;
            _outputStartPosition = Vector2::ZERO;
            _non_rendering_char = true;
            _entryline.clear();
            _cursor_position = _entryline.begin();
            _selection_position = _cursor_position;
        }
        return true;
    }

    void Console::SetHighlightMode(bool value) noexcept {
        _highlight_mode = value;
    }

    void Console::SetOutputChanged(bool value) noexcept {
        _output_changed = value;
    }

    void Console::SetSkipNonWhitespaceMode(bool value) noexcept {
        _skip_nonwhitespace_mode = value;
    }

    bool Console::HandleEndKey() noexcept {
        const auto offset = std::distance(_cursor_position, std::cend(_entryline));
        MoveCursorRight(offset);
        return true;
    }

    bool Console::HandleHomeKey() noexcept {
        const auto offset = std::distance(std::cbegin(_entryline), _cursor_position);
        MoveCursorLeft(offset);
        return true;
    }

    bool Console::HandleDelKey() noexcept {
        if(_cursor_position != _selection_position) {
            RemoveText(_cursor_position, _selection_position);
        } else {
            RemoveTextInFrontOfCaret();
        }
        return true;
    }

    bool Console::HandleRightKey() noexcept {
        const auto offset = std::distance(std::cbegin(_entryline), _cursor_position);
        const auto offset_from_next_space = _entryline.find_first_of(' ', offset);
        MoveCursorRight(_skip_nonwhitespace_mode ? offset + offset_from_next_space : 1);
        return true;
    }

    bool Console::HandleLeftKey() noexcept {
        const auto offset = std::distance(std::cbegin(_entryline), _cursor_position);
        const auto offset_from_previous_space = _entryline.find_last_of(' ', offset - 1);
        MoveCursorLeft(_skip_nonwhitespace_mode ? offset - offset_from_previous_space : 1);
        return true;
    }

    void Console::RunCommand(std::string name_and_args) noexcept {
        if(name_and_args.empty()) {
            return;
        }
        name_and_args = StringUtils::TrimWhitespace(name_and_args);
        const auto [command, args] = StringUtils::SplitOnFirst(name_and_args, ' ');
        const auto iter = _commands.find(command);
        if(iter == _commands.end()) {
            ErrorMsg("INVALID COMMAND");
            return;
        }
        iter->second.command_function(args);
    }

    void Console::RegisterCommand(const Command& command) noexcept {
        if(command.command_name.empty()) {
            return;
        }
        const auto iter = _commands.find(command.command_name);
        if(iter == _commands.end()) {
            _commands.insert_or_assign(command.command_name, command);
        }
    }

    void Console::UnregisterCommand(const std::string& command_name) noexcept {
        const auto iter = _commands.find(command_name);
        if(iter != _commands.end()) {
            _commands.erase(command_name);
        }
    }

    void Console::PushCommandList(const CommandList& list) noexcept {
        for(const auto& command : list.GetCommands()) {
            RegisterCommand(command);
        }
    }

    void Console::PopCommandList(const CommandList& list) noexcept {
        for(const auto& command : list.GetCommands()) {
            UnregisterCommand(command.command_name);
        }
    }

    void Console::UnregisterAllCommands() noexcept {
        _commands.clear();
    }

    void Console::ToggleConsole() noexcept {
        _is_open = !_is_open;
    }

    bool Console::IsOpen() const noexcept {
        return _is_open;
    }

    bool Console::IsClosed() const noexcept {
        return !_is_open;
    }

    void Console::Open() noexcept {
        _is_open = true;
    }

    void Console::Close() noexcept {
        _is_open = false;
    }

    void Console::ToggleHighlightMode() noexcept {
        _highlight_mode = !_highlight_mode;
    }

    bool Console::IsHighlighting() const noexcept {
        return _highlight_mode;
    }

    void Console::PostEntryLine() noexcept {
        if(_entryline.empty()) {
            return;
        }
        PushEntrylineToOutputBuffer();
        PushEntrylineToBuffer();
        RunCommand(_entryline);
        ClearEntryLine();
    }

    void Console::PushEntrylineToOutputBuffer() noexcept {
        PrintMsg(_entryline);
    }

    void Console::PushEntrylineToBuffer() noexcept {
        const auto already_in_buffer = !_entryline_buffer.empty() && _entryline_buffer.back() == _entryline;
        if(already_in_buffer) {
            return;
        }
        _entryline_buffer.push_back(_entryline);
        _current_history_position = _entryline_buffer.end();
    }

    void Console::ClearEntryLine() noexcept {
        _entryline.clear();
        _cursor_position = std::begin(_entryline);
        _selection_position = std::begin(_entryline);
    }

    void Console::MoveCursorLeft(std::string::difference_type distance /*= 1*/) noexcept {
        if(_cursor_position != _entryline.begin()) {
            if(!_highlight_mode) {
                if(std::distance(std::cbegin(_entryline), _cursor_position) > distance) {
                    _cursor_position -= distance;
                } else {
                    _cursor_position = std::begin(_entryline);
                }
                _selection_position = _cursor_position;
            } else {
                UpdateSelectedRange(-distance);
            }
        }
    }

    void Console::MoveCursorRight(std::string::difference_type distance /*= 1*/) noexcept {
        if(_cursor_position != _entryline.end()) {
            if(!_highlight_mode) {
                if(distance < std::distance(_cursor_position, std::cend(_entryline))) {
                    _cursor_position += distance;
                } else {
                    _cursor_position = std::end(_entryline);
                }
                _selection_position = _cursor_position;
            } else {
                UpdateSelectedRange(distance);
            }
        }
    }

    void Console::MoveCursorToEnd() noexcept {
        MoveCursorRight(_entryline.size() + 1);
    }

    void Console::MoveCursorToFront() noexcept {
        MoveCursorLeft(_entryline.size() + 1);
    }

    void Console::UpdateSelectedRange(std::string::difference_type distance) noexcept {
        if(distance > 0) {
            const auto distance_from_end = std::distance(_cursor_position, std::cend(_entryline));
            if(distance_from_end > std::abs(distance)) {
                _cursor_position += distance;
            } else {
                _cursor_position = std::end(_entryline);
            }

            auto rangeStart = _cursor_position;
            auto rangeEnd = _selection_position;
            if(!_highlight_mode && _selection_position < _cursor_position) {
                rangeStart = _selection_position;
                rangeEnd = _cursor_position;
            }
            _cursor_position = rangeStart;
            _selection_position = rangeEnd;
        } else if(distance < 0) {
            const auto distance_from_beginning = std::distance(std::cbegin(_entryline), _cursor_position);
            if(distance_from_beginning > std::abs(distance)) {
                _cursor_position += distance;
            } else {
                _cursor_position = std::begin(_entryline);
            }

            auto rangeStart = _cursor_position;
            auto rangeEnd = _selection_position;
            if(!_highlight_mode && _selection_position < _cursor_position) {
                rangeStart = _selection_position;
                rangeEnd = _cursor_position;
            }
            _cursor_position = rangeStart;
            _selection_position = rangeEnd;
        }
    }

    void Console::RemoveTextInFrontOfCaret() noexcept {
        if(!_entryline.empty()) {
            if(_cursor_position != _entryline.end()) {
                _cursor_position = _entryline.erase(_cursor_position);
                _selection_position = _cursor_position;
                _entryline_changed = true;
            }
        }
    }

    void Console::PopConsoleBuffer() noexcept {
        if(!_entryline.empty()) {
            if(_cursor_position == _entryline.end()) {
                _entryline.pop_back();
                --_cursor_position;
                _entryline_changed = true;
            }
        }
    }
    void Console::RemoveTextBehindCaret() noexcept {
        if(!_entryline.empty()) {
            if(_cursor_position != _entryline.end()) {
                if(_cursor_position != _entryline.begin()) {
                    _cursor_position = _entryline.erase(_cursor_position - 1);
                    _entryline_changed = true;
                }
            } else {
                PopConsoleBuffer();
            }
            _selection_position = _cursor_position;
        }
    }

    void Console::RemoveText(std::string::const_iterator start, std::string::const_iterator end) noexcept {
        if(end < start) {
            std::swap(start, end);
        }
        _cursor_position = _entryline.erase(start, end);
        _selection_position = _cursor_position;
        _entryline_changed = true;
    }

    std::string Console::CopyText(std::string::const_iterator start, std::string::const_iterator end) const noexcept {
        if(end < start) {
            std::swap(start, end);
        }
        return std::string(start, end);
    }

    void Console::PasteText(const std::string& text, std::string::const_iterator loc) noexcept {
        if(text.empty()) {
            return;
        }
        if(_cursor_position != _selection_position) {
            RemoveText(_cursor_position, _selection_position);
        }
        _cursor_position = _entryline.insert(loc, std::begin(text), std::end(text)) + text.size();
        _selection_position = _cursor_position;
        _entryline_changed = true;
    }

    void Console::Initialize() {
        _camera = std::make_unique<Camera2D>();
        RegisterDefaultCommands();
    }

    void Console::RegisterDefaultCommands() noexcept {
        Console::Command help{};
        help.command_name = "help";
        help.help_text_short = "Displays every command with brief description.";
        help.help_text_long = "help [command|string]: Displays command's long description or all commands starting with \'string\'.";
        help.command_function = [this](const std::string& args) -> void {
            ArgumentParser arg_set(args);
            std::string cur_arg{};
            if(arg_set >> cur_arg) { //help ...
                cur_arg = StringUtils::TrimWhitespace(cur_arg);
                const auto found_iter = _commands.find(cur_arg);
                if(found_iter != _commands.end()) {
                    PrintMsg(std::string{found_iter->second.command_name + ": " + found_iter->second.help_text_short});
                    return;
                }
                for(auto& [key,value] : _commands) {
                    if(StringUtils::StartsWith(key, cur_arg)) {
                        PrintMsg(std::string{value.command_name + ": " + value.help_text_short});
                    }
                }
            } else {
                for(auto& [_, value] : _commands) {
                    PrintMsg(std::string{value.command_name + ": " + value.help_text_short});
                }
            }
        };
        RegisterCommand(help);

        Console::Command echo{};
        echo.command_name = "echo";
        echo.help_text_short = "Displays text as arguments.";
        echo.help_text_long = "echo [text]: Displays text as if they were arguments, each on a separate line.";
        echo.command_function = [this](const std::string& args) -> void {
            ArgumentParser arg_set(args);
            std::string cur_arg{};
            while(arg_set >> cur_arg) {
                PrintMsg(cur_arg);
                cur_arg.clear();
            }
        };
        RegisterCommand(echo);

        Console::Command clear{};
        clear.command_name = "clear";
        clear.help_text_short = "Clears the output buffer.";
        clear.help_text_long = clear.help_text_short;
        clear.command_function = [this](const std::string & /*args*/) -> void {
            _output_changed = true;
            _output_buffer.clear();
        };
        RegisterCommand(clear);
    }

    void Console::BeginFrame() {
        if(_cursor_timer.CheckAndReset()) {
            _show_cursor = !_show_cursor;
        }
    }

    void Console::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) {
        /* DO NOTHING */
    }

    void Console::Render() const {
        if(IsClosed()) {
            return;
        }

        _renderer.ResetModelViewProjection();
        _renderer.SetRenderTarget();
        _renderer.SetViewportAsPercent(0.0f, 0.0f, 1.0f, 0.957f);

        const auto view_half_extents = SetupViewFromCamera();
        DrawBackground(view_half_extents);
        DrawOutput(view_half_extents);
        DrawEntryLine(view_half_extents);
        DrawCursor(view_half_extents);
    }

    void Console::DrawCursor(const Vector2& view_half_extents) const noexcept {
        if(!_show_cursor) {
            return;
        }
        const auto textline_bottom = view_half_extents.y * 0.99f;
        const auto textline_left = -view_half_extents.x * 0.99f;
        const auto font = _renderer.GetFont("System32");
        const auto first = _entryline.begin();
        const auto has_text = !_entryline.empty();
        const auto text_left_of_cursor = has_text ? std::string(first, _cursor_position) : std::string("");
        const auto xPosOffsetToCaret = font->CalculateTextWidth(text_left_of_cursor);
        const auto cursor_t = Matrix4::CreateTranslationMatrix(Vector3(textline_left + xPosOffsetToCaret, textline_bottom, 0.0f));
        const auto model_cursor_mat = cursor_t;
        _renderer.SetModelMatrix(model_cursor_mat);
        _renderer.SetMaterial(font->GetMaterial());
        _renderer.DrawTextLine(font, "|", Rgba::White);
    }

    void Console::DrawOutput(const Vector2& view_half_extents) const noexcept {
        if(_output_buffer.empty()) {
            return;
        }
        std::vector<Vertex3D> vbo{};
        std::vector<unsigned int> ibo{};
        auto font = _renderer.GetFont("System32");
        const auto max_vertical_start_position = (_output_buffer.size() * (1 + font->GetLineHeight()) - view_half_extents.y * 2.0f);
        if(_outputStartPosition.y <= max_vertical_start_position && WasMouseWheelJustScrolledUp()) {
            _outputStartPosition.y += font->GetLineHeight();
        }
        if(_outputStartPosition.y && WasMouseWheelJustScrolledDown()) {
            _outputStartPosition.y -= font->GetLineHeight();
        }
        {
            const auto draw_x = -view_half_extents.x;
            const auto draw_y = view_half_extents.y;
            auto draw_loc = _outputStartPosition + Vector2(draw_x * 0.99f, draw_y * 0.99f);
            for(auto iter = _output_buffer.cbegin(); iter != _output_buffer.cend(); ++iter) {
                draw_loc.y -= font->CalculateTextHeight(iter->str);
                _renderer.AppendMultiLineTextBuffer(font, iter->str, draw_loc, iter->color, vbo, ibo);
            }
        }
        _renderer.SetMaterial(font->GetMaterial());
        _renderer.EnableScissorTest();
        _renderer.SetScissorAsPercent(0.0f, 0.0f, 1.0f, 0.921f);
        _renderer.SetModelMatrix(Matrix4::I);
        _renderer.DrawIndexed(PrimitiveType::Triangles, vbo, ibo);
        _renderer.DisableScissorTest();
        _renderer.SetScissorAsPercent();
    }

    void Console::OutputMsg(const std::string& msg, const Rgba& color) noexcept {
        _output_changed = true;
        _output_buffer.push_back({msg, color});
    }

    void Console::HistoryUp() noexcept {
        if(_current_history_position == _entryline_buffer.begin()) {
            return;
        }
        --_current_history_position;
        _entryline = *_current_history_position;
        MoveCursorToEnd();
    }

    void Console::HistoryDown() noexcept {
        if(_current_history_position != _entryline_buffer.end()) {
            ++_current_history_position;
            if(_current_history_position == _entryline_buffer.end()) {
                ClearEntryLine();
            } else {
                _entryline = *_current_history_position;
            }
        }
        MoveCursorToEnd();
    }

    void Console::InsertCharInEntryLine(unsigned char c) noexcept {
        _entryline_changed = true;
        if(!_entryline.empty()) {
            if(_cursor_position != _selection_position) {
                RemoveText(_cursor_position, _selection_position);
            }
            if(_cursor_position == _entryline.end()) {
                _entryline.push_back(c);
                _cursor_position = _entryline.end();
            } else {
                _entryline.insert(_cursor_position, c);
                ++_cursor_position;
            }
        } else {
            _entryline += c;
            _cursor_position = _entryline.end();
        }
        _selection_position = _cursor_position;
    }

    void Console::PrintMsg(const std::string& msg) noexcept {
        OutputMsg(msg, Rgba::White);
    }

    void Console::WarnMsg(const std::string& msg) noexcept {
        OutputMsg(msg, Rgba::Yellow);
    }

    void Console::ErrorMsg(const std::string& msg) noexcept {
        OutputMsg(msg, Rgba::Red);
    }

    void Console::DrawBackground(const Vector2& view_half_extents) const noexcept {
        _renderer.SetModelMatrix(Matrix4::CreateScaleMatrix(view_half_extents * 2.0f));
        _renderer.SetMaterial(_renderer.GetMaterial("__2D"));
        _renderer.DrawQuad2D(Rgba(0, 0, 0, 128));
    }

    void Console::DrawEntryLine(const Vector2& view_half_extents) const noexcept {
        const auto font = _renderer.GetFont("System32");
        const float textline_bottom = view_half_extents.y * 0.99f;
        const float textline_left = -view_half_extents.x * 0.99f;

        const auto entryline_t = Matrix4::CreateTranslationMatrix(Vector3(textline_left, textline_bottom, 0.0f));
        const auto model_entryline_mat = entryline_t;

        if(_cursor_position != _selection_position) {
            auto xPosOffsetToCaret = font->CalculateTextWidth(std::string(std::begin(_entryline), _cursor_position));
            auto xPosOffsetToSelect = font->CalculateTextWidth(std::string(std::begin(_entryline), _selection_position));
            auto rangeStart = _cursor_position;
            auto rangeEnd = _selection_position;
            if(_selection_position < _cursor_position) {
                std::swap(rangeStart, rangeEnd);
                std::swap(xPosOffsetToCaret, xPosOffsetToSelect);
            }

            _renderer.SetModelMatrix(Matrix4::CreateScaleMatrix(Vector2(500.0f, 500.0f)));
            _renderer.SetMaterial(_renderer.GetMaterial("__2D"));
            _renderer.DrawQuad2D();

            _renderer.SetModelMatrix(model_entryline_mat);
            _renderer.SetMaterial(font->GetMaterial());

            _renderer.DrawTextLine(font, std::string(_entryline, 0, std::distance(std::cbegin(_entryline), rangeStart)), Rgba::White);
            auto rightside_t = Matrix4::CreateTranslationMatrix(Vector3(xPosOffsetToSelect, 0.0f, 0.0f));
            rightside_t = Matrix4::MakeRT(model_entryline_mat, rightside_t);
            _renderer.SetModelMatrix(rightside_t);
            _renderer.DrawTextLine(font, std::string(_entryline, std::distance(std::cbegin(_entryline), rangeEnd), std::distance(rangeEnd, std::cend(_entryline))), Rgba::White);

            const auto xPosOffsetToStart = font->CalculateTextWidth(std::string(std::begin(_entryline), rangeStart));
            const auto blacktext_t = Matrix4::CreateTranslationMatrix(Vector3(xPosOffsetToStart, 0.0f, 0.0f));
            auto model_mat_blacktext = Matrix4::MakeRT(model_entryline_mat, blacktext_t);
            _renderer.SetModelMatrix(model_mat_blacktext);
            _renderer.DrawTextLine(font, std::string(rangeStart, rangeEnd), Rgba::Black);

        } else {
            _renderer.SetModelMatrix(model_entryline_mat);
            _renderer.SetMaterial(font->GetMaterial());
            _renderer.DrawTextLine(font, _entryline, Rgba::White);
        }
    }

    Vector2 Console::SetupViewFromCamera() const noexcept {
        const auto& window = _renderer.GetOutput();
        const auto& window_dimensions = window->GetDimensions();
        const auto& aspect = window->GetAspectRatio();
        const auto window_width = static_cast<float>(window_dimensions.x);
        const auto window_height = static_cast<float>(window_dimensions.y);
        const auto view_half_width = window_width * 0.50f;
        const auto view_half_height = window_height * 0.50f;
        const auto leftBottom = Vector2(-view_half_width, view_half_height);
        const auto rightTop = Vector2(view_half_width, -view_half_height);
        const auto nearFar = Vector2(0.0f, 1.0f);
        _camera->SetupView(leftBottom, rightTop, nearFar, aspect);

        _renderer.SetViewMatrix(_camera->GetViewMatrix());
        _renderer.SetProjectionMatrix(_camera->GetProjectionMatrix());

        return Vector2(view_half_width, view_half_height);
    }

    int Console::GetMouseWheelPositionNormalized() const noexcept {
        if(_mouseWheelPosition) {
            return _mouseWheelPosition / std::abs(_mouseWheelPosition);
        }
        return 0;
    }

    bool Console::WasMouseWheelJustScrolledUp() const noexcept {
        return GetMouseWheelPositionNormalized() > 0;
    }

    bool Console::WasMouseWheelJustScrolledDown() const noexcept {
        return GetMouseWheelPositionNormalized() < 0;
    }

    void Console::EndFrame() {
        _mouseWheelPosition = 0;
    }

    Console::CommandList::CommandList(Console* console /*= nullptr*/) noexcept
    : _console(console) {
        /* DO NOTHING */
    }

    Console::CommandList::CommandList(Console* console, const std::vector<Command>& commands) noexcept
    : _console(console)
    , _commands(commands) {
        if(!_console) {
            return;
        }
        for(const auto& command : _commands) {
            _console->RegisterCommand(command);
        }
    }

    Console::CommandList::~CommandList() noexcept {
        if(!_console) {
            return;
        }
        for(const auto& command : _commands) {
            _console->UnregisterCommand(command.command_name);
        }
    }

    void Console::CommandList::AddCommand(const Command& command) {
        _commands.emplace_back(command);
    }

    void Console::CommandList::RemoveCommand(const std::string& name) {
        _commands.erase(std::remove_if(std::begin(_commands), std::end(_commands), [&name](const Console::Command& command) { return name == command.command_name; }), std::end(_commands));
    }

    void Console::CommandList::RemoveAllCommands() noexcept {
        _commands.clear();
    }

    const std::vector<Console::Command>& Console::CommandList::GetCommands() const noexcept {
        return _commands;
    }

} // namespace a2de
