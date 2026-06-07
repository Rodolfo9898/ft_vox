#ifndef APPLICATION_VALIDATOR_HPP
# define APPLICATION_VALIDATOR_HPP

#include "../../src/validators/CameraSpeedValidator.hpp"
#include "../../src/validators/CollisionValidator.hpp"
#include "../../src/validators/BlockEditValidator.hpp"

class ApplicationValidator
{
    public:
        ApplicationValidator();
        ApplicationValidator(const ApplicationValidator &other);
        ~ApplicationValidator();
        ApplicationValidator &operator=(const ApplicationValidator &other);

        static int validate_camera_speed();
        static int validate_collision();
        static int validate_block_edit();
};

#endif
