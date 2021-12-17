#include "simplefs.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

// JM Dichiarazione funzioni e descrizioni di tali

// ls
// inizializza un file system su un disco già creato
// restituisce un handle alla directory di primo livello memorizzata nel primo blocco
DirectoryHandle* SimpleFS_init(SimpleFS* fs, DiskDriver* disk){

    // ls inizializzo la struttura fs con il disco passato alla funzione
    fs->disk = disk;

    // ls formatto il fs per creare le strutture iniziali
    SimpleFS_format(fs);

    FirstDirectoryBlock* firstdirectoryblock_ = malloc(sizeof(FirstDirectoryBlock));
    // ls leggo il primo blocco libero del disco (aka il blocco 0 perchè dopo la chiamata a SimpleFS_format)
    DiskDriver_readBlock(disk, firstdirectoryblock_, fs->disk->header->first_free_block);

    // alloco la struttura DirectoryHandle da ritornare e ne inizializzo i campi
    DirectoryHandle* d_handle = malloc(sizeof(DirectoryHandle));

    d_handle->sfs = fs; // ls setto il filesystem corrente    
    d_handle->dcb = firstdirectoryblock_; // ls assegno il puntatore al primo blocco della directory root
    d_handle->directory = NULL; // ls non ha una directory padre
    d_handle->current_block = &(firstdirectoryblock_->header);
    d_handle->pos_in_dir = 0;
    d_handle->pos_in_block = firstdirectoryblock_->fcb.block_in_disk;

    return d_handle;
}

// ls
// crea le strutture iniziali, la directory di primo livello
// ha il nome "/" e il suo blocco di controllo è nella prima posizione
// cancella anche la bitmap dei blocchi occupati sul disco
// il blocco_directory_corrente è memorizzato nella cache nella struttura SimpleFS
// e imposta la directory di livello superiore
void SimpleFS_format(SimpleFS* fs){

    // ls se il filesystem passato è invalido non faccio nulla
    if (fs == NULL)
        return;

    // ls creo un file name per il disk
    char disk_filename[255];
    sprintf(disk_filename, "disk/%ld.txt", time(NULL));

    // ls inizializzo il DiskDriver per il disco corrente
    DiskDriver_init(fs->disk, disk_filename, fs->disk->header->num_blocks);

    // ls creo il primo blocco della top level directory "/"
    FirstDirectoryBlock root;

    // ls popolo la header
    root.header.previous_block = -1;
    root.header.next_block = -1;
    root.header.block_in_file = 0;

    // ls popolo il FCB per "/"
    root.fcb.directory_block = -1;
    root.fcb.block_in_disk = fs->disk->header->first_free_block;
    strcpy(root.fcb.name, "/");
    root.fcb.size_in_bytes = sizeof(FirstDirectoryBlock);
    root.fcb.size_in_blocks = 1;
    root.fcb.is_dir = 1;

    // ls popolo i rimanenti campi del FirstDirectoryBlock di root
    root.num_entries = 0;
    memset(root.file_blocks, 0, sizeof(root.file_blocks));

    // ls chiamo le funzioni del DiskDriver per memorzzare tutto nel disco
    DiskDriver_writeBlock(fs->disk, &root, fs->disk->header->first_free_block);
    DiskDriver_flush(fs->disk);
}

