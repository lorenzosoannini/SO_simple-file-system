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

	getchar();	
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

	printf("    Scrivo nel file 'file1' 10 bytes di '1'\n");
	printf("--- Test SimpleFS_write()\n");
		char data[10] = {1};
		ret = SimpleFS_write(file1, data, sizeof(data));
		printf("    Ora il file '%s' è grande %d bytes\n", file1->fcb->fcb.name, file1->fcb->fcb.size_in_bytes);
		assert(ret == sizeof(data) && file1->fcb->fcb.size_in_bytes == 10);
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

	printf("    Scrivo nel file 'file1' 10 bytes di '0' dalla posizione corrente\n");
	printf("--- Test SimpleFS_write()\n");
		memset(data, 0, 10);
		ret = SimpleFS_write(file1, data, sizeof(data));
		printf("    Ora il file '%s' è grande %d bytes\n", file1->fcb->fcb.name, file1->fcb->fcb.size_in_bytes);
		assert(ret == sizeof(data) && file1->fcb->fcb.size_in_bytes == 17);
		printf("    TEST PASSED\n");
	printf("\n");
	printf("Premi ENTER per continuare\n");  
	getchar();
	printf("\n");

  }
}
