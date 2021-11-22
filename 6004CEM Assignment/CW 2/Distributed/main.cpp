#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <vector>

static char const *const MODEL_NAME = "model name";
static char const *const INPUT_FILE_NAME = "poem.txt";
size_t const BUFFER_LENGTH = 256;
size_t const MAX_NODES = 256;

using namespace std;


int main(int argc, char **argv)
{
    int rank, size, i;
    size_t rsize = 0;
    size_t rsize2 = 0;
    MPI_Status status;
    char message[BUFFER_LENGTH] = { 0 };
    int values[MAX_NODES] = { 0 };
    char name[MPI_MAX_PROCESSOR_NAME] = { 0 };
    FILE *input = NULL;
    char *arg = 0;
    int countt = 0;
    vector<string> mas;
    vector<int> masi(1000);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Get_processor_name(name, &i);

    if (rank == 0)
    {
        printf("Comm size: %d\n", size);
        printf("Processor name: %s\n", name);

        FILE *cpuinfo = fopen("/proc/cpuinfo", "rb");
        size_t core_num = 1;
        int iii = 0;

        while (getdelim(&arg, &rsize, '\n', cpuinfo) != -1)
        {
            if (memcmp(arg, MODEL_NAME, strlen(MODEL_NAME)) == 0)
            {
                strtok(arg, ":");
                printf("Processor core%lu:%s", core_num, strtok(NULL, ":"));
                core_num++;
            }
        }
        free(arg);
        fclose(cpuinfo);
        printf("\nPoem:\n");
        arg = NULL;
        rsize = 0;

        input = fopen(INPUT_FILE_NAME, "r");
        if (!input)
        {
            goto exit;
        }

        while ((rsize2 = getline(&arg, &rsize, input)) != -1)
        {
            mas.push_back(string(arg));
            values[0] = iii;
            iii++;
            values[1] = 1 + rand() % (size - 1);
            MPI_Send(values, 1, MPI_INT, values[1], 7, MPI_COMM_WORLD);
            MPI_Send(arg, rsize2, MPI_BYTE, values[1], 7, MPI_COMM_WORLD);
        }
        arg[0] = 0;
        values[0] = -1;
        for (size_t node_num = 1; node_num < size; node_num++)
        MPI_Send(values, 1, MPI_INT, node_num, 7, MPI_COMM_WORLD);
        free(arg);

        strcpy(message, "getnum");
        for (size_t node_num = 1; node_num < size; node_num++)
        {
            MPI_Send(message, strlen(message) + 1, MPI_BYTE, node_num, 8, MPI_COMM_WORLD);
            MPI_Recv(values, MAX_NODES, MPI_INT, MPI_ANY_SOURCE, 9, MPI_COMM_WORLD, &status);
            for (size_t i = 0; i < values[0]; i++)
            {
                countt++;
                masi[values[i + 1]] = node_num;
            }
        }

        for (int i = 0; i < countt; i++)
        {
            values[0] = -2;
            values[1] = masi[i];
            memset(message, 0, sizeof(message));
            MPI_Send(values, 2, MPI_INT, values[1], 10, MPI_COMM_WORLD);
            MPI_Recv(message, BUFFER_LENGTH, MPI_BYTE, MPI_ANY_SOURCE, 11, MPI_COMM_WORLD, &status);
            printf("%s", message);
        }

    }
    else
    {
        int iii = 0;
        message[0] = 1;
        while (1)
        {
            memset(message, 0, sizeof(message));
            MPI_Recv(values, MAX_NODES, MPI_INT, MPI_ANY_SOURCE, 7, MPI_COMM_WORLD, &status);
            if (values[0] == -1)
            {
                break;
            }

            MPI_Recv(message, BUFFER_LENGTH, MPI_BYTE, MPI_ANY_SOURCE, 7, MPI_COMM_WORLD, &status);
            mas.push_back(string(message));
            masi.insert(masi.begin() + iii, values[0]);
            iii++;
        }

        MPI_Recv(message, BUFFER_LENGTH, MPI_BYTE, MPI_ANY_SOURCE, 8, MPI_COMM_WORLD, &status);
        if (strcmp(message, "getnum") == 0)
        {
            values[0] = mas.size();
            for (size_t i = 0; i < values[0]; i++) values[i + 1] = masi[i];
            MPI_Send(values, values[0] + 1, MPI_INT, 0, 9, MPI_COMM_WORLD);
        }
        for (int i = 0; i < iii; i++)
        {
            MPI_Recv(values, MAX_NODES, MPI_INT, MPI_ANY_SOURCE, 10, MPI_COMM_WORLD, &status);
            MPI_Send(mas[i].c_str(), strlen(mas[i].c_str()), MPI_BYTE, 0, 11, MPI_COMM_WORLD);
        }
    }

    if (rank == 0)
    {
        fclose(input);
    }

exit:
    MPI_Finalize();
    return 0;
}

