# SO_simple-file-system
Progetto per il corso di Sistemi Operativi 2020/21

## Autori
Lorenzo Soannini, @lorenzosoannini\
John Marcelo, @sanjohngiangino

## What
Implementazione di un semplice FileSystem per organizzare e gestire la memoria su disco.
È possibile gestire file e cartelle in modo gerarchico.

## How
Il disco da gestire è un file di testo.txt diviso a blocchi, tutti della stessa grandezza.\
Il progetto è diviso in due moduli principali:\
-DiskDriver\
-SimpleFS\
\
Il DiskDriver si occupa, attraverso una struttura dati di tipo BitMap, di leggere e scrivere i blocchi sul disco. Si occupa quindi della loro allocazione e deallocazione, per permettere al FileSystem di eseguire le sue funzioni.\
\
Il SimpleFS è il FileSystem vero e proprio, si appoggia al modulo DiskDriver per gestire il disco ad alto livello. Il FileSystem gerarchico è implementatato secondo una politica LLA (Linked List Allocation), secondo cui ogni file (o cartella) è diviso in uno o più blocchi della stessa grandezza ed esiste una lista concatenata di indici per scandire l'intero file. Per questo il FileSystem gestisce i file come catene continue di bytes ma in realtà il DiskDriver fornisce gli indici dei blocchi nel disco fisico che sono, in generale, discontinui: in questo modo i file occupano alla loro creazione un solo blocco del disco e, man mano che avanza la scrittura, si viene a popolare la lista degli indici del file con quelli ricevuti dal DiskDriver, risolvendo problemi di spreco di memoria fisica.

## How to run
Per compilare il codice bisogna scaricare l'intero contenuto del repository e dare i seguenti comandi da terminale:

```
make objs
make
```

Per eseguire lanciare il comando:

```
./simplefs_test
```
