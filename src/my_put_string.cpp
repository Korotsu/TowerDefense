
#include "./../include/my_put_string.h"
#include "./../include/my_put_char.h"

#include <stddef.h> // Define NULL

void my_put_string(char const* str)
{
    if (str == NULL)
        return;

    while (*str != '\0')
    {
        my_put_char(*str);
        str++;
    }
}

#ifdef TEST_21
int main(int argc, char* argv[])
{
    my_put_string(NULL);
    my_put_string("");
    my_put_string("Test\n");
    my_put_string("A long sentence\0Not visible\n");

    return 0;
}
#endif

