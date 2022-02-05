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

    // ls controllo corretteza parametri
    if(fs == NULL || disk == NULL){
        fprintf(stderr, "Error in SimpleFS_init: invalid parameters\n");
        return NULL;
    }

    // ls inizializzo la struttura fs con il disco passato alla funzione
    fs->disk = disk;

    // ls formatto il fs per creare le strutture iniziali
    SimpleFS_format(fs);

    FirstDirectoryBlock* firstdirectoryblock_ = malloc(sizeof(FirstDirectoryBlock));
    // ls leggo il primo blocco libero del disco (aka il blocco 0 perchè dopo la chiamata a SimpleFS_format)
    DiskDriver_readBlock(disk, firstdirectoryblock_, fs->disk->header->first_free_block);

    // ls alloco la struttura DirectoryHandle da ritornare e ne inizializzo i campi
    DirectoryHandle* d_handle = malloc(sizeof(DirectoryHandle));

    d_handle->sfs = fs; // ls setto il filesystem corrente    
    d_handle->dcb = firstdirectoryblock_; // ls assegno il puntatore al primo blocco della directory root
    d_handle->directory = NULL; // ls non ha una directory padre
    d_handle->current_block = &(firstdirectoryblock_->header);
    d_handle->pos_in_dir = 0;
    d_handle->pos_in_block = 0;

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
    if (fs == NULL){
        fprintf(stderr, "Error in SimpleFS_format: invalid parameter\n");
        return;
    }

    // ls creo un file name per il disk
    char disk_filename[255];
    sprintf(disk_filename, "disk/%ld.txt", time(NULL));

    // ls creo il primo blocco della top level directory "/"
    FirstDirectoryBlock root;

    // ls popolo la header
    root.header.previous_block = -1;
    root.header.next_block = -1;
    root.header.block_in_file = 0;
    root.header.block_in_disk = fs->disk->header->first_free_block;

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
}

