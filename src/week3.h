#pragma once

#include "runner.h"

namespace week3
{
    long day15a();

    const runner::method_map_t method_map =
    {
        { "15a", &day15a },
    };
};
