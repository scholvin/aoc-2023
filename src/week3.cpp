#include "week3.h"
#include "util.h"

#include <iostream>

namespace week3
{
    long day15(char part)
    {
        std::ifstream infile("../data/day15.dat");
        std::string line; //= "rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7";
        std::getline(infile, line);
        std::vector<std::string> parts = str::split(line, ",");

        if (part == 'a')
        {
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
        else
        {
            struct lens_t
            {
                std::string label;
                long focal;
            };

            std::array<std::list<lens_t>, 256> boxes;

            for (auto part: parts)
            {
                long value = 0;
                bool insert = false;
                bool remove = false;
                std::string label;
                long focal = 0;
                for (auto c: part)
                {
                    if (c >= 'a' && c <= 'z')
                    {
                        value += c;
                        value *= 17;
                        value %= 256;
                        label += c;
                    }
                    else if (c == '-')
                        remove = true;
                    else if (c == '=')
                        insert = true;
                    else if (c >= '0' && c <= '9' && focal == 0)
                        focal = c - '0';
                    else
                        throw std::logic_error("problem in parsing a part in 15b");
                }
                //std::cout << label << " " << value << " " << focal << std::endl;
                auto it = std::find_if(boxes[value].begin(), boxes[value].end(), [&](const lens_t& a) { return a.label == label; });
                if (insert)
                {
                    if (it == boxes[value].end())
                        boxes[value].push_back({label, focal});
                    else
                        it->focal = focal;
                }
                else if (remove)
                {
                    if (it != boxes[value].end())
                        boxes[value].erase(it);
                }
            }

            long power = 0;
            for (size_t b = 0; b < 256; b++)
            {
                int slot = 1;
                for (auto l: boxes[b])
                {
                    power += (b+1) * slot * l.focal;
                    slot++;
                }
            }
            return power;
        }
    }
};
