#pragma once

#include <type_traits>

class JobSystem;
class FileLogger;
class Renderer;
class Console;
class Config;
class UISystem;
class InputSystem;
class AudioSystem;
class EngineSubsystem;

class GameBase;

extern JobSystem* g_theJobSystem;
extern FileLogger* g_theFileLogger;
extern Renderer* g_theRenderer;
extern Console* g_theConsole;
extern Config* g_theConfig;
extern UISystem* g_theUISystem;
extern InputSystem* g_theInputSystem;
extern AudioSystem* g_theAudioSystem;
extern GameBase* g_theGame;
extern EngineSubsystem* g_theSubsystemHead;

template<typename GameDerived>
GameDerived* GetGameAs() noexcept {
    static_assert(std::is_base_of_v<std::remove_cv_t<std::remove_reference_t<std::remove_pointer_t<GameBase>>>, std::remove_cv_t<std::remove_reference_t<std::remove_pointer_t<GameDerived>>>>);
    return dynamic_cast<GameDerived*>(g_theGame);
}
