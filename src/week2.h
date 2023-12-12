#pragma once

#include "runner.h"

namespace week2
{
    long day08(char part);
    long day09(char part);
    long day10(char part);
    long day11(char part);

    const runner::method_map_t method_map =
    {
        {  "8a", std::bind(&day08, 'a') },
        {  "8b", std::bind(&day08, 'b') },
        {  "9a", std::bind(&day09, 'a') },
        {  "9b", std::bind(&day09, 'b') },
        { "10a", std::bind(&day10, 'a') },
        { "10b", std::bind(&day10, 'b') },
        { "11a", std::bind(&day11, 'a') },
    };
};
