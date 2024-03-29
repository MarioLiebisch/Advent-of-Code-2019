#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <set>

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
    const std::vector<value> program = { 1,330,331,332,109,2952,1101,1182,0,16,1101,1467,0,24,102,1,0,570,1006,570,36,1002,571,1,0,1001,570,-1,570,1001,24,1,24,1106,0,18,1008,571,0,571,1001,16,1,16,1008,16,1467,570,1006,570,14,21101,0,58,0,1105,1,786,1006,332,62,99,21102,1,333,1,21101,73,0,0,1106,0,579,1101,0,0,572,1101,0,0,573,3,574,101,1,573,573,1007,574,65,570,1005,570,151,107,67,574,570,1005,570,151,1001,574,-64,574,1002,574,-1,574,1001,572,1,572,1007,572,11,570,1006,570,165,101,1182,572,127,101,0,574,0,3,574,101,1,573,573,1008,574,10,570,1005,570,189,1008,574,44,570,1006,570,158,1105,1,81,21101,340,0,1,1105,1,177,21101,477,0,1,1106,0,177,21101,514,0,1,21101,0,176,0,1105,1,579,99,21102,1,184,0,1106,0,579,4,574,104,10,99,1007,573,22,570,1006,570,165,101,0,572,1182,21101,375,0,1,21101,211,0,0,1106,0,579,21101,1182,11,1,21101,222,0,0,1105,1,979,21101,0,388,1,21102,233,1,0,1106,0,579,21101,1182,22,1,21101,0,244,0,1106,0,979,21101,401,0,1,21102,1,255,0,1106,0,579,21101,1182,33,1,21101,266,0,0,1105,1,979,21101,414,0,1,21102,1,277,0,1106,0,579,3,575,1008,575,89,570,1008,575,121,575,1,575,570,575,3,574,1008,574,10,570,1006,570,291,104,10,21101,1182,0,1,21101,313,0,0,1105,1,622,1005,575,327,1101,0,1,575,21102,327,1,0,1106,0,786,4,438,99,0,1,1,6,77,97,105,110,58,10,33,10,69,120,112,101,99,116,101,100,32,102,117,110,99,116,105,111,110,32,110,97,109,101,32,98,117,116,32,103,111,116,58,32,0,12,70,117,110,99,116,105,111,110,32,65,58,10,12,70,117,110,99,116,105,111,110,32,66,58,10,12,70,117,110,99,116,105,111,110,32,67,58,10,23,67,111,110,116,105,110,117,111,117,115,32,118,105,100,101,111,32,102,101,101,100,63,10,0,37,10,69,120,112,101,99,116,101,100,32,82,44,32,76,44,32,111,114,32,100,105,115,116,97,110,99,101,32,98,117,116,32,103,111,116,58,32,36,10,69,120,112,101,99,116,101,100,32,99,111,109,109,97,32,111,114,32,110,101,119,108,105,110,101,32,98,117,116,32,103,111,116,58,32,43,10,68,101,102,105,110,105,116,105,111,110,115,32,109,97,121,32,98,101,32,97,116,32,109,111,115,116,32,50,48,32,99,104,97,114,97,99,116,101,114,115,33,10,94,62,118,60,0,1,0,-1,-1,0,1,0,0,0,0,0,0,1,20,14,0,109,4,2102,1,-3,586,21001,0,0,-1,22101,1,-3,-3,21102,1,0,-2,2208,-2,-1,570,1005,570,617,2201,-3,-2,609,4,0,21201,-2,1,-2,1105,1,597,109,-4,2105,1,0,109,5,2101,0,-4,629,21001,0,0,-2,22101,1,-4,-4,21101,0,0,-3,2208,-3,-2,570,1005,570,781,2201,-4,-3,652,21002,0,1,-1,1208,-1,-4,570,1005,570,709,1208,-1,-5,570,1005,570,734,1207,-1,0,570,1005,570,759,1206,-1,774,1001,578,562,684,1,0,576,576,1001,578,566,692,1,0,577,577,21101,702,0,0,1105,1,786,21201,-1,-1,-1,1106,0,676,1001,578,1,578,1008,578,4,570,1006,570,724,1001,578,-4,578,21101,0,731,0,1105,1,786,1106,0,774,1001,578,-1,578,1008,578,-1,570,1006,570,749,1001,578,4,578,21102,1,756,0,1105,1,786,1105,1,774,21202,-1,-11,1,22101,1182,1,1,21101,0,774,0,1105,1,622,21201,-3,1,-3,1106,0,640,109,-5,2106,0,0,109,7,1005,575,802,20102,1,576,-6,20101,0,577,-5,1106,0,814,21101,0,0,-1,21102,1,0,-5,21101,0,0,-6,20208,-6,576,-2,208,-5,577,570,22002,570,-2,-2,21202,-5,45,-3,22201,-6,-3,-3,22101,1467,-3,-3,1202,-3,1,843,1005,0,863,21202,-2,42,-4,22101,46,-4,-4,1206,-2,924,21102,1,1,-1,1106,0,924,1205,-2,873,21101,0,35,-4,1105,1,924,1201,-3,0,878,1008,0,1,570,1006,570,916,1001,374,1,374,1201,-3,0,895,1102,1,2,0,1201,-3,0,902,1001,438,0,438,2202,-6,-5,570,1,570,374,570,1,570,438,438,1001,578,558,921,21001,0,0,-4,1006,575,959,204,-4,22101,1,-6,-6,1208,-6,45,570,1006,570,814,104,10,22101,1,-5,-5,1208,-5,33,570,1006,570,810,104,10,1206,-1,974,99,1206,-1,974,1101,0,1,575,21101,973,0,0,1106,0,786,99,109,-7,2105,1,0,109,6,21101,0,0,-4,21102,1,0,-3,203,-2,22101,1,-3,-3,21208,-2,82,-1,1205,-1,1030,21208,-2,76,-1,1205,-1,1037,21207,-2,48,-1,1205,-1,1124,22107,57,-2,-1,1205,-1,1124,21201,-2,-48,-2,1106,0,1041,21102,-4,1,-2,1106,0,1041,21102,1,-5,-2,21201,-4,1,-4,21207,-4,11,-1,1206,-1,1138,2201,-5,-4,1059,1202,-2,1,0,203,-2,22101,1,-3,-3,21207,-2,48,-1,1205,-1,1107,22107,57,-2,-1,1205,-1,1107,21201,-2,-48,-2,2201,-5,-4,1090,20102,10,0,-1,22201,-2,-1,-2,2201,-5,-4,1103,1201,-2,0,0,1106,0,1060,21208,-2,10,-1,1205,-1,1162,21208,-2,44,-1,1206,-1,1131,1105,1,989,21101,0,439,1,1105,1,1150,21102,477,1,1,1106,0,1150,21102,1,514,1,21102,1149,1,0,1106,0,579,99,21102,1157,1,0,1105,1,579,204,-2,104,10,99,21207,-3,22,-1,1206,-1,1138,1202,-5,1,1176,2102,1,-4,0,109,-6,2105,1,0,10,11,34,1,9,1,34,1,9,1,7,9,18,1,9,1,7,1,7,1,18,1,9,1,7,1,7,1,18,1,9,1,7,1,7,1,18,1,9,1,7,1,7,1,18,1,9,1,7,1,7,1,18,9,1,13,3,1,26,1,9,1,3,1,3,1,20,11,5,1,1,9,18,1,5,1,3,1,5,1,1,1,1,1,3,1,1,1,18,1,5,1,3,1,5,1,1,1,1,1,3,1,1,1,18,1,5,1,3,1,5,1,1,1,1,1,3,1,1,1,18,1,5,1,1,9,1,1,1,1,3,9,12,1,5,1,3,1,7,1,1,1,5,1,5,1,10,9,3,1,1,9,5,1,5,1,10,1,1,1,9,1,1,1,5,1,7,1,5,14,9,9,7,1,5,2,9,1,13,1,13,1,5,2,7,9,7,1,13,1,5,2,7,1,1,1,5,1,7,1,19,2,7,1,1,1,5,1,7,1,19,2,7,1,1,1,5,1,7,1,19,2,7,1,1,13,1,1,7,14,7,1,7,1,5,1,1,1,7,1,12,1,7,1,7,1,5,1,1,1,7,1,12,1,7,1,7,1,5,1,1,1,7,1,12,9,7,9,7,1,34,1,9,1,34,1,9,1,34,1,9,1,34,11,12 };

    std::map<value, value> memory;
    std::size_t i = 0;
    // Copy the program to memory
    for (const value& byte : program) {
        memory[i++] = byte;
    }

    const std::size_t width = 45;
    const std::size_t height = 33;

    std::size_t x = 0;
    std::size_t y = 0;

    std::vector<char> map(width * height);

    std::map<std::size_t, std::set<std::pair<std::size_t, std::size_t>>> hscaffolds;
    std::map<std::size_t, std::set<std::pair<std::size_t, std::size_t>>> vscaffolds;

    std::size_t n = 0;

    // Star 1

    run(memory, [&]() -> value {
        return 0;
        }, [&](const value& value) {
            if (value == 10) {
                ++n;
                y = (y + 1) % height;
                x = 0;
            }
            else {
                map[y * width + x++] = static_cast<char>(value);
            }
        });

    std::size_t calibration = 0;
    for (std::size_t y = 0; y < height; ++y) {
        for (std::size_t x = 0; x < width; ++x) {
            // Current offset
            const std::size_t o = y * width + x;
            // Current cell/pixel
            char& c = map[o];

            // Check for intersection
            // Only if this is a scaffold
            if (c == '#' &&
                // Intersections can't be on the edges
                x > 1 && y > 1 && x < width - 1 && y < height - 1 &&
                // Check all four adjacent cells
                map[o - width] == '#' &&
                map[o + width] == '#' &&
                map[o + 1] == '#' &&
                map[o - 1] == '#'
                ) {
                // Mark as intersection
                c = 'O';
                // Add to the calibration value as defined
                calibration += x * y;
            }

            std::cout << c;
        }
        std::cout << "\n";
    }

    // Output the result (star 1)
    std::cout << "Calibration: " << calibration << "\n\n";

    // Star 2

    i = 0;
    // Copy the program to memory
    for (const value& byte : program) {
        memory[i++] = byte;
    }
    memory[0] = 2;

    // Would be fun to figure out programmatically, but can't be bothered due to lack of time
    std::size_t ci = 0;
    char commands[] =
        // main movement routine
        "A,B,B,A,C,A,A,C,B,C\n"
        // A
        "R,8,L,12,R,8\n"
        // B
        "R,12,L,8,R,10\n"
        // C
        "R,8,L,8,L,8,R,8,R,10\n"
        // Video feed?
        "n\n"
        ;
    const std::size_t nc = sizeof(commands);

    const auto& printMap = [=] {
        for (std::size_t y = 0; y < height; ++y) {
            for (std::size_t x = 0; x < width; ++x) {
                std::cout << map[y * width + x];
            }
            std::cout << '\n';
        }
        std::cout << '\n';
    };

    std::size_t last_value = 0;
    run(memory, [&]() -> value {
        return commands[ci++];
        }, [&](const value& value) {
            // Lazy here: Let's just get all values
            // Very last one will be the amount of dust
            last_value = value;
        });

    // Star 2
    std::cout << "Dust collected: " << last_value << std::endl;
    return 0;
}
