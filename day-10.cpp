#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cmath>
#include <set>

// Asteroid class (essentially a Point again)
struct Asteroid {
	int x;
	int y;

	// Operator overloads needed by the code below
	bool operator==(const Asteroid& other) const {
		return x == other.x && y == other.y;
	}

	bool operator<(const Asteroid& other) const {
		if (y < other.y)
			return true;
		if (y > other.y)
			return false;
		return x < other.x;
	}
};

// Helper to quickly print Asteroids to a stream
std::ostream& operator <<(std::ostream& stream, const Asteroid& asteroid) {
	stream << "(" << asteroid.x << ", " << asteroid.y << ")";
	return stream;
}

// Map of asteroids, built from the text input
class Map : public std::set<Asteroid> {
public:
	Map(const std::string &data) {
		int x = 0;
		int y = 0;
		// Iterate over all characters
		for (const char &c : data) {
			// Start a new line, reset x and increase y
			if (c == '\n') {
				x = 0;
				++y;
			}
			// Add a new asteroid
			else if (c == '#') {
				insert(Asteroid{ x++, y });
			}
			// Otherwise do nothing
			else {
				++x;
			}
		}
	}
};

int main(int argc, char** argv) {
	// Read the star map from a file into a string
	std::ifstream stream("starmap.txt");
	std::string line;
	std::string data;
	while (std::getline(stream, line)) {
		data += line + "\n";
	}

	// Create the map
	Map map(data);
	std::size_t best = 0;
	Asteroid besta {0, 0};
	// Iterate over all asteroids as base positions
	for (const auto& a : map) {
		// Store the angles at which we've seen other asteroids
		std::set<double> seen;
		// Iterate over all asteroids again
		for (const auto& b : map) {
			// Skip testing the current base itself
			if (a == b)
				continue;
			// Now just insert the current angle as a known angle
			// If one asteroid hides another, they'll both have the
			// very same angle, saving us tons of logic and checks
			seen.insert(atan2(b.x - a.x, b.y - a.y));
		}
		// Now we can just count the number of different angles to
		// determine the number of visible asteroids.
		const std::size_t count = seen.size();
		if (count > best) {
			best = count;
			besta = a;
		}
	}

	// Print the result (Star 1)
	std::cout << "Best " << besta << ": " << best << " visible asteroids\n\n";

	// We can only blast one asteroid per angle.
	// Tracking them with this map.
	std::map<double, Asteroid> targeting;
	std::size_t n = 0;

	// PI used to normalize angles to [0..3.14159]
	const double PI = atan2(0, -1);
	// As long as there are asteroids left...
	while (map.size() > 1) {
		for (const auto& a : map) {
			// Can't blast the station itself
			if (a == besta) {
				continue;
			}
			// Determine the angle from station to asteroid (0 is straight up)
			// Negating the result to get the intended orientation.
			double angle = -std::atan2(besta.x - a.x, besta.y - a.y);

			// We want to start at 0 degree, so wrap negative angles around
			if (angle < 0)
				angle += 2 * PI;

			// Test to see if we get a closer target at this angle
			const auto& test = targeting.find(angle);
			// If nothing has been found...
			if (test == targeting.end()) {
				// Add this asteroid
				targeting.insert(std::make_pair(angle, a));
			}
			// Use Pythagoras to determine whether the new target is closer (skippingthe square root)
			else if ((besta.x - a.x) * (besta.x - a.x) + (besta.y - a.y) * (besta.y - a.y) <
				(besta.x - test->second.x) * (besta.x - test->second.x) + (besta.y - test->second.y) * (besta.y - test->second.y)) {
				test->second = a;
			}
		}
		// Blast them
		for (const auto& t : targeting) {
			// We only want to print a specific subset for texting and the result (Star 2):
			const static std::set<std::size_t> prints{1, 2, 3, 10, 20, 50, 100, 199, 200, 201, 299};
			// Only print if the current asteroid is in the list
			if (prints.find(++n) != prints.end()) {
				std::cout << "Target " << n << " at " << t.second << "\n";
			}
			// Actually blast it off the map
			map.erase(t.second);
		}
		// Clear targets
		targeting.clear();
	}

	return 0;
}
