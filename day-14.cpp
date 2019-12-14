#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

typedef long long value;
typedef std::pair<value, std::string> Charge;
typedef std::vector<Charge> Recipe;

template<typename T>
T common_denominator(const T a, const T b) {
	if (a == 0)
		return b;
	return common_denominator(b % a, a);
}

template<typename T>
T common_multiple(const T a, const T b) {
	return a * b / common_denominator(a, b);
}


int main(int argc, char** argv) {
	std::ifstream stream("ore-to-fuel.txt");
	std::string line;

	std::map<Charge, Recipe> recipes;

	// Read all the recipes from the text file
	while (std::getline(stream, line)) {
		// Find the  separator
		std::size_t o = line.find(" => ");
		// Shouldn't happen, but if there isn't one...
		if (o == std::string::npos)
			continue;
		// For simplicity, save both sides
		const std::string inputs = line.substr(0, o);
		const std::string output = line.substr(o + 4);
		o = output.find(' ');
		if (o == std::string::npos)
			continue;
		// Create the current recipe identified by the output and get a reference
		std::vector<Charge> &recipe = recipes[std::make_pair(std::stoul(output), output.substr(o + 1))];
		o = 0;
		// For all products
		for (;;) {
			// Find the next offset (if any)
			const std::size_t no = inputs.find(", ", o);
			const std::string current = inputs.substr(o, no - o);
			// Find separating space
			o = current.find(' ');
			if (o == std::string::npos)
				continue;
			// Create and add the new charge/product to the recipe
			recipe.push_back(std::make_pair(std::stoul(current), current.substr(o + 1)));
			// No next one? break
			if (no == std::string::npos)
				break;
			// Skip the ", "
			o = no + 2;
		}
	}

	std::map<const std::string, value> inventory;

#define STAR2
#ifdef STAR2
	std::size_t fuel_produced = 0;
	// To speed up Star 2, try to create lots of fuel at once
	// This will create some overproduction at first, but should be neglectable
	for (value inc : {100000, 10000, 1000, 100, 10, 1}) {
		// Save the current inventory in case we overflow the capacity
		auto old_inventory = inventory;
		for (;;) {
#else
	// Only produce one unit of fuel
	const value inc = 1;
#endif
			// Set the intended fuel increment as the production
			inventory["FUEL"] = inc;
			bool done = false;
			do {
				// Iterate over all inventory elements
				for (auto& e : inventory) {
					const std::string product = e.first;
					value& amount = e.second;

					// If this position is empty or over-produced (i.e. negative), or ore, skip it
					if (amount < 1 || product == "ORE")
						continue;

#ifndef STAR2
					std::cout << "Need: " << amount << " " << product << "\n";
#endif

					bool found = false;
					// Go over all recipes and find the one producing this inventory position
					for (auto& r : recipes) {
						if (r.first.second == product) {
							const value out_count = r.first.first;
							// Determine the number of runs to do
							const value req_runs = static_cast<const value>(std::ceil(static_cast<double>(amount) / out_count));

#ifndef STAR2
							std::cout << "\t- " << req_runs << " runs for " << (out_count * req_runs) << " " << product << ":\n";
#endif

							// Subtract the total production
							amount -= req_runs * out_count;

							// Go over all requirements and add them
							for (auto& c : r.second) {
#ifndef STAR2
								std::cout << "\t\t" << c.first << " " << c.second << "\n";
#endif
								inventory[c.second] += c.first * req_runs;
							}

							// We found what produces this
							found = true;
							break;
						}
					}
					// Somehow couldn't find a matching recipe?
					if (!found) {
						std::cerr << "Don't know how to make '" << product << "'!\n";
						return 0;
					}
				}
				done = true;
				// Iterate once more over the whole inventory and determine if anything
				// other than ore is missing. If so, we're not done yet.
				for (auto& e : inventory) {
					if (e.first != "ORE" && e.second > 0) {
						done = false;
						break;
					}
				}
			} while (!done);

#ifndef STAR2
			// Result (star 1)
			std::cout << "Required ore: " << inventory["ORE"] << "\n";
#else
			// If we haven't consumed all the ore
			if (inventory["ORE"] <= 1000000000000u) {
				// Add the produced fuel
				fuel_produced += inc;
				// Backup the current inventory in case of an overflow
				old_inventory = inventory;
			}
			// Overflow
			else {
				// Restore the previous inventory, which will be usesd
				// with the next smaller incremental value.
				inventory = old_inventory;
				break;
			}
		}
	}
	// Result (star 2)
	std::cout << "Total fuel produced: " << fuel_produced << "\n";
#endif
	return 0;
}
