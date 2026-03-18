#include <stdio.h>

void writeInFile(char arr[]){

    FILE *file_pointer;

    file_pointer = fopen("dataset.txt","w");

    if(file_pointer == NULL){
        printf("Could not open file");
    }
    else{
        printf("Opened the file");
        fputs(arr,file_pointer);
        fputs("\n",file_pointer);
        
        fclose(file_pointer);
        printf("Data is written\nFile Closed");
    }
}