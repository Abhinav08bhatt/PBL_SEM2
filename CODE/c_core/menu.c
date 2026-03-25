#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "student.c"
#include "attendance.c"

void clearTerminal() {
#ifdef _WIN32
    system("cls");   // Windows
#else
    system("clear"); // Linux & macOS
#endif
}


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

void showExit(){
    printf("\n+=================================================================+");
    printf("\n|                   PBL PROJECT TERMINATED                        |");
    printf("\n+=================================================================+");   
    printf("\n");
}

int askMenu(){
    int user_menu_input;
    
    printf("\n  ->  Enter the input : ");
    
    if (scanf("%d", &user_menu_input) != 1) {

        clearTerminal();

        showMenu();

        printf("\n+=================================================================+");
        printf("\n|                  INVALID INPUT , TRY AGAIN                      |");
        printf("\n+=================================================================+");
        printf("\n");

        while (getchar() != '\n');

        return askMenu();
    }

    if(user_menu_input>=0 && user_menu_input<=2){
        return user_menu_input;
    }
    else{

        clearTerminal();

        showMenu();

        printf("\n+=================================================================+");
        printf("\n|                  INVALID INPUT , TRY AGAIN                      |");
        printf("\n+=================================================================+");
        printf("\n");

        return askMenu();
    }
}

int main(){

    showMenu();

    int user_menu_action = askMenu();

    switch (user_menu_action)
    {
    case 0:
        showExit();
        return 0;
    
    case 1:
        createStudent();
        main();
        break;
    
    case 2:
        /* code */
        break;
    
    case 3:
        /* code */
        break;
    
    case 4:
        /* code */
        break;
    
    default:
        printf("\n~ ! @ # $ ^ & * ( Uh oh! A BUG ) * & ^ $ # @ ! ~ \n");
        break;
    }

    return 0;
}