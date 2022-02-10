#include "simplefs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#define MAX_COMMAND_SIZE 1024
#define MAX_TOKENS 128

int main(int argc, char* argv[]){

  
        int i;

        printf("\n--- Creo e inizializzo un nuovo disco\n");
		//inizializzo il DiskDriver e la bitmap
		DiskDriver disk;
		DiskDriver_init(&disk, "file_test/SimpleFS.disk", 50); 
	    printf("    Inizializzazione Disk Driver Riuscita\n");
		BitMap bitmap;
		bitmap.num_bits = disk.header->bitmap_entries * 8;
		bitmap.entries = disk.bitmap_data;
	
	 	//setto tutti 0 nella bitmap
		for(i = 0; i < bitmap.num_bits; i++)
		BitMap_set(&bitmap, i, 0);
				
	    printf("    Creazione Bitmap e Azzeramento riuscito\n");
	    printf("\n");

	    printf("    Formatto e inizializzo il filesystem\n");
	    printf("--- Test SimpleFS_init()\n");
		SimpleFS fs;
		DirectoryHandle* d = SimpleFS_init(&fs, &disk);
		assert(d != NULL);
      
      while(1){
        printf("SimpleFS Shell>>> ");
        fflush(stdout);

        char command[MAX_COMMAND_SIZE];
        //prendo la stringa in input
        fgets(command, MAX_COMMAND_SIZE, stdin);
        //elimino lo \n

        command[strcspn(command, "\n")] = 0;

        /* if(!strcmp(command, "\n"))
            continue; */

        //divide la stringa in input in sottostringe leggibili tramite tokenizzazione
        
        int tok_num = 0;
        char command_tokens[MAX_TOKENS][MAX_COMMAND_SIZE];
        strcpy(command_tokens[0],"");
        char* token = strtok(command, " ");
    
        while (token != NULL) {
            strcpy(command_tokens[tok_num], token);
			tok_num++;
            token = strtok(NULL, " ");
        }

        if(!strcmp(command_tokens[0], "ls")){
             if(tok_num > 1){
                printf("SimpleFS Shell>>> Error: too many arguments for 'ls'\n");
                continue;
            }

            char* names[50];
            for(i = 0; i < sizeof(names)/sizeof(char*); i++){
                names[i] = malloc(128*sizeof(char));
            }

            int ret = SimpleFS_readDir((char**)names, d);
            assert(ret >= 0);
            printf("\n    Ho trovato %d file(s) nella cartella corrente:\n", ret);
            
            printf("\n");
            for(i = 0; i < ret; i++){
                printf("     \t%s\n", names[i]);
            }

            for(i = 0; i < sizeof(names)/sizeof(char*); i++){
                free(names[i]);
            }
           
        }
        if(!strcmp(command_tokens[0], "pwd")){
             if(tok_num > 1){
                printf("SimpleFS Shell>>> Error: too many arguments for 'pwd'\n");
                continue;
            }
            printf("SimpleFS Shell>>>\n %s \n",d->dcb->fcb.name);
            
           
        }
        if(!strcmp(command_tokens[0], "cd")){
             if(tok_num<2){
                printf("SimpleFS Shell>>> Error: Miss some arguments for 'cd'\n");
                continue;
            }
             if(tok_num>2){
                printf("SimpleFS Shell>>> Error: too many arguments for 'cd'\n");
                continue;
            }
          SimpleFS_changeDir(d, command_tokens[1]);
           
           // printf("SimpleFS Shell>>>\n Ho cambiato  la directory in %s  \n",command_tokens[1]);
        }
        if(!strcmp(command_tokens[0], "echo")){
            
            for(i = 1; i < tok_num; i++)
            printf("%s ", command_tokens[i]);
            printf("\n");
          
        }
        if(!strcmp(command_tokens[0], "mkdir")){
             if(tok_num<2){
                printf("SimpleFS Shell>>> Error: Miss some arguments for 'mkdir'\n");
                continue;
            }
             if(tok_num>2){
                printf("SimpleFS Shell>>> Error: too many arguments for 'mkdir'\n");
                continue;
            }
          SimpleFS_mkDir(d, command_tokens[1]);
		 
             printf("SimpleFS Shell>>>\n Ho creato  la cartella %s nella directory attuale \n",command_tokens[1]);

        }
    }
}