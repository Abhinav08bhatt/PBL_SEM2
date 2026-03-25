#include <stdio.h>
#include <string.h>

struct STUDENT{
    char name[50];
    int roll_number;
};


void createStudent(){
    
    printf("\n+=================================================================+");
    printf("\n|                     ADDING A NEW STUDENT                        |");
    printf("\n+=================================================================+");
    printf("\n");

    struct STUDENT new_student;

    printf("\n  ->  Enter the name of student : ");
    scanf(" %49[^\n]", new_student.name);
    
    printf("\n  ->  Enter the roll number of student -> %s : ",new_student.name);
    scanf("%d",&new_student.roll_number);

    FILE *student_data_file_pointer_to_read;
    student_data_file_pointer_to_read = fopen("student_data.txt","r");
    
    // TODO : Check if the roll number is already present in database :
    // ! if yes -> give info of given roll num and ask for input again
    // * if no --> continue to save in file

    fclose(student_data_file_pointer_to_read);

    FILE *student_data_file_pointer_to_write;
    student_data_file_pointer_to_write = fopen("student_data.txt","a");

    fprintf(student_data_file_pointer_to_write,"%d",new_student.roll_number);
    fprintf(student_data_file_pointer_to_write,"|");
    fprintf(student_data_file_pointer_to_write,"%s",new_student.name);
    fprintf(student_data_file_pointer_to_write,"\n");

    fclose(student_data_file_pointer_to_write);

    printf("\n+=================================================================+");
    printf("\n|                       NEW STUDENT ADDED                         |");
    printf("\n+=================================================================+");
    printf("\n| NAME : %-55s |", new_student.name);
    printf("\n| ROLL NUMBER : %-48d |", new_student.roll_number);
    printf("\n+=================================================================+");
    printf("\n");

}