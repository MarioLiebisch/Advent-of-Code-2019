#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

std::vector<long> modules;

// Small helper function to load module weights/inputs
void readModules() {
    std::ifstream moddata("modules.txt");
    std::string line;
    // No need to check for the stream/file
    // If something is wrong, std::getline() returns false
    while (std::getline(moddata, line)) {
        modules.push_back(std::stol(line));
    }

    // Just in case the file doesn't exist or is empty
    if (modules.empty()) {
        std::cout << "Oops! We have no modules!?\n";
    }
}

// Calculate the fuel costs according to the task
long calculateFuel(const long mass) {
    return std::max(0l, mass / 3 - 2);
}

int main(int argc, char **argv) {
    // Load module weights
    readModules();

    // The fuel required for the modules (star 1)
    long fuel_modules = 0;

    // The fuel required for the modules and the extra fuel (star 2)
    long fuel_total = 0;

    // For each module's mass...
    for (long mass : modules) {
        // Calculate the fuel required
        long fuel = calculateFuel(mass);

        // Update the fuel required for modules
        fuel_modules += fuel;

        // Consider it as added weight
        long added = fuel;

        // As long as we add more fuel...
        while (added > 0) {
            // Calculate the extra fuel needed
            added = calculateFuel(added);
            // And add it up
            fuel += added;
        }

        // Update the fuel required in total
        // (this now includes extra fuel)
        fuel_total += fuel;
    }

    // Output our results (for star 1 and star 2)
    std::cout << "Fuel required for modules: " << fuel_modules << "\n";
    std::cout << "Fuel required in total: " << fuel_total << "\n";
    return 0;
}
