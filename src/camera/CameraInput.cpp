#include "../../src/camera/CameraInput.hpp"

CameraInput::CameraInput()
{
    this->clear();
}

CameraInput::CameraInput(const CameraInput &other)
{
    *this = other;
}

CameraInput::~CameraInput()
{
}

CameraInput &CameraInput::operator=(const CameraInput &other)
{
    if (this != &other)
    {
        this->move_forward = other.move_forward;
        this->move_backward = other.move_backward;
        this->move_left = other.move_left;
        this->move_right = other.move_right;
        this->turn_left = other.turn_left;
        this->turn_right = other.turn_right;
        this->look_up = other.look_up;
        this->look_down = other.look_down;
        this->speed_boost = other.speed_boost;
        this->jump_pressed = other.jump_pressed;
        this->jump_held = other.jump_held;
        this->mouse_delta_x = other.mouse_delta_x;
        this->mouse_delta_y = other.mouse_delta_y;
    }
    return (*this);
}

void CameraInput::clear()
{
    this->move_forward = false;
    this->move_backward = false;
    this->move_left = false;
    this->move_right = false;
    this->turn_left = false;
    this->turn_right = false;
    this->look_up = false;
    this->look_down = false;
    this->speed_boost = false;
    this->jump_pressed = false;
    this->jump_held = false;
    this->mouse_delta_x = 0.0;
    this->mouse_delta_y = 0.0;
}
