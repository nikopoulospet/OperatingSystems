
/* doit.C */

#include <iostream>
using namespace std;
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern char **environ;		/* environment info */

char* which(){
    FILE* fp;
    char *data = (char*)malloc(1024 * sizeof(char));
    const char *testdata = "which wc > temp.txt";
    system(testdata);
    fp = fopen("temp.txt", "r");
    if (fp == NULL) perror ("error opening file");
    else {
        while (fgets (data, 1024, fp) != NULL);
    }
    fclose (fp);
    std::string str = std::string(data);
    str.erase(strlen(data) - 1, 1);
    data = strdup(str.c_str());
    return data;
}

main(int argc, char **argv)
     /* argc -- number of arguments */
     /* argv -- an array of strings */
{
    char *argvNew[argc];
    int pid, i;
    char *path = which();

    for (i=0;i<argc;i++){
        if (i == 0){
            /*find file path for command */
            argvNew[i] = path;
        }else {
            argvNew[i] = argv[i+1];
        }
    }

    argvNew[i+1] = NULL;


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
	wait(0);		/* wait for the child to finish */
    }
}
