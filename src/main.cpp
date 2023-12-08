#include "runner.h"
#include "week1.h"
#include "week2.h"
#if 0
#include "week3.h"
#include "week4.h"
#endif

#include <iostream>

int main(int argc, char** argv)
{
    using namespace std;
    runner run;
    run.add(week1::method_map);
    run.add(week2::method_map);
#if 0
    run.add(week3::method_map);
    run.add(week4::method_map);
#endif

    const std::string name = "aoc-2023";

    if (argc == 1)
    {
        cerr << "usage: " << name << " id [id id id...]" << endl
             << "       where 'id' is one of the following:" << endl
             << "           " << run.get_keys() << endl
             << "   or: " << name << " all" << endl;
        return 1;
    }

    // note: many of these solutions will throw a std::runtime_error if there's a problem.
    // we intentionally don't catch it, since a core dump will be more helpful for debugging
    run.run(argc, argv);
    return 0;
}

