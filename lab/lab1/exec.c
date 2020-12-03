#define DEFINE_I
#include "header.h"
#define clear() printf("\033[H\033[J")


struct rusage usage;

void readProcessInfoByPID(pid_t pid, char **status, char **time){
    char temp[10];
    sprintf(temp, "%d", pid);
    int fd[2];
    int Pstatus;
    pipe(fd);
    pid_t childPid = fork();

    char buf[1024];
    if (childPid == 0){
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        // close(fd[1]);

        char *args[6];
        args[0] = "ps";
        args[1] = "-p";
        args[2] = temp;
        args[3] = "-o";
        args[4] = "stat=,time=";
        args[5] = NULL;
        execvp(args[0], args);
        _exit(0);
    }
    else{
        
        close(fd[1]);
        read(fd[0], buf, 1024);
        
        char *sep = " ";
        *status = strtok(buf, sep);
        *time = strtok(NULL, sep);
        int hour, min, sec;
        sscanf(*time , "%d:%d.%d" , &hour,&min,&sec);
        int seconds = hour * 3600 + min * 60 + sec;
        sprintf(*time, "%d", seconds);
        close(fd[0]);
        waitpid(childPid, &Pstatus, WNOHANG);
    }
}

void initShell(){
    clear();
}

void printDir(){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\nSHELL379: %s: ", cwd);
}

bool isBackground(char *args[], int argNum){
    if (strcmp(args[argNum-1], "&") == 0){
        argNum--;
        args[argNum] = NULL;
        return 1;
    }
    return 0;
}



void exeBuiltin(char input[LINE_LENGTH], char *args[], bool backgroundBool){
    char *builtinCMD[8] = {"exit", "jobs", "kill", "resume", "sleep", "suspend", "wait", "cd"};
    // handleCode, a code to give switch instruction
    //            -1 (default) -> not a valid builtin
    //            0  -> exit    3 -> resume      6 -> wait
    //            1 -> jobs     4 -> sleep       7 -> cd
    //            2 -> kill     5 -> suspend
    int handleCode = -1;

    pid_t pid;

    for (int i = 0; i < 8; i++){
        if (strcmp(builtinCMD[i], args[0]) == 0){
            handleCode = i;
            break;
        }
    }
    switch(handleCode){
        case -1:
            perror("Error: not a builtin method!");
        case 0:
            removeAllProcess();
            sleep(1);
            getrusage(RUSAGE_CHILDREN, &usage);
            printf("Resources used\n");
            printf("User time =     %ld seconds\n", usage.ru_utime.tv_sec);
            printf("System time =    %ld seconds\n", usage.ru_stime.tv_sec);
            _exit(0);
        case 1:
            updateProcess();
            char *status = NULL;
            char *time = NULL;
            printf("Running processes:\n");
            if (processNum > 0){
                printf(" #      PID    S    SEC    COMMAND\n");
            }
            int i = 0;
            while(i < processNum){
                readProcessInfoByPID(pcb[i].pid, &status, &time);
                printf(" %d:   %d    %s    %s    %s\n", i, pcb[i].pid, status, time, pcb[i].pName);
                i++;
            }

            for (int j = 0; j < suspendProcessNum; j++){
                readProcessInfoByPID(suspendPCB[j].pid, &status, &time);
                printf(" %d:   %d    %s    %s    %s\n", j+processNum, suspendPCB[j].pid, status, time, suspendPCB[j].pName);
            }

            printf("Processes =         %d active\n", processNum);
            printf("Completed processes:\n");
            getrusage(RUSAGE_CHILDREN, &usage);
            printf("User time =     %ld seconds\n", usage.ru_utime.tv_sec);
            printf("System time =    %ld seconds\n", usage.ru_stime.tv_sec);
            break;
        case 2:
            if (args[1] == NULL){
                printf("kill: pid required!");
                break;
            }
            pid = atoi(args[1]);
            if (pid <= 0){
                perror("kill");
            }
            else{
                if (kill(pid, SIGKILL) < 0){
                    printf("kill: operation failed!\n");
                }
            }
            break;
        case 3:
            if (args[1] == NULL){
                printf("resume: PID is required");
                break;
            }
            pid = atoi(args[1]);
            if (pid <= 0){
                printf("resume: not a valid pid");
            }
            else{
                if (kill(pid, SIGCONT) < 0){
                    printf("resume: operation failed");
                }
                moveFrontProcess(pid);
            }
            break;
        case 4:
            if (args[1] == NULL){
                printf("sleep: time required!");
                break;
            }
            int seconds = atoi(args[1]);
            if (seconds <= 0){
                printf("sleep: operation failed!\n");
            } else{
                sleep(seconds);
            }
            break;
        case 5:
            if (args[1] == NULL){
                printf("suspend: pid required");
                break;
            }
            pid = atoi(args[1]);
            if (pid < 0){
                printf("suspend: not a valid pid");
            }
            else{
                if (kill(pid, SIGSTOP) < 0){
                    printf("suspend: operation failed!");
                }
                moveBackProcess(pid);
            }
            break;
        case 6:
            if (args[1] == NULL){
                printf("wait: time required");
                break;
            }
            pid = atoi(args[1]);
            if (pid < 0){
                printf("wait: operation failed!");
            } else{
                int status;
                waitpid(pid, &status, 0);
            }
            break;
        case 7:
            if (chdir(args[1]) != 0){
                printf("cd: no such directory: %s", args[1]);
            }
            break;
    }
}

