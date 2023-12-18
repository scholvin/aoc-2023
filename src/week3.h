#pragma once

#include "runner.h"

namespace week3
{
    long day15(char part);
    long day16(char part);

    const runner::method_map_t method_map =
    {
        { "15a", std::bind(&day15, 'a') },
        { "15b", std::bind(&day15, 'b') },
        { "16a", std::bind(&day16, 'a') },
        { "16b", std::bind(&day16, 'b') },
    };
};
