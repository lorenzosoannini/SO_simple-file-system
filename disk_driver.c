#include "disk_driver.h"
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include <unistd.h>
#include <stdlib.h>

//JM Apre il file (o lo crea) 
// alloca lo spazio necessario nel disco
// calcola quanto grande dovrebbe essere 
// se il file è nuovo compila un nuovo disk header,
// lo riempe  nella bitmap di dimensione appropiata con tutti 0 
// ad indicare gli spazi vuoti


void DiskDriver_init(DiskDriver* disk, const char* filename, int num_blocks){
	
	//variabile f dove memorizzo il file
	int f;
	
	//quanti bytes devo memorizzare nel disco
	int bitmap_entries = num_blocks/8;

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
		
		 posix_fallocate(f, 0, sizeof(DiskHeader) + bitmap_entries + num_blocks*BLOCK_SIZE);
		//assegno i valori int fd e DiskHeader header 
		
		disk->fd = f;
		disk->header = (DiskHeader*) mmap(0, sizeof(DiskHeader) + bitmap_entries + (num_blocks*BLOCK_SIZE), PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
		
		
	}else{
	//JM se il file non esiste lo creo
		f = open(filename, O_CREAT | O_RDWR, 0666);
		if(!f) {
			printf("C'è stato un errore nella creazione del file. Il programma è stato bloccato.");
			return;
		}
		
		// risoluzione errore di bus
		posix_fallocate(f, 0, sizeof(DiskHeader) + bitmap_entries + num_blocks*BLOCK_SIZE);
		
		disk->fd=f;
		//creo un nuovo DiskHeader e lo riempo nella bitmap n
		disk->header = (DiskHeader*) mmap(0, sizeof(DiskHeader) + bitmap_entries + (num_blocks*BLOCK_SIZE), PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
		
		int conta = bitmap_entries % BLOCK_SIZE == 0 ? bitmap_entries / BLOCK_SIZE : ( bitmap_entries / BLOCK_SIZE ) + 1;
		
		disk->header->num_blocks = num_blocks;
		disk->header->bitmap_blocks = conta;
		disk->header->bitmap_entries = bitmap_entries; // numero di bytes nella bitmap
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
	
	//JM devo verificare che il blocco di numeri sia minore del blocco di numeri effettivo altrimenti restituisco un errore
	if(block_num > disk->header->num_blocks) return -1;


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
	// ritorno 0 se è andato tutto a buon fine
	return 0;
}
// JM scrive un blocco in posizione block_num  e modifica consapevolmente la bitmap e ritorna -1 in caso di errore

int DiskDriver_writeBlock(DiskDriver * disk, void * src, int block_num) {
	
	// Come nella funzione read il numero del blocco da scrivere non deve essere  maggiore del numero di blocchi block_num
	if(block_num > disk->header->num_blocks) return -1;
 
	// come in readBlock inizializzo una bitmap che mi permette di utilizzare la bitmap_get() e la bitmap_set()
	BitMap bitmap;
	bitmap.num_bits = disk->header->bitmap_entries * 8;
	bitmap.entries = disk->bitmap_data;
	
	// Se il blocco è libero allora decremento free_block
	//utilizzo la bitmapget
	int ret = BitMap_get(&bitmap, block_num, 0);
	
	// se è diverso  a block_num il blocco è occupato quindi restituisco -1;
	if (ret !=block_num) return -1;
	// il blocco è libero quindi ci posso scrivere, decremento i blocchi liberi nell header
	else disk->header->free_blocks--;


	//setto nella bitmap il block_num occupato
	BitMap_set(&bitmap, block_num, 1);
	//setto destinatario il blocco di block_num
	char * dest = disk->bitmap_data + disk->header->bitmap_entries + (block_num * BLOCK_SIZE);
	// Scrivo il contenuto di src in dest ovvero il blocco di block_num block_num
	memcpy(dest, src, BLOCK_SIZE);		

	
	// JM Faccio il flush del driver
	ret = DiskDriver_flush(disk);
	//controllo fallimento del diskdriver_flush
	if (ret == -1) return -1;

  return 0;
}
//JM Ritorna il primo blocco libero che incontro da start
int DiskDriver_getFreeBlock(DiskDriver* disk, int start) {
	
	// Creo la bitmap che andrò ad utilizzare per la BitMap_get()
	BitMap bmap;
	bmap.num_bits = disk->header->bitmap_entries * 8;
	bmap.entries = disk->bitmap_data;

	// JM attravero la bitmap_get posso trovare il primo blocco libero da start
	int ret = BitMap_get(&bmap, start, 0);
	return ret;
	
}
//JM questa funzione mi permette di liberare il blocco in block_num del diskDriver

int DiskDriver_freeBlock(DiskDriver* disk, int block_num) {
	
	// JM Come sempre creo la bitmap di appoggio che utilizzero per la funzione bitmapGet()
	BitMap bmap;
	bmap.num_bits = disk->header->bitmap_entries * 8;
	bmap.entries = disk->bitmap_data;
	
	// JM verifico che il mio block_num sia all'interno del numero di blocchi nel disk header altrimenti restituisco -1
	if(block_num > disk->header->num_blocks) return -1;
	
	// JM verifico se  il blocco in block_num non sia vuoto utilizzo la getFreeBlock() per vedere quale sia il primo blocco libero
	 
	int ret = DiskDriver_getFreeBlock(disk,block_num-1);
	// JM se il primo blocco libero non è block_num incremento il valore dei free_blocks nel disk->header,
	// in caso contrario non è necessario aumentarlo
	if(ret != block_num) disk->header->free_blocks++;

	// JM per impostare il blocco Libero utilizzo la BitMapSet
	BitMap_set(&bmap, block_num, 0);
	disk->bitmap_data = bmap.entries;
	
	//JM faccio il flush del driver
	ret = DiskDriver_flush(disk);
	//controllo fallimento del diskdriver_flush
	if (ret == 0) return -1;
	
	return 0;
}

// JM aggiorna i dati (fa il flush del mmaps)

int DiskDriver_flush(DiskDriver* disk) {
	
	// JM assegno a disk_size la lunghezza della memoria da sincronizzare
	int disk_size = sizeof(DiskHeader) + disk->header->bitmap_entries + (disk->header->num_blocks*BLOCK_SIZE) ;
	
	/*
	JM Utilizzo mysync() che mi permette di sincronizzare la memoria modificata e il file collegato dalla mmap()
	
	msync() cancella le modifiche apportate alla copia interna di un file che
       è stato mappato in memoria usando mmap (2) nel filesystem.
       Senza l'uso di questa chiamata, non vi è alcuna garanzia che le modifiche siano
       riscritto prima che munmap (2) venga richiamato. Per essere più precisi, il
       parte del file che corrisponde all'area di memoria a partire da
       addr e con length length viene aggiornato.
       
	*/ 
	
	// Utilizzo il flag MS_SYNC che richiede un aggiornamento e aspetta il suo completamento
	// Ritorna -1 se il msync è fallito 
	return msync(disk->header, disk_size, MS_SYNC);

}


