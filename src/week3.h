#pragma once

#include "runner.h"

namespace week3
{
    long day15(char part);
    long day16a();

    const runner::method_map_t method_map =
    {
        { "15a", std::bind(&day15, 'a') },
        { "15b", std::bind(&day15, 'b') },
        { "16a", &day16a },
    };
};
