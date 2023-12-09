# AOC 2023

This is my code for the [Advent of Code 2023 challenge](https://adventofcode.com/2023).

I'm not really making any effort for this code to be crazy efficient. I lean heavily on the
STL, and am focusing more on code clarity and correctness than being super cute with language
features. I'm Just trying to get the answers right and get my stars.

I'm testing this with gcc 13 and cmake 3.20.

* It'll probably work with a lot of other compilers. 
* I'm using the C++20 flag, but it's unlikely I'll use any of those features.

## To build:
```
mkdir build
cd build
cmake ../src  # -DCMAKE_CXX_COMPILER=/usr/bin/clang++ if you wanna
make
```

This generates the `aoc-2023` executable in the build directory. Run it from there, as it will look to `../data` for input files.
Run-time checking on data files is minimal-to-nonexistent. Command line help is available from the executable.

## For an optimized build:
```
mkdir bfast
cd bfast
cmake -DCMAKE_BUILD_TYPE=Release ../src
make
```

Command line help is available from the executable.

