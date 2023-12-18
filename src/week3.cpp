#include "week3.h"
#include "util.h"

#include <iostream>
#include <unordered_set>

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

//#define SMOL
#ifdef SMOL
    const long CONTRAPTION_SZ = 10;
    const std::string day16file("../data/day16-smol.dat");
#else
    const long CONTRAPTION_SZ = 110;
    const std::string day16file("../data/day16.dat");
#endif
    typedef std::array<std::array<char, CONTRAPTION_SZ>, CONTRAPTION_SZ> contraption_t;

    struct char_in_line
    {
        char operator()(char c) { return c; }
    };

    struct coord_t
    {
        long x, y;

        bool operator == (const coord_t&) const = default;
    };

    struct coord_hash_t
    {
        std::size_t operator()(const coord_t& key) const noexcept
        {
            return std::hash<long>{}(key.x) ^ (std::hash<long>{}(key.y) << 8);
        }
    };

    enum direction_t { north, south, east, west };

    struct beam_t
    {
        coord_t position;
        direction_t dir;

        bool operator == (const beam_t&) const = default;
    };

    struct beam_hash_t
    {
        std::size_t operator()(const beam_t& key) const noexcept
        {
            size_t h = std::hash<long>{}(key.position.x) ^ (std::hash<int>{}(key.dir) << 8);
            return h ^ (std::hash<long>{}(key.position.y) << 16);
        }
    };

    typedef std::unordered_set<beam_t, beam_hash_t> visited_t;

    void process_beam(beam_t& beam, const contraption_t& contraption, visited_t& visited)
    {
        auto valid = [](const coord_t& p) -> bool
        {
            return (p.x >= 0 && p.x < CONTRAPTION_SZ && p.y >= 0 && p.y < CONTRAPTION_SZ);
        };

        auto advance = [](beam_t& beam)
        {
            switch (beam.dir)
            {
                case north: beam.position.y--; break;
                case south: beam.position.y++; break;
                case east: beam.position.x++; break;
                case west: beam.position.x--; break;
            }
        };

        // const char* pos = "NSEW";

        while (valid(beam.position))
        {
            // std::cout << beam.position.x << "," << beam.position.y << " " << pos[(int)beam.dir] << std::endl;
            if (visited.find(beam) != visited.end())
                break;
            visited.insert(beam);
            switch (contraption[beam.position.x][beam.position.y])
            {
                case '.':
                    advance(beam);
                    break;
                case '/':
                    if (beam.dir == east) beam.dir = north;
                    else if (beam.dir == north) beam.dir = east;
                    else if (beam.dir == west) beam.dir = south;
                    else if (beam.dir == south) beam.dir = west;
                    advance(beam);
                    break;
                case '\\':
                    if (beam.dir == east) beam.dir = south;
                    else if (beam.dir == north) beam.dir = west;
                    else if (beam.dir == west) beam.dir = north;
                    else if (beam.dir == south) beam.dir = east;
                    advance(beam);
                    break;
                case '-':
                    if (beam.dir == east || beam.dir == west)
                        advance(beam);
                    else
                    {
                        // existing beam goes west, create new beam east
                        beam_t newbeam{beam.position, west};
                        advance(newbeam);
                        process_beam(newbeam, contraption, visited);
                        beam.dir = east;
                        advance(beam);
                    }
                    break;
                case '|':
                    if (beam.dir == north || beam.dir == south)
                        advance(beam);
                    else
                    {
                        // existing beam goes north, create new beam south
                        beam_t newbeam{beam.position, south};
                        advance(newbeam);
                        process_beam(newbeam, contraption, visited);
                        beam.dir = north;
                        advance(beam);
                    }
                    break;
            }
        }
    }

    long day16(char part)
    {
        contraption_t contraption;
        readers::read_dense_2d_matrix(day16file, char_in_line(), contraption);

        auto wrap = [&](beam_t& beam) -> long
        {
            std::unordered_set<beam_t, beam_hash_t> visited;
            process_beam(beam, contraption, visited);

            // gotta count unique positions now
            std::unordered_set<coord_t, coord_hash_t> unique;
            for (auto v: visited)
            {
                unique.insert(v.position);
            }
            return unique.size();
        };

        if (part == 'a')
        {
            beam_t beam({{0, 0}, east});
            return wrap(beam);
        }
        else
        {
            long max = 0;

            // try all eastbound from left
            for (long y = 0; y < CONTRAPTION_SZ; y++)
            {
                beam_t beam({{0, y}, east});
                max = std::max(wrap(beam), max);
            }
            // try all southbound from top
            for (long x = 0; x < CONTRAPTION_SZ; x++)
            {
                beam_t beam({{x, 0}, south});
                max = std::max(wrap(beam), max);
            }
            // try all westbound from right
            for (long y = 0; y < CONTRAPTION_SZ; y++)
            {
                beam_t beam({{CONTRAPTION_SZ-1, y}, west});
                max = std::max(wrap(beam), max);
            }
            // try all northbound from bottom
            for (long x = 0; x < CONTRAPTION_SZ; x++)
            {
                beam_t beam({{x, CONTRAPTION_SZ-1}, north});
                max = std::max(wrap(beam), max);
            }

            return max;
        }
    }
};