// ls
// crea un file vuoto nella directory d
// restituisce null in caso di errore (file esistente, nessun blocco libero)
// un file vuoto consiste solo di un blocco di tipo FirstBlock
FileHandle* SimpleFS_createFile(DirectoryHandle* d, const char* filename){

    // ls verifico i parametri
    if (d == NULL || filename == NULL)
        return NULL;

    // ls alloco il FileHandle da ritornare
    FileHandle* f_handle = malloc(sizeof(FileHandle));

    // ls bisogna prima verificare che NON esista già un file con lo stesso filename nella directory d
    // ls devo scandire ogni elemento della directory d, che può essere un file o un'altra directory

    // ls qui andrò a salvare le informazioni del primo blocco di ogni file scandito dal ciclo for
    FirstFileBlock* first_f_block;

    // ls
    // i = indice per scandire tutti gli elementi della directory -> gestisce num_entries
    // j = indice corrente del blocco del corrispondente file corrente -> gestisce file_blocks[]
    int found, i, j = 0;

    // ls #elementi array file_blocks (diverso se FirstDirectoryBlock o DirectoryBlock)
    int db_len = sizeof(d->dcb->file_blocks) / sizeof(int);

    // ls inizio con il FirstDirectoryBlock
    for (i = 0; i < d->dcb->num_entries && j < db_len && !found; i++, j++){

        if (d->dcb->file_blocks[j] != -1){

            // ls leggo il primo blocco del file alla posizione corrente
            DiskDriver_readBlock(d->sfs->disk, first_f_block, d->dcb->file_blocks[j]);

            // ls se il nome appena letto corrisponde a quello del file che si vuole creare && non è una directory
            if (strcmp(first_f_block->fcb.name, filename) && first_f_block->fcb.is_dir)
                found = 1;
        }
    }

    // ls calcolo indice di blocco nel disco
    int block_idx = d->dcb->fcb.block_in_disk;

    DirectoryBlock db;

    // ls se non è stato ancora trovato proseguo la ricerca nei successivi DirectoryBlock
    if (!found && i < d->dcb->num_entries){

        // ls il nuovo indice di blocck è il blocco successivo al corrente
        block_idx = d->dcb->header.next_block;

        // ls #elementi array file_blocks di DirectoryBlock
        db_len = sizeof(db.file_blocks) / sizeof(int);

        while (i < d->dcb->num_entries && !found){

            DiskDriver_readBlock(d->sfs->disk, &db, block_idx);

            for (j = 0; i < d->dcb->num_entries && j < db_len; j++){

                if (db.file_blocks[j] != -1){

                    // ls leggo il primo blocco del file alla posizione corrente
                    DiskDriver_readBlock(d->sfs->disk, first_f_block, db.file_blocks[j]);

                    // ls se il nome appena letto corrisponde a quello del file che si vuole creare && non è una directory
                    if (strcmp(first_f_block->fcb.name, filename) && first_f_block->fcb.is_dir)
                        found = 1;

                    i++;
                }
            }

            block_idx = db.header.next_block;
        }
    }

    // ls se è stato trovato allora non può essere ricreato
    if(found)
        return NULL;

    // ls cerco un blocco libero
    int free_idx = DiskDriver_getFreeBlock(d->sfs->disk, d->sfs->disk->header->first_free_block);
    // se non ci sono blocchi liberi mi fermo e ritorno NULL
    if(free_idx == -1)
        return NULL;
    
    // ls alloco e popolo il primo ed unico blocco del file vuoto da creare
    first_f_block = malloc(sizeof(FirstFileBlock));

    first_f_block->header.previous_block = -1;
    first_f_block->header.next_block = -1;
    first_f_block->header.block_in_disk = free_idx;
    first_f_block->fcb.directory_block = d->dcb->fcb.block_in_disk;
    first_f_block->fcb.block_in_disk = free_idx;
    strcpy(first_f_block->fcb.name, filename);
    first_f_block->fcb.size_in_blocks = 1;

    // ls alloco e popolo la struttura FileHandle da restituire
    FileHandle* f_handle = malloc(sizeof(FileHandle));

    f_handle->sfs = d->sfs;
    f_handle->fcb = NULL; // da implementare
    f_handle->directory = d->dcb;
    f_handle->current_block = NULL; // da implementare
    f_handle->pos_in_file = 0;

    return f_handle;
}

// legge nell'array di blocchi (preallocato), il nome di tutti i file in una directory
int SimpleFS_readDir(char** names, DirectoryHandle* d);

