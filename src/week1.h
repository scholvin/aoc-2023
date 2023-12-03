#pragma once

#include "runner.h"

namespace week1
{
    long day01a();
    long day01b();

    const runner::method_map_t method_map =
    {
//        { "1a", std::bind(&day01, 'a') },
//        { "1b", std::bind(&day01, 'b') },
        { "1a", &day01a },
        { "1b", &day01b },
    };
};
