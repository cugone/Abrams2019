#include "Engine/Input/XboxController.hpp"

#include "Engine/Core/Win.hpp"

#include "Engine/Math/MathUtils.hpp"

#include <algorithm>

bool XboxController::WasAnyButtonJustPressed() const {
    return (_previousButtonState.to_ulong() < _currentButtonState.to_ulong());
}

bool XboxController::WasAnyButtonJustReleased() const {
    return (_currentButtonState.to_ulong() < _previousButtonState.to_ulong());
}

bool XboxController::IsAnyButtonDown() const {
    return _currentButtonState.any();
}

const Vector2& XboxController::GetLeftThumbPosition() const {
    return _leftThumbDistance;
}

const Vector2& XboxController::GetRightThumbPosition() const {
    return _rightThumbDistance;
}

float XboxController::GetLeftTriggerPosition() const {
    return _triggerDistances.x;
}

float XboxController::GetRightTriggerPosition() const {
    return _triggerDistances.y;
}

bool XboxController::IsButtonUp(const Button& button) const {
    return !_currentButtonState[(std::size_t)button];
}

bool XboxController::WasButtonJustPressed(const Button& button) const {
    return !_previousButtonState[(std::size_t)button] && _currentButtonState[(std::size_t)button];
}

bool XboxController::IsButtonDown(const Button& button) const {
    return _currentButtonState[(std::size_t)button];
}

bool XboxController::WasButtonJustReleased(const Button& button) const {
    return _previousButtonState[(std::size_t)button] && !_currentButtonState[(std::size_t)button];
}

bool XboxController::WasJustConnected() const {
    return !_previousActiveState[(std::size_t)ActiveState::Connected] && _currentActiveState[(std::size_t)ActiveState::Connected];
}

bool XboxController::IsConnected() const {
    return _previousActiveState[(std::size_t)ActiveState::Connected] && _currentActiveState[(std::size_t)ActiveState::Connected];
}

bool XboxController::WasJustDisconnected() const {
    return _previousActiveState[(std::size_t)ActiveState::Connected] && !_currentActiveState[(std::size_t)ActiveState::Connected];
}

bool XboxController::IsDisconnected() const {
    return !_previousActiveState[(std::size_t)ActiveState::Connected] && !_currentActiveState[(std::size_t)ActiveState::Connected];
}

void XboxController::Update(int controller_number) {
    XINPUT_STATE state;
    std::memset(&state, 0, sizeof(state));

    auto error_status = ::XInputGetState(controller_number, &state);
    _previousPacketNumber = _currentPacketNumber;
    _currentPacketNumber = state.dwPacketNumber;

    if(error_status == ERROR_DEVICE_NOT_CONNECTED) {
        _previousActiveState = _currentActiveState;
        _currentActiveState[(std::size_t)ActiveState::Connected] = false;
        return;
    }

    if(error_status == ERROR_SUCCESS) {
        _previousActiveState = _currentActiveState;
        if(!_currentActiveState[(std::size_t)ActiveState::Connected]) {
            _currentActiveState[(std::size_t)ActiveState::Connected] = true;
        }

        _previousRawInput = _currentRawInput;
        _currentRawInput = state.Gamepad.wButtons;

        UpdateState();

        _leftThumbDistance = Vector2(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY);
        _rightThumbDistance = Vector2(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY);
        _triggerDistances = Vector2(state.Gamepad.bLeftTrigger, state.Gamepad.bRightTrigger);

        float leftRadius = _leftThumbDistance.CalcLength();

        leftRadius = MathUtils::RangeMap<float>(leftRadius, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, 32000, 0.0f, 1.0f);
        leftRadius = std::clamp<float>(leftRadius, 0.0f, 1.0f);

        _leftThumbDistance.SetLength(leftRadius);

        float rightRadius = _rightThumbDistance.CalcLength();

        rightRadius = MathUtils::RangeMap<float>(rightRadius, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, 32000, 0.0f, 1.0f);
        rightRadius = std::clamp<float>(rightRadius, 0.0f, 1.0f);

        _rightThumbDistance.SetLength(rightRadius);

        _triggerDistances.x = MathUtils::RangeMap<float>(_triggerDistances.x, static_cast<float>(XINPUT_GAMEPAD_TRIGGER_THRESHOLD), 255.0f, 0.0f, 1.0f);
        _triggerDistances.y = MathUtils::RangeMap<float>(_triggerDistances.y, static_cast<float>(XINPUT_GAMEPAD_TRIGGER_THRESHOLD), 255.0f, 0.0f, 1.0f);

        if(DidMotorStateChange()) {
            SetMotorSpeed(controller_number, Motor::Left, _leftMotorState);
            SetMotorSpeed(controller_number, Motor::Right, _rightMotorState);
        }

    }
}

void XboxController::StopLeftMotor() {
    SetLeftMotorSpeed(0);
}

void XboxController::StopRightMotor() {
    SetRightMotorSpeed(0);
}

void XboxController::StopMotors() {
    StopLeftMotor();
    StopRightMotor();
}

void XboxController::SetLeftMotorSpeed(unsigned short speed) {
    if(speed == _leftMotorState) {
        return;
    }
    _leftMotorState = speed;
    _currentActiveState[(std::size_t)ActiveState::Motor] = true;
}

