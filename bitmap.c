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

