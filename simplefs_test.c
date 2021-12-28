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

// binary print from stackoverflow
void binary_print(char* str) {
	 if (strlen(str) == 0) printf("00000000");
	else{
	int i, j;
	for(i = 0; i < strlen(str); i++) {
		char c = str[i];
		for (j = 7; j >= 0; j--) {
	      printf("%d", !!((c >> j) & 0x01)); 
	  }
	}
	} 
}

//JM first look to bitmap test
int main(int agc, char** argv) {
		
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
  	printf("Hai scelto di testare la BitMap\n");
  	
  	// JM Eseguo il Test per la funzione BitMap_blockToIndex
	int num = 420;
	printf("\n--- Test BitMap_blockToIndex(%d)\n", num);   
	BitMapEntryKey block = BitMap_blockToIndex(num);
	printf("    La posizione del blocco è %d, ovvero la entry_num %d al bit_num %d", num, block.entry_num, block.bit_num);
	
	// JM Eseguo il Test BitMap_indexToBlock 
	
	printf("\n\n--- Test BitMap_indexToBlock(block.entry_num, block.bit_num)");
	int posizione = BitMap_indexToBlock(block.entry_num, block.bit_num); 
		printf("\n    Abbiamo la entry %d e il bit %d, ovvero la posizione %d", block.entry_num, block.bit_num, posizione);
	
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
		
	printf("\n\n--- Test BitMap_set(BitMap* bitmap, int pos, int status) :");
	printf("\n    Output before BitMap_set()    :");
	
	 binary_print(bitmap.entries);
	ret =  BitMap_set(&bitmap,3, 1);
	if (ret != -1 ) { printf("\n    Output after Bitmap_set(3, 1) :");
			   binary_print(bitmap.entries);
			   }
	else printf("TEST FAILED\n");
	ret =  BitMap_set(&bitmap,3, 0);
	if (ret != -1 ) { printf("\n    Output after Bitmap_set(3, 0) :");
			   binary_print(bitmap.entries); }
			  
	else printf("TEST FAILED\n");
	ret =  BitMap_set(&bitmap,4, 1);
	if (ret != -1 ) { printf("\n    Output after Bitmap_set(4, 1) :");
			   binary_print(bitmap.entries);
			   }
	else printf("TEST FAILED");
	ret =  BitMap_set(&bitmap,2, 1);
	if (ret != -1 ) { printf("\n    Output after Bitmap_set(2, 1) :");
			   binary_print(bitmap.entries);
			   }
	else printf("TEST FAILED\n");
	printf("\n"); 
	
	
	//JM Effettuo il test sulla BitMapGet()
		printf("\n\n--- Test BitMap_get()");
		int start = 1, status = 0;    
		printf("\n    Partiamo dalla posizione %d e cerchiamo %d => %d", start, status, BitMap_get(&bitmap, start, status));
		start = 2, status = 1;
		printf("\n    Partiamo dalla posizione %d e cerchiamo %d => %d", start, status, BitMap_get(&bitmap, start, status));
		start = 7, status = 0;
		printf("\n    Partiamo dalla posizione %d e cerchiamo %d => %d", start, status, BitMap_get(&bitmap, start, status));
		start = 3, status = 1;
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
	//JM effettuo il test DiskDriver_getFreeBlock()
	
		printf("\n\n--- Test DiskDriver_getFreeBlock()");
		printf("\n    Il primo blocco libero è %d \n", disk.header->first_free_block); 
		
	//JM effettuo il test DiskDriver_writeBlock()  
		printf("\n--- Test DiskDriver_writeBlock()");
		printf("\n    Output bitmap entries before DiskDriver_writeBlock() : ");
		binary_print(bitmap.entries);
		
		ret =  DiskDriver_writeBlock(&disk, "Panino",1);
		if(ret == 0) printf("\n    Scrittura <Panino> sul blocco 1 con SUCCESSO : ");
		else printf("TEST FAILED\n");
		binary_print(bitmap.entries);
		
		ret =  DiskDriver_writeBlock(&disk, "Alea Iacta Est",2);
		if(ret == 0) printf("\n    Scrittura <Alea Iacta Est> sul blocco 2 con SUCCESSO : ");
		else printf("TEST FAILED\n");
		binary_print(bitmap.entries);
		
		ret =  DiskDriver_writeBlock(&disk, "Test",3);
		if(ret == 0) printf("\n    Scrittura <Test> sul blocco 3 con SUCCESSO : ");
		else printf("TEST FAILED\n");
		binary_print(bitmap.entries);
		
		ret =  DiskDriver_writeBlock(&disk, "Tastiera",4);
		if(ret == 0) printf("\n    Scrittura <Tastiera> sul blocco 4 con SUCCESSO : ");
		else printf("TEST FAILED\n");
		binary_print(bitmap.entries);
		
		printf("\n    Output bitmap entries after DiskDriver_writeBlock()  : ");
		binary_print(bitmap.entries); 
		
		printf("\n");
		
	//JM effettuo il test DiskDriver_readBlock()
		 
		printf("\n--- Test DiskDriver_readBlock()");
		printf("\n    Output bitmap entries before DiskDriver_readBlock() : ");
		binary_print(bitmap.entries);
		 
		void * dest = malloc(BLOCK_SIZE);
		
		ret =  DiskDriver_readBlock(&disk, dest,1);
		
		if (ret == -1) printf("\n    TEST FAILED");
		else 	printf("\n    Lettura con successo di <%s> nel blocco 1", (char *) dest);

	
		ret =  DiskDriver_readBlock(&disk, dest, 2);
		if (ret == -1) printf("\n    TEST FAILED");
		else printf("\n    Lettura con successo di <%s> nel blocco 2", (char *) dest);
		
		ret =  DiskDriver_readBlock(&disk, dest, 3);
		if (ret == -1) printf("\n    TEST FAILED");
		else printf("\n    Lettura con successo di <%s> nel blocco 3", (char *) dest);
		
		ret =  DiskDriver_readBlock(&disk, dest, 4);
		if (ret == -1) printf("\n    TEST FAILED");
		printf("\n    Lettura con successo di <%s> nel blocco 4", (char *) dest);
		
		printf("\n    Output bitmap entries after DiskDriver_readBlock()  : ");
		binary_print(bitmap.entries); 
		printf("\n");
		
		//JM Effettuo test su  DiskDriver_freeBlock
		printf("\n\n--- Test DiskDriver_freeBlock()");
		
		printf("\n     Output bitmap entries before DiskDriver_readBlock() : ");
		binary_print(bitmap.entries);
		
		printf("\n     Output bitmap entries after DiskDriver_readBlock(4) : ");
		ret =  DiskDriver_freeBlock(&disk, 4);
	   	binary_print(bitmap.entries);
	   	
		printf("\n     Output bitmap entries after DiskDriver_readBlock(3) : ");
		ret =  DiskDriver_freeBlock(&disk, 3);
		binary_print(bitmap.entries);
		
		printf("\n     Output bitmap entries after DiskDriver_readBlock(2) : ");
		ret =  DiskDriver_freeBlock(&disk, 2);
		binary_print(bitmap.entries);
		
		printf("\n     Output bitmap entries after DiskDriver_readBlock(1) : ");
		ret =  DiskDriver_freeBlock(&disk, 1);
		binary_print(bitmap.entries);
		printf("\n");
		
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
	printf("\n--- Creo e inizializzo un nuovo disco\n");
		//inizializzo il DiskDriver e la bitmap
		DiskDriver disk;
		DiskDriver_init(&disk, "file_test/testosterone.txt", 50); 
	printf("    Inizializzazione Disk Driver Riuscita\n");
		BitMap bitmap;
		bitmap.num_bits = disk.header->bitmap_entries * 8;
		bitmap.entries = disk.bitmap_data;
	
	 	//setto tutti 0 nella bitmap
		for(i = 0; i < bitmap.num_bits; i++)
		BitMap_set(&bitmap, i, 0);
				
	printf("    Creazione Bitmap e Azzeramento riuscito\n");

	printf("    Formatto e inizializzo il filesystem\n");
	printf("--- Test SimpleFS_init()\n");
		SimpleFS fs;
		DirectoryHandle* d = SimpleFS_init(&fs, &disk);
		assert(d != NULL);
	printf("    TEST PASSED\n");

	printf("    Leggo il nome di tutti i file nella cartella corrente\n");
	printf("--- Test SimpleFS_readDir\n");
		char names[128][128];
		ret = SimpleFS_readDir((char**)names, d);
		assert(ret >= 0);
		printf("    Ho trovato %d file(s) nella cartella corrente:\n", ret);
		
		printf("\n");
		for(i=0; i < ret; i++){
			printf("     \t%s\n", names[i]);
		}
	printf("    TEST PASSED\n");

  }
}
