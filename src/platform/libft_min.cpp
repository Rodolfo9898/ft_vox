#ifndef LIBFT_INTERNAL_HEADERS
# define LIBFT_INTERNAL_HEADERS
#endif

#include "../../Libft/Modules/Basic/basic.hpp"
#include "../../Libft/Modules/CPP_class/class_nullptr.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"
#include "../../Libft/Modules/Errno/errno_internal.hpp"
#include <cstdlib>

const ft_nullptr_t ft_nullptr_instance;

ft_nullptr_t::ft_nullptr_t() noexcept
{
    return ;
}

ft_nullptr_t::ft_nullptr_t(const ft_nullptr_t &) noexcept
{
    return ;
}

ft_nullptr_t::ft_nullptr_t(ft_nullptr_t &&) noexcept
{
    return ;
}

ft_nullptr_t::~ft_nullptr_t() noexcept
{
    return ;
}

void *ft_memset(void *destination, int32_t value, ft_size_t number_of_bytes)
{
    unsigned char *bytes;
    ft_size_t     index;

    bytes = static_cast<unsigned char *>(destination);
    index = 0;
    while (index < number_of_bytes)
    {
        bytes[index] = static_cast<unsigned char>(value);
        index = index + 1;
    }
    return (destination);
}

void errno_abort_if_uninitialised_or_destroyed(uint8_t initialised_state,
    const char *method_name)
{
    (void)method_name;
    if (initialised_state == FT_CLASS_STATE_UNINITIALISED
        || initialised_state == FT_CLASS_STATE_DESTROYED)
        std::abort();
    return ;
}

void errno_abort_if_uninitialised(uint8_t initialised_state,
    const char *method_name)
{
    (void)method_name;
    if (initialised_state == FT_CLASS_STATE_UNINITIALISED)
        std::abort();
    return ;
}

void errno_abort_lifecycle(uint8_t initialised_state, const char *method_name,
    const char *reason)
{
    (void)initialised_state;
    (void)method_name;
    (void)reason;
    std::abort();
}

const char *ft_strerror(int32_t error_code)
{
    (void)error_code;
    return ("libft error");
}

void ft_perror(const char *error_msg, int32_t error_code)
{
    (void)error_msg;
    (void)error_code;
    return ;
}

void ft_exit(const char *error_msg, int32_t exit_code)
{
    (void)error_msg;
    std::exit(exit_code);
}
