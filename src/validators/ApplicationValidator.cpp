#include "../../src/validators/ApplicationValidator.hpp"

ApplicationValidator::ApplicationValidator()
{
}

ApplicationValidator::ApplicationValidator(const ApplicationValidator &other)
{
    *this = other;
}

ApplicationValidator::~ApplicationValidator()
{
}

ApplicationValidator &ApplicationValidator::operator=(const ApplicationValidator &other)
{
    (void)other;
    return (*this);
}

int ApplicationValidator::validate_camera_speed()
{
    return (CameraSpeedValidator::validate());
}

int ApplicationValidator::validate_collision()
{
    return (CollisionValidator::validate());
}

int ApplicationValidator::validate_block_edit()
{
    return (BlockEditValidator::validate());
}