// ls
// crea un file vuoto nella directory d
// restituisce null in caso di errore (file esistente, nessun blocco libero)
// un file vuoto consiste solo di un blocco di tipo FirstBlock
FileHandle* SimpleFS_createFile(DirectoryHandle* d, const char* filename){

    // ls verifico i parametri
    if (d == NULL || filename == NULL){
        fprintf(stderr, "Error in SimpleFS_createFile: invalid parameters\n");
        return NULL;
    }

    // ls bisogna prima verificare che NON esista già un file con lo stesso filename nella directory d
    // non uso la SimpleFS_readDir perchè il codice seguente è più efficiente con il controllo su !found

    // ls devo scandire ogni elemento della directory d, che può essere un file o un'altra directory

    // ls qui andrò a salvare le informazioni del primo blocco di ogni file scandito dal ciclo for
    FirstFileBlock first_f_block;

    // ls
    // i = indice per scandire tutti gli elementi della directory -> gestisce num_entries
    // j = indice corrente del blocco del corrispondente file corrente -> gestisce file_blocks[]
    int found = 0, i, j = 0;

    // ls #elementi array file_blocks (diverso se FirstDirectoryBlock o DirectoryBlock)
    int db_len = sizeof(d->dcb->file_blocks) / sizeof(int);

    // ls inizio con il FirstDirectoryBlock
    for (i = 0; i < d->dcb->num_entries && j < db_len && !found; j++){

        if (d->dcb->file_blocks[j] != -1){

            // ls leggo il primo blocco del file alla posizione corrente
            DiskDriver_readBlock(d->sfs->disk, &first_f_block, d->dcb->file_blocks[j]);

            // ls se il nome appena letto corrisponde a quello del file che si vuole creare && non è una directory
            if (!strcmp(first_f_block.fcb.name, filename) && !first_f_block.fcb.is_dir)
                found = 1;

            i++;
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
                    DiskDriver_readBlock(d->sfs->disk, &first_f_block, db.file_blocks[j]);

                    // ls se il nome appena letto corrisponde a quello del file che si vuole creare && non è una directory
                    if (!strcmp(first_f_block.fcb.name, filename) && !first_f_block.fcb.is_dir)
                        found = 1;

                    i++;
                }
            }

            block_idx = db.header.next_block;
        }
    }

    // ls se è stato trovato allora non può essere ricreato
    if(found){
        fprintf(stderr, "Error in SimpleFS_createFile: a file with filename '%s' already exists\n", filename);
        return NULL;
    }

    // ls cerco un blocco libero
    int free_idx = DiskDriver_getFreeBlock(d->sfs->disk, d->sfs->disk->header->first_free_block);
    // ls se non ci sono blocchi liberi mi fermo e ritorno NULL
    if(free_idx == -1){
        fprintf(stderr, "Error in SimpleFS_createFile: Error in DiskDriver_getFreeBlock: cannot get a new free block from disk\n");
        return NULL;
    }
    
    // ls creo e popolo il primo ed unico blocco del file vuoto da creare
    FirstFileBlock* new_first_f_block = malloc(sizeof(FirstFileBlock));

    new_first_f_block->header.previous_block = -1;
    new_first_f_block->header.next_block = -1;
    new_first_f_block->header.block_in_file = 0;
    new_first_f_block->header.block_in_disk = free_idx;
    new_first_f_block->fcb.directory_block = d->dcb->fcb.block_in_disk;
    new_first_f_block->fcb.block_in_disk = free_idx;
    strcpy(new_first_f_block->fcb.name, filename);
    new_first_f_block->fcb.size_in_bytes = 0;
    new_first_f_block->fcb.size_in_blocks = 1;
    new_first_f_block->fcb.is_dir = 0;

    // ls gestisco i blocchi del disco

    // ls se c'è ancora spazio nel blocco corrente
    if(j < db_len){

        // ls se la directory padre ha un solo blocco, il FirstDirectoryBlock
        if(d->dcb->header.next_block == -1)
            d->dcb->file_blocks[j] = free_idx;
        // ls se invece ci sono altri DirectoryBlock  
        else{

            // ls aggiungo il nuovo indice nell'array file_blocks alla posizione j
            db.file_blocks[j] = free_idx;
            // ls e scrivo il blocco su disco
            DiskDriver_writeBlock(d->sfs->disk, &db, block_idx);
        }
        
    }
    // ls altrimenti devo creare un nuovo blocco per la directory padre
    else{

        // ls chiedo al disco un nuovo indice di blocco libero
        int next_db_idx = DiskDriver_getFreeBlock(d->sfs->disk, d->sfs->disk->header->first_free_block);
        if(next_db_idx == -1){
            fprintf(stderr, "Error in SimpleFS_createFile: Error in DiskDriver_getFreeBlock: cannot get a new free block from disk\n");
            free(new_first_f_block);
            return NULL;
        }

        // ls creo e popolo il nuovo blocco
        DirectoryBlock new_db;

        new_db.header.previous_block = block_idx;
        new_db.header.next_block = -1;
        new_db.header.block_in_file = i / d->dcb->num_entries;
        new_db.header.block_in_disk = next_db_idx;
        memset(new_db.file_blocks, 0, sizeof(new_db.file_blocks));
        new_db.file_blocks[0] = free_idx;
        // ls scrivo il nuovo blocco su disco
        DiskDriver_writeBlock(d->sfs->disk, &new_db, next_db_idx);

        // ls se la directory padre aveva un solo blocco
        if(d->dcb->header.next_block == -1)
            d->dcb->header.next_block = next_db_idx;

        else{

            db.header.next_block = next_db_idx;
            DiskDriver_writeBlock(d->sfs->disk, &db, block_idx);
        }

        // ls aggiorno il FileControlBlock della directory
        d->dcb->fcb.size_in_bytes += sizeof(DirectoryBlock);
        d->dcb->fcb.size_in_blocks += 1;
    }

    // ls scrivo su disco il nuovo blocco della directory appena creata
    DiskDriver_writeBlock(d->sfs->disk, new_first_f_block, free_idx);
    d->dcb->num_entries++;
    DiskDriver_writeBlock(d->sfs->disk, d->dcb, d->dcb->fcb.block_in_disk);

    // ls alloco e popolo la struttura FileHandle da restituire
    FileHandle* f_handle = malloc(sizeof(FileHandle));

    f_handle->sfs = d->sfs;
    f_handle->fcb = new_first_f_block;
    f_handle->directory = d->dcb;
    f_handle->current_block = (BlockHeader*)new_first_f_block;
    f_handle->pos_in_file = 0;

    return f_handle;
}

// ls
// legge nell'array di blocchi (preallocato), il nome di tutti i file in una directory
int SimpleFS_readDir(char** names, DirectoryHandle* d){

    // ls verifico i paramentri
    if(names == NULL || d == NULL){
        fprintf(stderr, "Error in SimpleFS_readDir: invalid parameters\n");
        return -1;
    }

    // ls devo scandire ogni elemento della directory d, che può essere un file o un'altra directory

    // ls qui andrò a salvare le informazioni del primo blocco di ogni file scandito dal ciclo for
    FirstFileBlock first_f_block;

    // ls
    // i = indice per scandire tutti gli elementi della directory -> gestisce num_entries
    // j = indice corrente del blocco del corrispondente file corrente -> gestisce file_blocks[]
    int i, j = 0;

    // ls #elementi array file_blocks (diverso se FirstDirectoryBlock o DirectoryBlock)
    int db_len = sizeof(d->dcb->file_blocks) / sizeof(int);

    // ls inizio con il FirstDirectoryBlock
    for (i = 0; i < d->dcb->num_entries && j < db_len; j++){

        if (d->dcb->file_blocks[j] != -1){

            // ls leggo il primo blocco del file alla posizione corrente
            DiskDriver_readBlock(d->sfs->disk, &first_f_block, d->dcb->file_blocks[j]);

            // ls se non è una directory (aka è un file), ne aggiungo il nome all'array names
            if (!first_f_block.fcb.is_dir){

                names[i] = strdup(first_f_block.fcb.name);
            }
            // ls se è una directory aggiungo "(d)" al nome
            else{

                names[i] = strdup(strcat(first_f_block.fcb.name, " (d)"));
            }

            i++;
        }
    }

    // ls calcolo indice di blocco nel disco
    int block_idx = d->dcb->fcb.block_in_disk;

    DirectoryBlock db;

    // ls proseguo la ricerca nei successivi DirectoryBlock, se esistono
    if (i < d->dcb->num_entries){

        // ls il nuovo indice di block è il blocco successivo al corrente
        block_idx = d->dcb->header.next_block;

        // ls #elementi array file_blocks di DirectoryBlock
        db_len = sizeof(db.file_blocks) / sizeof(int);

        while (i < d->dcb->num_entries){

            DiskDriver_readBlock(d->sfs->disk, &db, block_idx);

            for (j = 0; i < d->dcb->num_entries && j < db_len; j++){

                if (db.file_blocks[j] != -1){

                    // ls leggo il primo blocco del file alla posizione corrente
                    DiskDriver_readBlock(d->sfs->disk, &first_f_block, db.file_blocks[j]);

                    // ls se non è una directory (aka è un file), ne aggiungo il nome all'array names
                    if (!first_f_block.fcb.is_dir){
                    
                        names[i] = strdup(first_f_block.fcb.name);
                    }
                    else{

                        names[i] = strdup(strcat(first_f_block.fcb.name, " (d)"));
                    }

                    i++;
                }
            }

            block_idx = db.header.next_block;
        }
    }

    // ls restituisco il numero di nomi di file letti nella directory
    return i;
}

