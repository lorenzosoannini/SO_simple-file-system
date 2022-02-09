#include "simplefs.h"
#include <stdio.h>
#include <string.h>
#define MAX_COMMAND_SIZE 1024
#define MAX_TOKENS 128

int main(int argc, char* argv[]){

    while(1){

        printf("SimpleFS Shell>>> ");
        fflush(stdout);

        char command[MAX_COMMAND_SIZE];
        
        fgets(command, MAX_COMMAND_SIZE, stdin);
        command[strcspn(command, "\n")] = 0;

        /* if(!strcmp(command, "\n"))
            continue; */

        int tok_num = 0;
        char command_tokens[MAX_TOKENS][MAX_COMMAND_SIZE];
 
        char* token = strtok(command, " ");
    
        while (token != NULL) {
            strcpy(command_tokens[tok_num], token);
			tok_num++;
            token = strtok(NULL, " ");
        }

        int i;
        for(i = 0; i < tok_num; i++)
            printf("%s\n", command_tokens[i]);
    }
}