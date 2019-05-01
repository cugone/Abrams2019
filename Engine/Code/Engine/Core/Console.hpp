#pragma once

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/Vertex3D.hpp"

#include "Engine/Math/Vector2.hpp"

#include <functional>
#include <map>
#include <string>
#include <vector>

class Camera2D;
class KerningFont;
class Renderer;

class Console : public EngineSubsystem {
public:
    struct Command {
        std::string command_name{};
        std::string help_text_short{};
        std::string help_text_long{};
        std::function<void(const std::string& args)> command_function = [](const std::string& /*args*/) {};
    };
    explicit Console(Renderer* renderer);
    virtual ~Console();

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update([[maybe_unused]]TimeUtils::FPSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    virtual bool ProcessSystemMessage(const EngineMessage& msg) override;

    void RunCommand(std::string name_and_args);
    void RegisterCommand(const Console::Command& command);
    void UnregisterCommand(const std::string& command_name);

    void PrintMsg(const std::string& msg);
    void WarnMsg(const std::string& msg);
    void ErrorMsg(const std::string& msg);

    void* GetAcceleratorTable() const;
    bool IsOpen() const;
    bool IsClosed() const;

protected:
private:
    struct OutputEntry {
        std::string str{};
        Rgba color = Rgba::White;
    };
    void PostEntryLine();
    void PushEntrylineToOutputBuffer();
    void PushEntrylineToBuffer();
    void ClearEntryLine();
    void MoveCursorLeft(std::string::difference_type distance = 1);
    void MoveCursorRight(std::string::difference_type distance = 1);
    void MoveCursorToEnd();
    void MoveCursorToFront();
    void UpdateSelectedRange(std::string::difference_type distance);

    bool HandleLeftKey();
    bool HandleRightKey();
    bool HandleDelKey();
    bool HandleHomeKey();
    bool HandleEndKey();
    bool HandleTildeKey();
    bool HandleReturnKey();
    bool HandleUpKey();
    bool HandleDownKey();
    bool HandleBackspaceKey();
    bool HandleEscapeKey();
    bool HandleTabKey();
    bool HandleClipboardCopy() const;
    void HandleClipboardPaste();
    void HandleClipboardCut();
    void HandleSelectAll();

    void HistoryUp();
    void HistoryDown();

    void InsertCharInEntryLine(unsigned char c);
    void PopConsoleBuffer();
    void RemoveTextInFrontOfCaret();
    void RemoveTextBehindCaret();
    void RemoveText(std::string::const_iterator start, std::string::const_iterator end);
    std::string CopyText(std::string::const_iterator start, std::string::const_iterator end) const;
    void PasteText(const std::string& text, std::string::const_iterator loc);
    void DrawBackground(const Vector2& view_half_extents) const;
    void DrawEntryLine(const Vector2& view_half_extents) const;
    void DrawCursor(const Vector2& view_half_extents) const;
    void DrawOutput(const Vector2& view_half_extents) const;

    void OutputMsg(const std::string& msg, const Rgba& color);

    void RegisterDefaultCommands();
    void RegisterDefaultFont();
    void UnregisterAllCommands();

    void ToggleConsole();
    void Open();
    void Close();

    void ToggleHighlightMode();
    void SetHighlightMode(bool value);
    bool IsHighlighting() const;
    void SetOutputChanged(bool value);
    void SetSkipNonWhitespaceMode(bool value);

    void AutoCompleteEntryline();

    Vector2 SetupViewFromCamera() const;

    Renderer* _renderer = nullptr;
    Camera2D* _camera = nullptr;
    std::map<std::string, Console::Command> _commands{};
    std::vector<std::string> _entryline_buffer{};
    std::vector<OutputEntry> _output_buffer{};
    std::string _entryline{};
    std::string::const_iterator _cursor_position{};
    std::string::const_iterator _selection_position{};
    decltype(_entryline_buffer)::const_iterator _current_history_position{};
    unsigned int _default_blink_rate = 4u;
    unsigned int _blink_rate = _default_blink_rate;
    Stopwatch _cursor_timer = Stopwatch(_blink_rate);
    uint8_t _show_cursor             : 1;
    uint8_t _is_open                 : 1;
    uint8_t _highlight_mode          : 1;
    uint8_t _skip_nonwhitespace_mode : 1;
    uint8_t _dirty_text              : 1;
    uint8_t _non_rendering_char      : 1;
    uint8_t _entryline_changed       : 1;
    uint8_t _output_changed          : 1;

};
