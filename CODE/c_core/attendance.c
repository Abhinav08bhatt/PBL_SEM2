#include <stdio.h>

void markAttendance(char name[], int present_num){
    FILE *attendance_data_file_pointer;

    attendance_data_file_pointer = fopen("attendance_data.txt","r");

    char attendance_data_instance[100000];
    while(fgets(attendance_data_instance,100000,attendance_data_file_pointer)!=NULL){
        printf("%s",attendance_data_instance);
    }

    printf("Marked attendance of the student");
}