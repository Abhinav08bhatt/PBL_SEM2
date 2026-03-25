#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/// @brief TO CLEAR TEH TERMINAL ON PURPOSE
void clearTerminal() {
#ifdef _WIN32
    system("cls");   
#else
    system("clear"); 
#endif
}

/// @brief Student structure : roll number | name
struct STUDENT{
    char name[50];
    int roll_number;
};


/// @brief FUNCTION OBJECTIVE : write in file -> student_data.txt -> roll_num|name 
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
    
    if (student_data_file_pointer_to_read != NULL) {
        struct STUDENT existing_student;
        int duplicate_found;

        do {
            duplicate_found = 0;
            rewind(student_data_file_pointer_to_read);

            while (fscanf(student_data_file_pointer_to_read, "%d|%49[^\n]\n",
                          &existing_student.roll_number, existing_student.name) == 2) {
                if (existing_student.roll_number == new_student.roll_number) {
                    duplicate_found = 1;
                    printf("\n  ->  Roll number %d is already assigned to %s.",
                           existing_student.roll_number, existing_student.name);
                    printf("\n  ->  Enter a different roll number for %s : ", new_student.name);
                    scanf("%d", &new_student.roll_number);
                    break;
                }
            }
        } while (duplicate_found);
    }

    fclose(student_data_file_pointer_to_read);

    FILE *student_data_file_pointer_to_write;
    student_data_file_pointer_to_write = fopen("student_data.txt","a");

    fprintf(student_data_file_pointer_to_write,"%d",new_student.roll_number);
    fprintf(student_data_file_pointer_to_write,"|");
    fprintf(student_data_file_pointer_to_write,"%s",new_student.name);
    fprintf(student_data_file_pointer_to_write,"\n");

    fclose(student_data_file_pointer_to_write);

    clearTerminal();

    printf("\n+=================================================================+");
    printf("\n|                       NEW STUDENT ADDED                         |");
    printf("\n+=================================================================+");
    printf("\n| NAME : %-55s |", new_student.name);
    printf("\n| ROLL NUMBER : %-48d |", new_student.roll_number);
    printf("\n+=================================================================+");
    printf("\n");

}


/// @brief FUNCTION OBJECTIVE : remove a student with specific roll number : file -> student.txt
void removeStudent(){

    

}