// ls
// apre un file nella directory d. Il file dovrebbe esistere
FileHandle* SimpleFS_openFile(DirectoryHandle* d, const char* filename){

    // ls verifico i parametri
    if (d == NULL || filename == NULL){
        fprintf(stderr, "Error in SimpleFS_openFile: invalid parameters\n");
        return NULL;
    }

    // ls qui andrò a salvare le informazioni del primo blocco di ogni file scandito dal ciclo for
    FirstFileBlock* first_f_block = malloc(sizeof(FirstFileBlock));

    // ls
    // i = indice per scandire tutti gli elementi della directory -> gestisce num_entries
    // j = indice corrente del blocco del corrispondente file corrente -> gestisce file_blocks[]
    int found = 0, i, j = 0;

    // ls #elementi array file_blocks (diverso se FirstDirectoryBlock o DirectoryBlock)
    int db_len = sizeof(d->dcb->file_blocks) / sizeof(int);

    // ls bisogna verificare che il file esista
    // ls non uso la SimpleFS_readDir perchè il codice seguente è più efficiente con il controllo su !found

    // ls devo scandire ogni elemento della directory d, che può essere un file o un'altra directory

    // ls inizio con il FirstDirectoryBlock
    for (i = 0; i < d->dcb->num_entries && j < db_len && !found; j++){

        if (d->dcb->file_blocks[j] != -1){

            // ls leggo il primo blocco del file alla posizione corrente
            DiskDriver_readBlock(d->sfs->disk, first_f_block, d->dcb->file_blocks[j]);

            // ls se il nome appena letto corrisponde a quello del file che si vuole aprire && non è una directory
            if (!strcmp(first_f_block->fcb.name, filename) && !first_f_block->fcb.is_dir)
                found = 1;

            i++;
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
                    if (!strcmp(first_f_block->fcb.name, filename) && !first_f_block->fcb.is_dir)
                        found = 1;

                    i++;
                }
            }

            block_idx = db.header.next_block;
        }
    }

    // ls se il file non è stato trovato, non può essere aperto e restituisco NULL
    if (!found){
        fprintf(stderr, "Error in SimpleFS_openFile: cannot find a file called '%s'\n", filename);
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
    if (f == NULL){
        fprintf(stderr, "Error in SimpleFS_close: invalid parameter\n");
        return -1;
    }

    // ls check per non far fallire la seguente free(f->fcb)
    if (&(f->fcb->header) != f->current_block)
        free(f->current_block);

    free(f->fcb);
    free(f);

    return 0;
}

