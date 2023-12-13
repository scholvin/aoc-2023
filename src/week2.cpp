#include "week2.h"
#include "util.h"

#include <unordered_set>
#include <iostream>
#include <numeric>
#include <cmath>

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

    struct day12_t
    {
        std::string record;
        std::vector<size_t> groups;

        day12_t(const std::string& line)
        {
            std::vector<std::string> parts = str::split(line, " ");
            record = parts[0];
            parts = str::split(parts[1], ",");
            for (auto p: parts)
                groups.push_back(boost::lexical_cast<size_t>(p));
        }

        bool verify(const std::string& candidate) const
        {
            size_t cur_grp = 0;
            std::vector<size_t> cand_groups;
            for (auto c: candidate)
            {
                if (cur_grp == 0 && c == '.')
                    continue;
                else if (cur_grp > 0 && c == '.')
                {
                    cand_groups.push_back(cur_grp);
                    cur_grp = 0;
                }
                else if (c == '#')
                    cur_grp++;
            }
            if (candidate[candidate.size()-1] == '#')
                cand_groups.push_back(cur_grp);
            return cand_groups == groups;
        }

        // this is slow and won't work for part b, it's O(2^n) where n is the number of ?'s
        long possible() const
        {
            std::vector<size_t> bitmap; // maps bit position to the index of a ? in the record
            for (size_t i = 0; i < record.size(); i++)
                if (record[i] == '?')
                    bitmap.push_back(i);

            long r = 0;
            for (size_t counter = 0; counter < (1u << bitmap.size()); counter++)
            {
                std::string t = record;
                for (size_t i = 0; i < bitmap.size(); i++)
                {
                    t[bitmap[i]] = counter & (1u << i) ? '#' : '.';
                }
                if (verify(t))
                    r++;
            }
            return r;
        }
    };

    long day12a()
    {
        std::ifstream infile("../data/day12.dat");
        std::string line;
        long sum = 0;
        while (std::getline(infile, line))
        {
            day12_t today(line);
            sum += today.possible();
        }

        return sum;
    }
};
