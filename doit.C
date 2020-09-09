
/* doit.C */

#include <iostream>
using namespace std;
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

extern char **environ;		/* environment info */

char* which(char *cmd){
    /* This function finds the path of a given command */
    /* Return: char* */
    FILE* fp;
    long LSize;
    /* Assemble and execute system command, pipe into "temp.txt" */
    std::string command = "which " + std::string(cmd) + " > temp.txt";
    system(command.c_str());
    fp = fopen("temp.txt", "r");

    /* find the size of whatever is in file and allocate a memory space */
    fseek(fp,0L,SEEK_END);
    LSize = ftell(fp) + 1;
    rewind(fp);
    char *data = (char*)malloc(LSize * sizeof(char));

    /* save data from file into the LoChars "data" */
    if (fp == NULL) perror ("error opening file");
    else {
        while (fgets (data, LSize, fp) != NULL);
    }
    fclose (fp);

    /* remove "\n" and return data */
    std::string str = std::string(data);
    str.erase(strlen(data) - 1, 1);
    data = strdup(str.c_str());
    return data;
}

void getUsageData(struct timeval &start, struct timeval &end){
    int who = RUSAGE_CHILDREN;
    struct rusage usage;
    
    getrusage(who, &usage);
    gettimeofday(&end, NULL);

    cout << "CPU TIME (USER): " << (double) usage.ru_utime.tv_usec / 1000 << "\n"; // CPU TIME
    cout << "CPU TIME (SYSTEM): " << (double) usage.ru_stime.tv_usec / 1000 << "\n";
    cout << "WALL_CLOCK ELAPSED: " << ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) << "\n";
    cout << "PREEMPTED INVOLUNTARILY: " << usage.ru_nivcsw << "\n";
    cout << "PROCESS STOPPED VOLUNTARILY: " << usage.ru_nvcsw << "\n";
    cout << "MAJOR PAGE FAULTS: " << usage.ru_minflt << "\n";
    cout << "MINOR PAGE FAULTS: " << usage.ru_majflt << "\n";
}

main(int argc, char **argv)
     /* argc -- number of arguments */
     /* argv -- an array of strings */
{
    char *argvNew[argc];
    int pid, i;
    char *path = which(argv[1]);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    /* iterate through list of args from terminal, replace command w/ path of command */
    for (i=0;i<argc;i++){
        if (i == 0){
            /*find file path for command */
            argvNew[i] = path;
        }else {
            argvNew[i] = argv[i+1];
        }
    }

    if ((pid = fork()) < 0) {
	cerr << "Fork error\n";
        exit(1);
    }
    else if (pid == 0) {
        /* child process */
        if (execve(argvNew[0], argvNew, environ) < 0) {
	    cerr << "Execve error\n";
            exit(1);
        }
    }
    else {
        /* parent */
	wait(0); //wait for the child to finish
    getUsageData(start, end);

    }
}