// ls
// scrive nel file, nella posizione corrente per i byte di dimensione memorizzati nei dati
// sovrascrivendo e allocando nuovo spazio se necessario
// restituisce il numero di byte scritti
int SimpleFS_write(FileHandle* f, void* data, int size){

    // ls lunghezza data del primo blocco (FirstFileBlock)
    int first_f_block_len = BLOCK_SIZE - sizeof(FileControlBlock) - sizeof(BlockHeader);

    // ls lunghezza data degli altri blocchi (FileBlock)
    int f_block_len = BLOCK_SIZE - sizeof(BlockHeader);

    int block_len;

    // ls #bytes liberi nel blocco dati corrente
    int free_bytes_in_block;

    // ls numero di blocco disco dove è mappato il blocco file
    int block_in_disk;

    // ls qui andrò a salvare il puntatore al campo data del blocco file
    char* file_data;

    // ls inizializzazione variabili condizionale
    // ls se mi trovo nel FirstFileBlock del file
    if (f->current_block->block_in_file == 0){

        free_bytes_in_block = first_f_block_len - f->pos_in_file;
        
        file_data = ((FirstFileBlock*)f->current_block)->data;

        block_len = first_f_block_len;

        block_in_disk = f->fcb->fcb.block_in_disk;
    }

    // ls altrimenti sono in un FileBlock 
    else{

        free_bytes_in_block = f_block_len - (f->pos_in_file - first_f_block_len - ((f->current_block->block_in_file - 1) * f_block_len));

        
        file_data = ((FileBlock*)f->current_block)->data;

        block_len = f_block_len;

        block_in_disk = f->current_block->block_in_file;
    }

    // ls inizio a scrivere nel file
    // ls se ho abbastanza spazio nel blocco corrente
    if (size <= free_bytes_in_block){

        // ls scrivo nel campo data del blocco
        memcpy(file_data + (block_len - free_bytes_in_block), data, size);

        // ls riscrivo il blocco corrente su disco
        DiskDriver_writeBlock(f->sfs->disk, f->current_block, block_in_disk);

        // ls aggiorno la grandezza del file
        f->fcb->fcb.size_in_bytes = f->pos_in_file + size;

        // ls aggiorno il cursore
        f->pos_in_file  += size;
        
        DiskDriver_writeBlock(f-> sfs->disk, f->fcb, f->fcb->header.block_in_disk);

        return size;
    }

    // ls se non ho abbastanza spazio nel blocco corrente
    else{

        // ls se il blocco successivo è già stato creato
        if(f->current_block->next_block != -1){

            // ls scrivo nel campo data del blocco fino a che ho spazio libero
            memcpy(file_data + (block_len - free_bytes_in_block), data, free_bytes_in_block);

            // ls riscrivo il blocco corrente su disco
            DiskDriver_writeBlock(f->sfs->disk, f->current_block, block_in_disk);

            // ls aggiorno cursore
            f->pos_in_file  += free_bytes_in_block;

            // ls qui salvo il blocco successivo a quello corrente
            FileBlock* next_block = malloc(sizeof(FileBlock));

            // ls leggo dal disco il blocco successivo
            DiskDriver_readBlock(f->sfs->disk, next_block, f->current_block->next_block);
            
            if(f->current_block != (BlockHeader*)f->fcb)
                free(f->current_block);
            
            // ls mi muovo al blocco successivo
            f->current_block = (BlockHeader*)next_block;

            f->fcb->fcb.size_in_bytes += free_bytes_in_block;
            f->fcb->fcb.size_in_blocks += 1;

            // ls chiamata ricorsiva per concludere la scrittura
            return free_bytes_in_block + SimpleFS_write(f, data + free_bytes_in_block, size-free_bytes_in_block);
        }

        // ls se il blocco successivo non è ancora stato creato
        else{

            // ls chiedo al DiskDriver un nuovo indice libero
            int next_block_idx =  DiskDriver_getFreeBlock(f->sfs->disk, f->sfs->disk->header->first_free_block);

            // ls creo e popolo il nuovo blocco file
            FileBlock next_block = {0};
            next_block.header.previous_block = block_in_disk;
            next_block.header.next_block = -1;
            next_block.header.block_in_file = f->current_block->block_in_file + 1;

            // ls scrivo il blocco sul disco
            DiskDriver_writeBlock(f->sfs->disk, &next_block, next_block_idx);

            // ls collego il blocco appena creato alla lista dei blocchi del file f
            f->current_block->next_block = next_block_idx;

            // ls la chimata ricorsiva farà la magia
            return SimpleFS_write(f, data, size);
        }        
    }    
}

