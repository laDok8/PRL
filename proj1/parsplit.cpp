#include <iostream>
#include <vector>
#include <fstream>
#include <numeric>
#include <mpi.h>

using namespace std;

#define ROOT 0
#define byte unsigned char

int main(int argc, char **argv) {
    int rank = 0, size = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    byte median;
    vector <byte> byteArray(64);

    //init maps ( for MPI i have different send-recv variables)
    map<char, vector<byte>> masterMap = {{'L', vector<byte>(64)},
                                         {'E', vector<byte>(64)},
                                         {'G', vector<byte>(64)}};
    map<char, vector<byte>> map;

    if (rank == ROOT) {
        //read file
        ifstream file("numbers");
        if (!file) {
            cerr << "Unable to open file", MPI_Abort(MPI_COMM_WORLD, 1);
        }

        file.seekg(0, file.end);
        int length = file.tellg();
        file.seekg(0, file.beg);
        byteArray.resize(length);
        file.read((char *) byteArray.data(), byteArray.size());
        file.close();

        median = byteArray[byteArray.size() / 2];
    }

    //bcast sendCount,data and median
    int sendCount = byteArray.size() / size;
    MPI_Bcast(&sendCount, 1, MPI_INT, ROOT, MPI_COMM_WORLD);// TODO ??
    vector <byte> Si(sendCount);

    MPI_Bcast(&median, 1, MPI_BYTE, ROOT, MPI_COMM_WORLD);
    MPI_Scatter(byteArray.data(), sendCount, MPI_BYTE, Si.data(), sendCount, MPI_BYTE, ROOT, MPI_COMM_WORLD);

    //split to LEG
    for (byte b: Si) {
        map[b < median ? 'L' : (b == median ? 'E' : 'G')].push_back(b);
    }

    //arrays of sizes and displacements
    vector<int> displs(size);
    vector<int> rcounts(size);
    vector<int> recvcounts(size);

    for (char cur: {'L', 'E', 'G'}) {
        rcounts[rank] = map[cur].size();

        //calculate displacements
        MPI_Reduce(rcounts.data(), recvcounts.data(), size, MPI_INT, MPI_MAX, ROOT, MPI_COMM_WORLD);
        if (rank == ROOT) {
            displs[0] = 0;
            for (int i = 1; i < size; i++) {
                displs[i] = displs[i - 1] + recvcounts[i - 1];
            }
        }

        if (rank == ROOT)
            masterMap[cur].resize(accumulate(recvcounts.begin(), recvcounts.end(), 0));

        MPI_Gatherv(map[cur].data(), map[cur].size(), MPI_BYTE, masterMap[cur].data(), recvcounts.data(), displs.data(),
                    MPI_BYTE, ROOT, MPI_COMM_WORLD);

    }

    if (rank == ROOT) {
        for (char cur: {'L', 'E', 'G'}) {
            cout << cur << ": ";
            for (byte b: masterMap[cur])
                cout << static_cast<int>(b) << " ";
            cout << endl;
        }
    }

    MPI_Finalize();
    return 0;
}