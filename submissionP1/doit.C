
/* doit.C */

#include <iostream>
using namespace std;
#include <unistd.h>
#include <sys/wait.h>
//#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

extern char **environ;		/* environment info */
std::string prompt = "===> ";
const extern int maximum = 1024;
extern bool background = false;

struct input {
    int argc = 0;
    char** argv;
};

struct process {
    int pid = 0;
    char* command;
    bool background = true;
};



void fillProcessArray(struct process **Processes){
    for(int i = 0; i < 10; i++){
        Processes[i] = (struct process*)malloc(sizeof(struct process));
    }
}

int searchProcessArray(int pid, struct process **Processes){
    for(int x = 0; x < 10; x++){
        if(Processes[x]->pid == pid) return x;
    }
    return -1;
}

void doJobsProcessArray(struct process **Processes){
    int index = searchProcessArray(0,Processes);
    cout << index << "\n";
    for(int x = 0; x <= index; x++){
        cout << " [" << x << "] ";
        cout << Processes[x]->pid << " " << Processes[x]->command << "\n";
        cout.flush();
    }
}

struct process **addPidToProcessArray(int pid, char* command, struct process **Processes){
    cout << "adding " << pid << "\n";
    cout.flush(); 
    int index = searchProcessArray(0,Processes);
    Processes[index]->pid = pid;
    Processes[index]->command = command;
    return Processes;    
}



void getUsageData(struct timeval &start, struct timeval &end){
    int who = RUSAGE_CHILDREN;
    struct rusage usage;
    
    getrusage(who, &usage);
    gettimeofday(&end, NULL);

    cout << "\n";
    cout << "CPU TIME (USER): " << (double) usage.ru_utime.tv_usec / 1000 << "\n"; // CPU TIME
    cout << "CPU TIME (SYSTEM): " << (double) usage.ru_stime.tv_usec / 1000 << "\n";
    cout << "WALL_CLOCK ELAPSED: " << ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) << "\n";
    cout << "PREEMPTED INVOLUNTARILY: " << usage.ru_nivcsw << "\n";
    cout << "PROCESS STOPPED VOLUNTARILY: " << usage.ru_nvcsw << "\n";
    cout << "MAJOR PAGE FAULTS: " << usage.ru_minflt << "\n";
    cout << "MINOR PAGE FAULTS: " << usage.ru_majflt << "\n";
}

void runCommand(char **commands, struct process **Processes){
    int pid;
    struct timeval start, end;
    gettimeofday(&start, NULL);


    if ((pid = fork()) < 0){
        cerr <<"Fork error\n";
        exit(1);
    }
    else if (pid == 0){
        /*child*/
        if (execvpe(commands[0],commands,environ) < 0){
            cerr << "Execvpe error\n";
            exit(1);
        }
    }
    else {
        /*parent*/

        if(background){
            addPidToProcessArray(pid,commands[0],Processes);
            background = false;
        }
        wait(0);
        //if(searchProcessArray(pid,loProcesses) > 0) cout << pid << "completed\n";
        getUsageData(start,end);
        
    }
}

void runBgCommand(char **commands, struct process **Processes){
    int pid;
    if((pid = fork()) < 0){
        cerr << "Fork error\n";
        exit(1);
    }
    else if (pid == 0){
        /* child, run run cmd */
        runCommand(commands, Processes);
    }
    else {
        /*parent do nothing*/
    }
}

struct input *readPrompt(){
    // maybe call a function to free inpt mem here
    char *line = (char*)malloc(maximum *sizeof(char));
    struct input *Input = (struct input*)malloc(sizeof(struct input));
    Input->argv = (char**)malloc(32 *sizeof(char*));
    for(int i = 0; i < 32; i++){
        Input->argv[i] = (char*)malloc(128*sizeof(char));
    }

    cout << "\n" << prompt;
    cin.getline(line,32*128);
    int x = 0;
    char *temp;

    temp = strtok(line," ");
    while(temp != NULL){
        Input->argv[x] = temp;
        temp = strtok(NULL," ");
        x++;
    }
    Input->argv[x] = NULL;
    Input->argc = x;
    return Input;
}

bool checkCommands(struct input *Input, struct process **Processes){
    char* cmd = Input->argv[0];
    if(!strcmp(cmd,"exit")){
        return false;
    }
    else if(!strcmp(cmd,"set")){
        if(!strcmp(Input->argv[1],"prompt") && !strcmp(Input->argv[2],"=")){
            prompt = Input->argv[3];
        }else{
            cout << "syntax error\n";
        }
    }
    else if(!strcmp(cmd,"cd")){
        if(Input->argc == 2){
            chdir(Input->argv[1]);
        }else{
            cout << "too many arguments\n";
        }
    }
    else if(!strcmp(Input->argv[Input->argc-1],"&")){
        Input->argv[Input->argc-1] = NULL;
        background = true;
        runBgCommand(Input->argv, Processes);
    }
    else if(!strcmp(cmd,"jobs")){
        if(Input->argc == 1){
            cout << "here\n";
            doJobsProcessArray(Processes);
            cout << "here\n";
            // list all children
        }else{
            cout << "too many arguments\n";
        }
    }
    else{
        runCommand(Input->argv, Processes);
    }
    return true;
}


main(int argc, char **argv)
     /* argc -- number of arguments */
     /* argv -- an array of strings */
{
    struct process **Processes = (struct process**)malloc(10*sizeof(struct process*));
    fillProcessArray(Processes);

    if (argc > 1){
        char *argvNew[argc];
        int i;
        for(i=0;i<argc;i++){
            argvNew[i] = argv[i+1];
        }
        runCommand(argvNew, Processes);
    }
    else{
        bool runShell = true;
        while(runShell){
            struct input *Input = readPrompt();

            if(Input->argc != 0){
                runShell = checkCommands(Input,Processes); 

                free(Input->argv);
                free(Input);
            }
        }
    }

}

/*
This mini shell shares many of the basic features of the linux shell,
aside from buggyness and the half working jobs function call
some limiations I noticed is the lack of a bashrc file that allows us to save env vars like prompt

Functionality:
this shell can spawn background processes but the job function and modified exit do not work
*/
