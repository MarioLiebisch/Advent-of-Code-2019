#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <algorithm>

// Thread-safe queue from SO
#include "safequeue.h"

typedef long value;

// This function is basically unchanged from day 5, but now it takes two
// functions to define the input and output routine rather than simply
// accessing std::cin and std::cout directly.
void run (std::vector<value> memory, std::function<value()> input, std::function<void(value)> output) {
    std::size_t ip = 0;
    std::map<const std::size_t, std::function<void()>> codes;
    unsigned char mode1 = 0, mode2 = 0, mode3 = 0;

    const std::function<value&(const std::size_t, const unsigned char)> read = [&](const std::size_t address, const unsigned char mode) -> value& { return mode == 1 ? memory[address] : memory[memory[address]]; };

    // add
    codes[1]  = [&]() { read(ip + 3, mode3) = read(ip + 1, mode1) + read(ip + 2, mode2); ip += 4; };
    // mul
    codes[2]  = [&]() { read(ip + 3, mode3) = read(ip + 1, mode1) * read(ip + 2, mode2); ip += 4; };
    // input
    codes[3]  = [&]() { read(ip + 1, mode1) = input(); ip += 2; };
    // output
    codes[4]  = [&]() { output(read(ip + 1, mode1)); ip += 2; };
    // jump if true
    codes[5]  = [&]() { ip = read(ip + 1, mode1) ? read(ip + 2, mode2) : ip + 3; };
    // jump if false
    codes[6]  = [&]() { ip = read(ip + 1, mode1) ? ip + 3 : read(ip + 2, mode2); };
    // less than
    codes[7]  = [&]() { read(ip + 3, mode3) = read(ip + 1, mode1) < read(ip + 2, mode2); ip += 4; };
    // equals
    codes[8]  = [&]() { read(ip + 3, mode3) = read(ip + 1, mode1) == read(ip + 2, mode2); ip += 4; };
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

// Helper function to simulate one thruster run
value simulateAmplifiers(const std::vector<value> &program, value sequence[5]) {
    std::cout << "{" << sequence[0] << ", " << sequence[1] << ", " << sequence[2] << ", " << sequence[3] << ", " << sequence[4] << "} = ";

    // This uses a multi-threaded approach since.
    // Each thruster runs in a separate thread,
    // waiting for inputs from other threads.

    SafeQueue<value> results[5];

    // First, push the input sequence as "results".
    // These will be read as the first inputs, saving us
    // from having to add any extra logic.
    for (int i = 0; i < 5; ++i)
        results[i].enqueue(sequence[i]);
    // Queue the original input for the first thruster,
    // which is alwas 0.
    results[0].enqueue(0);
    
    // One thread for each thruster, essentially running
    // the same code.
    std::thread amp1(run, program, [&]() -> value {
            // Retrieve the oldest queued value
            return results[0].dequeue();
        }, [&](value output) {
            // Push a new value to the next amplifier
            results[1].enqueue(output);
        });
    std::thread amp2(run, program, [&]() -> value {
            return results[1].dequeue();
        }, [&](value output) {
            results[2].enqueue(output);
        });
    std::thread amp3(run, program, [&]() -> value {
            return results[2].dequeue();
        }, [&](value output) {
            results[3].enqueue(output);
        });
    std::thread amp4(run, program, [&]() -> value {
            return results[3].dequeue();
        }, [&](value output) {
            results[4].enqueue(output);
        });
    std::thread amp5(run, program, [&]() -> value {
            return results[4].dequeue();
        }, [&](value output) {
            results[0].enqueue(output);
        });

    // Wait for all threads (i.e. programs) to finish
    amp1.join();
    amp2.join();
    amp3.join();
    amp4.join();
    amp5.join();

    // If everything went right, we should have only one
    // remaining result in the first queue.
    value result = results[0].empty() ? 0 : results[0].dequeue();
    std::cout << result << "\n";
    return result;
}

int main(int argc, char **argv) {
    const std::vector<value> program = { 3,8,1001,8,10,8,105,1,0,0,21,38,47,72,97,122,203,284,365,446,99999,3,9,1001,9,3,9,1002,9,5,9,1001,9,4,9,4,9,99,3,9,102,3,9,9,4,9,99,3,9,1001,9,2,9,102,5,9,9,101,3,9,9,1002,9,5,9,101,4,9,9,4,9,99,3,9,101,5,9,9,1002,9,3,9,101,2,9,9,102,3,9,9,1001,9,2,9,4,9,99,3,9,101,3,9,9,102,2,9,9,1001,9,4,9,1002,9,2,9,101,2,9,9,4,9,99,3,9,1001,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,102,2,9,9,4,9,3,9,1001,9,1,9,4,9,3,9,102,2,9,9,4,9,3,9,101,2,9,9,4,9,3,9,1001,9,1,9,4,9,3,9,101,2,9,9,4,9,3,9,101,1,9,9,4,9,3,9,1001,9,2,9,4,9,99,3,9,1001,9,1,9,4,9,3,9,101,1,9,9,4,9,3,9,101,2,9,9,4,9,3,9,102,2,9,9,4,9,3,9,101,2,9,9,4,9,3,9,101,1,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,1,9,9,4,9,3,9,102,2,9,9,4,9,3,9,102,2,9,9,4,9,99,3,9,1001,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,1001,9,2,9,4,9,3,9,102,2,9,9,4,9,3,9,102,2,9,9,4,9,3,9,101,2,9,9,4,9,3,9,1001,9,1,9,4,9,3,9,101,1,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,102,2,9,9,4,9,99,3,9,101,1,9,9,4,9,3,9,101,1,9,9,4,9,3,9,102,2,9,9,4,9,3,9,102,2,9,9,4,9,3,9,101,2,9,9,4,9,3,9,101,2,9,9,4,9,3,9,1001,9,2,9,4,9,3,9,1001,9,2,9,4,9,3,9,102,2,9,9,4,9,3,9,1001,9,1,9,4,9,99,3,9,101,2,9,9,4,9,3,9,101,2,9,9,4,9,3,9,1001,9,2,9,4,9,3,9,102,2,9,9,4,9,3,9,102,2,9,9,4,9,3,9,101,1,9,9,4,9,3,9,1002,9,2,9,4,9,3,9,1002,9,2,9,4,9,3,9,101,2,9,9,4,9,3,9,1001,9,2,9,4,9,99 };
    value sequence[5] = { 5,6,7,8,9 };
    value maximum = 0;

    // 5! combinations for 5 elements, i.e. 120
    const long total = 120;
    // Input is always 5 elements long
    const long length = 5;

    // Iterate over all possible combinations
    for (long i = 0; i < total; ++i) {
        // Get the result for this sequence
        const value result = simulateAmplifiers(program, sequence);

        // Determine if it's a new maximum
        if (maximum < result)
            maximum = result;

        // Get the next permutation
        std::next_permutation(sequence, sequence + 5);
    }
    std::cout << "Maximum thrust: " << maximum << "\n";
    return 0;
}
