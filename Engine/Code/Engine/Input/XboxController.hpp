#pragma once

#include "Engine/Core/Win.hpp"

#include <XInput.h>
#pragma comment(lib, "Xinput.lib")

#include "Engine/Math/Vector2.hpp"

#include <bitset>

class XboxController {
public:
    enum class Button : unsigned char {
        First_ /* Internal use only. */
        ,
        A = First_,
        B,
        X,
        Y,
        Start,
        Back,
        Up,
        Down,
        Left,
        Right,
        LeftThumb,
        RightThumb,
        RightBumper,
        LeftBumper,
        Last_ /* Internal use only. */
        ,
        Max = Last_ /* Internal use only. */
    };

    enum class Motor {
        Left,
        Right,
        Both
    };
    const Vector2& GetLeftThumbPosition() const noexcept;
    const Vector2& GetRightThumbPosition() const noexcept;

    float GetLeftTriggerPosition() const noexcept;
    float GetRightTriggerPosition() const noexcept;

    bool IsButtonUp(const Button& button) const noexcept;
    bool WasButtonJustPressed(const Button& button) const noexcept;
    bool IsButtonDown(const Button& button) const noexcept;
    bool WasButtonJustReleased(const Button& button) const noexcept;

    bool WasJustConnected() const noexcept;
    bool IsConnected() const noexcept;
    bool WasJustDisconnected() const noexcept;
    bool IsDisconnected() const noexcept;

    bool WasAnyButtonJustPressed() const noexcept;
    bool WasAnyButtonJustReleased() const noexcept;
    bool IsAnyButtonDown() const noexcept;

    void Update(int controller_number) noexcept;

    void StopLeftMotor() noexcept;
    void StopRightMotor() noexcept;
    void StopMotors() noexcept;

    void SetLeftMotorSpeed(unsigned short speed) noexcept;
    void SetRightMotorSpeed(unsigned short speed) noexcept;
    void SetBothMotorSpeed(unsigned short speed) noexcept;

    void SetLeftMotorSpeedToMax() noexcept;
    void SetRightMotorSpeedToMax() noexcept;
    void SetBothMotorSpeedToMax() noexcept;

    void SetLeftMotorSpeedAsPercent(float speed) noexcept;
    void SetRightMotorSpeedAsPercent(float speed) noexcept;
    void SetBothMotorSpeedAsPercent(float speed) noexcept;

    void UpdateConnectedState(int controller_number) noexcept;

protected:
private:
    void UpdateState() noexcept;
    void SetMotorSpeed(int controller_number, const Motor& motor, unsigned short value) noexcept;

    bool DidMotorStateChange() const noexcept;

    enum class ActiveState {
        Connected,
        Motor,
        Max
    };

    Vector2 _leftThumbDistance = Vector2::ZERO;
    Vector2 _rightThumbDistance = Vector2::ZERO;
    Vector2 _triggerDistances = Vector2::ZERO;
    unsigned short _leftMotorState = 0;
    unsigned short _rightMotorState = 0;
    unsigned short _previousRawInput = 0;
    unsigned short _currentRawInput = 0;
    unsigned long _previousPacketNumber = 0;
    unsigned long _currentPacketNumber = 0;
    std::bitset<(std::size_t)ActiveState::Max> _previousActiveState{};
    std::bitset<(std::size_t)ActiveState::Max> _currentActiveState{};
    std::bitset<(std::size_t)Button::Max> _previousButtonState{};
    std::bitset<(std::size_t)Button::Max> _currentButtonState{};
};