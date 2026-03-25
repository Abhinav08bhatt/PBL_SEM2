#include <stdio.h>
#include <string.h>

#include "student.c"
#include "attendance.c"

void showMenu(){
    printf("\n+=================================================================+");
    printf("\n|=======  Enter valid input number for respective action  ========|");
    printf("\n+=================================================================+");
    printf("\n|                                                                 |");
    printf("\n+====  Student  ==================================================+");
    printf("\n|                                                                 |");
    printf("\n|  ( 1 )   ADD a new student                                      |");
    printf("\n|  ( 2 )   REMOVE an old student                                  |");
    printf("\n|                                                                 |");
    printf("\n+====  Attendance  ===============================================+");
    printf("\n|                                                                 |");
    printf("\n|  ( 3 )   MARK attendance of day                                 |");
    printf("\n|  ( 4 )   CHECK attendance details of students                   |");
    printf("\n|                                                                 |");
    printf("\n+====  Exit  =====================================================+");
    printf("\n|                                                                 |");
    printf("\n|  ( 0 )   to EXIT                                                |");
    printf("\n|                                                                 |");
    printf("\n+=================================================================+");
    printf("\n");
}

int askMenu(){
    int user_menu_input;
    
    printf("\n  ->  Enter the input : ");
    
    if (scanf("%d", &user_menu_input) != 1) {
        printf("\n+=================================================================+");
        printf("\n|                  INVALID INPUT , TRY AGAIN                      |");
        printf("\n+=================================================================+\n");

        while (getchar() != '\n');

        return askMenu();
    }

    if(user_menu_input>=0 && user_menu_input<=2){
        return user_menu_input;
    }
    else{
        printf("\n+=================================================================+");
        printf("\n|                  INVALID INPUT , TRY AGAIN                      |");
        printf("\n+=================================================================+");
        printf("\n");

        return askMenu();
    }
}

int main(){

    showMenu();

    printf("%d",askMenu());


    return 0;
}