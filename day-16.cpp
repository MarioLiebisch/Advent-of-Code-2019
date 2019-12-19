#include <iostream>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define abs(a) ((a) < 0 ? -1 * (a) : (a))

int main(int argc, char** argv) {

	constexpr const char input[] = "59756772370948995765943195844952640015210703313486295362653878290009098923609769261473534009395188480864325959786470084762607666312503091505466258796062230652769633818282653497853018108281567627899722548602257463608530331299936274116326038606007040084159138769832784921878333830514041948066594667152593945159170816779820264758715101494739244533095696039336070510975612190417391067896410262310835830006544632083421447385542256916141256383813360662952845638955872442636455511906111157861890394133454959320174572270568292972621253460895625862616228998147301670850340831993043617316938748361984714845874270986989103792418940945322846146634931990046966552";
	const std::size_t phases = 100;

	// Determine/store input's length
	constexpr const std::size_t length = sizeof(input) / sizeof(char) - 1;

	long long data[length];
	long long ndata[length];
	const long long pattern[] = { 0, 1, 0, -1 };
	const std::size_t pattern_length = 4;

	// Star 1
	// Transfer/translate input into our data array
	for (std::size_t i = 0; i < length; ++i) {
		data[i] = static_cast<long long>(input[i]) - '0';
	}

	// Iterate over all phases
	for (std::size_t n = 0; n < phases; ++n) {
		// Iterate over all elements
		for (std::size_t i = 0; i < length; ++i) {
			ndata[i] = 0;
			// Iterate over all elements
			for (std::size_t j = 0; j < length; ++j) {
				// Add the current element with the given pattern/modifier
				ndata[i] += data[j] * pattern[(pattern_length + (1 + j) / (i + 1)) % pattern_length];
			}
		}

		// Move new data to old data
		for (std::size_t i = 0; i < length; ++i) {
			data[i] = abs(ndata[i]) % 10;
		}

		// Star 1
		// Output first 8 "digits"
		std::cout << "After " << (n + 1) << " phases: ";
		for (std::size_t i = 0; i < min(8, length); ++i) {
			std::cout << data[i];
		}
		std::cout << "\n";
	}

	std::cout << "\n---\n\n";

	// Star 2

	// Determine the result's offset
	constexpr const std::size_t final_offset =	(static_cast<std::size_t>(input[0]) - '0') * 1000000 +
												(static_cast<std::size_t>(input[1]) - '0') *  100000 +
												(static_cast<std::size_t>(input[2]) - '0') *   10000 +
												(static_cast<std::size_t>(input[3]) - '0') *    1000 +
												(static_cast<std::size_t>(input[4]) - '0') *     100 +
												(static_cast<std::size_t>(input[5]) - '0') *      10 +
												(static_cast<std::size_t>(input[6]) - '0');
	constexpr const std::size_t total_length = 10000 * length;

	constexpr const std::size_t used_length = total_length - final_offset;

	static_assert(final_offset >= total_length / 2 && final_offset <= total_length - 8, "final_offset outside defined range");

	// This time allocate heap data
	long long* data2 = new long long[used_length];

	// Transfer/translate input into our data array (by looping through the original input)
	for (std::size_t i = 0; i < used_length; ++i) {
		data2[i] = static_cast<long long>(input[(final_offset + i) % length]) - '0';
	}

	// Iterate over all phases
	for (std::size_t n = 0; n < phases; ++n) {
		// Iterate over all elements
		for (std::size_t i = 0; i < used_length - 1; ++i) {
			// f(x) = (f_prev(x) + f(x + 1)) modulo 10
			data2[used_length - i - 2] = abs(data2[used_length - i - 2] + data2[used_length - i - 1]) % 10;
		}

		// Star 2
		// Output first 8 "digits"
		std::cout << "After " << (n + 1) << " phases: ";
		for (std::size_t i = 0; i < min(8, used_length); ++i) {
			std::cout << data2[i];
		}
		std::cout << "\n";
	}

	delete[] data2;

	return 0;
}
