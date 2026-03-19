#include <stdio.h>

#include "student.c"
#include "attendance.c"

int main(){

    char name_of_student[10] = {'A','V','I'};
    
    createStudent(name_of_student);
    
    return 0;
}