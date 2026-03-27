#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_DATE_LENGTH 11

struct AttendanceSummary {
    int classes_held;
    int present_count;
    int absent_count;
};

int has_attendance_records_for_date(const char *target_date) {
    FILE *attendance_file_pointer = fopen(ATTENDANCE_DATA_FILE, "r");
    if (attendance_file_pointer == NULL) {
        return 0;
    }

    char attendance_date[MAX_DATE_LENGTH];
    int attendance_roll_number;
    char attendance_status;

    while (fscanf(attendance_file_pointer, "%10[^|]|%d|%c\n",
                  attendance_date,
                  &attendance_roll_number,
                  &attendance_status) == 3) {
        if (strcmp(attendance_date, target_date) == 0) {
            fclose(attendance_file_pointer);
            return 1;
        }
    }

    fclose(attendance_file_pointer);
    return 0;
}

int rewrite_attendance_without_date(const char *target_date) {
    FILE *attendance_file_pointer = fopen(ATTENDANCE_DATA_FILE, "r");
    if (attendance_file_pointer == NULL) {
        return 1;
    }

    FILE *temporary_file_pointer = fopen("attendance_tmp.txt", "w");
    if (temporary_file_pointer == NULL) {
        fclose(attendance_file_pointer);
        return 0;
    }

    char attendance_date[MAX_DATE_LENGTH];
    int attendance_roll_number;
    char attendance_status;

    while (fscanf(attendance_file_pointer, "%10[^|]|%d|%c\n",
                  attendance_date,
                  &attendance_roll_number,
                  &attendance_status) == 3) {
        if (strcmp(attendance_date, target_date) != 0) {
            fprintf(temporary_file_pointer, "%s|%d|%c\n",
                    attendance_date,
                    attendance_roll_number,
                    attendance_status);
        }
    }

    fclose(attendance_file_pointer);
    fclose(temporary_file_pointer);

    if (remove(ATTENDANCE_DATA_FILE) != 0) {
        return 0;
    }

    if (rename("attendance_tmp.txt", ATTENDANCE_DATA_FILE) != 0) {
        return 0;
    }

    return 1;
}

void mark_attendance_for_day() {
    clear_terminal_screen();

    struct StudentRecord students[MAX_STUDENTS];
    int student_count = load_all_students(students);

    if (student_count == 0) {
        printf("\nNo students found. Add students before marking attendance.\n");
        wait_for_enter_key();
        return;
    }

    sort_student_records_by_roll(students, student_count);

    char attendance_date[MAX_DATE_LENGTH];
    printf("\nEnter attendance date (YYYY-MM-DD): ");
    flush_stdin_buffer();
    read_text_input(attendance_date, MAX_DATE_LENGTH);

    if (!is_valid_date_format(attendance_date)) {
        printf("\nInvalid date format. Use YYYY-MM-DD.\n");
        wait_for_enter_key();
        return;
    }

    if (has_attendance_records_for_date(attendance_date)) {
        char choice_value;
        printf("\nAttendance already exists for this date. Overwrite? (Y/N): ");
        scanf(" %c", &choice_value);
        choice_value = (char)toupper((unsigned char)choice_value);

        if (choice_value != 'Y') {
            printf("\nAttendance marking cancelled.\n");
            wait_for_enter_key();
            return;
        }

        if (!rewrite_attendance_without_date(attendance_date)) {
            printf("\nFailed to overwrite existing attendance.\n");
            wait_for_enter_key();
            return;
        }
    }

    FILE *attendance_file_pointer = fopen(ATTENDANCE_DATA_FILE, "a");
    if (attendance_file_pointer == NULL) {
        printf("\nUnable to open attendance file.\n");
        wait_for_enter_key();
        return;
    }

    printf("\nMark attendance: P for Present, A for Absent\n");

    for (int student_index = 0; student_index < student_count; student_index++) {
        char attendance_status;

        while (1) {
            printf("Roll %d | %-30s : ",
                   students[student_index].roll_number,
                   students[student_index].name);

            scanf(" %c", &attendance_status);
            attendance_status = (char)toupper((unsigned char)attendance_status);

            if (attendance_status == 'P' || attendance_status == 'A') {
                break;
            }

            printf("Invalid status. Enter only P or A.\n");
        }

        fprintf(attendance_file_pointer, "%s|%d|%c\n",
                attendance_date,
                students[student_index].roll_number,
                attendance_status);
    }

    fclose(attendance_file_pointer);

    printf("\nAttendance marked successfully for %s.\n", attendance_date);
    wait_for_enter_key();
}

