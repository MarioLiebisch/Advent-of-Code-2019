#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <algorithm>

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

int main(int argc, char** argv) {
    const std::vector<value> program = { 3,8,1005,8,321,1106,0,11,0,0,0,104,1,104,0,3,8,102,-1,8,10,1001,10,1,10,4,10,1008,8,1,10,4,10,1002,8,1,29,3,8,1002,8,-1,10,101,1,10,10,4,10,108,0,8,10,4,10,1002,8,1,50,3,8,102,-1,8,10,1001,10,1,10,4,10,1008,8,0,10,4,10,1001,8,0,73,1,1105,16,10,2,1004,8,10,3,8,1002,8,-1,10,1001,10,1,10,4,10,1008,8,0,10,4,10,1002,8,1,103,1006,0,18,1,105,14,10,3,8,102,-1,8,10,101,1,10,10,4,10,108,0,8,10,4,10,102,1,8,131,1006,0,85,1,1008,0,10,1006,0,55,2,104,4,10,3,8,102,-1,8,10,1001,10,1,10,4,10,1008,8,1,10,4,10,1001,8,0,168,2,1101,1,10,1006,0,14,3,8,102,-1,8,10,101,1,10,10,4,10,108,1,8,10,4,10,102,1,8,196,1006,0,87,1006,0,9,1,102,20,10,3,8,1002,8,-1,10,101,1,10,10,4,10,108,1,8,10,4,10,1001,8,0,228,3,8,1002,8,-1,10,101,1,10,10,4,10,108,0,8,10,4,10,1002,8,1,250,2,5,0,10,2,1009,9,10,2,107,17,10,1006,0,42,3,8,102,-1,8,10,101,1,10,10,4,10,108,1,8,10,4,10,1001,8,0,287,2,102,8,10,1006,0,73,1006,0,88,1006,0,21,101,1,9,9,1007,9,925,10,1005,10,15,99,109,643,104,0,104,1,21102,1,387353256856,1,21101,0,338,0,1105,1,442,21101,936332866452,0,1,21101,349,0,0,1105,1,442,3,10,104,0,104,1,3,10,104,0,104,0,3,10,104,0,104,1,3,10,104,0,104,1,3,10,104,0,104,0,3,10,104,0,104,1,21101,0,179357024347,1,21101,0,396,0,1105,1,442,21102,1,29166144659,1,21102,407,1,0,1105,1,442,3,10,104,0,104,0,3,10,104,0,104,0,21102,1,718170641252,1,21102,430,1,0,1106,0,442,21101,825012151040,0,1,21102,441,1,0,1106,0,442,99,109,2,21202,-1,1,1,21102,1,40,2,21102,1,473,3,21102,463,1,0,1105,1,506,109,-2,2106,0,0,0,1,0,0,1,109,2,3,10,204,-1,1001,468,469,484,4,0,1001,468,1,468,108,4,468,10,1006,10,500,1102,1,0,468,109,-2,2105,1,0,0,109,4,1202,-1,1,505,1207,-3,0,10,1006,10,523,21101,0,0,-3,22101,0,-3,1,21202,-2,1,2,21102,1,1,3,21102,1,542,0,1105,1,547,109,-4,2106,0,0,109,5,1207,-3,1,10,1006,10,570,2207,-4,-2,10,1006,10,570,22102,1,-4,-4,1105,1,638,22102,1,-4,1,21201,-3,-1,2,21202,-2,2,3,21101,0,589,0,1106,0,547,22102,1,1,-4,21101,1,0,-1,2207,-4,-2,10,1006,10,608,21102,0,1,-1,22202,-2,-1,-2,2107,0,-3,10,1006,10,630,21202,-1,1,1,21102,630,1,0,105,1,505,21202,-2,-1,-2,22201,-4,-2,-4,109,-5,2106,0,0 };

    // Since memory should now be "much larger" than the program, we'll
    // just use a std::map, so we only access/simulate memory that's
    // actually used.
    std::map<value, value> memory;
    std::size_t i = 0;
    // Copy the program to memory
    for (const value& byte : program) {
        memory[i++] = byte;
    }

    // Robot's possible directions and current orientation
    enum class Direction : unsigned char {
        Up = 0,
        Right,
        Down,
        Left,
        Count
    } dir = Direction::Up;

    // Colors to draw
    enum class Color : unsigned char {
        Black = 0,
        White = 1
    };

    // Robot's current mode for output values (i.e. what do they mean?)
    enum class OutputMode : unsigned char {
        Paint = 0,
        Turn,
        Count
    } mode = OutputMode::Paint;

    for (unsigned char start = 0; start < 2; ++start) {
        // The panels, Color based on position
        std::map<std::tuple<int, int>, Color> panels;

        // The robot's position
        int x = 0, y = 0;

        if (start == 1) {
            // For Star 2, start on a white panel
            panels.insert(std::make_pair(std::make_pair(0, 0), Color::White));
        }

        // Running as usual
        run(memory, [&]() -> value {
            const auto& tmp = panels.find(std::make_pair(x, y));
            // Panel isn't painted yet (defaults to Black)
            if (tmp == panels.end())
                return static_cast<value>(Color::Black);
            // Return panel's color
            return static_cast<value>(tmp->second);
            }, [&](value output) {
                switch (mode) {
                case OutputMode::Paint:
                    // std::cout << "Paint (" << x << ", " << y << "): " << output << "\n";
                    // Paint the panel
                    panels[std::make_tuple(x, y)] = static_cast<Color>(output);
                    break;
                case OutputMode::Turn:
                    // Turn left
                    if (output == 0) {
                        dir = static_cast<Direction>((static_cast<unsigned char>(Direction::Count) + static_cast<unsigned char>(dir) - 1) % static_cast<int>(Direction::Count));
                    }
                    // Turn right
                    else {
                        dir = static_cast<Direction>((static_cast<unsigned char>(dir) + 1) % static_cast<int>(Direction::Count));
                    }
                    // std::cout << "Moving in direction " << output << "\n";
                    switch (dir) {
                    case Direction::Up:
                        --y;
                        break;
                    case Direction::Down:
                        ++y;
                        break;
                    case Direction::Left:
                        --x;
                        break;
                    case Direction::Right:
                        ++x;
                        break;
                    }
                    break;
                }
                // Switching to the next mode
                mode = static_cast<OutputMode>((static_cast<unsigned char>(mode) + 1) % static_cast<unsigned char>(OutputMode::Count));
            });

        if (start == 0) {
            // Conveniently, to count the panels that got painted
            // all we have to do is count entries in our map (Star 1)
            std::cout << "Panels painted: " << panels.size() << "\n\n";
        }
        else if (start == 1) {
            // Determine the extends of the drawn panels
            int minx = 0;
            int miny = 0;
            int maxx = 0;
            int maxy = 0;
            for (const auto& p : panels) {
                const int x = std::get<0>(p.first);
                const int y = std::get<1>(p.first);
                if (x < minx)
                    minx = x;
                if (y < miny)
                    miny = y;
                if (x > maxx)
                    maxx = x;
                if (y > maxy)
                    maxy = y;
            }

            // Draw the panels with an extra border
            for (int y = miny - 1; y <= maxy; ++y) {
                for (int x = minx - 1; x <= maxx; ++x) {
                    const auto& p = panels.find(std::make_pair(x, y));
                    // Unpainted panel
                    if (p == panels.end()) {
                        std::cout << ' ';
                    }
                    // Painted panel
                    else {
                        switch (p->second) {
                        case Color::White:
                            std::cout << 'Û';
                            break;
                        default:
                            std::cout << ' ';
                            break;
                        }
                    }
                }
                std::cout << '\n';
            }
        }
    }
    return 0;
}
