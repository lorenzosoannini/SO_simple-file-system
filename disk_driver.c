#include "disk_driver.h"

//JM Apre il file (o lo crea) 
// alloca lo spazio necessario nel disco
// calcola quanto grande dovrebbe essere 
// se il file è nuovo compila un nuovo disk header,
// lo riempe  nella bitmap di dimensione appropiata con tutti 0 
// ad indicare gli spazi vuoti


void DiskDriver_init(DiskDriver* disk, const char* filename, int num_blocks){
	
	//variabile f dove memorizzo il file
	int f;
	
	//JM se il file esiste
	if(!access(filename, F_OK)) {
		f = open(filename, O_RDWR, 0666);
		if(!f) {
			printf("C'è stato un errore nell'apertura del file. Il programma è stato bloccato.");
			return;
		}
		
		/*
		JM
		L'ERRORE DI BUS è possibile risolverlo tramite la funzione posix_fallocate()
		
		La funzione posix_fallocate () deve garantire che qualsiasi spazio di archiviazione richiesto per i dati di file regolari a partire da offset e continuando per len byte sia allocato sul supporto di archiviazione del file system. 
		
		Se posix_fallocate() viene restituito correttamente, le successive scritture sui dati del file specificati non falliranno a causa della mancanza di spazio libero sul supporto di memorizzazione del file system.

		*/
		
		 posix_fallocate(f, 0, sizeof(DiskHeader) + num_blocks + num_blocks*BLOCK_SIZE);
		//assegno i valori int fd e DiskHeader header 
		
		disk->fd = f;
		disk->header = (DiskHeader*) mmap(0, sizeof(DiskHeader) + num_blocks + (num_blocks*BLOCK_SIZE), PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
		
		
	}else{
	//JM se il file non esiste lo creo
		f = open(filename, O_CREAT | O_RDWR, 0666);
		if(!f) {
			printf("C'è stato un errore nella creazione del file. Il programma è stato bloccato.");
			return;
		}
		
		// risoluzione errore di bus
		posix_fallocate(f, 0, sizeof(DiskHeader) + num_blocks + num_blocks*BLOCK_SIZE);
		
		disk->fd=f;
		//creo un nuovo DiskHeader e lo riempo nella bitmap n
		disk->header = (DiskHeader*) mmap(0, sizeof(DiskHeader) + num_blocks + (num_blocks*BLOCK_SIZE), PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
		
		
		disk->header->num_blocks = num_blocks;
		disk->header->bitmap_blocks = count_blocks(num_blocks);
		disk->header->bitmap_entries = num_blocks;
		disk->header->free_blocks = num_blocks;
	}
	lseek(f, 0, SEEK_SET);
	// Memorizzo in bitmap_data il puntatore alla mmap saltando lo spazio dedicato a DiskHeader
	disk->bitmap_data = (char *) disk->header + sizeof(DiskHeader);

	// Calcolo il primo blocco libero dopo aver assegnato il valore alle entries
	disk->header->first_free_block = DiskDriver_getFreeBlock(disk,0);
	
	return;
}
// JMlegge il blocco nella posizione block_num
// ritorna -1 se è il blocco è libero come dice nella bitmap
// 0 altrimenti
int DiskDriver_readBlock(DiskDriver* disk, void * dest, int block_num){

	//JM ho bisogno di inizializzare una bitmap per utilizzare la bitmap get 
	BitMap bitmap;
	bitmap.num_bits = disk->header->bitmap_entries * 8;
	bitmap.entries = disk->bitmap_data;
	
	//utilizzo la bitmapget
	int ret = BitMap_get(&bitmap, block_num, 0);
	// se è uguale a block_num il blocco è libero quindi restituisco -1;
	if (ret ==block_num) return -1;
	// assegno a src il blocco block_num
	const void * src = disk->bitmap_data + disk->header->bitmap_entries + (block_num * BLOCK_SIZE);
	//inserisco in dest il valore del blocco block_num
	memcpy(dest, src, BLOCK_SIZE);
	
	return 0;
}
