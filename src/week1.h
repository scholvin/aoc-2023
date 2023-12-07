#pragma once

#include "runner.h"

namespace week1
{
    long day01a();
    long day01b();
    long day02a();
    long day02b();
    long day03a();
    long day03b();
    long day04a();
    long day04b();
    long day05a();
    long day05b();

    const runner::method_map_t method_map =
    {
//        { "1a", std::bind(&day01, 'a') },
//        { "1b", std::bind(&day01, 'b') },
        { "1a", &day01a },
        { "1b", &day01b },
        { "2a", &day02a },
        { "2b", &day02b },
        { "3a", &day03a },
        { "3b", &day03b },
        { "4a", &day04a },
        { "4b", &day04b },
        { "5a", &day05a },
        { "5b", &day05b },
    };
};
