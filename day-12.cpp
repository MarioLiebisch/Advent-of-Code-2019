#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

class Vector {
public:
	int x, y, z;
	Vector(int x, int y, int z) : x(x), y(y), z(z) {}

	Vector operator+(const Vector& other) const {
		Vector result(*this);
		result.x += other.x;
		result.y += other.y;
		result.z += other.z;
		return result;
	}

	Vector &operator+=(const Vector& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	bool operator==(const Vector& other) const {
		return x == other.x && y == other.y && z == other.z;
	}
};

class Moon {
public:
	Vector position;
	Vector velocity;

	Moon(const Vector position) : position(position), velocity(0, 0, 0) {}

	bool operator==(const Moon& other) const {
		return position == other.position && velocity == other.velocity;
	}
};

template<typename T> T sign(T value) {
	if (value == 0)
		return 0;
	if (value < 0)
		return -1;
	return 1;
}

long long common_denominator(const long long a, const long long b) {
	if (a == 0)
		return b;
	return common_denominator(b % a, a);
}

long long common_multiple(const long long a, const long long b) {
	return a * b / common_denominator(a, b);
}

int main(int argc, char** argv) {
	std::vector<Moon> moons;

	// Inputs
#if 0 // Test set 1
	moons.push_back(Moon{ { -1, 0, 2 } });
	moons.push_back(Moon{ { 2, -10, -7 } });
	moons.push_back(Moon{ { 4, -8, 8 } });
	moons.push_back(Moon{ { 3, 5, -1 } });
#elif 0 // Test set 2
	moons.push_back(Moon{ { -8, -10, 0 } });
	moons.push_back(Moon{ { 5, 5, 10 } });
	moons.push_back(Moon{ { 2, -7, 3 } });
	moons.push_back(Moon{ { 9, -8, -3 } });
#else // Personal puzzle input
	moons.push_back(Moon{ { 14, 9, 14 } });
	moons.push_back(Moon{ { 9, 11, 6 } });
	moons.push_back(Moon{ { -6, 14, -4 } });
	moons.push_back(Moon{ { 4, -4, -3 } });
#endif

	const long long steps = 1000;

	// Iterate over all moons to print their stats
	std::cout << "After 0 steps:\n";
	for (std::size_t i = 0; i < moons.size(); ++i) {
		std::cout << "pos=<x=" << std::setw(3) << moons[i].position.x << ", y=" << std::setw(3) << moons[i].position.y << ", z=" << std::setw(3) << moons[i].position.z << ">, "
			"vel=<x=" << std::setw(2) << moons[i].velocity.x << ", y=" << std::setw(2) << moons[i].velocity.y << ", z=" << std::setw(2) << moons[i].velocity.z << ">\n";
	}

	for (long long step = 0; step < steps; ++step) {
		// Iterate over all moons to update velocities
		for (std::size_t i = 0; i < moons.size(); ++i) {
			// Iterate over the remaining moons to get all pairs once
			for (std::size_t j = i + 1; j < moons.size(); ++j) {
				// Get the difference in position and get the directional change
				// as -1/0/1 based on the difference.
				const int dir_x = sign(moons[j].position.x - moons[i].position.x);
				const int dir_y = sign(moons[j].position.y - moons[i].position.y);
				const int dir_z = sign(moons[j].position.z - moons[i].position.z);
				// Modify the first moon's velocity
				moons[i].velocity.x += dir_x;
				moons[i].velocity.y += dir_y;
				moons[i].velocity.z += dir_z;
				// Modify the second moon's velocity (opposite)
				moons[j].velocity.x -= dir_x;
				moons[j].velocity.y -= dir_y;
				moons[j].velocity.z -= dir_z;
			}
		}

		// Iterate over all moons to update positions
		for (std::size_t i = 0; i < moons.size(); ++i) {
			moons[i].position += moons[i].velocity;
		}

		// Start printing results every 10 steps
		if ((step + 1) % 10 == 0) {
			std::cout << "\nAfter " << (step + 1) << " steps:\n";
			// Iterate over all moons to print their stats
			for (std::size_t i = 0; i < moons.size(); ++i) {
				std::cout << "pos=<x=" << std::setw(3) << moons[i].position.x << ", y=" << std::setw(3) << moons[i].position.y << ", z=" << std::setw(3) << moons[i].position.z << ">, "
					"vel=<x=" << std::setw(2) << moons[i].velocity.x << ", y=" << std::setw(2) << moons[i].velocity.y << ", z=" << std::setw(2) << moons[i].velocity.z << ">\n";
			}
		}
	}

	std::cout << "\nEnergy after " << steps << " steps:\n";

	std::size_t total = 0;

	// Iterate over all moons
	for (std::size_t i = 0; i < moons.size(); ++i) {
		const std::size_t p1 = std::abs(moons[i].position.x);
		const std::size_t p2 = std::abs(moons[i].position.y);
		const std::size_t p3 = std::abs(moons[i].position.z);
		const std::size_t pot = p1 + p2 + p3;
		const std::size_t k1 = std::abs(moons[i].velocity.x);
		const std::size_t k2 = std::abs(moons[i].velocity.y);
		const std::size_t k3 = std::abs(moons[i].velocity.z);
		const std::size_t kin = k1 + k2 + k3;
		const std::size_t energy = pot * kin;
		std::cout << "pot: " << std::setw(2) << p1 << " + " << std::setw(2) << p2 << " + " << std::setw(2) << p3 << " = " << std::setw(2) << pot << "; ";
		std::cout << "kin: " << std::setw(2) << k1 << " + " << std::setw(2) << k2 << " + " << std::setw(2) << k3 << " = " << std::setw(2) << kin << "; ";
		std::cout << "total: " << pot << " * " << kin << " = " << energy << "\n";
		total += energy;
	}
	// Print total energy (Star 1)
	std::cout << "Sum of total energy: " << total << "\n";

	// Save the goal state (i.e. current state)
	std::vector<Moon> goals = moons;

	// Now simulate until the original state is reached once more
	bool done = false;
	std::cout << "Looking for periods...\n";
	std::size_t n = 0;

	long long period_x = 0;
	long long period_y = 0;
	long long period_z = 0;

	while (!done) {
		// Same simulation code as above
		for (std::size_t i = 0; i < moons.size(); ++i) {
			for (std::size_t j = i + 1; j < moons.size(); ++j) {
				const int dir_x = sign(moons[j].position.x - moons[i].position.x);
				const int dir_y = sign(moons[j].position.y - moons[i].position.y);
				const int dir_z = sign(moons[j].position.z - moons[i].position.z);
				moons[i].velocity.x += dir_x;
				moons[i].velocity.y += dir_y;
				moons[i].velocity.z += dir_z;
				moons[j].velocity.x -= dir_x;
				moons[j].velocity.y -= dir_y;
				moons[j].velocity.z -= dir_z;
			}
		}
		for (std::size_t i = 0; i < moons.size(); ++i) {
			moons[i].position += moons[i].velocity;
		}

		++n;

		if (period_x == 0) {
			bool period = true;
			for (std::size_t i = 0; i < moons.size(); ++i) {
				period &= moons[i].position.x == goals[i].position.x && moons[i].velocity.x == goals[i].velocity.x;
			}
			if (period)
				period_x = n;
		}

		if (period_y == 0) {
			bool period = true;
			for (std::size_t i = 0; i < moons.size(); ++i) {
				period &= moons[i].position.y == goals[i].position.y && moons[i].velocity.y == goals[i].velocity.y;
			}
			if (period)
				period_y = n;
		}

		if (period_z == 0) {
			bool period = true;
			for (std::size_t i = 0; i < moons.size(); ++i) {
				period &= moons[i].position.z == goals[i].position.z && moons[i].velocity.z == goals[i].velocity.z;
			}
			if (period)
				period_z = n;
		}

		done = period_x && period_y && period_z;
	}

	std::cout << "\nPeriod for X coordinate: " << period_x << "\n";
	std::cout << "Period for Y coordinate: " << period_y << "\n";
	std::cout << "Period for Z coordinate: " << period_z << "\n";

	// Output the result (Star 2)
	std::cout << "\nPeriod for system: " << common_multiple(common_multiple(period_x, period_y), period_z) << "\n";

	return 0;
}
