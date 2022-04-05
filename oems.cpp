// Project: Odd Even Merge Sort using OpenMPI (PRL)
// Author:  Daniel Pátek
// Date:    2022-03-26

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
    file.close();
    return numbers;
}

/**
 * @brief Prints loaded numbers with space between each number.
 *
 * @param numbers Vector of numbers loaded from file.
 */
void printNumbers(vector<int> numbers, bool isSorted) {
    bool f = true;
    for (int i : numbers) {
        cout << (f ? "" : (isSorted ? "\n" : " ")) << i;
        f = false;
    }
    cout << endl << flush;
}

/**
 * @brief Send numbers from one processor using MPI.
 *
 * @param min minimum number
 * @param max maximum number
 * @param addressL Number of processor where to send lower value.
 * @param addressH Number of processor where to send higher value.
 * @param tagL Tag of lower value.
 * @param tagH Tag of higher value.
 */
void send(int min, int max, int addressL, int addressH, int tagL, int tagH) {
    MPI_Send(&max, 1, MPI_INT, addressH, tagH, MPI_COMM_WORLD);
    MPI_Send(&min, 1, MPI_INT, addressL, tagL, MPI_COMM_WORLD);
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
        // check if the vector is not empty
        if (numbers.size() <= 0) return EXIT_FAILURE;
        printNumbers(numbers, false);  // false - unordered

        // initial sending numbers to the 1x1 relays
        for (int i = 0; i < numbers.size(); i += 2) {
            MPI_Send(&(numbers[i]), 1, MPI_INT, i / 2, 0, MPI_COMM_WORLD);
            MPI_Send(&(numbers[i + 1]), 1, MPI_INT, i / 2, 1, MPI_COMM_WORLD);
        }
    }

    // wait for two inputs to come
    int input[2];
    MPI_Recv(&input[0], 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, nullptr);
    MPI_Recv(&input[1], 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, nullptr);

    int min = input[0] < input[1] ? input[0] : input[1];
    int max = input[0] > input[1] ? input[0] : input[1];

    // 1x1 network
    if (processRank == 0) send(min, max, 4, 5, 0, 0);  // min max L H tL tH
    if (processRank == 1) send(min, max, 4, 5, 1, 1);
    if (processRank == 2) send(min, max, 6, 7, 0, 0);
    if (processRank == 3) send(min, max, 6, 7, 1, 1);

    // 2x2 network
    if (processRank == 4) send(min, max, 10, 8, 0, 0);
    if (processRank == 5) send(min, max, 8, 13, 1, 0);
    if (processRank == 6) send(min, max, 10, 9, 1, 0);
    if (processRank == 7) send(min, max, 9, 13, 1, 1);
    if (processRank == 8) send(min, max, 12, 11, 0, 0);
    if (processRank == 9) send(min, max, 12, 11, 1, 1);

    // 4x4 network
    if (processRank == 10) send(min, max, 0, 14, 0, 0);
    if (processRank == 11) send(min, max, 14, 18, 1, 0);
    if (processRank == 12) send(min, max, 16, 15, 1, 0);
    if (processRank == 13) send(min, max, 15, 0, 1, 7);
    if (processRank == 14) send(min, max, 16, 17, 0, 0);
    if (processRank == 15) send(min, max, 17, 18, 1, 1);
    if (processRank == 16) send(min, max, 0, 0, 1, 2);
    if (processRank == 17) send(min, max, 0, 0, 3, 4);
    if (processRank == 18) send(min, max, 0, 0, 5, 6);

    if (processRank == 0) {
        vector<int> sorted;
        // wait for sorted numbers to arrive to processor 0
        for (int i = 0; i < numbers.size(); i++) {
            int temp;
            MPI_Recv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, nullptr);
            sorted.push_back(temp);
        }
        printNumbers(sorted, true);  // print sorted values
    }

    return MPI_Finalize();
}