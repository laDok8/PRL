#include <iostream>
#include <vector>
#include <fstream>
#include <numeric>
#include <mpi.h>
#include <algorithm>
#include <iomanip>

using namespace std;

#define ROOT 0
#define K 4
#define byte unsigned char

int main(int argc, char **argv) {
    int rank = 0, size = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    vector<byte> input(32);

    //per CPU
    int argMin;
    vector<byte> distances(K);
    vector<int> meanSums(K), meanCounts(K), recvCounts(K), recvSums(K);
    vector<float> means(K);
    std::cout << std::setprecision(2) << std::fixed;

    if (rank == ROOT) {
        //read file
        ifstream file("numbers");
        if (!file) {
            cerr << "Unable to open file" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        file.seekg(0, file.end);
        int length = file.tellg() > size ? size : static_cast<int>(file.tellg());
        file.seekg(0, file.beg);
        input.resize(length);
        file.read((char *) input.data(), input.size());
        file.close();

        if (length < K || length != size) {
            cerr << "Not enough CPUs" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        //set means
        copy(input.begin(), input.begin() + K, means.begin());
    }


    //bcast value and initial means
    MPI_Bcast(means.data(), K, MPI_FLOAT, ROOT, MPI_COMM_WORLD);
    byte myVal;
    MPI_Scatter(input.data(), 1, MPI_BYTE, &myVal, 1, MPI_BYTE, ROOT, MPI_COMM_WORLD);


    bool changed;
    do {
        //calculate distance and find min
        transform(means.begin(), means.end(), distances.begin(), [myVal](byte b) { return abs(myVal - b); });
        argMin = distance(distances.begin(), min_element(distances.begin(), distances.end()));

        //calculate mean using SumReduction (sum and count)
        fill(meanCounts.begin(), meanCounts.end(), 0);
        fill(meanSums.begin(), meanSums.end(), 0);
        meanCounts[argMin] = 1;
        meanSums[argMin] = myVal;
        MPI_Reduce(meanCounts.data(), recvCounts.data(), K, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);
        MPI_Reduce(meanSums.data(), recvSums.data(), K, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);

        changed = false;
        if (rank == ROOT) {
            for (int i = 0; i < K; i++) {
                if (recvCounts[i] != 0 && (recvSums[i] / recvCounts[i]) != means[i]) {
                    means[i] = recvSums[i] / recvCounts[i];
                    changed = true;
                }
            }
        }

        //broadcast is not needed, but imo makes code more readable
        MPI_Bcast(&changed, 1, MPI_C_BOOL, ROOT, MPI_COMM_WORLD);
    } while (changed);

    //send argMin to root
    vector<byte> recvArgMin(size);
    MPI_Gather(&argMin, 1, MPI_BYTE, recvArgMin.data(), 1, MPI_BYTE, ROOT, MPI_COMM_WORLD);


    if (rank == ROOT) {
        vector <vector<byte>> meanVals(K);
        //put data from corresponding argMin to their K-mean
        for (int i = 0; i < size; i++) {
            meanVals[recvArgMin[i]].push_back(input[i]);
        }
        //print output
        for (int i = 0; i < K; i++) {
            cout << "[" << means[i] << "] <- ";
            for (byte b: meanVals[i])
                cout << static_cast<int>(b) << " ";
            cout << endl;
        }
    }

    MPI_Finalize();
    return 0;
}