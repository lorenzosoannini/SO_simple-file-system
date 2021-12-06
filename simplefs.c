#include "simplefs.h"
#include <stdlib.h>


//JM Dichiarazione funzioni e descrizioni di tali

// ls
// inizializza un file system su un disco già creato
// restituisce un handle alla directory di primo livello memorizzata nel primo blocco
DirectoryHandle* SimpleFS_init(SimpleFS* fs, DiskDriver* disk){

    // ls inizializzo la struttura fs con il disco passato alla funzione
    fs->disk = disk;

    // formatto il fs per creare le strutture iniziali
    SimpleFS_format(fs);

    // alloco una struttura data DirectoryHandle a ne inizializzo i campi
    DirectoryHandle* d_handle = malloc(sizeof(DirectoryHandle));

    d_handle->sfs = fs; // ls setto il filesystem corrente
    FirstDirectoryBlock* firstdirectoryblock_ = malloc(sizeof(FirstDirectoryBlock));
	DiskDriver_readBlock(disk, firstdirectoryblock_, 0); // ls leggo il primo blocco del disco
	d_handle->dcb = firstdirectoryblock_; // ls assegno il puntatore al primo blocco della directory
	d_handle->directory = NULL; // ls non ha una directory padre
	d_handle->current_block = &(firstdirectoryblock_->header);
	d_handle->pos_in_dir = 0;
	d_handle->pos_in_block = firstdirectoryblock_->fcb.block_in_disk;


    return d_handle;
}

// crea le strutture iniziali, la directory di primo livello
// ha il nome "/" e il suo blocco di controllo è nella prima posizione
// cancella anche la bitmap dei blocchi occupati sul disco
// il blocco_directory_corrente è memorizzato nella cache nella struttura SimpleFS
// e imposta la directory di livello superiore
void SimpleFS_format(SimpleFS* fs){
    return;
}

// crea un file vuoto nella directory d
// restituisce null in caso di errore (file esistente, nessun blocco libero)
// un file vuoto consiste solo di un blocco di tipo FirstBlock
FileHandle* SimpleFS_createFile(DirectoryHandle* d, const char* filename);

// legge nell'array di blocchi (preallocato), il nome di tutti i file in una directory
int SimpleFS_readDir(char** names, DirectoryHandle* d);


// apre un file nella directory d. Il file dovrebbe esistere
FileHandle* SimpleFS_openFile(DirectoryHandle* d, const char* filename);

// chiude un file handle ( e lo distrugge)
int SimpleFS_close(FileHandle* f);

// scrive nel file, nella posizione corrente per i byte di dimensione memorizzati nei dati
// sovrascrivendo e allocando nuovo spazio se necessario
// restituisce il numero di byte scritti
int SimpleFS_write(FileHandle* f, void* data, int size);

// legge nel file, nella posizione corrente  i byte di dimensione memorizzati nei dati
// restituisce il numero di byte letti
int SimpleFS_read(FileHandle* f, void* data, int size);

// restituisce il numero di byte letti (spostando il puntatore corrente su pos)
// restituisce pos in caso di successo
// -1 in caso di errore (file troppo corto)
int SimpleFS_seek(FileHandle* f, int pos);

// cerca una directory in d. Se dirname è uguale a ".." sale di un livello
// 0 in caso di successo, valore negativo in caso di errore
// fa effetto collaterale sulla maniglia fornita
 int SimpleFS_changeDir(DirectoryHandle* d, char* dirname);

// crea una nuova directory in quella corrente (memorizzata in fs->current_directory_block)
// 0 in caso di successo
// -1 in caso di errore
int SimpleFS_mkDir(DirectoryHandle* d, char* dirname);

// rimuove il file nella directory corrente
// restituisce -1 in caso di errore 0 in caso di successo
// se una directory, rimuove ricorsivamente tutti i file contenuti
int SimpleFS_remove(SimpleFS* fs, char* filename);


  

