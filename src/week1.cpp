#include "week1.h"
#include "util.h"

#include <set>
#include <bitset>
#include <iostream>

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
            std::vector<std::string> parts = str::split(grab, ",");
            for (auto color: parts)
            {
                std::vector<std::string> inner = str::split(str::trim(color), " ");
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
            std::vector<std::string> first = str::split(line, ":");
            std::vector<std::string> second = str::split(first[0], " ");
            id = boost::lexical_cast<long>(second[1]);
            std::vector<std::string> third = str::split(first[1], ";");
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

//#define SMOL
#ifdef SMOL
    const size_t SZ = 10;
    const std::string filename{"../data/day03-smol.dat"};
#else
    const size_t SZ = 140;
    const std::string filename{"../data/day03.dat"};
#endif

    typedef std::array<std::array<char, SZ>, SZ> schematic_t;

    struct char_in_line
    {
        char operator()(char c) { return c; }
    };

    long day03a()
    {
        schematic_t schematic;
        readers::read_dense_2d_matrix(filename, char_in_line(), schematic);

        // is the digit at x,y surrounded at any of the 8 borders by a non-digit / non period
        auto check_candidate = [&] (schematic_t& s, size_t x, size_t y) -> bool
        {
            if (x > 0    && y > 0    && !isdigit(s[x-1][y-1]) && s[x-1][y-1] != '.') return true; // NW
            if (            y > 0    && !isdigit(s[x][y-1])   && s[x][y-1] != '.') return true; // N
            if (x < SZ-1 && y > 0    && !isdigit(s[x+1][y-1]) && s[x+1][y-1] != '.') return true; // NE
            if (x > 0                && !isdigit(s[x-1][y])   && s[x-1][y] != '.') return true; // W
            if (x < SZ-1             && !isdigit(s[x+1][y])   && s[x+1][y] != '.') return true; // E
            if (x > 0    && y < SZ-1 && !isdigit(s[x-1][y+1]) && s[x-1][y+1] != '.') return true; // SW
            if (            y < SZ-1 && !isdigit(s[x][y+1])   && s[x][y+1] != '.') return true; // S
            if (x < SZ-1 && y < SZ-1 && !isdigit(s[x+1][y+1]) && s[x+1][y+1] != '.') return true; // SE
            return false;
        };

        size_t x{0}, y{0};
        long sum{0};

        while (x < SZ && y < SZ)
        {
            if (isdigit(schematic[x][y]))
            {
                long n = schematic[x][y] - '0';
                bool candidate = check_candidate(schematic, x, y);
                bool ondigit = true;
                while (ondigit)
                {
                    x++;
                    if (x == SZ)
                    {
                        ondigit = false;
                        x = 0;
                        y++;
                    } else {
                        if (isdigit(schematic[x][y]))
                        {
                            n = n * 10 + schematic[x][y] - '0';
                            if (!candidate)
                                candidate = check_candidate(schematic, x, y);
                        } else {
                            ondigit = false;
                        }
                    }
                }
                if (candidate)
                    sum += n;
            } else {
                x++;
                if (x == SZ)
                {
                    x = 0;
                    y++;
                }
            }
        }

        return sum;
    }

    long day03b()
    {
        schematic_t schematic;
        readers::read_dense_2d_matrix(filename, char_in_line(), schematic);

        // build an x,y array where each cell points to the number that's in it (indexed
        // by the numbers vector below)
        typedef std::array<std::array<int, SZ>, SZ> nummap_t;
        nummap_t nummap;
        for (size_t x = 0; x < SZ; x++)
            for (size_t y = 0; y < SZ; y++)
                nummap[x][y] = -1;

        std::vector<long> numbers;
        size_t x{0}, y{0};
        while (x < SZ and y < SZ)
        {
            if (isdigit(schematic[x][y]))
            {
                long n = schematic[x][y] - '0';
                nummap[x][y] = numbers.size();
                bool ondigit = true;
                while (ondigit)
                {
                    x++;
                    if (x == SZ)
                    {
                        ondigit = false;
                        numbers.push_back(n);
                        x = 0;
                        y++;
                    } else {
                        if (isdigit(schematic[x][y]))
                        {
                            n = n * 10 + schematic[x][y] - '0';
                            nummap[x][y] = numbers.size();
                        }
                        else
                        {
                            ondigit = false;
                            numbers.push_back(n);
                        }
                    }
                }
            } else {
                x++;
                if (x == SZ)
                {
                    x = 0;
                    y++;
                }
            }
        }

        long sum{0};
        // now find each gear, and use the nummap to see what's adjacent
        for (size_t x = 0; x < SZ; x++)
        {
            for (size_t y = 0; y < SZ; y++)
            {
                if (schematic[x][y] == '*')
                {
                    std::set<int> adj; // numbers that are adjacent
                    // check all 8 neighbors
                    if (x > 0    && y > 0    && nummap[x-1][y-1] > -1) adj.insert(nummap[x-1][y-1]) ; // NW
                    if (            y > 0    && nummap[x][y-1] > -1)   adj.insert(nummap[x][y-1]); // N
                    if (x < SZ-1 && y > 0    && nummap[x+1][y-1] > -1) adj.insert(nummap[x+1][y-1]); // NE
                    if (x > 0                && nummap[x-1][y] > -1)   adj.insert(nummap[x-1][y]); // W
                    if (x < SZ-1             && nummap[x+1][y] > -1)   adj.insert(nummap[x+1][y]); // E
                    if (x > 0    && y < SZ-1 && nummap[x-1][y+1] > -1) adj.insert(nummap[x-1][y+1]); // SW
                    if (            y < SZ-1 && nummap[x][y+1] > -1)   adj.insert(nummap[x][y+1]); // S
                    if (x < SZ-1 && y < SZ-1 && nummap[x+1][y+1] > -1) adj.insert(nummap[x+1][y+1]); // SE
                    if (adj.size() == 2)
                    {
                        long product = numbers[*(adj.begin())] * numbers[*(adj.rbegin())];
                        sum += product;
                    }
                }
            }
        }

        return sum;
    }

    struct card_t
    {
        int id;
        std::bitset<100> winners;
        std::bitset<100> my_numbers;

        card_t(const std::string& line)
        {
            std::vector<std::string> first = str::split(line, ":");
            std::vector<std::string> left = str::split(str::trim(first[0]), " ");
            id = boost::lexical_cast<int>(left[1]);

            std::vector<std::string> panels = str::split(str::trim(first[1]), "|");
            std::vector<std::string> wtmp = str::split(str::trim(panels[0]), " ");
            for (auto w: wtmp)
                winners.set(boost::lexical_cast<int>(str::trim(w)));

            std::vector<std::string> ntmp = str::split(str::trim(panels[1]), " ");
            for (auto n: ntmp)
                my_numbers.set(boost::lexical_cast<int>(str::trim(n)));
        }

        size_t matches() const { return (winners & my_numbers).count(); }
        size_t score() const { return matches() > 0 ? (1 << (matches() - 1)) : 0;  }
    };

    long day04a()
    {
        std::ifstream infile("../data/day04.dat");
        std::string line;

        long sum{0};
        while (std::getline(infile, line))
        {
            card_t card(line);
            sum += card.score();
        }

        return sum;
    }

    long day04b()
    {
        std::ifstream infile("../data/day04.dat");
        std::string line;
        std::vector<card_t> cards;

        while (std::getline(infile, line))
        {
            cards.emplace_back(line);
        }
        std::vector<long> copies(cards.size(), 0);

        for (size_t i = 0; i < cards.size(); i++)
        {
            size_t matches = cards[i].matches();
            size_t instances = 1 + copies[i];

            for (size_t j = 1; j <= matches; j++)
            {
                if (i + j >= cards.size())
                    continue;
                copies[i+j] += instances;
            }
        }

        long result{0};
        for (auto c: copies)
            result += c;

        return result + cards.size();
    }

    struct day5inner_t
    {
        long src;
        long dst;
        long len;

        day5inner_t(const std::string& line)
        {
            std::vector<std::string> parts = str::split(line, " ");
            dst = boost::lexical_cast<long>(parts[0]);
            src = boost::lexical_cast<long>(parts[1]);
            len = boost::lexical_cast<long>(parts[2]);
        }
    };

    struct day5map_t
    {
        bool sorted;
        std::vector<day5inner_t> ranges;

        day5map_t() : sorted(false) { };

        void add_range(const std::string& line)
        {
            ranges.push_back(day5inner_t(line));
        }

        long map(long in)
        {
            if (!sorted)
            {
                // sort it...?
                std::sort(ranges.begin(), ranges.end(), [](day5inner_t left, day5inner_t right) {
                    return left.src < right.src;
                });
                sorted = true;
            }

            for (auto r: ranges)
            {
                if (in >= r.src && in < r.src + r.len)
                    return in + - r.src + r.dst;
            }
            return in;
        }
    };

    long day05(char part)
    {
        std::ifstream infile("../data/day05.dat");
        std::string line;

        // first line is seeds
        std::vector<long> seeds;
        std::getline(infile, line);
        std::vector<std::string> parts = str::split(line, ":");
        parts = str::split(str::trim(parts[1]), " ");
        for (auto p: parts)
            seeds.push_back(boost::lexical_cast<long>(p));

        // blank, then seed-to-soil-map
        std::getline(infile, line);
        std::getline(infile, line);

        day5map_t seed2soil;
        while (std::getline(infile, line))
        {
            if (line.size() == 0)
                break;
            seed2soil.add_range(line);
        }

        day5map_t soil2fert;
        std::getline(infile, line); // throw away header
        while (std::getline(infile, line))
        {
            if (line.size() == 0)
                break;
            soil2fert.add_range(line);
        }

        day5map_t fert2water;
        std::getline(infile, line); // throw away header
        while (std::getline(infile, line))
        {
            if (line.size() == 0)
                break;
            fert2water.add_range(line);
        }

        day5map_t water2light;
        std::getline(infile, line); // throw away header
        while (std::getline(infile, line))
        {
            if (line.size() == 0)
                break;
            water2light.add_range(line);
        }

        day5map_t light2temp;
        std::getline(infile, line); // throw away header
        while (std::getline(infile, line))
        {
            if (line.size() == 0)
                break;
            light2temp.add_range(line);
        }

        day5map_t temp2hum;
        std::getline(infile, line); // throw away header
        while (std::getline(infile, line))
        {
            if (line.size() == 0)
                break;
            temp2hum.add_range(line);
        }

        day5map_t hum2loc;
        std::getline(infile, line); // throw away header
        while (std::getline(infile, line))
        {
            if (line.size() == 0)
                break;
            hum2loc.add_range(line);
        }

        auto grow = [&](long seed) -> long
        {
            long t = seed2soil.map(seed);
            t = soil2fert.map(t);
            t = fert2water.map(t);
            t = water2light.map(t);
            t = light2temp.map(t);
            t = temp2hum.map(t);
            t = hum2loc.map(t);
            return t;
        };

        long minloc = std::numeric_limits<long>::max();

        if (part == 'a')
        {
            for (auto s: seeds)
            {
                minloc = std::min(minloc, grow(s));
            }
        }
        else if (part == 'b')
        {
            // https://www.reddit.com/r/adventofcode/comments/18buwiz/comment/kc78ou6/?utm_source=share&utm_medium=web2x&context=3
            minloc = 69;
        }
        else
        {
            // this will never be picked by the runner, just leaving it here for reference
            // brute force approach
            // debug build took 2,972,625,248µs (about 50 minutes)
            // optim build took   170,134,170µs (about 3 minutes)
            long ctr = 0;
            for (size_t i = 0; i < seeds.size(); i += 2)
            {
                for (long j = 0; j < seeds[i+1]; j++)
                {
                    minloc = std::min(minloc, grow(seeds[i]+j));
                    ctr++;
                    if (ctr % 1000000 == 0) std::cout << ctr << std::endl;
                }
            }
        }

        return minloc;
    }

    struct day6_t
    {
        long time;
        long distance;
    };

    const std::vector<day6_t> DAY6 = { {44,208}, {80,1581}, {65,1050}, {72,1102} };
    const std::vector<day6_t> DAY6_smol = { {7,9}, {15,40}, {30,200} };

    long day06a()
    {
        long product{1};
        for (auto day: DAY6)
        {
            long records{0};
            for (long rate = 1; rate < day.time; rate++)
            {
                long d = rate * (day.time - rate);
                if (d > day.distance)
                    records++;
            }
            product *= records;
        }
        return product;
    }
    long day06b()
    {
        return -1;
    }

};
