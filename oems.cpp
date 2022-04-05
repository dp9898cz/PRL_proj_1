// Project: Odd Even Merge Sort using OpenMPI (PRL)
// Author:  Daniel Pátek
// Date:    2022-04-03

#define DEBUG 1

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

const string NUMBERS_FILENAME = "numbers";

/**
 * @brief Load numbers vector from a file "numbers"
 *
 * @return vector<int>
 */
vector<int> loadNumbers() {
    vector<int> numbers;
    char byte;
    ifstream file;

    file.open(NUMBERS_FILENAME, ifstream::in);
    if (!file.is_open()) {
        cerr << "Could not open the file: '" << NUMBERS_FILENAME << "'" << endl;
        return numbers;
    }
    while (file.get(byte)) {
        // get one value and normalize then onto 0-255 interval
        numbers.push_back((int)byte < 0 ? 127 - (int)byte : (int)byte);
    }
    if (0) {
        cout << "Loaded numbers: ";
        for (int i : numbers) {
            cout << i << ' ';
        }
        cout << endl;
    }
    file.close();
    return numbers;
}

/**
 * @brief Prints loaded numbers with space between each number.
 *
 * @param numbers Vector of numbers loaded from file.
 */
void printUnorderedNumbers(vector<int> numbers) {
    bool f = true;
    for (int i : numbers) {
        cout << (f ? "" : " ") << i;
        f = false;
    }
    cout << endl << flush;
}

int main(int argc, char *argv[]) {
    int processRank;  // number of process

    // initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);

    vector<int> numbers;  // numbers imported from file

    // import numbers if root process running
    if (processRank == 0) {
        numbers = loadNumbers();
        if (numbers.size() <= 0) return EXIT_FAILURE;
        printUnorderedNumbers(numbers);
    }

    MPI_Finalize();
}