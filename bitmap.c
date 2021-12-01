#include "bitmap.h"

// JM implemento funzione BitMapEntryKey BitMap_blockToIndex(int num)
//  converto un blocco indice a un indice nell'array
// e un char che indica l'offset del bit all'interno dell'array

BitMapEntryKey BitMap_blockToIndex(int num) {
	BitMapEntryKey obstructionum;
	
	//JM la divisione intera restituisce l'indice dell array
	obstructionum.entry_num = num / 8 ;
	
	//JM il resto della divisione indica lo spiazzamento all'interno del blocco restituito dalla divisione precedente
	obstructionum.bit_num = num % 8;
	
	
	return obstructionum;
	
}

// ls operazione inversa a BitMap_blockToIndex: la moltiplicazione per 8 calcola il blocco nella memoria e poi
// aggiunge l'offset per trovare la poizione esatta
// converts a bit to a linear index
int BitMap_indexToBlock(int entry, uint8_t bit_num){
    return (entry.entry_num * 8) + entry.bit_num;
}

// returns the index of the first bit having status "status"
// in the bitmap bmap, and starts looking from position start
int BitMap_get(BitMap* bmap, int start, int status){

}

//ls
// sets the bit at index pos in bmap to status
int BitMap_set(BitMap* bmap, int pos, int status){
    if(pos > bitmap->num_bits)  //se pos > num_bits la posizione "pos" indica un blocco invalido
        return -1;

    BitMapEntryKey bmentry = BitMap_blockToIndex(pos); //prendo l'indice della posizione "pos"
	uint8_t mask = 1 << (7 - bmentry.bit_num); //imposto la maschera di bit

	// setto la maschera dopo il controllo di status
	if(status)
		bitmap->entries[bmentry.entry_num] |= mask;
	else
    	bitmap->entries[bmentry.entry_num] &= ~(mask);

    return status;
}