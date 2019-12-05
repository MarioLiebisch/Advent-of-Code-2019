#include <iostream>
#include <functional>
#include <map>
#include <vector>

// The actual program code or "memory content"
// This can be replaced with inputs from the task directly
const std::vector<size_t> org_memory = {1,0,0,3,1,1,2,3,1,3,4,3,1,5,0,3,2,1,10,19,2,9,19,23,2,23,10,27,1,6,27,31,1,31,6,35,2,35,10,39,1,39,5,43,2,6,43,47,2,47,10,51,1,51,6,55,1,55,6,59,1,9,59,63,1,63,9,67,1,67,6,71,2,71,13,75,1,75,5,79,1,79,9,83,2,6,83,87,1,87,5,91,2,6,91,95,1,95,9,99,2,6,99,103,1,5,103,107,1,6,107,111,1,111,10,115,2,115,13,119,1,119,6,123,1,123,2,127,1,127,5,0,99,2,14,0,0};

// The actual "interpreter" or emulated CPU
const std::size_t run (const std::size_t noun, const std::size_t verb, std::vector<size_t> memory) {
    // The Instruction Pointer stores the address/offset of the current instruction
    std::size_t ip = 0;
    // Mapping functions to their op codes makes things a lot more convenient later on
    std::map<const std::size_t, std::function<void()>> codes;

    // Addition
    codes[1] = [&]() { memory[memory[ip + 3]] = memory[memory[ip + 1]] + memory[memory[ip + 2]]; ip += 4; };
    // Multiplication
    codes[2] = [&]() { memory[memory[ip + 3]] = memory[memory[ip + 1]] * memory[memory[ip + 2]]; ip += 4; };
    // Halt
    codes[99] = [&]() { ip = -1; };

    // Set current state
    memory[1] = noun;
    memory[2] = verb;

    // As long as our instruction pointer is valid...
    while (ip != -1) {
        // Get the current op code
        const std::size_t op = memory[ip];
        // Try to find amatching instruction
        const auto& ins = codes.find(op);

        // If something is found...
        if (ins != codes.end()) {
            // Execute the instruction
            ins->second();
        }
        else {
            // Spit out an error otherwise
            std::cout << "Unknown op code: " << op << std::endl;
            // And stop the program
            ip = -1;
        }
    }
    // Return the result
    return memory[0];
}

int main(int argc, char **argv) {
    // Our goal to find
    const std::size_t target = 19690720;

    // Try all possible nouns
    for (size_t noun = 0; noun < 100; ++noun) {
        // Try all possible verbs
        for (size_t verb = 0; verb < 100; ++verb) {
            // Get the result for this combination
            const std::size_t result = run(noun, verb, org_memory);
            // Is the result our target?
            if (result == target) {
                // Print the solution and end
                std::cout << "Solution: " << (noun * 100 + verb) << "\n";
                return 0;
            }
        }
    }

    // Should never get here...
    std::cout << "Couldn't find any solution!\n";
    return 0;
}
