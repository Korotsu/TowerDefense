#include <stdio.h>
#include "./../include/my_put_char.h"

void my_put_char(char c)
{
	fwrite(&c,sizeof(char),1,stdout);
}
