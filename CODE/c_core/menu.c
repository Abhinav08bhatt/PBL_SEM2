#include <stdio.h>

#include "student.c"
#include "attendance.c"

void display_main_menu() {
    clear_terminal_screen();

    printf("\n+=================================================================+");
    printf("\n|                      TEACHER ERP SYSTEM                         |");
    printf("\n+=================================================================+");
    printf("\n| 1. Add a new student                                            |");
    printf("\n| 2. Remove a student                                             |");
    printf("\n| 3. View all students                                            |");
    printf("\n| 4. Mark attendance for a day                                    |");
    printf("\n| 5. Search student by roll number                                |");
    printf("\n| 6. View overall attendance summary                              |");
    printf("\n| 0. Exit                                                         |");
    printf("\n+=================================================================+\n");
}

int read_main_menu_choice() {
    int menu_choice;

    printf("\nEnter your choice: ");
    if (scanf("%d", &menu_choice) != 1) {
        flush_stdin_buffer();
        return -1;
    }

    return menu_choice;
}

int main() {
    while (1) {
        display_main_menu();

        int selected_menu_choice = read_main_menu_choice();

        switch (selected_menu_choice) {
            case 1:
                add_new_student();
                break;
            case 2:
                remove_student_by_roll_number();
                break;
            case 3:
                display_all_students();
                break;
            case 4:
                mark_attendance_for_day();
                break;
            case 5:
                search_student_and_show_report();
                break;
            case 6:
                display_overall_attendance_table();
                break;
            case 0:
                clear_terminal_screen();
                printf("\nTeacher ERP closed successfully.\n\n");
                return 0;
            default:
                printf("\nInvalid menu choice.\n");
                wait_for_enter_key();
                break;
        }
    }

    return 0;
}
