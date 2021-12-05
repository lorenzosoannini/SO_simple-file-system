#include "bitmap.c" 
#include "disk_driver.c"
#include "simplefs.c"
#include <stdio.h>

int choose;

//JM first look to bitmap test
int main(int agc, char** argv) {
  printf("FirstBlock size %ld\n", sizeof(FirstFileBlock));
  printf("DataBlock size %ld\n", sizeof(FileBlock));
  printf("FirstDirectoryBlock size %ld\n", sizeof(FirstDirectoryBlock));
  printf("DirectoryBlock size %ld\n", sizeof(DirectoryBlock));
  printf("\n");
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
  	
  	// Eseguo il Test per la funzione BitMap_blockToIndex
	int num = 420;
	printf("\n--- Test BitMap_blockToIndex(%d)\n", num);   
	BitMapEntryKey block = BitMap_blockToIndex(num);
	printf("    La posizione del blocco è %d, ovvero la entry_num %d al bit_num %d\n", num, block.entry_num, block.bit_num);
	
	// Test BitMap_indexToBlock 
	printf("\n\n--- Test BitMap_indexToBlock(block)");
	int pos = BitMap_indexToBlock(block.entry_num,block.bit_num); 
	printf("\n    Abbiamo la entry_num %d e il bit_num %d, ovvero la posizione %d\n", block.entry_num, block.bit_num, pos);

  	
  	
  }
  else if (choose ==2){
	  	
	printf("██████╗░██╗░██████╗██╗░░██╗██████╗░██████╗░██╗██╗░░░██╗███████╗██████╗░\n");
	printf("██╔══██╗██║██╔════╝██║░██╔╝██╔══██╗██╔══██╗██║██║░░░██║██╔════╝██╔══██╗\n");
	printf("██║░░██║██║╚█████╗░█████═╝░██║░░██║██████╔╝██║╚██╗░██╔╝█████╗░░██████╔╝\n");
	printf("██║░░██║██║░╚═══██╗██╔═██╗░██║░░██║██╔══██╗██║░╚████╔╝░██╔══╝░░██╔══██╗\n");
	printf("██████╔╝██║██████╔╝██║░╚██╗██████╔╝██║░░██║██║░░╚██╔╝░░███████╗██║░░██║\n");
	printf("╚═════╝░╚═╝╚═════╝░╚═╝░░╚═╝╚═════╝░╚═╝░░╚═╝╚═╝░░░╚═╝░░░╚══════╝╚═╝░░╚═╝\n");
	printf("Hai scelto di testare il DiskDriver\n");
  }
  else {
  	
	printf("░██████╗██╗███╗░░░███╗██████╗░██╗░░░░░███████╗███████╗░██████╗\n");
	printf("██╔════╝██║████╗░████║██╔══██╗██║░░░░░██╔════╝██╔════╝██╔════╝\n");
	printf("╚█████╗░██║██╔████╔██║██████╔╝██║░░░░░█████╗░░█████╗░░╚█████╗░\n");
	printf("░╚═══██╗██║██║╚██╔╝██║██╔═══╝░██║░░░░░██╔══╝░░██╔══╝░░░╚═══██╗\n");
	printf("██████╔╝██║██║░╚═╝░██║██║░░░░░███████╗███████╗██║░░░░░██████╔╝\n");
	printf("╚═════╝░╚═╝╚═╝░░░░░╚═╝╚═╝░░░░░╚══════╝╚══════╝╚═╝░░░░░╚═════╝░\n");
	printf("Hai scelto di testare il SimpleFS\n");
  }
}
