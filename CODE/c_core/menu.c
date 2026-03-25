#include <stdio.h>
#include <string.h>

#include "student.c"
#include "attendance.c"

void showMenu(){
    printf("\n+=================================================================+");
    printf("\n|======  Enter the valid input number for respective action ======|");
    printf("\n+=================================================================+");
    printf("\n|  ( 1 )   ADD a new student                                      |");
    printf("\n|  ( 2 )   REMOVE an old student                                  |");
    printf("\n+=================================================================+");
    printf("\n");
}

int main(){

    showMenu();

    return 0;
}