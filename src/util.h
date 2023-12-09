#pragma once

#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

struct readers
{
    // this is to read a file that is newline delimited POD types into a container of type T
    // uses boost::lexical_cast to convert to the inner type
    template <typename T>
    static void read_by_line(const std::string& filename, T& result)
    {
        std::ifstream infile(filename);
        std::string line;
        while (std::getline(infile, line))
        {
            result.push_back(boost::lexical_cast<typename T::value_type>(line));
        }
    }

    // this is to read a file that is newline delimited, but each line isn't a simple POD type
    // pass in a functor that does the parsing and returns the object inside the container
    template <typename T, typename F>
    static void read_by_line(const std::string& filename, F functor, T& result)
    {
        std::ifstream infile(filename);
        std::string line;
        while (std::getline(infile, line))
        {
            result.push_back(functor(line));
        }
    }

    // this was created for the monkey parser (2022 day 11) but maybe it'll be useful some other day
    template <typename T, typename F>
    static void read_by_n_lines(const std::string& filename, int n, F functor, T& result)
    {
        std::ifstream infile(filename);
        while (infile.peek() != EOF)
        {
            std::vector<std::string> lines;
            for (auto i = 0; i < n; i++)
            {
                std::string line;
                std::getline(infile, line);
                lines.push_back(line);
            }
            result.push_back(functor(lines));
        }
    }

    template <typename T>
    struct lexi_caster
    {
        T operator()(const std::string& str) { return boost::lexical_cast<T>(str); }
    };

    // this is to read a file that is a single line of PODs, delimited by spaces, into a container of type T
    template <typename T>
    static void read_delimited_line(const std::string& filename, T& result)
    {
        std::ifstream infile(filename);
        std::string line;
        std::getline(infile, line);

        boost::char_separator<char> sep(" ");
        boost::tokenizer<boost::char_separator<char>> tokens(line, sep);
        std::transform(tokens.begin(), tokens.end(), std::back_inserter(result), lexi_caster<typename T::value_type>());
    }

    // this is to read a file that is many lines of PODs, delimited by spaces, into a container of type T
    template <typename T>
    static void read_delimited_lines(const std::string& filename, T& result)
    {
        std::ifstream infile(filename);
        std::string line;
        while (std::getline(infile, line))
        {
            boost::char_separator<char> sep(" ");
            boost::tokenizer<boost::char_separator<char>> tokens(line, sep);
            typename T::value_type next;
            std::transform(tokens.begin(), tokens.end(), std::back_inserter(next), lexi_caster<typename T::value_type::value_type>());
            result.push_back(next);
        }
    }

    struct digit_parser
    {
        uint8_t operator()(char c) { return c - '0'; }
    };

    // this is for 2-dimensional character matrices
    template <typename T, typename F>
    static void read_dense_2d_matrix(const std::string& filename, F functor, T& result)
    {
        std::ifstream infile(filename);
        std::string line;
        size_t x = 0, y = 0;
        while (std::getline(infile, line))
        {
            for (x = 0; x < line.length(); x++)
            {
                result[x][y] = functor(line[x]);
            }
            y++;
        }
    }
};

struct str
{
    static const char* ws;

    // trim from end of string (right)
    static inline std::string& rtrim(std::string& s, const char* t = ws)
    {
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }

    // trim from beginning of string (left)
    static inline std::string& ltrim(std::string& s, const char* t = ws)
    {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }

    // trim from both ends of string (right then left)
    static inline std::string& trim(std::string& s, const char* t = ws)
    {
        return ltrim(rtrim(s, t), t);
    }

    // split a list of strings by a string of delimiters
    static inline std::vector<std::string> split(const std::string& s, const std::string& splitters)
    {
        std::vector<std::string> v;
        boost::split(v, s, boost::is_any_of(splitters), boost::algorithm::token_compress_on);
        return v;
    }

};