void helper(){
    // reset signal
    signal(SIGCHLD, SIG_DFL);
    while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
}

void exeCMD(char input[LINE_LENGTH], char *args[], int argNum, bool backgroundBool){
    // file descriptors; get started
    char *inputFileNames[argNum];
    char *outputFileNames[argNum];

    getFileOpCode(args, inputFileNames, outputFileNames);

    // grab files names
    int i = 0;
    while (inputFileNames[i]){
        i++;
    }
    int j = 0;
    while (outputFileNames[j]){
        j++;
    }

    // open file
    int status;
    pid_t pid = fork();

    int inputFd0 = open(inputFileNames[0], O_RDONLY, S_IRUSR|S_IWUSR);
    int outputFD0 = open(outputFileNames[0], O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);

    if (processNum >= 32){
        perror("too many process! exceeds 32");
        return;
    }

    pcb[processNum].pid = pid;
    strcpy(pcb[processNum].pName, input);
    processNum++;

    if (pid == -1){
        perror("fork failed!");
        return;
    }
    else if(pid == 0){
        // if background need a newline to make ouput looks nicer
        if (backgroundBool){
            printf("\n");
        }

        if (inputFd0 > 0){
            dup2(inputFd0, STDIN_FILENO);
        } 

        if (outputFD0 > 0){
            dup2(outputFD0, STDOUT_FILENO);
        }
        int codeExec = execvp(args[0], args);
        if(codeExec < 0){
            perror("command execution failed");
        }

        _exit(0);
    }
    else{
        // parent process
        if (!backgroundBool){
            waitpid(pid, &status, 0);
            removeProcessFromPCB(pid);
        }
        else{
            signal(SIGCHLD,helper);            
        }
    }

    if (inputFd0 >= 0){
        close(inputFd0);
    }
    if (outputFD0 >= 0){
        close(outputFD0);
    }
}


int main(int argc, char *argv[]) {
    initShell();

    while (1){
        // update pcb
        updateProcess();

        char input[LINE_LENGTH], *args[MAX_ARGS];
        int argNum = 0;

        // printout shell text
        printDir();

        // get user command input
        if (readCMD(input)){
            continue;
        }

        char oriInput[LINE_LENGTH];
        strcpy(oriInput, input);
        parseArgs(input, args, &argNum);

        // backgroundBool, whether the process should be executed in the background
        bool backgroundBool = isBackground(args, argNum);

        // get parsed args, if there is multiple args, meaning 
        // resulting input string is to be executed by shell379
        // statusCode: 0 , system call -> run execution
        //             1, specified builtin command -> run as instructed
        int statusCode = getStatus(args, argNum);


        // if builtin execute the command
        if (statusCode == 1){
            exeBuiltin(oriInput, args, backgroundBool);
        }
        else if (statusCode == 0){
            exeCMD(oriInput, args, argNum, backgroundBool);
       }

    }
    return 0;
}
