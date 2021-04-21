/* By Peter Nikopoulos */
/* g++ -o run proj4.cpp */
#include <iostream>
using namespace std;
#include <cstdlib>
#include <ctype.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#define DEFAULT 1024
#define MAXBUFF 8192

struct data
{
    long printable = 0;
    long count = 0;
};

void checkPrintable(char *buffer, int size, struct data *Data)
{
    for (int i = 0; i < size; i++)
    {
        Data->count++;
        if (isprint(buffer[i]) || isspace(buffer[i]))
        {
            Data->printable++;
        }
    }
}

void runREAD(int bufferSize, struct data *Data, int file, struct stat *sb)
{
    char *buffer = new char[bufferSize];
    int remainder;
    int size = sb->st_size;
    int sizepass;
    ioctl(file, TIOCINQ, &remainder);
    while (read(file, buffer, bufferSize) > 0)
    {
        if (size < bufferSize)
        {
            sizepass = size;
        }
        else
        {
            sizepass = bufferSize;
        }
        checkPrintable(buffer, sizepass, Data);
        size -= bufferSize;
    }
}

void runMMAP(struct data *Data, int file, struct stat *sb)
{
    char *pchFile;
    if ((pchFile = (char *)mmap(NULL, sb->st_size, PROT_READ, MAP_SHARED, file, 0)) == (char *)-1)
    {
        perror("Could not mmap file");
        exit(1);
    }

    checkPrintable(pchFile, sb->st_size, Data);

    if (munmap(pchFile, sb->st_size) < 0)
    {
        perror("Could not unmap memory");
        exit(1);
    }
}

void printData(struct data *Data)
{
    int percent = ((float)Data->printable / (float)Data->count) * 100;
    cout << Data->printable << " printable characters out of " << Data->count << " bytes, " << percent << "%\n";
}

int main(int argc, char **argv)
{
    struct data *Data = new struct data;
    int file;
    int bufferSize;
    bool mmapRUN = false;
    struct stat sb;

    if (argc < 2)
    {
        perror("not enough arguments");
        exit(1);
    }
    else if (argc >= 2)
    {
        file = open(argv[1], O_RDONLY);
        if (file < 0)
        {
            perror("failed to open file");
            exit(1);
        }
        if (fstat(file, &sb) < 0)
        {
            perror("Could not stat file to obtain its size");
            exit(1);
        }

        if (argc == 3)
        {
            if (!strcmp(argv[2], "mmap"))
            {
                mmapRUN = true;
            }
            else
            {
                if (atoi(argv[2]) > MAXBUFF)
                {
                    bufferSize = MAXBUFF;
                }
                else
                {
                    bufferSize = atoi(argv[2]);
                }
            }
        }
        else
        {
            bufferSize = DEFAULT;
        }
    }

    if (!mmapRUN)
    {
        runREAD(bufferSize, Data, file, &sb);
    }
    else
    {
        runMMAP(Data, file, &sb);
    }

    printData(Data);

    close(file);
    return 0;
}