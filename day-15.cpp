#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <stack>

// For simplicity, let's go WIN32 only to get console controls
#include <Windows.h>

// Yep, have to up the type size:
// 'long' on Windows x64 is still just 32 bit, but we need 64 bit
typedef long long value;

// Again this function is basically unchanged from day 7, except for the
// new op code (9) and new addressing mode (2).
void run(std::map<value, value> memory, std::function<value()> input, std::function<void(value)> output) {
    std::size_t ip = 0;
    std::size_t relative_base = 0;
    std::map<const std::size_t, std::function<void()>> codes;
    unsigned char mode1 = 0, mode2 = 0, mode3 = 0;

    const std::function<value & (const std::size_t, const unsigned char)> read = [&](const std::size_t address, const unsigned char mode) -> value& {
        switch (mode) {
        case 2: // relative
            return memory[relative_base + memory[address]];
        case 1: // immediate
            return memory[address];
        case 0: // positional
            return memory[memory[address]];
        default: // unhandled
            std::cerr << "Unhandled access mode: " << mode << "\n";
            throw "Unhandled access mode!";
            return memory[0];
        }
    };

    // add
    codes[1] = [&]() { read(ip + 3, mode3) = read(ip + 1, mode1) + read(ip + 2, mode2); ip += 4; };
    // mul
    codes[2] = [&]() { read(ip + 3, mode3) = read(ip + 1, mode1) * read(ip + 2, mode2); ip += 4; };
    // input
    codes[3] = [&]() { read(ip + 1, mode1) = input(); ip += 2; };
    // output
    codes[4] = [&]() { output(read(ip + 1, mode1)); ip += 2; };
    // jump if true
    codes[5] = [&]() { ip = read(ip + 1, mode1) ? read(ip + 2, mode2) : ip + 3; };
    // jump if false
    codes[6] = [&]() { ip = read(ip + 1, mode1) ? ip + 3 : read(ip + 2, mode2); };
    // less than
    codes[7] = [&]() { read(ip + 3, mode3) = read(ip + 1, mode1) < read(ip + 2, mode2); ip += 4; };
    // equals
    codes[8] = [&]() { read(ip + 3, mode3) = read(ip + 1, mode1) == read(ip + 2, mode2); ip += 4; };
    // adjust relative base
    codes[9] = [&]() { relative_base += read(ip + 1, mode1); ip += 2; };
    // halt
    codes[99] = [&]() { ip = -1; };

    while (ip != -1) {
        const std::size_t raw = memory[ip];
        const std::size_t op = raw % 100;
        const std::size_t modes = (raw / 100) % 1000;
        mode1 = modes % 10;
        mode2 = (modes / 10) % 10;
        mode3 = (modes / 100) % 100;
        const auto& ins = codes.find(op);

        if (ins != codes.end()) {
            ins->second();
        }
        else {
            std::cout << "Unknown op code: " << op << std::endl;
            ip = -1;
        }
    }
}

void gotoxy(SHORT x, SHORT y) {
    const static HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(output, { x, y });
}