// ls
// legge nel file, nella posizione corrente i byte di dimensione memorizzati nei dati
// restituisce il numero di byte letti
int SimpleFS_read(FileHandle* f, void* data, int size){

    // ls lunghezza data del primo blocco (FirstFileBlock)
    int first_f_block_len = BLOCK_SIZE - sizeof(FileControlBlock) - sizeof(BlockHeader);

    // ls lunghezza data degli altri blocchi (FileBlock)
    int f_block_len = BLOCK_SIZE - sizeof(BlockHeader);

    int block_len;

    // ls #bytes liberi nel blocco dati corrente
    int free_bytes_in_block;

    // ls qui andrò a salvare il puntatore al campo data del blocco file
    char* file_data;

    // ls inizializzazione variabili condizionale
    // ls se mi trovo nel FirstFileBlock del file
    if (f->current_block->block_in_file == 0){

        free_bytes_in_block = first_f_block_len - f->pos_in_file;
        
        file_data = ((FirstFileBlock*)f->current_block)->data;

        block_len = first_f_block_len;
    }

    // ls altrimenti sono in un FileBlock 
    else{

        free_bytes_in_block = f_block_len - (f->pos_in_file - first_f_block_len - ((f->current_block->block_in_file - 1) * f_block_len));

        
        file_data = ((FileBlock*)f->current_block)->data;

        block_len = f_block_len;
    }

    // ls inizio a leggere dal file
    // ls se quello che devo leggere è tutto nel blocco corrente
    if (size <= free_bytes_in_block){

        // ls leggo dal campo data del blocco
        memcpy(data, file_data + (block_len - free_bytes_in_block), size);

        // ls aggiorno il cursore
        f->pos_in_file  += size;
        
        DiskDriver_writeBlock(f-> sfs->disk, f->fcb, f->fcb->header.block_in_disk);

        return size;
    }

    // ls se i bytes da leggere non sono tutti nel blocco corrente
    else{

        // ls se esiste il blocco successivo
        if(f->current_block->next_block != -1){

            // ls leggo dal campo data del blocco fino a che ho bytes da leggere
            memcpy(data, file_data + (block_len - free_bytes_in_block), free_bytes_in_block);

            // ls aggiorno cursore
            f->pos_in_file  += free_bytes_in_block;

            // ls qui salvo il blocco successivo a quello corrente
            FileBlock* next_block = malloc(sizeof(FileBlock));

            // ls leggo dal disco il blocco successivo
            DiskDriver_readBlock(f->sfs->disk, next_block, f->current_block->next_block);
            
            if(f->current_block != (BlockHeader*)f->fcb)
                free(f->current_block);
            
            // ls mi muovo al blocco successivo
            f->current_block = (BlockHeader*)next_block;

            // ls chiamata ricorsiva per concludere la lettura
            return free_bytes_in_block + SimpleFS_read(f, data + free_bytes_in_block, size-free_bytes_in_block);
        }

        // ls altrimenti sono nell'ultimo blocco
        else{

            // ls leggo dal campo data del blocco fino a che ho bytes da leggere
            memcpy(data, file_data + (block_len - free_bytes_in_block), free_bytes_in_block);

            // ls aggiorno cursore
            f->pos_in_file  += free_bytes_in_block;

            return free_bytes_in_block;
        }        
    }
}

// ls
// restituisce il numero di byte letti (spostando il puntatore corrente su pos)
// restituisce pos in caso di successo
// -1 in caso di errore (file troppo corto)
int SimpleFS_seek(FileHandle* f, int pos){

    // ls verifico i parametri
    if (f == NULL || pos < 0){
        fprintf(stderr, "Error in SimpleFS_seek: invalid parameters\n");
        return -1;
    }

    // ls se pos è più grande della dimensione del file, errore: il file è troppo corto per la posizione desiderata
    if(pos >= f->fcb->fcb.size_in_bytes){
        fprintf(stderr, "Error in SimpleFS_seek: file too short (%d bytes) for position %d\n", f->fcb->fcb.size_in_bytes, pos);
        return -1;
    }

    int delta_pos = pos - f->pos_in_file;
    int free_bytes_in_block;

    // ls inizializzazione variabile condizionale
    if(f->current_block->block_in_file == 0)
        free_bytes_in_block = (BLOCK_SIZE - sizeof(FileControlBlock) - sizeof(BlockHeader)) - f->pos_in_file;
    else
        free_bytes_in_block = (BLOCK_SIZE - sizeof(BlockHeader)) - f->pos_in_file;

    // ls se c'è abbastanza spazio nel blocco corrente
    if(delta_pos <= free_bytes_in_block){

        f->pos_in_file += delta_pos;

        return delta_pos > 0 ? delta_pos : -delta_pos;
    }

    // ls altrimenti vado al blocco successivo
    else{

        f->pos_in_file += free_bytes_in_block;

        FileBlock * tmp = malloc(sizeof(FileBlock));

        DiskDriver_readBlock(f->sfs->disk, tmp, f->current_block->next_block);

        if(f->current_block != (BlockHeader*)f->fcb)
            free(f->current_block);
            
        f->current_block = (BlockHeader*)tmp;

	    return free_bytes_in_block > 0 ? free_bytes_in_block + SimpleFS_seek(f, pos) : -free_bytes_in_block + SimpleFS_seek(f, pos);
    }
}

