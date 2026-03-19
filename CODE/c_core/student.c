#include <stdio.h>
#include <string.h>

void createStudent(char name[]){
    
    FILE *student_data_file_pointer;

    student_data_file_pointer = fopen("student_data.txt","a");

    fputs(name,student_data_file_pointer);
    fputs("\n",student_data_file_pointer);
    fclose(student_data_file_pointer);

    FILE *attendance_data_file_pointer;

    attendance_data_file_pointer = fopen("attendance_data.txt","a");

    // char attendance_details = name; 
    // char attendance_details = strcat(name," : 0"); 

    fputs(name,attendance_data_file_pointer);
    // fputs(attendance_details,attendance_data_file_pointer);
    fputs("\n",attendance_data_file_pointer);
    fclose(attendance_data_file_pointer);

    printf("Student name added successfully");
}