int main(int argc, char** argv) {
    const std::vector<value> program = { 3,1033,1008,1033,1,1032,1005,1032,31,1008,1033,2,1032,1005,1032,58,1008,1033,3,1032,1005,1032,81,1008,1033,4,1032,1005,1032,104,99,1002,1034,1,1039,102,1,1036,1041,1001,1035,-1,1040,1008,1038,0,1043,102,-1,1043,1032,1,1037,1032,1042,1106,0,124,1002,1034,1,1039,101,0,1036,1041,1001,1035,1,1040,1008,1038,0,1043,1,1037,1038,1042,1105,1,124,1001,1034,-1,1039,1008,1036,0,1041,102,1,1035,1040,1002,1038,1,1043,1002,1037,1,1042,1106,0,124,1001,1034,1,1039,1008,1036,0,1041,1002,1035,1,1040,101,0,1038,1043,1002,1037,1,1042,1006,1039,217,1006,1040,217,1008,1039,40,1032,1005,1032,217,1008,1040,40,1032,1005,1032,217,1008,1039,9,1032,1006,1032,165,1008,1040,3,1032,1006,1032,165,1102,2,1,1044,1105,1,224,2,1041,1043,1032,1006,1032,179,1101,1,0,1044,1106,0,224,1,1041,1043,1032,1006,1032,217,1,1042,1043,1032,1001,1032,-1,1032,1002,1032,39,1032,1,1032,1039,1032,101,-1,1032,1032,101,252,1032,211,1007,0,29,1044,1105,1,224,1101,0,0,1044,1105,1,224,1006,1044,247,102,1,1039,1034,1002,1040,1,1035,1001,1041,0,1036,1002,1043,1,1038,102,1,1042,1037,4,1044,1106,0,0,19,27,41,9,17,87,2,1,91,14,15,99,17,13,40,13,7,33,23,28,7,21,75,15,41,83,18,4,28,1,21,99,3,2,4,60,16,5,16,22,59,18,37,21,62,96,11,63,46,16,27,76,7,36,38,28,53,18,84,52,12,47,25,93,10,57,64,21,41,75,52,9,80,60,21,86,60,21,70,21,13,72,78,22,61,17,28,54,51,93,18,3,87,21,4,98,17,59,2,17,18,71,5,20,16,39,66,18,7,62,15,37,25,52,27,17,15,10,48,11,39,18,20,68,83,22,36,9,3,69,56,64,21,39,93,1,90,18,57,52,14,41,32,57,5,7,72,18,35,66,21,22,88,2,31,52,7,35,25,50,14,35,7,11,92,38,14,66,3,28,84,18,17,48,15,34,40,4,21,92,52,27,5,4,53,65,59,24,88,24,66,88,85,26,8,26,10,64,99,9,44,38,14,26,74,75,24,31,7,6,62,9,57,75,18,22,52,57,15,3,87,21,39,24,12,8,70,8,19,3,89,16,36,15,36,16,30,28,8,89,12,99,98,16,78,24,11,63,87,55,51,19,57,18,28,9,90,15,95,56,57,1,93,77,24,36,14,44,46,25,66,37,23,8,12,10,58,27,66,4,72,1,2,16,91,16,66,26,24,53,25,20,41,8,75,23,2,20,91,19,3,12,32,30,3,33,85,17,21,92,17,1,12,73,9,34,12,85,42,5,69,67,4,87,70,6,49,96,12,5,37,62,54,72,13,52,14,21,84,68,54,22,78,11,93,12,90,55,7,19,44,21,98,4,46,50,27,30,2,99,27,35,8,5,62,1,91,65,12,80,16,17,81,14,73,60,69,24,23,13,74,57,10,26,21,80,60,10,79,3,9,37,77,73,16,10,3,13,95,4,91,65,11,86,16,24,71,22,6,63,90,56,15,64,8,25,46,77,71,24,13,72,96,22,8,15,79,39,19,19,47,14,16,92,69,73,23,76,23,28,60,84,14,54,62,11,8,30,75,44,16,4,30,82,14,80,11,1,70,85,10,14,73,70,9,54,25,26,12,51,23,86,92,18,11,19,74,55,51,10,73,7,13,43,89,5,55,2,18,82,2,14,63,71,28,7,94,61,10,51,8,53,63,22,39,19,79,20,99,2,66,22,7,68,71,17,19,45,10,14,42,99,9,9,13,75,84,14,83,75,19,92,22,47,4,83,18,46,91,22,61,28,6,71,17,10,1,81,6,60,83,21,14,13,71,11,68,73,52,10,25,30,91,6,25,86,89,19,39,18,95,1,52,23,91,20,14,41,91,26,59,16,85,99,4,15,96,51,19,25,51,73,3,48,79,14,14,41,5,17,59,8,51,43,21,15,47,3,28,53,12,22,23,2,94,74,23,53,20,20,98,21,14,46,61,26,6,55,20,69,28,6,41,19,70,48,6,9,32,32,28,20,21,62,22,38,7,90,3,32,24,92,49,23,72,63,17,18,89,85,33,28,23,27,5,42,52,7,54,18,17,21,63,98,8,9,84,31,24,80,70,22,51,28,61,77,6,25,68,66,8,47,22,7,44,26,37,15,28,68,23,18,18,14,34,3,85,99,31,41,53,28,20,43,90,22,13,70,27,27,17,35,48,11,92,4,60,84,4,38,27,25,89,99,74,2,31,63,13,50,1,54,4,59,3,59,2,54,15,37,19,74,45,75,7,84,19,96,72,75,9,34,18,52,23,99,11,45,81,53,7,71,24,80,26,31,11,74,27,57,0,0,21,21,1,10,1,0,0,0,0,0,0 };

    std::map<value, value> memory;
    std::size_t i = 0;
    // Copy the program to memory
    for (const value& byte : program) {
        memory[i++] = byte;
    }

    std::map<std::pair<int, int>, std::size_t> map;

    int gx = 0;
    int gy = 0;
    int x = 0;
    int y = 0;
    int minx = -1;
    int miny = -1;
    int maxx = 1;
    int maxy = 1;

    unsigned int last_code = 0;

    unsigned char dir = 1;

    std::stack<std::pair<int, int>> history;

#define NORTH 1
#define SOUTH 2
#define WEST 3
#define EAST 4

// Undefine to solve Star 1
#define STAR2
// Undefine for faster solving - show progress every n steps
#define PATHING_UPDATES 100

    std::size_t n = 0;

    const auto& draw_map = [&]{
        gotoxy(0, 0);
        for (int ty = miny; ty <= maxy; ++ty) {
            for (int tx = minx; tx <= maxx; ++tx) {
                if (tx == x && ty == y) {
                    std::cout << 'D';
                }
                else {
                    const auto& a = map.find(std::make_pair(tx, ty));
                    if (a == map.end()) {
                        std::cout << ' ';
                    }
                    else if (a->second == -1) {
                        std::cout << 'Û';
                    }
                    else {
                        std::cout << '°';
                    }
                }
            }
            std::cout << "\n";
        }
    };

    run(memory, [&]() -> value {
#ifndef STAR2
        // Are we done, exit?
        if (last_code == 2)
            return 0;
#endif

#ifdef PATHING_UPDATES
        if (n++ % PATHING_UPDATES == 0) {
            draw_map();
        }
#endif

        const bool know_n = map.find(std::make_pair(x, y - 1)) != map.end();
        const bool know_s = map.find(std::make_pair(x, y + 1)) != map.end();
        const bool know_w = map.find(std::make_pair(x - 1, y)) != map.end();
        const bool know_e = map.find(std::make_pair(x + 1, y)) != map.end();

        if (know_n && know_s && know_w && know_e) {
            history.pop();
            if (history.empty()) {
                // We're done!
                return 0;
            }
            const auto& last = history.top();
            if (last.second < y)
                return dir = NORTH;
            else if (last.second > y)
                return dir = SOUTH;
            else if (last.first < x)
                return dir = WEST;
            else
                return dir = EAST;
        }
        if (!know_n)
            return dir = NORTH;
        if (!know_s)
            return dir = SOUTH;
        if (!know_w)
            return dir = WEST;
        if (!know_e)
            return dir = EAST;
        return 0;
    }, [&](const value& value) {
        int dx = 0;
        int dy = 0;

        switch (dir) {
        case NORTH:
            dx = 0;
            dy = -1;
            break;
        case SOUTH:
            dx = 0;
            dy = 1;
            break;
        case WEST:
            dx = -1;
            dy = 0;
            break;
        case EAST:
            dx = 1;
            dy = 0;
            break;
        }

        if (x <= minx)
            minx = x - 1;
        if (y <= miny)
            miny = y - 1;
        if (x >= maxx)
            maxx = x + 1;
        if (y >= maxy)
            maxy = y + 1;

        const std::size_t prev = map[std::make_pair(x, y)];

        last_code = static_cast<unsigned int>(value);
        switch (value) {
        case 0: // Wall
            map[std::make_pair(x + dx, y + dy)] = -1;
            break;
#ifdef STAR2
        case 2: // Goal
            gx = x + dx;
            gy = y + dy;
#endif
        case 1: // Ok
        {
            x += dx;
            y += dy;
            const auto& a = map.find(std::make_pair(x, y));
            if (a == map.end()) {
                history.push(std::make_pair(x, y));
                map.insert(std::make_pair(std::make_pair(x, y), prev + 1));
            }
            break;
        }
#ifndef STAR2
        case 2: // Goal
            x += dx;
            y += dy;
            map[std::make_pair(x, y)] = prev;
            break;
#endif
        }
    });
    draw_map();

    // For oxygen "math" let's use a vector, it's a lot easier
    const std::size_t width = static_cast<std::size_t>(maxx) - minx;
    const std::size_t height = static_cast<std::size_t>(maxy) - miny;

    enum class Status : unsigned int {
        Empty = 0,
        Wall,
        Oxygen
    };

    std::vector<Status> smap(width * height);

    // Copy the whole map (ignoring the previously introduced border)
    for (int ty = miny; ty < maxy; ++ty) {
        for (int tx = minx; tx < maxx; ++tx) {
            const auto& t = map.find(std::make_pair(tx, ty));
            const std::size_t x = static_cast<std::size_t>(tx) - minx;
            const std::size_t y = static_cast<std::size_t>(ty) - miny;
            if (t == map.end()) {
                smap[y * width + x] = Status::Wall;
            }
            else {
                switch (t->second) {
                case -1:
                    smap[y * width + x] = Status::Wall;
                    break;
                default:
                    smap[y * width + x] = Status::Empty;
                    break;
                }
            }
        }
    }

#ifdef STAR2
    std::size_t empty_cells = 0;
    std::size_t minutes = 0;
    smap[(static_cast<std::size_t>(gy) - miny) * width + static_cast<std::size_t>(gx) - minx] = Status::Oxygen;

    // Repeat while we have empty cells
    do {
        empty_cells = 0;

        // Create a copy of the previous state for easier updates
        const std::vector<Status> smap_back(smap);

        // Iterate over the whole map
        for (std::size_t y = 0; y < height; ++y) {
            for (std::size_t x = 0; x < width; ++x) {
                // Calculate an index once
                const std::size_t index = y * width + x;
                // If this tile had oxygen
                if (smap_back[index] == Status::Oxygen) {
                    // tile to the top is possible and is still empty
                    if (y > 0 && smap_back[index - width] == Status::Empty)
                        smap[index - width] = Status::Oxygen;
                    // tile to the bottom is possible and is still empty
                    if (y < height - 1 && smap_back[index + width] == Status::Empty)
                        smap[index + width] = Status::Oxygen;
                    // tile to the left is possible and is still empty
                    if (x > 0 && smap_back[index - 1] == Status::Empty)
                        smap[index - 1] = Status::Oxygen;
                    // tile to the right is possible and is still empty
                    if (x < width - 1 && smap_back[index + 1] == Status::Empty)
                        smap[index + 1] = Status::Oxygen;
                }
            }
        }
        // Draw the map and count empty cells while doing so
        gotoxy(0, 0);
        for (std::size_t y = 0; y < height; ++y) {
            for (std::size_t x = 0; x < width; ++x) {
                switch (smap[y * width + x]) {
                case Status::Empty:
                    std::cout << '°';
                    // Count this empty cell
                    ++empty_cells;
                    break;
                case Status::Oxygen:
                    std::cout << '%';
                    break;
                case Status::Wall:
                    std::cout << 'Û';
                    break;
                }
            }
            std::cout << '\n';
        }
        // Output results (Star 2)
        std::cout << "\n\nAfter " << ++minutes << " minutes:\n\t" << empty_cells << " empty cells\n";
    } while (empty_cells);
#else
    std::cout << "\n\nSteps took: " << map[std::make_pair(x, y)] << "\n";
#endif
    return 0;
}
