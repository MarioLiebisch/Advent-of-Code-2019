#include <iostream>
#include <string>

// Let's just squeeze everything into a totally insecure class
class PasswordGenerator {
private:
    // The current combination
    unsigned char data[6];

public:
    // Setup a new  generator with a given starting combination
    PasswordGenerator(unsigned int combination) {
        // Subtract 1 from the starting combination since
        // we'll jump to the *next* valid combination, which
        // might be the first one after all
        --combination;

        // Move the single digits to our data array
        // 123456 is essentially stored as {6, 5, 4, 3, 2, 1}
        for (char i = 5; i >= 0; --i) {
            data[i] = combination % 10;
            combination /= 10;
        }

        // Find the next valid combination
        findNext();
    }

    // Find the next valid password
    void findNext() {
        // Did we find a doubled digit?
        bool doubled = false;
        do {
            // Increase the current value by 1
            // and apply any carry to the next digit
            for (char i = 5; i >= 0; --i) {
                if (data[i] = (data[i] + 1) % 10)
                    break;
            }

            // Haven't found a pair of digits  yet
            doubled = false;
            // Count how many identical digits in sequence
            // (always at least one, obviously)
            unsigned char seq = 1;
            // Go through all digits (skipping the first, since
            // it never has anything to its left)
            for (char i = 1; i < 6; ++i) {
                // While at it, make sure the digits never decrease
                // (as defined)
                if (data[i] < data[i - 1]) {
                    data[i] = data[i - 1];
                }
                // Count identical digits next to each other
                if (data[i] == data[i - 1]) {
                    ++seq;
                // If we didn't have identical ones now, see if there
                // were exactly two (star 2)
                } else {
                    if (seq == 2) {
                        doubled = true;
                        // Might be tempted to just `break` here,
                        // but there might be more digits to adjust...
                    }
                    // Reset to a single digit again
                    seq = 1;
                }
            }
            // See if the last two digits are identical and a sequence of 2
            if (seq == 2)
                doubled = true;
        // Continue until we've found a doubled pair of digits (i.e. a valid one)
        } while(!doubled);
    }

    // Retrieve the current (valid) combination
    // and return it after finding the next one
    operator const unsigned int() {
        unsigned int res = 0;
        // Move the data to a regular integer to be returned
        for (char i = 0; i < 6; res = res * 10 + data[i], ++i);
        // Find the next valid password
        findNext();
        return res;
    }
};

const unsigned int from = 134564;
const unsigned int to = 585159;

int main(int argc, char **argv) {
    // Start the password generator with our starting value
    PasswordGenerator pass(from);
    unsigned int current;

    // Count the number of valid passwords found
    std::size_t n = 0;
    for(;;) {
        // Retrieve the current (i.e. next) password
        current = pass;
        // If the retrieved password is outside our given distance,
        // we've got an over- or underflow wrapping around, so stop.
        if (current < from || current > to)
            break;
        // Increase the count by 1 and output the found password
        std::cout << ++n << ": " << current << "\n";
    }
    
    return 0;
}
