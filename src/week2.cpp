#include "week2.h"
#include "util.h"

#include <unordered_set>
#include <iostream>
#include <numeric>
#include <cmath>
#include <boost/functional/hash.hpp>

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

    long day08(char part)
    {
        std::ifstream infile("../data/day08.dat");
        std::string line;
        std::string instructions;
        std::vector<day8_t> nodes;

        std::getline(infile, instructions);
        std::getline(infile, line); // blank

        std::unordered_map<std::string, std::string> left;
        std::unordered_map<std::string, std::string> right;

        std::vector<std::string> part_b_starters; // starting points

        while (std::getline(infile, line))
        {
            std::string dest = line.substr(0, 3);
            left[dest] = line.substr(7, 3);
            right[dest] = line.substr(12, 3);
            if (dest[2] == 'A')
                part_b_starters.push_back(dest);
        }

        if (part == 'a')
        {
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
        else
        {
            std::vector<long> results;
            for (auto here: part_b_starters)
            {
                long steps = 0;
                while (here[2] != 'Z')
                {
                    for (auto i: instructions)
                    {
                        if (i == 'L')
                            here = left[here];
                        else if (i == 'R')
                            here = right[here];
                        steps++;
                        if (here[2] == 'Z')
                            break;
                    }
                }
                results.push_back(steps);
            }
            long result = 1;
            for (size_t i = 0; i < results.size(); i++)
                result = std::lcm(result, results[i]);
            return result;
        }
    }

    long day09(char part)
    {
        typedef std::deque<long> seq_t;
        typedef std::deque<seq_t> seqseq_t;
        seqseq_t sequences;
        readers::read_delimited_lines("../data/day09.dat", sequences);
        long sum{0};

        auto all_zeroes = [](const seq_t& seq) -> bool
        {
            for (auto s: seq)
                if (s)
                    return false;
            return true;
        };

        for (auto seq: sequences)
        {
            // build the data structure
            seqseq_t work;
            work.push_back(seq);
            bool done = false;
            while (!done)
            {
                seq_t& cur = work.back();
                seq_t next;
                for (auto it = cur.begin(); it != cur.end() - 1; it++)
                    next.push_back(*(it+1) - *it);
                if (all_zeroes(next))
                    done = true;
                work.push_back(next);
            }

            // solve
            if (part == 'a')
            {
                work.back().push_back(0);
                for (auto it = work.rbegin() + 1; it != work.rend(); it++)
                    it->push_back((*(it-1)).back() + (*it).back());

                sum += work.front().back();
            }
            else
            {
                work.back().push_front(0);
                for (auto it = work.rbegin() + 1; it != work.rend(); it++)
                    it->push_front((*it).front() - (*(it-1)).front());

                sum += work.front().front();
            }
        }

        return sum;
    }

//#define SMOL
#ifdef SMOL
    const long MAZE_SZ_X = 20;
    const long MAZE_SZ_Y = 10;
    const std::string day10file("../data/day10-smol2.dat");
#else
    const long MAZE_SZ_X = 140;
    const long MAZE_SZ_Y = 140;
    const std::string day10file("../data/day10.dat");
#endif

    typedef std::array<std::array<char, MAZE_SZ_Y>, MAZE_SZ_X> maze_t;

    struct char_in_line
    {
        char operator()(char c) { return c; }
    };

    struct coord_t
    {
        long x, y;

        bool operator == (const coord_t&) const = default;
    };

    // find the outbound flows from coord and return them - for starting pos
    std::vector<coord_t> find_out_flows(const maze_t& maze, const coord_t& s)
    {
        std::vector<coord_t> result;
        if (s.x < MAZE_SZ_X - 1 && (maze[s.x+1][s.y] == '-' || maze[s.x+1][s.y] == 'J' || maze[s.x+1][s.y] == '7')) // east
            result.push_back({s.x+1, s.y});
        if (s.x > 0 && (maze[s.x-1][s.y] == '-' || maze[s.x-1][s.y] == 'L' || maze[s.x-1][s.y] == 'F')) // west
            result.push_back({s.x-1, s.y});
        if (s.y > 0 && (maze[s.x][s.y-1] == '|' || maze[s.x][s.y-1] == '7' || maze[s.x][s.y-1] == 'F')) // north
            result.push_back({s.x, s.y-1});
        if (s.y < MAZE_SZ_Y - 1 && (maze[s.x][s.y+1] == '|' || maze[s.x][s.y+1] == 'L' || maze[s.x][s.y+1] == 'J')) // south
            result.push_back({s.x, s.y+1});
        if (result.size() != 2)
            throw std::logic_error("bug in find_out_flows");
        return result;
    }

    // figure out what kind of shape S really is for part b
    char find_start_shape(const maze_t& maze, const coord_t& s)
    {
        std::vector<coord_t> turns = find_out_flows(maze, s);
        bool north{false}, south{false}, east{false}, west{false};
        if (turns[0].x < s.x || turns[1].x < s.x)
            west = true;
        if (turns[0].x > s.x || turns[1].x > s.x)
            east = true;
        if (turns[0].y < s.y || turns[1].y < s.y)
            north = true;
        if (turns[0].y > s.y || turns[1].y > s.y)
            south = true;
        if (north && west)
            return 'J';
        else if (north && east)
            return 'L';
        else if (south && west)
            return '7';
        else if (south && east)
            return 'F';
        else if (north && south)
            return '|';
        else if (east && west)
            return '-';
        throw std::logic_error("bug in find_start_shape");
    }

    // find the next flow from curr (having come from prev)
    coord_t find_next(const maze_t& maze, const coord_t& prev, const coord_t& curr)
    {
        coord_t result = curr;
        switch (maze[curr.x][curr.y])
        {
            case '-': if (prev.x < curr.x) result.x++; else result.x--; break;
            case '|': if (prev.y < curr.y) result.y++; else result.y--; break;
            case 'J': if (prev.x < curr.x) result.y--; else result.x--; break;
            case 'F': if (prev.y > curr.y) result.x++; else result.y++; break;
            case '7': if (prev.y > curr.y) result.x--; else result.y++; break;
            case 'L': if (prev.y < curr.y) result.x++; else result.y--; break;
        }
        return result;
    }

    void print(const maze_t maze)
    {
        for (size_t y = 0; y < MAZE_SZ_Y; y++)
        {
            for (size_t x = 0; x < MAZE_SZ_X; x++)
            {
                std::cout << maze[x][y];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    long day10(char part)
    {
        maze_t maze_a, maze_b;
        readers::read_dense_2d_matrix(day10file, char_in_line(), maze_a);

        coord_t start;
        for (auto x = 0; x < MAZE_SZ_X; x++)
        {
            for (auto y = 0; y < MAZE_SZ_Y; y++)
            {
                if (maze_a[x][y] == 'S')
                {
                    start = {x, y};
                }
                maze_b[x][y] = '.';
            }
        }

        maze_b[start.x][start.y] = '#';

        long count = 1;
        coord_t curr = find_out_flows(maze_a, start)[0];
        maze_b[curr.x][curr.y] = '#';
        coord_t prev = start;
        while (curr != start)
        {
            coord_t next = find_next(maze_a, prev, curr);
            prev = curr;
            curr = next;
            maze_b[curr.x][curr.y] = '#';
            count++;
        }

        if (part == 'a')
        {
            return count / 2;
        }
        else
        {
            // let's try the scan line algorithm!

            // need to have a real shape where the S goes
            maze_a[start.x][start.y] = find_start_shape(maze_a, start);
            long count = 0;

            for (size_t y = 0; y < MAZE_SZ_Y; y++)
            {
                bool region = false;
                char last_elbow = ' ';
                for (size_t x = 0; x < MAZE_SZ_X; x++)
                {
                    if (maze_b[x][y] == '.')
                    {
                        if (region)
                        {
                            count++;
                            maze_b[x][y] = 'I'; // for visual clarity when printing
                        }
                        else
                            ; // just keep going...
                    }
                    else if (maze_b[x][y] == '#')
                    {
                        switch (maze_a[x][y])
                        {
                            case '|': region = !region; break;
                            case '-': break;
                            case 'F': last_elbow = 'F'; break;
                            case 'L': last_elbow = 'L'; break;
                            case 'J': if (last_elbow == 'F') region = !region; break;
                            case '7': if (last_elbow == 'L') region = !region; break;
                            default: throw std::logic_error("badly constructed maze_b 1");
                        }

                    }
                    else
                        throw std::logic_error("badly constructed maze_b 2");
                }
            }

            return count;
        }
    }

//#define SMOL
#ifdef SMOL
    const long UNIVERSE_SZ = 10;
    const std::string day11file("../data/day11-smol.dat");
#else
    const long UNIVERSE_SZ = 140;
    const std::string day11file("../data/day11.dat");
#endif

    typedef std::array<std::array<char, UNIVERSE_SZ>, UNIVERSE_SZ> universe_t;

    long day11(char part)
    {
        universe_t universe;
        readers::read_dense_2d_matrix(day11file, char_in_line(), universe);

        std::vector<coord_t> galaxies;
        for (auto x = 0; x < UNIVERSE_SZ; x++)
            for (auto y = 0; y < UNIVERSE_SZ; y++)
                if (universe[x][y] == '#')
                    galaxies.push_back({x, y});

        std::vector<long> empty_rows;
        for (auto y = 0; y < UNIVERSE_SZ; y++)
        {
            auto x = 0;
            for (; x < UNIVERSE_SZ; x++)
                if (universe[x][y] == '#')
                    break;
            if (x == UNIVERSE_SZ)
                empty_rows.push_back(y);
        }

        std::vector<long> empty_cols;
        for (auto x = 0; x < UNIVERSE_SZ; x++)
        {
            auto y = 0;
            for ( ; y < UNIVERSE_SZ; y++)
                if (universe[x][y] == '#')
                    break;
            if (y == UNIVERSE_SZ)
                empty_cols.push_back(x);
        }

        long delta = part == 'a' ? 1 : 999999;

        // expand spacetime
        for (auto it = empty_cols.begin(); it != empty_cols.end(); it++)
        {
            for (auto git = galaxies.begin(); git != galaxies.end(); git++)
            {
                if (git->x > *it)
                    (git->x) += delta; // move a galaxy right
            }
            for (auto sub = it + 1; sub != empty_cols.end(); sub++)
                (*sub) += delta; // move a blank column right
        }
        for (auto it = empty_rows.begin(); it != empty_rows.end(); it++)
        {
            for (auto git = galaxies.begin(); git != galaxies.end(); git++)
            {
                if (git->y > *it)
                    (git->y) += delta; // move a galaxy down
            }
            for (auto sub = it + 1; sub != empty_rows.end(); sub++)
                (*sub) += delta; // move a blank row down
        }

        // find the paths
        long sum = 0;
        for (auto git = galaxies.begin(); git != galaxies.end(); git++)
            for (auto sub = git+1; sub != galaxies.end(); sub++)
                sum += std::abs(git->x - sub->x) + std::abs(git->y - sub->y);

        return sum;
    }

    // shameless lift from https://www.reddit.com/r/adventofcode/comments/18hbbxe/2023_day_12python_stepbystep_tutorial_with_bonus/
    typedef std::vector<size_t> group_t;
    typedef std::pair<std::string, group_t> key12_t;

    struct key12_hash_t
    {
        std::size_t operator()(const key12_t& key) const noexcept
        {
            size_t h = std::hash<std::string>{}(key.first);
            int i = 1;
            for (auto g: key.second)
                h = h ^ (g << i++);
            return h;
        }
    };

    typedef std::unordered_map<key12_t, long, key12_hash_t> map12_t;

    map12_t memo;

    long calc12(const std::string& record, const group_t& groups)
    {
        // Did we run out of groups? We might still be valid
        if (groups.size() == 0)
        {
            if (record.find('#') == std::string::npos)
                // This will return true even if record is empty, which is valid
                return 1;
            else
                // More damaged springs that we can't fit
                return 0;
        }

        // There are more groups, but no more record
        if (record.size() == 0)
            return 0;

        char next_char = record[0];
        size_t next_group = groups[0];

        auto pound = [](size_t next_group, const std::string& record, const group_t& groups) -> long
        {
            // if the first is a pound, then the first n characters must be
            // able to be treated as a pound, where n is the first group number
            std::string this_group = record.substr(0, next_group);
            std::replace(this_group.begin(), this_group.end(), '?', '#');

            // if the next group can't fit all the damaged springs, then abort
            if (this_group != std::string(next_group, '#'))
                return 0;

            // If the rest of the record is just the last group, then we're
            // done and there's only one possibility
            if (record.size() == next_group)
            {
                // make sure this is the last group
                if (groups.size() == 1)
                    // we are valid
                    return 1;
                else
                    // there's more groups, we can't make it work
                    return 0;
            }

            // make sure the character that follows. this group can be a separator
            if (record[next_group] == '?' || record[next_group] == '.')
            {
                // it can be a separator, so skip it and reduce to the next group
                group_t n(groups.size()-1);
                std::copy(groups.begin()+1, groups.end(), n.begin());
                // memoize
                auto x = memo.find({record.substr(next_group+1), n});
                if (x == memo.end())
                {
                    long y = calc12(record.substr(next_group+1), n);
                    memo[{record.substr(next_group+1), n}] = y;
                    return y;
                }
                return x->second;
            };

            // can't be handled, there are no possibilities
            return 0;
        };

        auto dot = [](const std::string& record, const group_t& groups) -> long
        {
            // memoize
            auto x = memo.find({record.substr(1), groups});
            if (x == memo.end())
            {
                long y = calc12(record.substr(1), groups);
                memo[{record.substr(1), groups}] = y;
                return y;
            }
            return x->second;
        };

        long out = 0;
        if (next_char == '#')
            out = pound(next_group, record, groups);
        else if (next_char == '.')
            out = dot(record, groups);
        else if (next_char == '?')
            out = dot(record, groups) + pound(next_group, record, groups);
        else
            throw std::logic_error("serious bug");

        return out;
    }

    long day12(char part)
    {
        std::ifstream infile("../data/day12.dat");
        std::string line;
        long sum = 0;
        while (std::getline(infile, line))
        {
            std::vector<std::string> parts = str::split(line, " ");
            std::string record = parts[0];
            group_t groups;
            parts = str::split(parts[1], ",");
            for (auto p: parts)
                groups.push_back(boost::lexical_cast<size_t>(p));

            if (part == 'b')
            {
                record = record + "?" + record + "?" + record + "?" + record + "?" + record;
                auto ogroups = groups;
                for (int i = 0; i < 4; i++)
                    groups.insert(groups.end(), ogroups.begin(), ogroups.end());
            }

            sum += calc12(record, groups);
        }
#if 0
        std::cout << "memo size: " << memo.size()
                  << " buckets: " << memo.bucket_count()
                  << " load factor: " << memo.load_factor() << std::endl;
#endif
        return sum;
    }

    typedef std::vector<std::string> mirror13_t;

    struct reflection_t
    {
        char type; // 'R' or 'C';
        int position;

        bool operator ==(const reflection_t& other) const = default;
    };

    void solve13(const mirror13_t& mirror, std::vector<reflection_t>& solutions)
    {
        solutions.clear();

        // note that we are in [row][col] notation not [x][y]
        const int COLS = mirror[0].size();
        const int ROWS = mirror.size();

        // check for row symmetry
        bool symmetric;
        int r0 = 1;
        for ( ; r0 < ROWS; r0++)
        {
            symmetric = true;
            int rl = r0 - 1;
            int rh = r0;
            while (rl >= 0 && rh < ROWS && symmetric)
            {
                // does rl == rh?
                symmetric = symmetric && (mirror[rl] == mirror[rh]);
                rl--;
                rh++;
            }
            if (symmetric)
            {
                solutions.push_back({ 'R', r0 });
                // std::cout << "solution at " << solutions.back().type << " " << solutions.back().position << std::endl;
            }
        }

        int c0 = 1;
        for ( ; c0 < COLS; c0++)
        {
            symmetric = true;
            int cl = c0 - 1;
            int ch = c0;
            while (cl >= 0 && ch < COLS && symmetric)
            {
                // does cl == ch?
                bool colsym = true;
                for (int i = 0; i < ROWS; i++)
                    colsym = colsym && (mirror[i][cl] == mirror[i][ch]);
                symmetric = symmetric && colsym;
                cl--;
                ch++;
            }
            if (symmetric)
            {
                solutions.push_back({ 'C', c0 });
                // std::cout << "solution at " << solutions.back().type << " " << solutions.back().position << std::endl;
            }
        }
    }

    long day13(char part)
    {
        std::ifstream infile("../data/day13.dat");
        std::string line;

        long result = 0;
        while (infile.peek() != EOF)
        {
            mirror13_t mirror;
            while (std::getline(infile, line))
            {
                if (line.size() > 0)
                    mirror.push_back(line);
                else
                    break;
            }

            if (part == 'a')
            {
                std::vector<reflection_t> solutions;
                solve13(mirror, solutions);
                if (solutions.size() == 1)
                {
                    if (solutions.back().type == 'C')
                        result += solutions.back().position;
                    else
                        result += solutions.back().position * 100;
                }
                else
                    throw std::logic_error("unexpected solution space found in part a: " + boost::lexical_cast<std::string>(solutions.size()));
            }
            else
            {
                std::vector<reflection_t> solutions;
                reflection_t reference;
                solve13(mirror, solutions);
                if (solutions.size() == 1)
                    reference = solutions.back();
                else
                    throw std::logic_error("unexpected solution space found in part b: " + boost::lexical_cast<std::string>(solutions.size()));

                for (size_t r = 0; r < mirror.size(); r++)
                {
                    for (size_t c = 0; c < mirror[0].size(); c++)
                    {
                        mirror13_t mcopy = mirror;
                        mcopy[r][c] = (mcopy[r][c] == '#' ? '.' : '#');
                        solve13(mcopy, solutions);
                        for (auto s: solutions)
                        {
                            if (s != reference)
                            {
                                if (s.type == 'C')
                                    result += s.position;
                                else
                                    result += s.position * 100;
                                goto finish;
                            }
                        }
                    }
                }
                throw std::logic_error("no altered solutions found in part b");
finish:
                ;
            }
        }

        return result;
    }

//#define SMOL
#ifdef SMOL
    const long MIRROR_SZ = 10;
    const std::string day14file("../data/day14-smol.dat");
#else
    const long MIRROR_SZ = 100;
    const std::string day14file("../data/day14.dat");
#endif

    typedef std::array<std::array<char, MIRROR_SZ>, MIRROR_SZ> mirror14_t;

    size_t hash(const mirror14_t& mirror)
    {
        std::string s;
        for (size_t x = 0; x < MIRROR_SZ; x++)
            for (size_t y = 0; y < MIRROR_SZ; y++)
                s += mirror[x][y];
        return std::hash<std::string>{}(s);
    }

    void print(const mirror14_t& mirror)
    {
        for (size_t y = 0; y < MIRROR_SZ; y++)
        {
            for (size_t x = 0; x < MIRROR_SZ; x++)
            {
                std::cout << mirror[x][y];
            }
            std::cout << std::endl;
        }
        std::cout << std::hex << hash(mirror) << std::endl << std::endl;
    }

    long day14(char part)
    {
        mirror14_t mirror;
        readers::read_dense_2d_matrix(day14file, char_in_line(), mirror);

        auto north = [&]()
        {
            bool moved = true;
            while (moved)
            {
                moved = false;
                for (size_t y = 1; y < MIRROR_SZ; y++)
                {
                    for (size_t x = 0; x < MIRROR_SZ; x++)
                    {
                        if (mirror[x][y] == 'O' && mirror[x][y-1] == '.')
                        {
                            moved = true;
                            mirror[x][y] = '.';
                            mirror[x][y-1] = 'O';
                        }
                    }
                }
            }
        };

        auto south = [&]()
        {
            bool moved = true;
            while (moved)
            {
                moved = false;
                for (int y = MIRROR_SZ - 2; y >= 0; y--)
                {
                    for (size_t x = 0; x < MIRROR_SZ; x++)
                    {
                        if (mirror[x][y] == 'O' && mirror[x][y+1] == '.')
                        {
                            moved = true;
                            mirror[x][y] = '.';
                            mirror[x][y+1] = 'O';
                        }
                    }
                }
            }
        };

        auto east = [&]()
        {
            bool moved = true;
            while (moved)
            {
                moved = false;
                for (int x = MIRROR_SZ - 2; x >= 0; x--)
                {
                    for (size_t y = 0; y < MIRROR_SZ; y++)
                    {
                        if (mirror[x][y] == 'O' && mirror[x+1][y] == '.')
                        {
                            moved = true;
                            mirror[x][y] = '.';
                            mirror[x+1][y] = 'O';
                        }
                    }
                }
            }
        };

        auto west = [&]()
        {
            bool moved = true;
            while (moved)
            {
                moved = false;
                for (size_t x = 1 ; x < MIRROR_SZ; x++)
                {
                    for (size_t y = 0; y < MIRROR_SZ; y++)
                    {
                        if (mirror[x][y] == 'O' && mirror[x-1][y] == '.')
                        {
                            moved = true;
                            mirror[x][y] = '.';
                            mirror[x-1][y] = 'O';
                        }
                    }
                }
            }
        };

        auto load = [&]() -> long
        {
            long load = 0;
            for (size_t x = 0; x < MIRROR_SZ; x++)
                for (size_t y = 0; y < MIRROR_SZ; y++)
                    if (mirror[x][y] == 'O')
                        load += (MIRROR_SZ - y);
            return load;
        };

        if (part == 'a')
        {
            north();
            return load();
        }
        else
        {
            // do one cycle first
            north();
            west();
            south();
            east();

            struct answer_t
            {
                size_t hash;
                long load;
            };

            /*
                strategy here is to hash each map (and store the load)
                and every time we encounter a new one, we'll see if we've
                seen it before. if so, we've found the start of the loop.
                note that this is O(n^2) to search the loop, but the real
                expense is in the constants, since n is ~175
            */
            std::vector<answer_t> hashes;
            hashes.push_back({hash(mirror), load()});

            size_t cycle = 0;
            size_t offset = 0;

            while (true)
            {
                north();
                west();
                south();
                east();

                size_t h = hash(mirror);
                long l = load();
                auto it = std::find_if(hashes.begin(), hashes.end(), [&](const answer_t& a) { return a.hash == h; });

                if (it != hashes.end())
                {
                    cycle = std::distance(it, hashes.end());
                    offset = std::distance(hashes.begin(), it);
                    hashes.push_back({h, l}); // just for pretty printing at the end
                    break;
                }
                hashes.push_back({h, l});
            }

#if 0
            for (size_t i = 0; i < hashes.size(); i++)
            {
                std::cout << i << " " << hashes[i].hash << " " << hashes[i].load << std::endl;
            }
            std::cout << "cycle: " << cycle << " offset: " << offset << std::endl;
#endif
            const long CYCLES = 1000000000 - 1; // zero based

            return hashes[(CYCLES - offset) % cycle + offset].load;
        }
    }
};







