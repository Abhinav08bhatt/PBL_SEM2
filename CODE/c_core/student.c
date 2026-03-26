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

int countLines(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    int count = 0;
    char ch;

    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') {
            count++;
        }
    }

    fclose(fp);
    return count;
}

/// @brief FUNCTION OBJECTIVE : read from file -> student_data.txt and display info
void viewStudent(int clear){

    if(clear == 0){
        clearTerminal();
    }

    struct STUDENT new_student;

    FILE *student_data_file_pointer_to_read;
    student_data_file_pointer_to_read = fopen("student_data.txt","r");
    
    if (student_data_file_pointer_to_read == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("\n+=================================================================+");
    printf("\n|                     LIST OF ALL STUDENT                         |");
    printf("\n+=================================================================+");
    printf("\n|  ROLL NUM  ||  NAME                                             |");
    printf("\n+------------++---------------------------------------------------+");
    printf("\n");
    
    while (fscanf(student_data_file_pointer_to_read,"%d|%49[^\n]\n",&new_student.roll_number,new_student.name)==2){
        printf("|     %-5d  ||  %-49s|\n", new_student.roll_number, new_student.name);
    }

    printf("+=================================================================+\n");

    fclose(student_data_file_pointer_to_read);

    getchar();
    printf("\n+=================================================================+");
    printf("\n|                 PRESS ENTER TO CONTINUE                         |");
    printf("\n+=================================================================+\n");
    getchar();
}

/// @brief FUNCTION OBJECTIVE : write in file -> student_data.txt -> roll_num|name 
void createStudent(){

    clearTerminal();
    
    printf("\n+=================================================================+");
    printf("\n|                     ADDING A NEW STUDENT                        |");
    printf("\n+=================================================================+");
    printf("\n");

    struct STUDENT new_student;
    
    printf("\n  ->  Enter the roll number of new student : ");
    scanf("%d",&new_student.roll_number);

    // TODO : Check if the roll number is already present in file or not

    printf("\n  ->  Enter the name of student : ");
    scanf(" %49[^\n]", new_student.name);

    FILE *student_data_file_pointer_to_write;
    student_data_file_pointer_to_write = fopen("student_data.txt","a");

    fprintf(student_data_file_pointer_to_write,"%d",new_student.roll_number);
    fprintf(student_data_file_pointer_to_write,"|");
    fprintf(student_data_file_pointer_to_write,"%s",new_student.name);
    fprintf(student_data_file_pointer_to_write,"\n");

    fclose(student_data_file_pointer_to_write);

    // clearTerminal();

    printf("\n+=================================================================+");
    printf("\n|                       NEW STUDENT ADDED                         |");
    printf("\n+=================================================================+");
    printf("\n| ROLL NUMBER : %-48d |", new_student.roll_number);
    printf("\n| NAME : %-55s |", new_student.name);
    printf("\n+=================================================================+");
    printf("\n");

    getchar();
    printf("\n+=================================================================+");
    printf("\n|                 PRESS ENTER TO CONTINUE                         |");
    printf("\n+=================================================================+\n");
    getchar();
}


/// @brief FUNCTION OBJECTIVE : remove a student with specific roll number : file -> student.txt
void removeStudent(){

    

}

int compareRoll(const void *a, const void *b) {
    struct STUDENT *s1 = (struct STUDENT *)a;
    struct STUDENT *s2 = (struct STUDENT *)b;
    
    return s1->roll_number - s2->roll_number;
}

/// @brief FUCTION OBJECTIVE : sort the students from file -> student.txt roll number wise (a->d)
void sortStudentsRollNum(){

    clearTerminal();

    int number_of_lines_in_file = countLines("student_data.txt"); 

    struct STUDENT allStudents[number_of_lines_in_file];       

    FILE *file_pointer_to_read_for_sort;
    file_pointer_to_read_for_sort = fopen("student_data.txt","r");

    if (file_pointer_to_read_for_sort == NULL) {
        printf("Error opening file!\n");
        return;
    }

    int i = 0;

    while (i < number_of_lines_in_file &&
           fscanf(file_pointer_to_read_for_sort,
                  "%d|%49[^\n]\n",
                  &allStudents[i].roll_number,
                  allStudents[i].name) == 2) {

        i++;
    }

    fclose(file_pointer_to_read_for_sort);

    int count = i;

    qsort(allStudents, count, sizeof(struct STUDENT), compareRoll);

    FILE *file_pointer_to_write_sorted;
    file_pointer_to_write_sorted = fopen("student_data.txt", "w");

    for (int j = 0; j < count; j++) {
        fprintf(file_pointer_to_write_sorted, "%d|%s\n",
                allStudents[j].roll_number,
                allStudents[j].name);
    }

    fclose(file_pointer_to_write_sorted);

    printf("\n+=================================================================+");
    printf("\n|                         FILE SORTED                             |");
    printf("\n+=================================================================+\n");

    viewStudent(1);
}