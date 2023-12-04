#include "week1.h"
#include "util.h"

#include <boost/algorithm/string.hpp>

namespace week1
{
    const std::string DIGITS { "0123456789" };
    long day01a()
    {
        std::ifstream infile("../data/day01.dat");
        std::string line;
        long sum = 0;
        while (std::getline(infile, line))
        {
            long first = line[line.find_first_of(DIGITS)] - '0';
            long last = line[line.find_last_of(DIGITS)] - '0';
            sum += first * 10 + last;
        }

        return sum;
    }

    long day01b()
    {
        const std::vector<std::string> NAMES {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
        std::ifstream infile("../data/day01.dat");
        std::string line;
        long sum = 0;
        while (std::getline(infile, line))
        {
            size_t first = line.find_first_of(DIGITS);
            size_t last = line.find_last_of(DIGITS);
            int fv = first == std::string::npos ? -1 : line[first] - '0';
            int lv = last == std::string::npos ? -1 : line[last] - '0';
            for (int i = 0; i < 10; i++)
            {
                size_t ftmp = line.find(NAMES[i]);
                size_t ltmp = line.rfind(NAMES[i]);
                if (ftmp != std::string::npos && ftmp < first)
                {
                    fv = i;
                    first = ftmp;
                }
                if (ltmp != std::string::npos && ltmp > last)
                {
                    lv = i;
                    last = ltmp;
                }
            }
            sum += fv * 10 + lv;
        }
        
        return sum;
    }

    // Game 100: 8 red, 4 blue, 4 green; 10 blue, 3 red, 4 green; 10 green, 4 red; 18 red, 9 blue, 2 green; 12 red, 4 green, 2 blue
    struct grab_t {
        long red;
        long green;
        long blue;

        grab_t(const std::string& grab)
        {
            red = green = blue = 0;
            std::vector<std::string> parts;
            boost::split(parts, grab, boost::is_any_of(","));
            for (auto color: parts)
            {
                std::vector<std::string> inner;
                boost::split(inner, str::trim(color), boost::is_any_of(" "));
                if (inner[1] == "red") red = boost::lexical_cast<long>(inner[0]);
                else if (inner[1] == "green") green = boost::lexical_cast<long>(inner[0]);
                else if (inner[1] == "blue") blue = boost::lexical_cast<long>(inner[0]);
            }

        }
    };

    struct game_t {
        long id;
        std::vector<grab_t> grabs;

        game_t(const std::string& line)
        {
            std::vector<std::string> first;
            boost::split(first, line, boost::is_any_of(":"));
            std::vector<std::string> second;
            boost::split(second, first[0], boost::is_any_of(" "));
            id = boost::lexical_cast<long>(second[1]);
            std::vector<std::string> third;
            boost::split(third, first[1], boost::is_any_of(";"));
            for (auto grab: third)
            {
                grabs.push_back(grab_t{grab});
            }
        }
    };

    long day02a()
    {
        std::ifstream infile("../data/day02.dat");
        std::string line;
        long sum = 0;
        const long RED = 12;
        const long GREEN = 13;
        const long BLUE = 14;
        while (std::getline(infile, line))
        {
            game_t game(line);
            bool possible = true;
            for (auto grab: game.grabs)
            {
                if (grab.red > RED || grab.green > GREEN || grab.blue > BLUE)
                {
                    possible = false;
                    break;
                }
            }
            if (possible) 
                sum += game.id;
        }
        return sum;
    }

    long day02b()
    {
        std::ifstream infile("../data/day02.dat");
        std::string line;
        long sum = 0;
        while (std::getline(infile, line))
        {
            game_t game(line);
            long rmax{0}, gmax{0}, bmax{0};
            for (auto grab: game.grabs)
            {
                rmax = std::max(rmax, grab.red);
                gmax = std::max(gmax, grab.green);
                bmax = std::max(bmax, grab.blue);
            }
            sum += rmax * gmax * bmax;
        }
        return sum;
    }
};