// ls
// cerca una directory in d. Se dirname è uguale a ".." sale di un livello
// 0 in caso di successo, valore negativo in caso di errore
// fa effetto collaterale sull'handle fornito
int SimpleFS_changeDir(DirectoryHandle* d, char* dirname){

    // ls verifico i paremtri
    if(d == NULL || !strcmp(dirname, "")){
        fprintf(stderr, "Error in SimpleFS_changeFile: invalid parameters\n");
        return -1;
    }

    // ls se dirname == '..' devo andare alla directory padre
    if(!strcmp("..", dirname)){

        // ls qui salvo le informazioni della directory padre della directory '..' (per popolare l'handle di '..')
        FirstDirectoryBlock* parent_of_parent = malloc(sizeof(FirstDirectoryBlock));
        // ls leggo il FirstDirectoryBlock del padre di '..'
        DiskDriver_readBlock(d->sfs->disk, parent_of_parent, d->directory->fcb.directory_block);

        // ls varibile di appoggio per la successiva free del vecchio puntatore
        FirstDirectoryBlock* tmp = d->dcb;

        // ls popolo l'handle di '..'
        d->dcb = d->directory; // ls l'handle d punta ora come primo blocco della directory corrente alla directory '..'
        d->directory = parent_of_parent; // ls l'handle d punta ora come primo blocco della directory padre al padre di '..'
        
        free(tmp);
        return 0;
    }

    // ls bisogna cercare all'interno della directory corrente la directory dirname

    // ls qui andrò a salvare le informazioni del primo blocco di ogni directory scandito dal ciclo for
    FirstDirectoryBlock* first_d_block = malloc(sizeof(FirstFileBlock));

    // ls
    // i = indice per scandire tutti gli elementi della directory -> gestisce num_entries
    // j = indice corrente del blocco del corrispondente file corrente -> gestisce file_blocks[]
    int found, i, j = 0;

    // ls #elementi array file_blocks (diverso se FirstDirectoryBlock o DirectoryBlock)
    int db_len = sizeof(d->dcb->file_blocks) / sizeof(int);

    // ls bisogna verificare che la directory esista
    // ls non uso la SimpleFS_readDir perchè il codice seguente è più efficiente con il controllo su !found

    // ls devo scandire ogni elemento della directory d, che può essere un file o un'altra directory

    // ls inizio con il FirstDirectoryBlock
    for (i = 0; i < d->dcb->num_entries && j < db_len && !found; i++, j++){

        if (d->dcb->file_blocks[j] != -1){

            // ls leggo il primo blocco della directory alla posizione corrente
            DiskDriver_readBlock(d->sfs->disk, first_d_block, d->dcb->file_blocks[j]);

            // ls se il nome appena letto corrisponde a quello della directory che si vuole aprire && è una directory
            if (!strcmp(first_d_block->fcb.name, dirname) && first_d_block->fcb.is_dir)
                found = 1;
        }
    }

    // ls calcolo indice di blocco nel disco
    int block_idx = d->dcb->fcb.block_in_disk;

    DirectoryBlock db;

    // ls se non è stato ancora trovato proseguo la ricerca nei successivi DirectoryBlock
    if (!found && i < d->dcb->num_entries){

        // ls il nuovo indice di block è il blocco successivo al corrente
        block_idx = d->dcb->header.next_block;

        // ls #elementi array file_blocks di DirectoryBlock
        db_len = sizeof(db.file_blocks) / sizeof(int);

        while (i < d->dcb->num_entries && !found){

            DiskDriver_readBlock(d->sfs->disk, &db, block_idx);

            for (j = 0; i < d->dcb->num_entries && j < db_len; j++){

                if (db.file_blocks[j] != -1){

                    // ls leggo il primo blocco della directory alla posizione corrente
                    DiskDriver_readBlock(d->sfs->disk, first_d_block, db.file_blocks[j]);

                    // ls se il nome appena letto corrisponde a quello della directory che si vuole aprire && è una directory
                    if (!strcmp(first_d_block->fcb.name, dirname) && first_d_block->fcb.is_dir)
                        found = 1;

                    i++;
                }
            }

            block_idx = db.header.next_block;
        }
    }

    // ls se la directory non è stata trovata, errore
    if (!found){
        fprintf(stderr, "Error in SimpleFS_changeDir: cannot find a directory called '%s'\n", dirname);
        free(first_d_block);
        return -1;
    }

    // ls se è stata trovata la directory dirname, faccio side-effect sull'handle
    d->directory = d->dcb;
    d->dcb = first_d_block;
    d->current_block = &(first_d_block->header);
    d->pos_in_dir = 0;
    d->pos_in_block = first_d_block->fcb.block_in_disk;
    
    return 0;
}

