#include "week2.h"
#include "util.h"

#include <unordered_set>
#include <iostream>
#include <numeric>

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

    const size_t MAZE_SZ = 140;
    typedef std::array<std::array<char, MAZE_SZ>, MAZE_SZ> maze_t;

    struct char_in_line
    {
        char operator()(char c) { return c; }
    };

    struct coord_t
    {
        size_t x, y;
    };

    bool operator == (const coord_t& left, const coord_t& right)
    {
        return left.x == right.x && left.y == right.y;
    }

    // find the first outbound flow from coord and return it - for starting pos
    coord_t find_out_flow(const maze_t& maze, const coord_t& s)
    {
        if (s.x < MAZE_SZ - 1 && (maze[s.x+1][s.y] == '-' || maze[s.x+1][s.y] == 'J' || maze[s.x+1][s.y] == '7')) // east
            return {s.x+1, s.y};
        else if (s.x > 0 && (maze[s.x-1][s.y] == '-' || maze[s.x-1][s.y] == 'L' || maze[s.x-1][s.y] == 'F')) // west
            return {s.x-1, s.y};
        else if (s.y < MAZE_SZ - 1 && (maze[s.x][s.y+1] == '|' || maze[s.x][s.y+1] == '7' || maze[s.x][s.y+1] == 'F')) // north
            return {s.x, s.y+1};
        else if (s.y > 0 && (maze[s.x][s.y-1] == '|' || maze[s.x][s.y-1] == 'L' || maze[s.x][s.y-1] == 'J')) // south
            return {s.x, s.y-1};
        throw std::logic_error("bug in find_out_flow");
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

    long day10a()
    {
        const std::string filename{"../data/day10.dat"};
        maze_t maze;
        readers::read_dense_2d_matrix(filename, char_in_line(), maze);

        coord_t start;
        for (size_t x = 0; x < MAZE_SZ; x++)
            for (size_t y = 0; y < MAZE_SZ; y++)
                if (maze[x][y] == 'S')
                {
                    start = {x, y};
                    goto found;
                }
found:
        long count = 1;
        coord_t curr = find_out_flow(maze, start);
        coord_t prev = start;
        while (curr != start)
        {
            coord_t next = find_next(maze, prev, curr);
            prev = curr;
            curr = next;
            count++;
        }

        return count / 2;
    }
};
