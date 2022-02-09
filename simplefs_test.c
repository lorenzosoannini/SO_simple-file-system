#include "bitmap.h" 
#include "disk_driver.h"
#include "simplefs.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>


int choose;
int i;
int ret;
char* entry;

// binary print from stackoverflow

void binary_print(char* str) {
	 if (strlen(str) == 0)printf("00000000") ;
	else{
	int i, j;
	for(i = 0; i < strlen(str); i++) {
		char c = str[i];
		for (j = 7; j >= 0; j--) {
	      printf("%d", !!((c >> j) & 0x01)); 
	  }
	  printf("\n");
	}
	} 
}


void binary_print_ablock(char* str,int block) {
	 if (strlen(str) == 0)printf("00000000") ;
	else{
	int i, j;
	i = block/8;
	
		char c = str[i];
		for (j = 7; j >= 0; j--) {
	      printf("%d", !!((c >> j) & 0x01)); 
	  	}
	} 
}

void binary_print_block(char* str,int block) {
	 if (strlen(str) == 0) printf("❏ ❏ ❏ ❏ ❏ ❏ ❏ ❏");
	else{
	int i, j;
	i = block/8;
		char c = str[i];

		for (j = 7; j >= 0; j--) {
			fflush(stdout);
			if (!!((c >> j) & 0x01)){
				printf("▨ ");
			}
	      else printf("❏ ");
		  usleep(30000);
	  }
	
	} 
}

