
#include "./../include/my_put_string.h"

#include <stddef.h> // Define NULL

unsigned int string_length(char const* str)
{
    if (str == NULL)
        return 0;

    char const* cursor = str;
    while (*cursor != '\0')
    {
        cursor++;
    }

    return cursor - str;
}

#ifdef TEST_22
int main(int argc, char* argv[])
{
    int result;

    result = string_length("Bonjour");
    if (result != 7)
        my_put_string("Error 1\n");

    result = string_length("");
    if (result != 0)
        my_put_string("Error 2\n");

    result = string_length(NULL);
    if (result != 0)
        my_put_string("Error 3\n");

    return 0;
}
#endif

