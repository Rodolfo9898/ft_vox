#ifndef DEBUG_CRASH_HANDLER_HPP
#define DEBUG_CRASH_HANDLER_HPP

#include "../../src/diagnostics/ApplicationError.hpp"
#include "../../Libft/Modules/Debug/debug.hpp"
#include "../../Libft/Modules/Errno/errno.hpp"
#include <cstdint>

class DebugCrashHandler
{
  public:
    DebugCrashHandler();
    DebugCrashHandler(const DebugCrashHandler &other);
    ~DebugCrashHandler();
    DebugCrashHandler &operator=(const DebugCrashHandler &other);

    static int enable();
};

#endif
