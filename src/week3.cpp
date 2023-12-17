#include "week3.h"
#include "util.h"

#include <iostream>

namespace week3
{
    long day15a()
    {
        std::ifstream infile("../data/day15.dat");
        std::string line; // = "rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7";
        std::getline(infile, line);
        std::vector<std::string> parts = str::split(line, ",");

        long sum = 0;
        for (auto p: parts)
        {
            long value = 0;
            for (auto c: p)
            {
                value += c;
                value *= 17;
                value %= 256;
            }
            sum += value;
        }

        return sum;
    }
};