// ls
// apre un file nella directory d. Il file dovrebbe esistere
FileHandle* SimpleFS_openFile(DirectoryHandle* d, const char* filename){

    // ls verifico i parametri
    if (d == NULL || filename == NULL)
        return NULL;

    // ls qui andrò a salvare le informazioni del primo blocco di ogni file scandito dal ciclo for
    FirstFileBlock* first_f_block = malloc(sizeof(FirstFileBlock));

    // ls
    // i = indice per scandire tutti gli elementi della directory -> gestisce num_entries
    // j = indice corrente del blocco del corrispondente file corrente -> gestisce file_blocks[]
    int found, i, j = 0;

    // ls #elementi array file_blocks (diverso se FirstDirectoryBlock o DirectoryBlock)
    int db_len = sizeof(d->dcb->file_blocks) / sizeof(int);

    // ls bisogna verificare che il file esista
    // ls devo scandire ogni elemento della directory d, che può essere un file o un'altra directory

    // ls inizio con il FirstDirectoryBlock
    for (i = 0; i < d->dcb->num_entries && j < db_len && !found; i++, j++){

        if (d->dcb->file_blocks[j] != -1){

            // ls leggo il primo blocco del file alla posizione corrente
            DiskDriver_readBlock(d->sfs->disk, first_f_block, d->dcb->file_blocks[j]);

            // ls se il nome appena letto corrisponde a quello del file che si vuole aprire && non è una directory
            if (strcmp(first_f_block->fcb.name, filename) && first_f_block->fcb.is_dir)
                found = 1;
        }
    }

    // ls calcolo indice di blocco nel disco
    int block_idx = d->dcb->fcb.block_in_disk;

    DirectoryBlock db;

    // ls se non è stato ancora trovato proseguo la ricerca nei successivi DirectoryBlock
    if (!found && i < d->dcb->num_entries){

        // ls il nuovo indice di blocck è il blocco successivo al corrente
        block_idx = d->dcb->header.next_block;

        // ls #elementi array file_blocks di DirectoryBlock
        db_len = sizeof(db.file_blocks) / sizeof(int);

        while (i < d->dcb->num_entries && !found){

            DiskDriver_readBlock(d->sfs->disk, &db, block_idx);

            for (j = 0; i < d->dcb->num_entries && j < db_len; j++){

                if (db.file_blocks[j] != -1){

                    // ls leggo il primo blocco del file alla posizione corrente
                    DiskDriver_readBlock(d->sfs->disk, first_f_block, db.file_blocks[j]);

                    // ls se il nome appena letto corrisponde a quello del file che si vuole aprire && non è una directory
                    if (strcmp(first_f_block->fcb.name, filename) && first_f_block->fcb.is_dir)
                        found = 1;

                    i++;
                }
            }

            block_idx = db.header.next_block;
        }
    }

    // ls se il file non è stato trovato, non può essere aperto e restituisco NULL
    if (!found){

        free(first_f_block);
        return NULL;
    }

    // ls alloco il FileHandle da ritornare
    FileHandle* f_handle = malloc(sizeof(FileHandle));

    // ls setto i dati del FileHandle
    f_handle->sfs = d->sfs;
    f_handle->fcb = first_f_block;
    f_handle->directory = d->dcb;
    f_handle->current_block = &(first_f_block->header);
    f_handle->pos_in_file = 0;

    // ls restituisco il FileHandle appena popolato
    return f_handle;
}

// ls
// chiude un file handle ( e lo distrugge)
int SimpleFS_close(FileHandle* f){

    // ls nulla da fare
    if (f == NULL)
        return -1;

    // ls check per non far fallire la seguente free(f->fcb)
    if (&(f->fcb->header) != f->current_block)
        free(f->current_block);

    free(f->fcb);
    free(f);

    return 0;
}

// scrive nel file, nella posizione corrente per i byte di dimensione memorizzati nei dati
// sovrascrivendo e allocando nuovo spazio se necessario
// restituisce il numero di byte scritti
int SimpleFS_write(FileHandle* f, void* data, int size);

// legge nel file, nella posizione corrente i byte di dimensione memorizzati nei dati
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
