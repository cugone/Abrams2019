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
class FileLogger;
class Renderer;

class Console : public EngineSubsystem {
public:
    struct Command {
        std::string command_name{};
        std::string help_text_short{};
        std::string help_text_long{};
        std::function<void(const std::string& args)> command_function = [](const std::string& /*args*/) {};
    };
    class CommandList {
    public:
        explicit CommandList(Console* console = nullptr) noexcept;
        CommandList(Console* console, const std::vector<Command>& commands) noexcept;
        ~CommandList() noexcept;
        void AddCommand(const Command& command);
        void RemoveCommand(const std::string& name);
        void RemoveAllCommands() noexcept;
        const std::vector<Command>& GetCommands() const noexcept;

    private:
        Console* _console = nullptr;
        std::vector<Command> _commands{};
    };
    Console() = delete;
    explicit Console(FileLogger& fileLogger, Renderer& renderer) noexcept;
    Console(const Console& other) = delete;
    Console(Console&& other) = delete;
    Console& operator=(const Console& other) = delete;
    Console& operator=(Console&& other) = delete;
    virtual ~Console() noexcept;

    virtual void Initialize() override;
    virtual void BeginFrame() override;
    virtual void Update([[maybe_unused]] TimeUtils::FPSeconds) override;
    virtual void Render() const override;
    virtual void EndFrame() override;
    virtual bool ProcessSystemMessage(const EngineMessage& msg) noexcept override;

    void RunCommand(std::string name_and_args) noexcept;
    void RegisterCommand(const Console::Command& command) noexcept;
    void UnregisterCommand(const std::string& command_name) noexcept;

    void PushCommandList(const Console::CommandList& list) noexcept;
    void PopCommandList(const Console::CommandList& list) noexcept;

    void PrintMsg(const std::string& msg) noexcept;
    void WarnMsg(const std::string& msg) noexcept;
    void ErrorMsg(const std::string& msg) noexcept;

    void* GetAcceleratorTable() const noexcept;
    bool IsOpen() const noexcept;
    bool IsClosed() const noexcept;

protected:
private:
    struct OutputEntry {
        std::string str{};
        Rgba color = Rgba::White;
    };
    void PostEntryLine() noexcept;
    void PushEntrylineToOutputBuffer() noexcept;
    void PushEntrylineToBuffer() noexcept;
    void ClearEntryLine() noexcept;
    void MoveCursorLeft(std::string::difference_type distance = 1) noexcept;
    void MoveCursorRight(std::string::difference_type distance = 1) noexcept;
    void MoveCursorToEnd() noexcept;
    void MoveCursorToFront() noexcept;
    void UpdateSelectedRange(std::string::difference_type distance) noexcept;

    bool HandleLeftKey() noexcept;
    bool HandleRightKey() noexcept;
    bool HandleDelKey() noexcept;
    bool HandleHomeKey() noexcept;
    bool HandleEndKey() noexcept;
    bool HandleTildeKey() noexcept;
    bool HandleReturnKey() noexcept;
    bool HandleUpKey() noexcept;
    bool HandleDownKey() noexcept;
    bool HandleBackspaceKey() noexcept;
    bool HandleEscapeKey() noexcept;
    bool HandleTabKey() noexcept;
    bool HandleClipboardCopy() const noexcept;
    void HandleClipboardPaste() noexcept;
    void HandleClipboardCut() noexcept;
    void HandleSelectAll() noexcept;

    void HistoryUp() noexcept;
    void HistoryDown() noexcept;

    void InsertCharInEntryLine(unsigned char c) noexcept;
    void PopConsoleBuffer() noexcept;
    void RemoveTextInFrontOfCaret() noexcept;
    void RemoveTextBehindCaret() noexcept;
    void RemoveText(std::string::const_iterator start, std::string::const_iterator end) noexcept;
    std::string CopyText(std::string::const_iterator start, std::string::const_iterator end) const noexcept;
    void PasteText(const std::string& text, std::string::const_iterator loc) noexcept;
    void DrawBackground(const Vector2& view_half_extents) const noexcept;
    void DrawEntryLine(const Vector2& view_half_extents) const noexcept;
    void DrawCursor(const Vector2& view_half_extents) const noexcept;
    void DrawOutput(const Vector2& view_half_extents) const noexcept;

    void OutputMsg(const std::string& msg, const Rgba& color) noexcept;

    void RegisterDefaultCommands() noexcept;
    void RegisterDefaultFont() noexcept;
    void UnregisterAllCommands() noexcept;

    void ToggleConsole() noexcept;
    void Open() noexcept;
    void Close() noexcept;

    void ToggleHighlightMode() noexcept;
    void SetHighlightMode(bool value) noexcept;
    bool IsHighlighting() const noexcept;
    void SetOutputChanged(bool value) noexcept;
    void SetSkipNonWhitespaceMode(bool value) noexcept;

    void AutoCompleteEntryline() noexcept;

    Vector2 SetupViewFromCamera() const noexcept;

    int GetMouseWheelPositionNormalized() const noexcept;

    bool WasMouseWheelJustScrolledUp() const noexcept;
    bool WasMouseWheelJustScrolledDown() const noexcept;

    FileLogger& _fileLogger;
    Renderer& _renderer;
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
    int _mouseWheelPosition{0};
    mutable Vector2 _outputStartPosition{};
    Stopwatch _cursor_timer = Stopwatch(_blink_rate);
    uint8_t _show_cursor : 1;
    uint8_t _is_open : 1;
    uint8_t _highlight_mode : 1;
    uint8_t _skip_nonwhitespace_mode : 1;
    uint8_t _dirty_text : 1;
    uint8_t _non_rendering_char : 1;
    uint8_t _entryline_changed : 1;
    uint8_t _output_changed : 1;
};