void XboxController::SetRightMotorSpeed(unsigned short speed) {
    if(speed == _rightMotorState) {
        return;
    }
    _rightMotorState = speed;
    _currentActiveState[(std::size_t)ActiveState::Motor] = true;
}

void XboxController::SetBothMotorSpeed(unsigned short speed) {
    SetLeftMotorSpeed(speed);
    SetRightMotorSpeed(speed);
}

void XboxController::SetLeftMotorSpeedToMax() {
    SetLeftMotorSpeedAsPercent(1.0f);
}

void XboxController::SetRightMotorSpeedToMax() {
    SetRightMotorSpeedAsPercent(1.0f);
}

void XboxController::SetBothMotorSpeedToMax() {
    SetLeftMotorSpeedToMax();
    SetRightMotorSpeedToMax();
}

void XboxController::SetLeftMotorSpeedAsPercent(float speed) {
    SetLeftMotorSpeed(static_cast<unsigned short>(static_cast<float>((std::numeric_limits<unsigned short>::max)()) * speed));
}

void XboxController::SetRightMotorSpeedAsPercent(float speed) {
    SetRightMotorSpeed(static_cast<unsigned short>(static_cast<float>((std::numeric_limits<unsigned short>::max)()) * speed));
}

void XboxController::SetBothMotorSpeedAsPercent(float speed) {
    SetLeftMotorSpeedAsPercent(speed);
    SetRightMotorSpeedAsPercent(speed);
}

void XboxController::UpdateConnectedState(int controller_number) {
    XINPUT_STATE state;
    std::memset(&state, 0, sizeof(state));

    auto error_status = ::XInputGetState(controller_number, &state);
    _previousPacketNumber = _currentPacketNumber;
    _currentPacketNumber = state.dwPacketNumber;
    if(_previousPacketNumber == _currentPacketNumber) {
        return;
    }

    if(error_status == ERROR_DEVICE_NOT_CONNECTED) {
        _previousActiveState = _currentActiveState;
        _currentActiveState[(std::size_t)ActiveState::Connected] = false;
        return;
    }

    if(error_status == ERROR_SUCCESS) {
        _previousActiveState = _currentActiveState;
        if(!_currentActiveState[(std::size_t)ActiveState::Connected]) {
            _currentActiveState[(std::size_t)ActiveState::Connected] = true;
        }
    }
}

void XboxController::UpdateState() {

    _previousButtonState = _currentButtonState;

    _currentButtonState[(std::size_t)Button::Up]    = (_currentRawInput & XINPUT_GAMEPAD_DPAD_UP) != 0;
    _currentButtonState[(std::size_t)Button::Down]  = (_currentRawInput & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
    _currentButtonState[(std::size_t)Button::Left]  = (_currentRawInput & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
    _currentButtonState[(std::size_t)Button::Right] = (_currentRawInput & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;

    _currentButtonState[(std::size_t)Button::Start]      = (_currentRawInput & XINPUT_GAMEPAD_START) != 0;
    _currentButtonState[(std::size_t)Button::Back]       = (_currentRawInput & XINPUT_GAMEPAD_BACK) != 0;
    _currentButtonState[(std::size_t)Button::LeftThumb]  = (_currentRawInput & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
    _currentButtonState[(std::size_t)Button::RightThumb] = (_currentRawInput & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;

    _currentButtonState[(std::size_t)Button::LeftBumper]  = (_currentRawInput & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
    _currentButtonState[(std::size_t)Button::RightBumper] = (_currentRawInput & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;

    _currentButtonState[(std::size_t)Button::A] = (_currentRawInput & XINPUT_GAMEPAD_A) != 0;
    _currentButtonState[(std::size_t)Button::B] = (_currentRawInput & XINPUT_GAMEPAD_B) != 0;
    _currentButtonState[(std::size_t)Button::X] = (_currentRawInput & XINPUT_GAMEPAD_X) != 0;
    _currentButtonState[(std::size_t)Button::Y] = (_currentRawInput & XINPUT_GAMEPAD_Y) != 0;

}

void XboxController::SetMotorSpeed(int controller_number, const Motor& motor, unsigned short value) {
    XINPUT_VIBRATION vibration;
    std::memset(&vibration, 0, sizeof(vibration));
    switch(motor) {
        case Motor::Left:
            vibration.wLeftMotorSpeed = value;
            break;
        case Motor::Right:
            vibration.wRightMotorSpeed = value;
            break;
        case Motor::Both:
            vibration.wLeftMotorSpeed = value;
            vibration.wRightMotorSpeed = value;
        default:
            /* DO NOTHING */;
    }
    DWORD errorStatus = ::XInputSetState(controller_number, &vibration);
    if(errorStatus == ERROR_SUCCESS) {
        return;
    } else if(errorStatus == ERROR_DEVICE_NOT_CONNECTED) {
        _previousActiveState = _currentActiveState;
        _currentActiveState[(std::size_t)ActiveState::Connected] = false;
        return;
    } else {
        return;
    }
}

bool XboxController::DidMotorStateChange() const {
    return _previousActiveState[(std::size_t)ActiveState::Motor] ^ _currentActiveState[(std::size_t)ActiveState::Motor];
}
