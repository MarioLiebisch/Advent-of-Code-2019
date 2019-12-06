#include <iostream>
#include <string>
#include <set>
#include <map>
#include <fstream>

#if 0
void readModules() {
    std::ifstream moddata("modules.txt");
    std::string line;

    while (std::getline(moddata, line)) {
        modules.push_back(std::stol(line));
    }

    if (modules.empty()) {
        std::cout << "Oops! We have no modules!?\n";
    }
}
#endif

// This map holds all objects by name and their orbiters
std::map<const std::string, std::set<std::string>> orbits;
// This map contains all possible routes
std::map<const std::string, std::set<std::string>> routes;
// One more map to map out distances
std::map<const std::string, std::size_t> distances;

// Helper function to recursively count orbits
const std::size_t countOrbits(const std::set<std::string> &orbiters) {
    // First the count of orbiters
    std::size_t count = orbiters.size();
    // Then iterate over all orbiters and get
    // indirect orbit counts
    for (const std::string& name : orbiters) {
        count += countOrbits(orbits[name]);
    }
    // Return the result
    return count;
}

// Helper for mapping distances
// Since we don't care for the actual path, we can just map distances recursively
void mapPath(const std::string &current, const std::string &to, const std::size_t distance = 0) {
    // We're at the start/first node
    if (distance == 0) {
        // Clear all distances
        distances.clear();
        // Cheating here, since all distances are 0 by default
        distances[current] = 1;
    }
    // Current node already has a distance, but the old one is shorter (and not 0)
    else if (distances[current] && distances[current] < distance) {
        // Done here
        return;
    }
    // All other cases
    else
    {
        // Set the current node's distance
        distances[current] = distance;
    }

    // Is this the target? Then we're done
    if (current == to) {
        return;
    }

    // Update all adjacent nodes with a distance 1 greater
    for (const auto& name : routes[current]) {
        mapPath(name, to, distance + 1);
    }
}

int main(int argc, char** argv) {
    std::ifstream orbitdata("orbits.txt");
    std::string line;

    // Read all lines in the file
    while (std::getline(orbitdata, line)) {
        // Find the ')' separator
        const std::size_t o = line.find_first_of(')');
        // No separator for whatever reason? skip
        if (o == std::string::npos) {
            continue;
        }
        // Now get the two names
        const std::string parent = line.substr(0, o);
        const std::string child = line.substr(o + 1);
        // And save it
        orbits[parent].insert(child);
    }

    // Now let's iterate over all our objects and count orbits
    std::size_t count = 0;
    for (const auto& obj : orbits) {
        count += countOrbits(obj.second);
    }

    // Output the orbit count (star 1)
    std::cout << "Orbit count: " << count << "\n";

    // Since star 2 requires a path finding algorithm that may work
    // in both directions, we're creating a new map that has both
    // directions (no longer only from parent to orbiters)

    // First copy the existing orbits – these are all routes in
    // one direction.
    routes.insert(orbits.begin(), orbits.end());

    // Iterate over all objects to create the backwards routes
    for (const auto& obj : orbits) {
        // Iterate over all orbiters
        // obj.first is the name
        // obj.second the list of orbiters
        for (const std::string& child : obj.second) {
            // Save the back route
            routes[child].insert(obj.first);
        }
    }

    // Map the path recursively
    mapPath("YOU", "SAN");

    // Now all elements in `distances` have their corresponding shortest
    // distance from the start, so we'll just have to print out the one
    // for SAN. However, since we only want to get into the same orbit,
    // we'll save two jumps in the calculation (from YOU to its orbit and
    // from SAN's orbit to SAN).
    std::cout << "Shortest route from 'YOU' to 'SAN': " << (distances["SAN"] - 2) << "\n";
    return 0;
}
