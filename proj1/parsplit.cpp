#include <iostream>
#include <vector>
#include <fstream>
#include <mpi.h>

using namespace std;

#define ROOT 0

int main(int argc, char **argv)
{
    int rank=0, size=0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    byte median;
    vector<byte> byteArray;
    vector<byte> bigL(64);

    if(rank==ROOT) {
        //read file
        ifstream file("numbers");
        if (!file) {
            cerr << "Unable to open file",
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        file.seekg(0, file.end);
        int length = file.tellg();
        file.seekg(0, file.beg);
        byteArray.resize(length);
        file.read((char*)byteArray.data(), byteArray.size());
        file.close();

        //get middle number TODO: paralel select ?
        median = byteArray[byteArray.size()/2];
    }

    int sendCount = byteArray.size()/size;
    MPI_Bcast(&sendCount, 1, MPI_INT, ROOT, MPI_COMM_WORLD);// TODO ??
    //cout << "Process " << rank << " sendCount: " << sendCount << endl;
    vector<byte> Si(sendCount);

    MPI_Bcast(&median, 1, MPI_BYTE, ROOT, MPI_COMM_WORLD);
    MPI_Scatter(byteArray.data(), sendCount, MPI_BYTE, Si.data(), sendCount, MPI_BYTE, ROOT, MPI_COMM_WORLD);



    //cout << "Median: " << static_cast<int>(median) << endl;
    //print split data


    //split to LEG
    vector<byte> L,E,G;
    for (byte b : Si) {
        if (b < median)
            L.push_back(b);
        else if (b == median)
            E.push_back(b);
        else
            G.push_back(b);
    }

    cout << "Process " << rank << " data: ";
    for (byte b : L)
        cout << static_cast<int>(b) << " ";
    cout << endl;


    vector<int> displs(size);
    vector<int> rcounts(size);
    vector<int> recvcounts(size);
    vector<int> displs2(size);

    int sSize = L.size();
    int displ = 0;
    MPI_Scan(&sSize, &displ, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    rcounts[rank] = sSize;
    displs[rank] = displ-1;
    //keep max of rcounts
    MPI_Reduce(rcounts.data(), recvcounts.data(), size, MPI_INT, MPI_MAX, ROOT, MPI_COMM_WORLD);
    MPI_Reduce(displs.data(), displs2.data(), size, MPI_INT, MPI_MAX, ROOT, MPI_COMM_WORLD);

    //gather Ls
    MPI_Gatherv(L.data(), L.size(), MPI_BYTE, bigL.data(), recvcounts.data(), displs2.data(), MPI_BYTE, ROOT, MPI_COMM_WORLD);
    if(rank==ROOT) {
        bigL.resize(displs2[size-1]+1);
        cout << "bigL: ";
        for (byte b : bigL)
            cout << static_cast<int>(b) << " ";
        cout << endl;
    }


    //cout << "Hello World from process " << rank << " of " << size << endl;
    MPI_Finalize();
    return 0;
}