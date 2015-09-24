#include "functii_tabla.h"
#include "functii_invatare.h"
#include "inlines.h"
#include "cache.h"
#include <string.h>
#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <iostream>
#include "retea.h"
#include "sah.h"


extern float param[PARAMETERS];

int read_eval_parameters()
{

	FILE *f = NULL;
	char line[80];

	if (fopen_s(&f, "D:\\param.txt", "r") || (f == NULL)) 
	{
		return -1;
	}
	
	int contor_param = 0;
	while (contor_param < PARAMETERS) 
	{
		sscanf_s(fgets(line, 80, f), "%f", param + contor_param);
		contor_param++;
	}

	fclose(f);
	return 0;

}













