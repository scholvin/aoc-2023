#pragma once

#include "runner.h"

namespace week2
{
    long day08(char part);
    long day09a()

    const runner::method_map_t method_map =
    {
        { "8a", std::bind(&day08, 'a') },
        { "8b", std::bind(&day08, 'b') },
        { "9a", &day09a },
    };
};