struct AttendanceSummary calculate_student_attendance_summary(int roll_number) {
    struct AttendanceSummary summary_data;
    summary_data.classes_held = 0;
    summary_data.present_count = 0;
    summary_data.absent_count = 0;

    FILE *attendance_file_pointer = fopen(ATTENDANCE_DATA_FILE, "r");
    if (attendance_file_pointer == NULL) {
        return summary_data;
    }

    char attendance_date[MAX_DATE_LENGTH];
    int attendance_roll_number;
    char attendance_status;

    while (fscanf(attendance_file_pointer, "%10[^|]|%d|%c\n",
                  attendance_date,
                  &attendance_roll_number,
                  &attendance_status) == 3) {
        if (attendance_roll_number != roll_number) {
            continue;
        }

        summary_data.classes_held++;

        if (attendance_status == 'P') {
            summary_data.present_count++;
        } else if (attendance_status == 'A') {
            summary_data.absent_count++;
        }
    }

    fclose(attendance_file_pointer);
    return summary_data;
}

void search_student_and_show_report() {
    clear_terminal_screen();

    struct StudentRecord students[MAX_STUDENTS];
    int student_count = load_all_students(students);

    if (student_count == 0) {
        printf("\nNo students available.\n");
        wait_for_enter_key();
        return;
    }

    int search_roll_number;
    printf("\nEnter roll number to search: ");
    if (scanf("%d", &search_roll_number) != 1 || search_roll_number <= 0) {
        printf("Invalid roll number.\n");
        wait_for_enter_key();
        return;
    }

    int student_index = find_student_index_by_roll_number(students, student_count, search_roll_number);
    if (student_index == -1) {
        printf("\nStudent not found.\n");
        wait_for_enter_key();
        return;
    }

    struct AttendanceSummary summary_data = calculate_student_attendance_summary(search_roll_number);

    double attendance_percentage = 0.0;
    if (summary_data.classes_held > 0) {
        attendance_percentage =
            ((double)summary_data.present_count * 100.0) / (double)summary_data.classes_held;
    }

    printf("\n+===============================================================+");
    printf("\n|                     STUDENT REPORT                            |");
    printf("\n+===============================================================+");
    printf("\n| Roll Number       : %-42d |", students[student_index].roll_number);
    printf("\n| Name              : %-42s |", students[student_index].name);
    printf("\n| Classes Held      : %-42d |", summary_data.classes_held);
    printf("\n| Present Count     : %-42d |", summary_data.present_count);
    printf("\n| Absent Count      : %-42d |", summary_data.absent_count);
    printf("\n| Attendance Avg    : %-41.2f%% |", attendance_percentage);
    printf("\n+===============================================================+\n");

    wait_for_enter_key();
}

void display_overall_attendance_table() {
    clear_terminal_screen();

    struct StudentRecord students[MAX_STUDENTS];
    int student_count = load_all_students(students);

    if (student_count == 0) {
        printf("\nNo students available.\n");
        wait_for_enter_key();
        return;
    }

    sort_student_records_by_roll(students, student_count);

    printf("\n+=================================================================================+");
    printf("\n|                           OVERALL ATTENDANCE SUMMARY                            |");
    printf("\n+=================================================================================+");
    printf("\n| %-8s | %-24s | %-8s | %-8s | %-8s | %-10s |",
           "ROLL NO", "NAME", "CLASSES", "PRESENT", "ABSENT", "AVG %");
    printf("\n+=================================================================================+\n");

    for (int student_index = 0; student_index < student_count; student_index++) {
        struct AttendanceSummary summary_data =
            calculate_student_attendance_summary(students[student_index].roll_number);

        double attendance_percentage = 0.0;
        if (summary_data.classes_held > 0) {
            attendance_percentage =
                ((double)summary_data.present_count * 100.0) / (double)summary_data.classes_held;
        }

        printf("| %-8d | %-24.24s | %-8d | %-8d | %-8d | %-9.2f |\n",
               students[student_index].roll_number,
               students[student_index].name,
               summary_data.classes_held,
               summary_data.present_count,
               summary_data.absent_count,
               attendance_percentage);
    }

    printf("+=================================================================================+\n");
    wait_for_enter_key();
}
