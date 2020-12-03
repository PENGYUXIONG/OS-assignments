#include "header.h"

void removeProcessFromPCB(pid_t pid){
    for (int i =0; i < 32; i++){
                if(pcb[i].pid == pid){
                    while (pcb[i].pid){
                        pcb[i] = pcb[i+1];
                        i++;
                    }
                    processNum--;
                    break;
                }
    }
}

void removeAllProcess(){
    int i = 0;
    while (pcb[i].pid){
        kill(pcb[i].pid, SIGKILL);
        i++;
    }

    int j = 0;
    while (suspendPCB[j].pid){
        kill(suspendPCB[j].pid, SIGKILL);
        j++;
    }
}

void updateProcess(){
    int i = 0;
    int status;
    while (pcb[i].pid){
        if (waitpid(pcb[i].pid, &status, WNOHANG) < 0){
            removeProcessFromPCB(pcb[i].pid);
        }
        i++;
    }
}

// input: pid_t pid
// put given process at the front of the pcb
void moveFrontProcess(pid_t pid){
    int index;
    for (int i=0; i < suspendProcessNum; i++){
        if (suspendPCB[i].pid == pid){
            index = i;
            break;
        }
    }
    
    struct ProcessInfo temp = suspendPCB[index];

    for (int i = index; i < suspendProcessNum; i++){
        suspendPCB[i] = suspendPCB[i+1];
    }
    suspendProcessNum--;

    for (int i = processNum-1; i >= 0; i--){
        pcb[i+1] = pcb[i];
    }
    pcb[0] = temp;
    processNum++;
}
// input: pid_t pid
// put the process at the back of the pcb
void moveBackProcess(pid_t pid){
    int index;
    for (int i=0; i < processNum; i++){
        if (pcb[i].pid == pid){
            index = i;
            break;
        }
    }
    struct ProcessInfo temp = pcb[index];
    
    for(int i = index; i < processNum-1; i++){
        pcb[i] = pcb[i+1];
    }
    processNum--;

    suspendPCB[suspendProcessNum] = temp;
    suspendProcessNum++;
}