//JM first look to bitmap test
int main(int argc, char** argv) {
		
  	printf("░██████╗██╗███╗░░░███╗██████╗░██╗░░░░░███████╗\n");
	printf("██╔════╝██║████╗░████║██╔══██╗██║░░░░░██╔════╝\n");
	printf("╚█████╗░██║██╔████╔██║██████╔╝██║░░░░░█████╗░░\n");
	printf("░╚═══██╗██║██║╚██╔╝██║██╔═══╝░██║░░░░░██╔══╝░░\n");
	printf("██████╔╝██║██║░╚═╝░██║██║░░░░░███████╗███████╗\n");
	printf("╚═════╝░╚═╝╚═╝░░░░░╚═╝╚═╝░░░░░╚══════╝╚══════╝\n");
	printf("\n");

	printf("███████╗██╗██╗░░░░░███████╗  ░██████╗██╗░░░██╗░██████╗████████╗███████╗███╗░░░███╗\n");
	printf("██╔════╝██║██║░░░░░██╔════╝  ██╔════╝╚██╗░██╔╝██╔════╝╚══██╔══╝██╔════╝████╗░████║\n");
	printf("█████╗░░██║██║░░░░░█████╗░░  ╚█████╗░░╚████╔╝░╚█████╗░░░░██║░░░█████╗░░██╔████╔██║\n");	
	printf("██╔══╝░░██║██║░░░░░██╔══╝░░  ░╚═══██╗░░╚██╔╝░░░╚═══██╗░░░██║░░░██╔══╝░░██║╚██╔╝██║\n");
	printf("██║░░░░░██║███████╗███████╗  ██████╔╝░░░██║░░░██████╔╝░░░██║░░░███████╗██║░╚═╝░██║\n");
	printf("╚═╝░░░░░╚═╝╚══════╝╚══════╝  ╚═════╝░░░░╚═╝░░░╚═════╝░░░░╚═╝░░░╚══════╝╚═╝░░░░░╚═╝\n");
	printf("\n");
	printf("████████╗███████╗░██████╗████████╗\n");
	printf("╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝\n");
	printf("░░░██║░░░█████╗░░╚█████╗░░░░██║░░░\n");
	printf("░░░██║░░░██╔══╝░░░╚═══██╗░░░██║░░░\n");
	printf("░░░██║░░░███████╗██████╔╝░░░██║░░░\n");
	printf("░░░╚═╝░░░╚══════╝╚═════╝░░░░╚═╝░░░\n");
		printf("\n");
	printf(" By Lorenzo Soannini and John Mark Alexis Marcelo\n");
	
  if(!choose) {
	  printf("\n Simple File System Testing scegli cosa vuoi testare: \n1 = BitMap\n2 = DiskDriver\n3 = SimpleFS\n\n>>> ");
	  scanf("%d", &choose);
  }
  if (choose ==1 ){
  
	printf("██████╗░██╗████████╗███╗░░░███╗░█████╗░██████╗░\n");
	printf("██╔══██╗██║╚══██╔══╝████╗░████║██╔══██╗██╔══██╗\n");
	printf("██████╦╝██║░░░██║░░░██╔████╔██║███████║██████╔╝\n");
	printf("██╔══██╗██║░░░██║░░░██║╚██╔╝██║██╔══██║██╔═══╝░\n");
	printf("██████╦╝██║░░░██║░░░██║░╚═╝░██║██║░░██║██║░░░░░\n");
	printf("╚═════╝░╚═╝░░░╚═╝░░░╚═╝░░░░░╚═╝╚═╝░░╚═╝╚═╝░░░░░\n");
	printf("\n");
  	printf("Hai scelto di testare la BitMap\n\n");
  	
	getchar();
	
	// JM Eseguo il Test per la funzione BitMap_blockToIndex
	//  converto un blocco indice a un indice nell'array
	// e un char che indica l'offset del bit all'interno dell'array
	int num = 0;
	printf("\n--- Test BitMap_blockToIndex() con %d input \n", num);
	if( num == 0){
		printf("\n    Il blocco indice è nullo \n");
	}   
	else{
		BitMapEntryKey block1 = BitMap_blockToIndex(num);
		printf("    La posizione del blocco è %d, ovvero la entry_num %d al bit_num %d", num, block1.entry_num, block1.bit_num);
	}
	printf("\n    CLICCA ENTER PER ANDARE AVANTI ");
	getchar();
	printf("\n\n\n");
  	
	  // JM Eseguo il Test per la funzione BitMap_blockToIndex
	num = 420;
	printf("\n--- Test BitMap_blockToIndex(%d)\n", num);   
	BitMapEntryKey block = BitMap_blockToIndex(num);
	printf("\n    La posizione del blocco è %d, ovvero la entry_num %d al bit_num %d\n ", num, block.entry_num, block.bit_num);
		

	printf("\n    Scanning to block %d ....\n    ",block.entry_num);
	for (int i = 0;i<block.entry_num;i++){
		fflush(stdout);
		printf("❏ ");
		usleep(20000);
	}
	printf("\n\n    Showing the %d block with bitnum %d ....\n    ",block.entry_num,block.bit_num);
	for (int i = 0;i<8;i++){
		fflush(stdout);
		if(i<block.bit_num){
			printf("▢ ");
		}
		else printf("▨ ");
		usleep(70000);
	}
	printf("\n\n    CLICCA ENTER PER ANDARE AVANTI ");
	getchar();
	// JM Eseguo il Test BitMap_indexToBlock 
	printf("\n\n\n");
	
	printf("\n\n--- Test BitMap_indexToBlock(block.entry_num, block.bit_num)\n\n");
	int posizione = BitMap_indexToBlock(block.entry_num, block.bit_num); 
		printf("    Abbiamo la entry %d e il bit %d, ovvero la posizione %d", block.entry_num, block.bit_num, posizione);
	
	//JM Eseguo il test per la BitMap_set() per il quale ho bisogno di un DiskDriver e bitmap
	DiskDriver disk;
	BitMap bitmap;
	//inizializzo il DiskDriver e la bitmap
	DiskDriver_init(&disk, "file_test/testosterone.txt", 50); 
	bitmap.num_bits = disk.header->bitmap_entries * 8;
	bitmap.entries = disk.bitmap_data;
	
 	//setto tutti 0 nella bitmap
	for(i = 0; i < bitmap.num_bits; i++)
		BitMap_set(&bitmap, i, 0);
	
	printf("\n\n    CLICCA ENTER PER ANDARE AVANTI ");
	getchar();
	printf("\n\n\n");

	printf("\n\n--- Test BitMap_set(BitMap* bitmap, int pos, int status) :");
	printf("\n\n    Output before BitMap_set()    :");
	

	binary_print(bitmap.entries);
	printf("\n    ");
	for (int i = 0;i<8;i++){
		fflush(stdout);
		printf("❏ ");
		usleep(30000);
	}

	printf("\n");
	ret =  BitMap_set(&bitmap,3, 1);
	if (ret != -1 ) { printf("\n    Output after Bitmap_set(3, 1) :");
			   binary_print_ablock(bitmap.entries,3);
			   printf("\n    ");
			   binary_print_block(bitmap.entries,3);
			   }
	else printf("TEST FAILED\n");

	getchar();

	ret =  BitMap_set(&bitmap,3, 0);
	if (ret != -1 ) { printf("\n    Output after Bitmap_set(3, 0) :");
			   binary_print_ablock(bitmap.entries,3);
			   printf("\n    ");
			   binary_print_block(bitmap.entries,3);
			    }
			  
	else printf("TEST FAILED\n");
	
	getchar();
	ret =  BitMap_set(&bitmap,4, 1);
	if (ret != -1 ) { printf("\n    Output after Bitmap_set(4, 1) :");
			   binary_print_ablock(bitmap.entries,4);
			   printf("\n    ");
			   binary_print_block(bitmap.entries,4);
			   }
	else printf("TEST FAILED\n");
	
	getchar();
	ret =  BitMap_set(&bitmap,2, 1);
	if (ret != -1 ) { printf("\n    Output after Bitmap_set(2, 1) :");
			   binary_print_ablock(bitmap.entries,2);
			   printf("\n    ");
			   binary_print_block(bitmap.entries,2);
			   }
	else printf("TEST FAILED\n");

	getchar(); 
	ret =  BitMap_set(&bitmap,9, 1);
	if (ret != -1 ) { printf("\n    Output after Bitmap_set(9, 1) :");
			   
			   binary_print_ablock(bitmap.entries,9);
			   printf("\n    ");
			   binary_print_block(bitmap.entries,9);
			   }
	else printf("TEST FAILED\n");

	getchar(); 
	ret =  BitMap_set(&bitmap,20, 1);
	if (ret != -1 ) { printf("\n    Output after Bitmap_set(20, 1) :");
			   binary_print_ablock(bitmap.entries,20);
			   printf("\n    ");
			   binary_print_block(bitmap.entries,20);
			   }
	else printf("TEST FAILED\n");

	getchar();
	ret =  BitMap_set(&bitmap,3, 1);
	if (ret != -1 ) { printf("\n    Output after Bitmap_set(3, 1) :");
			   binary_print_ablock(bitmap.entries,3);
			   printf("\n    ");
			   binary_print_block(bitmap.entries,3);
			   }
	else printf("TEST FAILED\n");

	getchar();
	printf("\n");
	printf("    STAMPO BITMAP ATTUALE in bit: \n\n    ");
	for (int i=0;i/8<strlen(bitmap.entries);i+=8){
		binary_print_ablock(bitmap.entries,i);
		printf("\n    ");
	}
	printf("\n    STAMPO BITMAP ATTUALE in blocchi: \n\n    ");
	for (int i=0;i/8<strlen(bitmap.entries);i+=8){
		binary_print_block(bitmap.entries,i);
		printf("\n    ");
	}

	printf("\n\n    CLICCA ENTER PER ANDARE AVANTI ");
	getchar();
	printf("\n\n\n");
	
	//JM Effettuo il test sulla BitMapGet()
		printf("\n\n--- Test BitMap_get()");
		int start = 1, status = 0;    
		printf("\n    Partiamo dalla posizione %d e cerchiamo %d => %d", start, status, BitMap_get(&bitmap, start, status));
		
		getchar();
		start = 2, status = 1;
		printf("\n    Partiamo dalla posizione %d e cerchiamo %d => %d", start, status, BitMap_get(&bitmap, start, status));
		
		getchar();
		start = 7, status = 0;
		printf("\n    Partiamo dalla posizione %d e cerchiamo %d => %d", start, status, BitMap_get(&bitmap, start, status));
		
		getchar();
		start = 3, status = 1;
		printf("\n    Partiamo dalla posizione %d e cerchiamo %d => %d", start, status, BitMap_get(&bitmap, start, status));

		getchar();
		start = 14, status = 1;
		printf("\n    Partiamo dalla posizione %d e cerchiamo %d => %d", start, status, BitMap_get(&bitmap, start, status));

		getchar();
		start = 19, status = 1;
		printf("\n    Partiamo dalla posizione %d e cerchiamo %d => %d", start, status, BitMap_get(&bitmap, start, status));
		printf("\n");



  	
  	
  }
  else if (choose ==2){
	  	
	printf("██████╗░██╗░██████╗██╗░░██╗██████╗░██████╗░██╗██╗░░░██╗███████╗██████╗░\n");
	printf("██╔══██╗██║██╔════╝██║░██╔╝██╔══██╗██╔══██╗██║██║░░░██║██╔════╝██╔══██╗\n");
	printf("██║░░██║██║╚█████╗░█████═╝░██║░░██║██████╔╝██║╚██╗░██╔╝█████╗░░██████╔╝\n");
	printf("██║░░██║██║░╚═══██╗██╔═██╗░██║░░██║██╔══██╗██║░╚████╔╝░██╔══╝░░██╔══██╗\n");
	printf("██████╔╝██║██████╔╝██║░╚██╗██████╔╝██║░░██║██║░░╚██╔╝░░███████╗██║░░██║\n");
	printf("╚═════╝░╚═╝╚═════╝░╚═╝░░╚═╝╚═════╝░╚═╝░░╚═╝╚═╝░░░╚═╝░░░╚══════╝╚═╝░░╚═╝\n");
	printf("Hai scelto di testare il DiskDriver\n");
	getchar();
	//JM effettuo il test DiskDriver_init   
		printf("\n--- Test DiskDriver_init()");
			//inizializzo il DiskDriver e la bitmap
			DiskDriver disk;
			DiskDriver_init(&disk, "file_test/testosterone.txt", 50); 
		printf("\n    Inizializzazione Disk Driver Riuscita");
			BitMap bitmap;
			bitmap.num_bits = disk.header->bitmap_entries * 8;
			bitmap.entries = disk.bitmap_data;
			
		 	//setto tutti 0 nella bitmap
			for(i = 0; i < bitmap.num_bits; i++)
				BitMap_set(&bitmap, i, 0);
				
		printf("\n    Creazione Bitmap e Azzeramento riuscito");
		printf("\n\n    CLICCA ENTER PER ANDARE AVANTI ");
		getchar();
		printf("\n\n\n");
	//JM effettuo il test DiskDriver_getFreeBlock()
		
		printf("\n\n--- Test DiskDriver_getFreeBlock()");
		printf("\n    Il primo blocco libero è %d \n", disk.header->first_free_block); 
		printf("\n\n    CLICCA ENTER PER ANDARE AVANTI ");
		getchar();
		printf("\n\n\n");
	//JM effettuo il test DiskDriver_writeBlock()  
		printf("\n--- Test DiskDriver_writeBlock()");
		printf("\n\n    Output bitmap entries before DiskDriver_writeBlock() : ");
		binary_print(bitmap.entries);
		getchar();
		
		ret =  DiskDriver_writeBlock(&disk, "Panino",1);
		if(ret == 0) printf("\n    Scrittura <Panino> sul blocco 1 con SUCCESSO : ");
		else printf("TEST FAILED\n");
		binary_print_ablock(bitmap.entries,1);
		printf("\n    ");
		binary_print_block(bitmap.entries,1);
		getchar();


		ret =  DiskDriver_writeBlock(&disk, "Alea Iacta Est",2);
		if(ret == 0) printf("    Scrittura <Alea Iacta Est> sul blocco 2 con SUCCESSO : ");
		else printf("TEST FAILED\n");
		binary_print_ablock(bitmap.entries,2);
		printf("\n    ");
		binary_print_block(bitmap.entries,2);
		getchar();

		ret =  DiskDriver_writeBlock(&disk, "Test",23);
		if(ret == 0) printf("    Scrittura <Test> sul blocco 23 con SUCCESSO : ");
		else printf("TEST FAILED\n");
		binary_print_ablock(bitmap.entries,23);
		printf("\n    ");
		binary_print_block(bitmap.entries,23);
		getchar();

		
		ret =  DiskDriver_writeBlock(&disk, "Tastiera",14);
		if(ret == 0) printf("    Scrittura <Tastiera> sul blocco 14 con SUCCESSO : ");
		else printf("TEST FAILED\n");
		binary_print_ablock(bitmap.entries,14);
		printf("\n    ");
		binary_print_block(bitmap.entries,14);
		getchar();

		printf("\n");
		printf("    STAMPO BITMAP ATTUALE in bit: \n\n    ");
		for (int i=0;i/8<strlen(bitmap.entries);i+=8){
			binary_print_ablock(bitmap.entries,i);
			printf("\n    ");
		}
		printf("\n    STAMPO BITMAP ATTUALE in blocchi: \n\n    ");
		for (int i=0;i/8<strlen(bitmap.entries);i+=8){
			binary_print_block(bitmap.entries,i);
			printf("\n    ");
		}
			
		printf("\n");
		
	//JM effettuo il test DiskDriver_readBlock()
		printf("\n\n    CLICCA ENTER PER ANDARE AVANTI ");
		getchar();
		printf("\n\n\n"); 
		printf("\n--- Test DiskDriver_readBlock()");
		
		void * dest = malloc(BLOCK_SIZE);
		
		ret =  DiskDriver_readBlock(&disk, dest,1);
		
		if (ret == -1) printf("\n    TEST FAILED");
		else 	printf("\n    Lettura con successo di <%s> nel blocco 1", (char *) dest);
		getchar(); 
		printf("\n");
		ret =  DiskDriver_readBlock(&disk, dest, 2);
		if (ret == -1) printf("\n    TEST FAILED");
		else printf("    Lettura con successo di <%s> nel blocco 2", (char *) dest);
		getchar(); 
		printf("\n");
		ret =  DiskDriver_readBlock(&disk, dest, 23);
		if (ret == -1) printf("\n    TEST FAILED");
		else printf("    Lettura con successo di <%s> nel blocco 23", (char *) dest);
		getchar(); 
		printf("\n");
		ret =  DiskDriver_readBlock(&disk, dest, 14);
		if (ret == -1) printf("\n    TEST FAILED");
		printf("    Lettura con successo di <%s> nel blocco 14", (char *) dest);
		getchar(); 
		printf("\n");
	
		printf("\n");

		printf("\n\n    CLICCA ENTER PER ANDARE AVANTI ");
		getchar();
		printf("\n\n\n"); 
		//JM Effettuo test su  DiskDriver_freeBlock
		printf("\n\n--- Test DiskDriver_freeBlock()");
		
		printf("\n     Output bitmap entries before DiskDriver_freeBlock() : ");
			printf("    STAMPO BITMAP ATTUALE in bit: \n\n    ");
		for (int i=0;i/8<strlen(bitmap.entries);i+=8){
			binary_print_ablock(bitmap.entries,i);
			printf("\n    ");
		}
		printf("\n    STAMPO BITMAP ATTUALE in blocchi: \n\n    ");
		for (int i=0;i/8<strlen(bitmap.entries);i+=8){
			binary_print_block(bitmap.entries,i);
			printf("\n    ");
		}
			
		getchar(); 
	
		printf("     Output bitmap entries after DiskDriver_freeBlock(14) : ");
		ret =  DiskDriver_freeBlock(&disk, 14);
	   	binary_print_ablock(bitmap.entries,14);
		printf("\n    ");
		binary_print_block(bitmap.entries,14);
		getchar();

		printf("     Output bitmap entries after DiskDriver_freeBlock(23) : ");
		ret =  DiskDriver_freeBlock(&disk, 23);
		binary_print_ablock(bitmap.entries,23);
		printf("\n    ");
		binary_print_block(bitmap.entries,23);
		getchar();

		printf("     Output bitmap entries after DiskDriver_freeBlock(2) : ");
		ret =  DiskDriver_freeBlock(&disk, 2);
		binary_print_ablock(bitmap.entries,2);
		printf("\n    ");
		binary_print_block(bitmap.entries,2);
		getchar();
		printf("     Output bitmap entries after DiskDriver_freeBlock(1) : ");
		ret =  DiskDriver_freeBlock(&disk, 1);
		
		binary_print_ablock(bitmap.entries,1);
		printf("\n    ");
		binary_print_block(bitmap.entries,1);
		getchar();
		free(dest);
		
  }
  else if (choose ==3){
  	
	printf("░██████╗██╗███╗░░░███╗██████╗░██╗░░░░░███████╗███████╗░██████╗\n");
	printf("██╔════╝██║████╗░████║██╔══██╗██║░░░░░██╔════╝██╔════╝██╔════╝\n");
	printf("╚█████╗░██║██╔████╔██║██████╔╝██║░░░░░█████╗░░█████╗░░╚█████╗░\n");
	printf("░╚═══██╗██║██║╚██╔╝██║██╔═══╝░██║░░░░░██╔══╝░░██╔══╝░░░╚═══██╗\n");
	printf("██████╔╝██║██║░╚═╝░██║██║░░░░░███████╗███████╗██║░░░░░██████╔╝\n");
	printf("╚═════╝░╚═╝╚═╝░░░░░╚═╝╚═╝░░░░░╚══════╝╚══════╝╚═╝░░░░░╚═════╝░\n");
	printf("Hai scelto di testare il SimpleFS\n");

	// ls

	getchar();	
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
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Chiamata prima della SimpleFS_createFile (mi aspetto 0 file nella cartella)\n");
	printf("--- Test SimpleFS_readDir()\n");
		char* names[50];
		for(i = 0; i < sizeof(names)/sizeof(char*); i++){
			names[i] = malloc(128*sizeof(char));
		}

		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		
		printf("\n");
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Creo un nuovo file chiamato 'file1'\n");
	printf("--- Test SimpleFS_createFile()\n");
		FileHandle* f1 = SimpleFS_createFile(d, "file1");
		assert(f1 != NULL);
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Chiamata dopo la SimpleFS_createFile (mi aspetto 1 file nella cartella)\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir(names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Provo ad aprire il file 'file2' che NON esiste nella directory corrente\n");
	printf("--- Test SimpleFS_openFile()\n");
		FileHandle* file_not_exists = SimpleFS_openFile(d, "file2");
		assert(file_not_exists == NULL);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Creo un nuovo file chiamato 'file2'\n");
	printf("--- Test SimpleFS_createFile()\n");
		FileHandle* f2 = SimpleFS_createFile(d, "file2");
		assert(f2 != NULL);
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Chiamata dopo la SimpleFS_createFile (mi aspetto 2 file nella cartella)\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Provo a creare un file chiamato 'file2' che esiste già\n");
	printf("--- Test SimpleFS_createFile()\n");
		FileHandle* fexists = SimpleFS_createFile(d, "file2");
		assert(fexists == NULL);
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Creo una nuova directory 'dir1' nella directory corrente (aka: '/')\n");
	printf("--- Test SimpleFS_mkDir()\n");
		ret = SimpleFS_mkDir(d, "dir1");
		assert(ret == 0);
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Mi aspetto 2 file e 1 cartella\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Entro dentro la directory 'dir1'\n");
	printf("--- Test SimpleFS_changeDir\n");
		ret = SimpleFS_changeDir(d, "dir1");
		assert(ret == 0);
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Mi aspetto 0 file nella directory\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);

		printf("\n");
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Creo un nuovo file chiamato 'file3'\n");
	printf("--- Test SimpleFS_createFile()\n");
		FileHandle* f3 = SimpleFS_createFile(d, "file3");
		assert(f3 != NULL);
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Mi aspetto 1 file nella directory\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Creo un nuovo file chiamato 'file4'\n");
	printf("--- Test SimpleFS_createFile()\n");
		FileHandle* f4 = SimpleFS_createFile(d, "file4");
		assert(f4 != NULL);
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Mi aspetto 2 file nella directory\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Torno alla directory padre di 'dir1' (aka: '/')\n");
	printf("--- Test SimpleFS_changeDir\n");
		ret = SimpleFS_changeDir(d, "..");
		assert(ret == 0);
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Mi aspetto 2 file e 1 cartella nella directory\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Creo una nuova directory 'dir2' nella directory corrente (aka: '/')\n");
	printf("--- Test SimpleFS_mkDir()\n");
		ret = SimpleFS_mkDir(d, "dir2");
		assert(ret == 0);
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Mi aspetto 2 file e 2 cartelle nella directory\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Entro dentro la directory 'dir2'\n");
	printf("--- Test SimpleFS_changeDir\n");
		ret = SimpleFS_changeDir(d, "dir2");
		assert(ret == 0);
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Mi aspetto 0 file nella directory\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);

		printf("\n");
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Creo un nuovo file chiamato 'file5'\n");
	printf("--- Test SimpleFS_createFile()\n");
		FileHandle* f5 = SimpleFS_createFile(d, "file5");
		assert(f5 != NULL);
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Mi aspetto 1 file nella directory\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Creo una nuova directory 'dir3' nella directory corrente (aka: 'dir2')\n");
	printf("--- Test SimpleFS_mkDir()\n");
		ret = SimpleFS_mkDir(d, "dir3");
		assert(ret == 0);
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Mi aspetto 1 file e 1 cartella nella directory\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Provo ad entrare dentro la directory 'dir8' che NON esiste\n");
	printf("--- Test SimpleFS_changeDir\n");
		ret = SimpleFS_changeDir(d, "dir8");
		assert(ret < 0);
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Entro dentro la directory padre di 'dir2' (aka: '/')\n");
	printf("--- Test SimpleFS_changeDir\n");
		ret = SimpleFS_changeDir(d, "..");
		assert(ret == 0);
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Mi aspetto 2 file e 2 cartelle nella directory\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Rimuovo il file 'file2' nella directory corrente\n");
	printf("--- Test SimpleFS_remove()\n");
		ret = SimpleFS_remove(d, "file2");
		assert(ret >= 0);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Mi aspetto 1 file e 2 cartelle nella directory\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Rimuovo la cartella 'dir2' nella directory corrente\n");
	printf("--- Test SimpleFS_remove()\n");
		ret = SimpleFS_remove(d, "dir2");
		assert(ret >= 0);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Mi aspetto 1 file e 1 cartelle nella directory\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Provo a rimuovere il file 'file4' che NON esiste nella directory corrente\n");
	printf("--- Test SimpleFS_remove()\n");
		ret = SimpleFS_remove(d, "file4");
		assert(ret < 0);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("    Mi aspetto 1 file e 2 cartelle nella directory\n");
	printf("--- Test SimpleFS_readDir()\n");
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		for(i = 0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Apro il file 'file1' nella directory corrente\n");
	printf("--- Test SimpleFS_openFile()\n");
		FileHandle* file1 = SimpleFS_openFile(d, "file1");
		printf("    Ho aperto il file '%s' nella directory '%s'\n", file1->fcb->fcb.name, d->dcb->fcb.name);
		assert(!strcmp(file1->fcb->fcb.name, f1->fcb->fcb.name));
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Scrivo nel file 'file1' 10 bytes di 'a'\n");
	printf("--- Test SimpleFS_write()\n");
		char write_data[50] = {0};
		memset(write_data, 'a', 10);
		ret = SimpleFS_write(file1, write_data, 10);
		printf("    Ora il file '%s' è grande %d bytes\n", file1->fcb->fcb.name, file1->fcb->fcb.size_in_bytes);
		assert(ret == 10 && file1->fcb->fcb.size_in_bytes == 10);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Sposto il cursore di 'file1' alla posizione 0\n");
	printf("--- Test SimpleFS_seek()\n");
		ret = SimpleFS_seek(file1, 0);
		printf("    Ho letto dal file '%s' %d bytes\n", file1->fcb->fcb.name, ret);
		assert(ret == 10);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo dal file 'file1' 10 bytes dalla posizione corrente\n");
	printf("--- Test SimpleFS_read()\n");
		char read_data[128];
		ret = SimpleFS_read(file1, read_data, 10);
		printf("    Ho letto dal file '%s' i seguenti %d bytes:\n", file1->fcb->fcb.name, ret);
		for(i = 0; i < ret; i++){
			printf("    \tByte #%d: \t%c\n", i+1, read_data[i]);
		}
		assert(ret == 10);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Sposto il cursore di 'file1' alla posizione 0\n");
	printf("--- Test SimpleFS_seek()\n");
		ret = SimpleFS_seek(file1, 0);
		printf("    Ho letto dal file '%s' %d bytes\n", file1->fcb->fcb.name, ret);
		assert(ret == 10);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Sposto il cursore di 'file1' alla posizione 7\n");
	printf("--- Test SimpleFS_seek()\n");
		ret = SimpleFS_seek(file1, 7);
		printf("    Ho letto dal file '%s' %d bytes\n", file1->fcb->fcb.name, ret);
		assert(ret == 7);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Scrivo nel file 'file1' 10 bytes di 'b' dalla posizione corrente\n");
	printf("--- Test SimpleFS_write()\n");
		memset(write_data, 'b', 10);
		ret = SimpleFS_write(file1, write_data, 10);
		printf("    Ora il file '%s' è grande %d bytes\n", file1->fcb->fcb.name, file1->fcb->fcb.size_in_bytes);
		assert(ret == 10 && file1->fcb->fcb.size_in_bytes == 17);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Sposto il cursore di 'file1' alla posizione 0\n");
	printf("--- Test SimpleFS_seek()\n");
		ret = SimpleFS_seek(file1, 0);
		printf("    Ho letto dal file '%s' %d bytes\n", file1->fcb->fcb.name, ret);
		assert(ret == 17);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Leggo dal file 'file1' 17 bytes dalla posizione corrente\n");
	printf("--- Test SimpleFS_read()\n");
		ret = SimpleFS_read(file1, read_data, 17);
		printf("    Ho letto dal file '%s' i seguenti %d bytes:\n", file1->fcb->fcb.name, ret);
		for(i = 0; i < ret; i++){
			printf("    \tByte #%d: \t%c\n", i+1, read_data[i]);
		}
		assert(ret == 17);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Chiudo il FileHandle f1 per il file 'file1'\n");
	printf("--- Test SimpleFS_close()\n");
		ret = SimpleFS_close(f1);
		assert(ret >= 0);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Chiudo il FileHandle file1 per il file 'file1'\n");
	printf("--- Test SimpleFS_close()\n");
		ret = SimpleFS_close(file1);
		assert(ret >= 0);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Chiudo il FileHandle per il file 'file2'\n");
	printf("--- Test SimpleFS_close()\n");
		ret = SimpleFS_close(f2);
		assert(ret >= 0);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Chiudo il FileHandle per il file 'file3'\n");
	printf("--- Test SimpleFS_close()\n");
		ret = SimpleFS_close(f3);
		assert(ret >= 0);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Chiudo il FileHandle per il file 'file4'\n");
	printf("--- Test SimpleFS_close()\n");
		ret = SimpleFS_close(f4);
		assert(ret >= 0);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	printf("    Chiudo il FileHandle per il file 'file5'\n");
	printf("--- Test SimpleFS_close()\n");
		ret = SimpleFS_close(f5);
		assert(ret >= 0);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

	// ls pulizia della memoria allocata
	for(i = 0; i < sizeof(names)/sizeof(char*); i++){
		free(names[i]);
	}

	free(d->directory);
	free(d->dcb);
	free(d);

  }
}