// ls
// crea una nuova directory in quella corrente (memorizzata in fs->current_directory_block)
// 0 in caso di successo
// -1 in caso di errore
int SimpleFS_mkDir(DirectoryHandle* d, char* dirname){

    // ls verifico i parametri
    if(d == NULL || !strcmp(dirname, "")){
        fprintf(stderr, "Error in SimpleFS_mkDir: invalid parameters\n");
        return -1;
    }

    // ls qui andrò a salvare le informazioni del primo blocco di ogni file scandito dal ciclo for
    FirstFileBlock first_f_block;

    // ls
    // i = indice per scandire tutti gli elementi della directory -> gestisce num_entries
    // j = indice corrente del blocco del corrispondente file corrente -> gestisce file_blocks[]
    int found = 0, i, j = 0;

    // ls #elementi array file_blocks (diverso se FirstDirectoryBlock o DirectoryBlock)
    int db_len = sizeof(d->dcb->file_blocks) / sizeof(int);

    // ls bisogna prima controllare che NON esista già una directory con lo stesso nome dirname
    // ls non uso la SimpleFS_readDir perchè il codice seguente è più efficiente con il controllo su !found

    // ls devo scandire ogni elemento della directory d, che può essere un file o un'altra directory

    // ls inizio con il FirstDirectoryBlock
    for (i = 0; i < d->dcb->num_entries && j < db_len && !found; i++, j++){

        if (d->dcb->file_blocks[j] != -1){

            // ls leggo il primo blocco del file alla posizione corrente
            DiskDriver_readBlock(d->sfs->disk, &first_f_block, d->dcb->file_blocks[j]);

            // ls se il nome appena letto corrisponde a quello della directory che si vuole creare && è una directory
            if (!strcmp(first_f_block.fcb.name, dirname) && first_f_block.fcb.is_dir)
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
                    DiskDriver_readBlock(d->sfs->disk, &first_f_block, db.file_blocks[j]);

                    // ls se il nome appena letto corrisponde a quello della directory che si vuole creare && è una directory
                    if (!strcmp(first_f_block.fcb.name, dirname) && first_f_block.fcb.is_dir)
                        found = 1;

                    i++;
                }
            }

            block_idx = db.header.next_block;
        }
    }

    // ls se è stata trovata una directory con il nome dirname, errore
    if (found){
        fprintf(stderr, "Error in SimpleFS_mkDir: a directory called '%s' already exists\n", dirname);
        return -1;
    }

    // ls chiedo al DiskDriver un nuovo blocco libero per la directory
    int free_idx = DiskDriver_getFreeBlock(d->sfs->disk, d->sfs->disk->header->first_free_block);
    if(free_idx == -1){
        fprintf(stderr, "Error in SimpleFS_mkDir: Error in DiskDriver_getFreeBlock: cannot get a new free block from disk\n");
        return -1;
    }

    // ls creo un nuovo FirstDirecotryBlock e lo popolo
    FirstDirectoryBlock new_f_dir;

    new_f_dir.header.previous_block = -1;
    new_f_dir.header.next_block = -1;
    new_f_dir.header.block_in_file = 0;
    new_f_dir.header.block_in_disk = free_idx;
    new_f_dir.fcb.directory_block = d->dcb->fcb.block_in_disk;
    new_f_dir.fcb.block_in_disk = free_idx;
    strcpy(new_f_dir.fcb.name, dirname);
    new_f_dir.fcb.size_in_bytes = 0;
    new_f_dir.fcb.size_in_blocks = 1;
    new_f_dir.fcb.is_dir = 1;
    new_f_dir.num_entries = 0;
    memset(new_f_dir.file_blocks, 0, sizeof(new_f_dir.file_blocks));

    // ls gestisco i blocchi del disco

    // ls se c'è ancora spazio nel blocco corrente
    if(j < db_len){

        // ls se la directory padre ha un solo blocco, il FirstDirectoryBlock
        if(d->dcb->header.next_block == -1)
            d->dcb->file_blocks[j] = free_idx;
        // ls se invece ci sono altri DirectoryBlock  
        else{

            // ls aggiungo il nuovo indice nell'array file_blocks alla posizione j
            db.file_blocks[j] = free_idx;
            // ls e scrivo il blocco su disco
            DiskDriver_writeBlock(d->sfs->disk, &db, block_idx);
        }
        
    }
    // ls altrimenti devo creare un nuovo blocco per la directory padre
    else{

        // ls chiedo al disco un nuovo indice di blocco libero
        int next_db_idx = DiskDriver_getFreeBlock(d->sfs->disk, d->sfs->disk->header->first_free_block);
        if(next_db_idx == -1){
            fprintf(stderr, "Error in SimpleFS_mkDir: Error in DiskDriver_getFreeBlock: cannot get a new free block from disk\n");
            return -1;
        }

        // ls creo e popolo il nuovo blocco
        DirectoryBlock new_db;

        new_db.header.previous_block = block_idx;
        new_db.header.next_block = -1;
        new_db.header.block_in_file = i / d->dcb->num_entries;
        new_db.header.block_in_disk = next_db_idx;
        memset(new_db.file_blocks, 0, sizeof(new_db.file_blocks));
        new_db.file_blocks[0] = free_idx;
        // ls scrivo il nuovo blocco su disco
        DiskDriver_writeBlock(d->sfs->disk, &new_db, next_db_idx);

        // ls se la directory padre aveva un solo blocco
        if(d->dcb->header.next_block == -1)
            d->dcb->header.next_block = next_db_idx;

        else{

            db.header.next_block = next_db_idx;
            DiskDriver_writeBlock(d->sfs->disk, &db, block_idx);
        }

        // ls aggiorno il FileControlBlock della directory
        d->dcb->fcb.size_in_bytes += sizeof(DirectoryBlock);
        d->dcb->fcb.size_in_blocks += 1;
    }

    // ls scrivo su disco il nuovo blocco della directory appena creata
    DiskDriver_writeBlock(d->sfs->disk, &new_f_dir, free_idx);
    d->dcb->num_entries++;
    DiskDriver_writeBlock(d->sfs->disk, d->dcb, d->dcb->fcb.block_in_disk);

    return 0;
}

// ls funzione per la rimozione di tutti i blocchi di un file
void remove_file(FileBlock* read_block, DiskDriver* disk){

    if (read_block->header.next_block == -1)
        DiskDriver_freeBlock(disk, read_block->header.block_in_disk);

    else{
        FileBlock* tmp = (FileBlock*) malloc (sizeof(FileBlock));
        DiskDriver_readBlock(disk, tmp, read_block->header.next_block);
        remove_file(tmp, disk);
        free(tmp);
        DiskDriver_freeBlock(disk, read_block->header.block_in_disk);
    }
}

