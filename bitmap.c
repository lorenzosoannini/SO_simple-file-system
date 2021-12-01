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

// JM ritorna l'indice del del primo bit che ha lo status "status" 
// nella bitmap bmap e inizo a cercare dalla posizione "start"
int BitMap_get(BitMap* bmap, int start, int status){


	// il for permette di cercare l'indice del primo bit con lo status "status" nella bmap
	int i;  
	// variabile che indica la posizione dell offset
	int offsetp;
	 
	for(i = start; i<= bitmap->num_bits-1; i++) {

		
		
		BitMapEntryKey bmkey = BitMap_blockToIndex(i);
		
		 // setta una maschera shiftando 1 nella posizione dell'offset
		uint8_t bmask = 1 << (7 - bmek.bit_num);
		
		// faccio un and bit a bit per selezionare il bit nella posizone nella posizione dell'offset
	 	offsetp = (bitmap->entries[bmkey.entry_num] & bmask); 

		// Se dobbiamo verificare "status=1", result deve essere ">0", altrimenti deve essere "==0"
		if(status == 1) {
			if(offsetp > 0) return i;
		}else{
			if(offsetp == 0) return i;
		}
	}
}

// sets the bit at index pos in bmap to status
int BitMap_set(BitMap* bmap, int pos, int status){

}
