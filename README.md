# Advent of Code 2019

This repository includes my C++ code/solutions for the [Advent of Code 2019](https://adventofcode.com/2019/about).

*If you want to solve the puzzles yourself first, do so, **then** look for the solution to compare.* Do **not** cheat yourself out of a great time and learning experience by just copying code!

**Please keep in mind most of this is hacked together. I don't claim it's perfect code or even in all cases 100% valid/defined/proper code or whatever. It's for fun and it got the job done in a short amount of time.** 😀

I'll update this repository now and then to include all my solutions whenever I've got time to properly comment and upload it.

This is mostly for those that are curious. If you want to look at the code, experiment with it, change it, etc. be my guest.

## Running the code

All my snippets are self-contained (with some minor exceptions) in their respective cpp files. All you have to do is pass them to your toolchain and let it build an executable file out of it:

```
g++ -o day-01 day-01.cpp
```

You may also use the included `CMakeLists.txt` file to create makefiles and/or project files for all days using the excellent [CMake](https://www.cmake.org/):

```
cmake -G "Unix Makefiles" path/to/source
```
```
cmake -G "Visual Studio 16 2019" -A x64 path/to/source
```

After this just use the generated files to build and/or run the snippets.

If you have happen to have any questions or problems, feel free to [create an issue](https://github.com/MarioLiebisch/Advent-of-Code-2019/issues) and ask away – or just check out the [Advent of Code Subreddit](https://www.reddit.com/r/adventofcode/).