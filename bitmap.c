#include "bitmap.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>


// Prendiamo in ingresso il parametro "num" che rappresenta la posizione di un blocco nella memoria, 
// lo convertiamo in due valori che rappresentano rispettivamente l'indice dell'entry e lo spiazzamento all'interno di essa
// Converts a linear index to an index in the array, and a char that indicates the offset of the bit inside the array
BitMapEntryKey BitMap_blockToIndex(int num) {
	BitMapEntryKey blocco;
	// Calcolo l'indice della entry
	blocco.entry_num = num / 8 ;
	// Calcolo lo spiazzamento della posizione reale all'interno della entry
	blocco.bit_num = num % 8;
	return blocco;
}

// Questa funzione converte l'indice dell'entry e lo spiazzamento nell'entry, in un intero che rappresenta la posizione del blocco nella memoria
// Converts a bit to a linear index
int BitMap_indexToBlock(BitMapEntryKey entry) {
	return (entry.entry_num*8) + entry.bit_num;
}


// Imposta il bit all'indice "pos" in bitmap a "status"
// Sets the bit at index pos in bitmap to status
 int BitMap_set(BitMap* bitmap, int pos, int status) {
	
		// Controllo che pos sia contenuto nella BitMap
		if(pos > bitmap->num_bits || pos <0 || pos >8) return -1;

		// Dichiaro la BitMapEntryKey e la maschera per i bit
    BitMapEntryKey bmek = BitMap_blockToIndex(pos);
    // setta una maschera shiftando 1 nella posizione dell'offset
		uint8_t mask = 1 << (7 - bmek.bit_num);

		// Se bisogna impostare a "1", si mette l'OR, altrimenti si usa l'AND con la negazione della maschera
		if(status){
			bitmap->entries[bmek.entry_num] |= mask;
		}else{
    	bitmap->entries[bmek.entry_num] &= ~(mask);
    }

		// Restituisco il bit "status" dopo aver modificato la entry
    return status;
 }

// Restituisce l'indice del primo bit avente status "status" nella bitmap bitmap, iniziando a cercare dalla posizione "start"
// Returns the index of the first bit having status "status" in the bitmap bitmap, and starts looking from position start
int BitMap_get(BitMap* bitmap, int start, int status) {

	// Se si inizia a cercare da una posizione che esce dall'entry, si restituisce -1
	if(start > bitmap->num_bits) return -1;

	// Definiamo le variabili che si useranno all'interno
	int posizione, i, result;

	// Per ogni bit a partire da "start", verifichiamo
	for(i = start; i <= bitmap->num_bits-1; i++) {
		
		BitMapEntryKey bmek = BitMap_blockToIndex(i);
		
		char bmask = (1 << (7 - bmek.bit_num));
	 	
	 	result = (bitmap->entries[bmek.entry_num] & (1 << (7 - bmek.bit_num))); 

		// Se dobbiamo verificare "status=1", il risultato deve essere ">0", altrimenti deve essere "=0"
		if(status == 1) {
			if(result > 0) return i;
		}else{
			if(result == 0) return i;
		}
	}
}
