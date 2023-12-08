#include "week2.h"
#include "util.h"

namespace week2
{
    struct day8_t
    {
        std::string result;
        std::string left;
        std::string right;

        day8_t(const std::string& line)
        {
            // 0123456789ABCDE
            // ABC = (DEF, GHI)
            result = line.substr(0, 3);
            left = line.substr(7, 3);
            right = line.substr(12, 3);
        }
    };

    long day08a()
    {
        std::ifstream infile("../data/day08.dat");
        std::string line;
        std::string instructions;
        std::vector<day8_t> nodes;

        std::getline(infile, instructions);
        std::getline(infile, line); // blank

        std::unordered_map<std::string, std::string> left;
        std::unordered_map<std::string, std::string> right;

        while (std::getline(infile, line))
        {
            left[line.substr(0, 3)] = line.substr(7, 3);
            right[line.substr(0, 3)] = line.substr(12, 3);
        }

        std::string here = "AAA";
        long steps = 0;
        while (here != "ZZZ")
        {
            for (auto i: instructions)
            {
                if (i == 'L')
                    here = left[here];
                else if (i == 'R')
                    here = right[here];
                steps++;
                if (here == "ZZZ")
                    break;
            }
        }
        return steps;

    }

    long day08b()
    {
        return -1;
    }
};
