#include <string.h>
#include <stdlib.h>
#include "fortran.h"

int ntab = 0;
struct id_tab Table[10000];

struct id_tab *get_id(string name)
{
	int i;
	for (i = 0; i < ntab; ++i) {
		if (name == Table[i].name)
		       return &(Table[i]);
	}
	return NULL;
}

void add_id(string name, int value)
{
	Table[ntab].name = name;
	Table[ntab].value = value; 
	++ntab;
}
