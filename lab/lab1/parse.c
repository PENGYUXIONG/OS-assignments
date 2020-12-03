#include "header.h"

int readCMD(char *input){
    size_t bufsize = 100;
    size_t chNum;

    char buffer[100];
    char *b = buffer;
    
    chNum = getline(&b, &bufsize, stdin) ;
    if (buffer[0] == ' '){
        return 1;
    }
    (buffer)[chNum-1] = 0;


    // the arguement is empty, current process done
    if (strlen(buffer) == 0){
        return 1;
    }
    else{
        strcpy(input, buffer);
        return 0;
    }
}

void parseArgs(char *input, char *parsedInput[], int *argNum){
    // parsing arguement
    char* parsed;
    char* sep = " ";
    int index = 0;

    parsed = strtok(input, sep);

    while (parsed != NULL){
        
        // store each arg into the array
        parsedInput[index] = parsed;
        ++index;

        parsed = strtok(NULL, sep);
    }
    
    parsedInput[index] = NULL;
    *argNum = index;
}

void getFileOpCode(char *args[], char *inputFile[], char *outputFile[]){
    int i = 1;
    int inputIndex = 0;
    int outputIndex = 0;
    while (args[i]){
        if (args[i][0] == '<' ){
            char *temp = args[i];
            (temp)++;
            inputFile[inputIndex] = temp;
            inputIndex++;
        } 

        if (args[i][0] == '>' ){
            char *temp = args[i];
            (temp)++;
            outputFile[outputIndex] = temp;
            outputIndex++;
        }
        i++;
    }
    inputFile[inputIndex] = NULL;
    outputFile[outputIndex] = NULL;

    // remove arg with < or >
    i = 0;
    while (args[i]){
        if (args[i][0] == '<' || args[i][0] == '>'){
            int k = i;
            while (args[k]){
                args[k] = args[k+1];
                k++;
            }
        }
        else{
            i++;
        }
    }
    i = 0;

}


int getStatus(char *args[], int argNum){
    char *builtinCMD[6] = { "cd", "kill", "resume", "sleep", "suspend", "wait"};
    int statusCode = 0; 
    
    // get first arg
    char *headArg = args[0];
    if ((strcmp(headArg, "exit") == 0) || (strcmp(headArg, "jobs") == 0)){
        if (args[1] == NULL){
            statusCode = 1;
        }
    }
    else{
        for (int i = 0; i < 6; i++){
            if (strcmp(headArg, builtinCMD[i]) == 0){
                statusCode = 1;
            }
        }
    }
    return statusCode;
}