#include "disk_driver.h"

//JM Apre il file (o lo crea) 
// alloca lo spazio necessario nel disco
// calcola quanto grande dovrebbe essere 
// se il file è nuovo compila un nuovo disk header,
// lo riempe  nella bitmap di dimensione appropiata con tutti 0 
// ad indicare gli spazi vuoti


void DiskDriver_init(DiskDriver* disk, const char* filename, int num_blocks){

	int f;
	
	//int bitmap_entries = num_blocks;
	
	//calculating the bitmap size (rounded up)
	int bitmap_blocks = (num_blocks + 7) / 8;
	
	size_t occupation = (sizeof(DiskHeader) + bitmap_blocks + sizeof(BitMap) + BLOCK_SIZE - 1) / BLOCK_SIZE;
	
	
	//JM se il file esiste
	if(!access(filename, F_OK)) {
		f = open(filename, O_RDWR, 0666);
		if(!f) {
			printf("C'è stato un errore nell'apertura del file. Il programma è stato bloccato.");
			return;
		}
		//assegno i valori int fd e DiskHeader header 
		disk->fd = f;
		disk->header = (DiskHeader*) mmap(0, sizeof(DiskHeader) + bitmap_entries + (num_blocks*BLOCK_SIZE), PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);
		
		
	}else{
	//JM se il file non esiste lo creo
		f = open(filename, O_CREAT | O_RDWR, 0666);
		if(!f) {
			printf("C'è stato un errore nella creazione del file. Il programma è stato bloccato.");
			return;
		}
		
		disk->fd=f;
		//creo un nuovo DiskHeader e lo riempo nella bitmap n
		disk->header = (DiskHeader*) mmap(0, sizeof(DiskHeader) + bitmap_entries + (num_blocks*BLOCK_SIZE), PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);
		
		
		disk->header->num_blocks = num_blocks;
		disk->header->bitmap_blocks = (bitmap_blocks + sizeof(BitMap) + BLOCK_SIZE - 1) / BLOCK_SIZE;
		disk->header->bitmap_entries = bitmap_blocks;
		disk->header->free_blocks = (int) (num_blocks - occupation);
		disk->header->first_free_block = (int) occupation;
	}

	return;
}
