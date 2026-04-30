#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "attendance.h"

int parse_attendance_record_line(const char *line, char *date, int *roll, char *status) {
    if (sscanf(line, "%10[^|]|%d|%c", date, roll, status) != 3) return 0;
    *status = (char)toupper((unsigned char)*status);
    if (*roll <= 0) return 0;
    if (*status != 'P' && *status != 'A') return 0;
    if (!is_valid_date_format(date)) return 0;
    return 1;
}

int date_already_has_attendance(const char *course, const char *section, const char *target_date) {
    char filepath[256];
    get_data_file_path(filepath, "attendance", course, section);
    FILE *fp = fopen(filepath, "r");
    char line[300];
    if (fp == NULL) return 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        char d[11];
        int r;
        char stat;
        if (parse_attendance_record_line(line, d, &r, &stat)) {
            if (strcmp(d, target_date) == 0) {
                fclose(fp);
                return 1;
            }
        }
    }
    fclose(fp);
    return 0;
}

int rewrite_attendance_file_without_date(const char *course, const char *section, const char *target_date) {
    char filepath[256];
    char temp_filepath[256];
    get_data_file_path(filepath, "attendance", course, section);
    sprintf(temp_filepath, "data/attendance_tmp.txt");
    
    FILE *input = fopen(filepath, "r");
    FILE *temp;
    char line[300];
    if (input == NULL) return 1;

    temp = fopen(temp_filepath, "w");
    if (temp == NULL) {
        fclose(input);
        return 0;
    }

    while (fgets(line, sizeof(line), input) != NULL) {
        char d[11];
        int r;
        char stat;
        if (parse_attendance_record_line(line, d, &r, &stat)) {
            if (strcmp(d, target_date) != 0) {
                fprintf(temp, "%s|%d|%c\n", d, r, stat);
            }
        }
    }

    fclose(input);
    fclose(temp);
    remove(filepath);
    if (rename(temp_filepath, filepath) != 0) return 0;
    return 1;
}

void mark_attendance_screen(void) {
    struct StudentRecord students[MAX_SECTION_STUDENTS];
    char course[50];
    char section[10];
    int count;
    char date[11];
    int i;
    char filepath[256];

    print_cli_section_title("MARK ATTENDANCE");
    
    printf("| Enter course (BSc CS, BSc IT, BCA, BCA (AIDS)): ");
    read_line_input(course, sizeof(course));
    
    printf("| Enter section (A, B, C): ");
    read_line_input(section, sizeof(section));

    count = load_students_from_file(course, section, students);

    if (count == 0) {
        print_cli_status_message("No students found in this section.");
        wait_for_user_enter();
        return;
    }

    sort_students_by_roll_number(students, count);
    
    printf("| Enter date (YYYY-MM-DD): ");
    read_line_input(date, sizeof(date));
    if (!is_valid_date_format(date)) {
        print_cli_status_message("Invalid date format.");
        wait_for_user_enter();
        return;
    }

    if (date_already_has_attendance(course, section, date)) {
        char ch;
        printf("| Attendance already exists for this section. Overwrite? (Y/N): ");
        scanf(" %c", &ch);
        ch = (char)toupper((unsigned char)ch);
        if (ch != 'Y') {
            clear_input_buffer();
            print_cli_status_message("Attendance update cancelled.");
            wait_for_user_enter();
            return;
        }
        clear_input_buffer();
        if (!rewrite_attendance_file_without_date(course, section, date)) {
            print_cli_status_message("Could not rewrite attendance.");
            wait_for_user_enter();
            return;
        }
    }

    get_data_file_path(filepath, "attendance", course, section);
    FILE *fp = fopen(filepath, "a");
    if (fp == NULL) {
        print_cli_status_message("Could not open attendance file.");
        wait_for_user_enter();
        return;
    }

    printf("+----------------------------------------------------------+\n");
    printf("| Enter P for Present and A for Absent                    |\n");
    printf("+----------------------------------------------------------+\n");

    for (i = 0; i < count; i++) {
        char s;
        while (1) {
            printf("| Roll %d (%s) -> P/A: ", students[i].roll_number, students[i].name);
            scanf(" %c", &s);
            s = (char)toupper((unsigned char)s);
            if (s == 'P' || s == 'A') break;
            printf("| Only P or A allowed.\n");
        }
        clear_input_buffer();
        fprintf(fp, "%s|%d|%c\n", date, students[i].roll_number, s);
    }

    fclose(fp);
    print_cli_status_message("Attendance saved successfully.");
    wait_for_user_enter();
}

