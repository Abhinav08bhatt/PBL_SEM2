#include <stdio.h>

#include "student.c"
#include "attendance.c"

void print_main_menu_screen() {
    printf("\n+==========================================================+\n");
    printf("|                 STUDENT ATTENDANCE CLI                  |\n");
    printf("+==========================================================+\n");
    printf("| 1. Add Student                                           |\n");
    printf("| 2. Remove Student                                        |\n");
    printf("| 3. Show All Students                                     |\n");
    printf("| 4. Mark Attendance                                       |\n");
    printf("| 5. Search Student Report                                 |\n");
    printf("| 6. Show Overall Attendance                               |\n");
    printf("| 0. Exit                                                  |\n");
    printf("+==========================================================+\n");
}

int read_main_menu_command() {
    int menu_choice = -1;
    printf("| Enter command (0-6): ");
    if (scanf("%d", &menu_choice) != 1) {
        clear_input_buffer();
        return -1;
    }
    clear_input_buffer();
    return menu_choice;
}

void show_command_finish_message(int command_number) {
    printf("| Command %d finished.                                      |\n", command_number);
}

int main() {
    clear_terminal_screen();
    while (1) {
        print_main_menu_screen();

        int selected_menu_choice = read_main_menu_command();

        switch (selected_menu_choice) {
            case 1:
                add_student_screen();
                show_command_finish_message(1);
                clear_terminal_screen();
                break;
            case 2:
                remove_student_screen();
                show_command_finish_message(2);
                clear_terminal_screen();
                break;
            case 3:
                show_all_students_screen();
                show_command_finish_message(3);
                clear_terminal_screen();
                break;
            case 4:
                mark_attendance_screen();
                show_command_finish_message(4);
                clear_terminal_screen();
                break;
            case 5:
                search_student_report_screen();
                show_command_finish_message(5);
                clear_terminal_screen();
                break;
            case 6:
                show_overall_attendance_screen();
                show_command_finish_message(6);
                clear_terminal_screen();
                break;
            case 0:
                printf("\n+----------------------------------------------------------+\n");
                printf("| Program closed successfully.                             |\n");
                printf("+----------------------------------------------------------+\n");
                return 0;
            default:
                printf("\n+----------------------------------------------------------+\n");
                print_cli_status_message("Invalid command. Please enter only 0 to 6.");
                printf("+----------------------------------------------------------+\n");
                wait_for_user_and_clear_screen();
                break;
        }
    }

    return 0;
}