// ls funzione per la rimozione di tutti i blocchi di una directory
void remove_directory(FirstDirectoryBlock* read_block, DiskDriver* disk){

    FirstFileBlock first_f_block;

    int i, j = 0;

    int db_len = sizeof(read_block->file_blocks) / sizeof(int);

    for (i = 0; i < read_block->num_entries && j < db_len; j++){

        if (read_block->file_blocks[j] != -1){

            DiskDriver_readBlock(disk, &first_f_block, read_block->file_blocks[j]);

            remove_blocks(&first_f_block, disk);

            i++;
        }
    }

    int block_idx = read_block->fcb.block_in_disk;

    DirectoryBlock db;

    if (i < read_block->num_entries){

        block_idx = read_block->header.next_block;

        db_len = sizeof(db.file_blocks) / sizeof(int);

        while (i < read_block->num_entries){

            DiskDriver_readBlock(disk, &db, block_idx);

            for (j = 0; i < read_block->num_entries && j < db_len; j++){

                if (db.file_blocks[j] != -1){

                    DiskDriver_readBlock(disk, &first_f_block, db.file_blocks[j]);

                    remove_blocks(&first_f_block, disk);

                    i++;
                }
            }

            block_idx = db.header.next_block;
        }
    }

    remove_directory_blocks((DirectoryBlock* )read_block, disk);
}

// ls funzione d'appoggio per la ricorsione
void remove_directory_blocks (DirectoryBlock* db, DiskDriver* disk){

    if (db->header.next_block == -1)
        DiskDriver_freeBlock(disk, db->header.block_in_disk);

    else{
        DirectoryBlock tmp;
        DiskDriver_readBlock(disk, &tmp, db->header.next_block);
        remove_directory_blocks(&tmp, disk);
        
        DiskDriver_freeBlock(disk, db->header.block_in_disk);
    }
}

// ls funzione d'appoggio per la rimozione ricorsiva 
void remove_blocks(FirstFileBlock* read_block, DiskDriver* disk){

    if (read_block->fcb.is_dir)
        remove_directory((FirstDirectoryBlock*) read_block, disk);
    
    else
        remove_file((FileBlock*) read_block, disk);
}

// ls
// rimuove il file nella directory corrente
// restituisce -1 in caso di errore 0 in caso di successo
// se una directory, rimuove ricorsivamente tutti i file contenuti
int SimpleFS_remove(DirectoryHandle* d, char* filename){

    // ls verifico i parametri
    if (d == NULL || filename == NULL){
        fprintf(stderr, "Error in SimpleFS_remove: invalid parameters\n");
        return -1;
    }

    // ls qui andrò a salvare le informazioni del primo blocco di ogni file scandito dal ciclo for
    FirstFileBlock first_f_block;

    // ls
    // i = indice per scandire tutti gli elementi della directory -> gestisce num_entries
    // j = indice corrente del blocco del corrispondente file corrente -> gestisce file_blocks[]
    int found = 0, i, j = 0;

    // ls #elementi array file_blocks (diverso se FirstDirectoryBlock o DirectoryBlock)
    int db_len = sizeof(d->dcb->file_blocks) / sizeof(int);

    // ls bisogna prima verificare che un file o una directory con il nome 'filename' esista già
    // ls non uso la SimpleFS_readDir perchè il codice seguente è più efficiente con il controllo su !found

    // ls devo scandire ogni elemento della directory d, che può essere un file o un'altra directory

    // ls inizio con il FirstDirectoryBlock
    for (i = 0; i < d->dcb->num_entries && j < db_len && !found; j++){

        if (d->dcb->file_blocks[j] != -1){

            // ls leggo il primo blocco del file alla posizione corrente
            DiskDriver_readBlock(d->sfs->disk, &first_f_block, d->dcb->file_blocks[j]);

            // ls se il nome appena letto corrisponde a quello del file o directory che si vuole rimuovere
            if (!strcmp(first_f_block.fcb.name, filename)){
                found = 1;
                d->dcb->file_blocks[j] = -1;
            }

            i++;
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
                    DiskDriver_readBlock(d->sfs->disk, &first_f_block, db.file_blocks[j]);

                    // ls se il nome appena letto corrisponde a quello del file o directory che si vuole rimuovere
                    if (!strcmp(first_f_block.fcb.name, filename)){
                        found = 1;
                        db.file_blocks[j] = -1;
                        DiskDriver_writeBlock(d->sfs->disk, &db, block_idx);
                    }

                    i++;
                }
            }

            block_idx = db.header.next_block;
        }
    }

    // ls se il file o directory non è stato trovato, non può essere rimosso e restituisco -1
    if (!found){
        fprintf(stderr, "Error in SimpleFS_remove: cannot find a file called '%s'\n", filename);
        return -1;
    }    

    d->dcb->num_entries--;
    DiskDriver_writeBlock(d->sfs->disk, d->dcb, d->dcb->header.block_in_disk);
   
    remove_blocks(&first_f_block, d->sfs->disk);

    return 0;
}