struct AttendanceSummary get_student_attendance_summary(const char *course, const char *section, int roll_number) {
    struct AttendanceSummary result;
    char filepath[256];
    get_data_file_path(filepath, "attendance", course, section);
    FILE *fp = fopen(filepath, "r");
    char line[300];
    result.classes_held = 0;
    result.present_count = 0;
    result.absent_count = 0;

    if (fp == NULL) return result;

    while (fgets(line, sizeof(line), fp) != NULL) {
        char d[11];
        int r;
        char stat;
        if (parse_attendance_record_line(line, d, &r, &stat)) {
            if (r == roll_number) {
                result.classes_held++;
                if (stat == 'P') result.present_count++;
                else result.absent_count++;
            }
        }
    }

    fclose(fp);
    return result;
}

void search_student_report_screen(void) {
    struct StudentRecord students[MAX_SECTION_STUDENTS];
    char course[50];
    char section[10];
    int count;
    int roll;
    int index;
    struct AttendanceSummary a;
    double percent = 0.0;

    print_cli_section_title("SEARCH STUDENT REPORT");
    
    printf("| Enter course (BSc CS, BSc IT, BCA, BCA (AIDS)): ");
    read_line_input(course, sizeof(course));
    
    printf("| Enter section (A, B, C): ");
    read_line_input(section, sizeof(section));

    count = load_students_from_file(course, section, students);
    
    if (count == 0) {
        print_cli_status_message("No students found in this section.");
        wait_for_user_enter();
        return;
    }

    printf("| Enter roll number: ");
    if (scanf("%d", &roll) != 1 || roll <= 0) {
        clear_input_buffer();
        print_cli_status_message("Invalid roll number.");
        wait_for_user_enter();
        return;
    }
    clear_input_buffer();

    index = find_student_index_by_roll(students, count, roll);
    if (index == -1) {
        print_cli_status_message("Student not found in this section.");
        wait_for_user_enter();
        return;
    }

    a = get_student_attendance_summary(course, section, roll);
    if (a.classes_held > 0) {
        percent = ((double)a.present_count * 100.0) / (double)a.classes_held;
    }

    printf("+----------------------------------------------------------+\n");
    printf("| Roll Number : %-42d |\n", students[index].roll_number);
    printf("| Name        : %-42s |\n", students[index].name);
    printf("| Classes     : %-42d |\n", a.classes_held);
    printf("| Present     : %-42d |\n", a.present_count);
    printf("| Absent      : %-42d |\n", a.absent_count);
    printf("| Attendance  : %-41.2f%% |\n", percent);
    printf("+----------------------------------------------------------+\n");
    wait_for_user_enter();
}

void show_overall_attendance_screen(void) {
    struct StudentRecord students[MAX_SECTION_STUDENTS];
    char course[50];
    char section[10];
    int count;
    int i;

    print_cli_section_title("OVERALL ATTENDANCE");
    
    printf("| Enter course (BSc CS, BSc IT, BCA, BCA (AIDS)): ");
    read_line_input(course, sizeof(course));
    
    printf("| Enter section (A, B, C): ");
    read_line_input(section, sizeof(section));

    count = load_students_from_file(course, section, students);

    if (count == 0) {
        print_cli_status_message("No students available in this section.");
        wait_for_user_enter();
        return;
    }

    sort_students_by_roll_number(students, count);
    printf("+--------------------------------------------------------------------------------+\n");
    printf("| %-8s | %-20s | %-7s | %-7s | %-7s | %-10s |\n", "Roll", "Name", "Classes", "Present", "Absent", "Percent");
    printf("+--------------------------------------------------------------------------------+\n");

    for (i = 0; i < count; i++) {
        struct AttendanceSummary a = get_student_attendance_summary(course, section, students[i].roll_number);
        double percent = 0.0;

        if (a.classes_held > 0) {
            percent = ((double)a.present_count * 100.0) / (double)a.classes_held;
        }
        printf("| %-8d | %-20s | %-7d | %-7d | %-7d | %-9.2f%% |\n",
               students[i].roll_number,
               students[i].name,
               a.classes_held,
               a.present_count,
               a.absent_count,
               percent);
    }
    printf("+--------------------------------------------------------------------------------+\n");
    wait_for_user_enter();
}
