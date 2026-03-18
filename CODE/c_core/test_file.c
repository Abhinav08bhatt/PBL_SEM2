#include <stdio.h>
#include "test_header_file.c"

int main(){


    char data[1000] = "This is a sample data";

    writeInFile(data);

    return 0;
}