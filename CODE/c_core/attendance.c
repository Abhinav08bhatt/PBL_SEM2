#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct AttendanceSummary {
    int classes_held;
    int present_count;
    int absent_count;
};

int parse_attendance_record_line(const char *line, char *date, int *roll, char *status) {
    if (sscanf(line, "%10[^|]|%d|%c", date, roll, status) != 3) return 0;
    *status = (char)toupper((unsigned char)*status);
    if (*roll <= 0) return 0;
    if (*status != 'P' && *status != 'A') return 0;
    if (!is_valid_date_format(date)) return 0;
    return 1;
}

int date_already_has_attendance(const char *target_date) {
    FILE *fp = fopen(ATTENDANCE_DATA_FILE, "r");
    char line[300];
    if (fp == NULL) return 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        char d[11];
        int r;
        char s;
        if (parse_attendance_record_line(line, d, &r, &s)) {
            if (strcmp(d, target_date) == 0) {
                fclose(fp);
                return 1;
            }
        }
    }
    fclose(fp);
    return 0;
}

int rewrite_attendance_file_without_date(const char *target_date) {
    FILE *input = fopen(ATTENDANCE_DATA_FILE, "r");
    FILE *temp;
    char line[300];
    if (input == NULL) return 1;

    temp = fopen("attendance_tmp.txt", "w");
    if (temp == NULL) {
        fclose(input);
        return 0;
    }

    while (fgets(line, sizeof(line), input) != NULL) {
        char d[11];
        int r;
        char s;
        if (parse_attendance_record_line(line, d, &r, &s)) {
            if (strcmp(d, target_date) != 0) {
                fprintf(temp, "%s|%d|%c\n", d, r, s);
            }
        }
    }

    fclose(input);
    fclose(temp);
    remove(ATTENDANCE_DATA_FILE);
    if (rename("attendance_tmp.txt", ATTENDANCE_DATA_FILE) != 0) return 0;
    return 1;
}

void mark_attendance_screen() {
    struct StudentRecord students[MAX_STUDENTS];
    int count = load_students_from_file(students);
    char date[11];
    int i;

    print_cli_section_title("MARK ATTENDANCE");
    if (count == 0) {
        print_cli_status_message("No students found.");
        wait_for_user_enter();
        return;
    }

    sort_students_by_roll_number(students, count);
    clear_input_buffer();
    printf("| Enter date (YYYY-MM-DD): ");
    read_line_input(date, sizeof(date));
    if (!is_valid_date_format(date)) {
        print_cli_status_message("Invalid date format.");
        wait_for_user_enter();
        return;
    }

    if (date_already_has_attendance(date)) {
        char ch;
        printf("| Date already exists. Overwrite? (Y/N): ");
        scanf(" %c", &ch);
        ch = (char)toupper((unsigned char)ch);
        if (ch != 'Y') {
            clear_input_buffer();
            print_cli_status_message("Attendance update cancelled.");
            wait_for_user_enter();
            return;
        }
        clear_input_buffer();
        if (!rewrite_attendance_file_without_date(date)) {
            print_cli_status_message("Could not rewrite attendance.");
            wait_for_user_enter();
            return;
        }
    }

    FILE *fp = fopen(ATTENDANCE_DATA_FILE, "a");
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

struct AttendanceSummary get_student_attendance_summary(int roll_number) {
    struct AttendanceSummary result;
    FILE *fp = fopen(ATTENDANCE_DATA_FILE, "r");
    char line[300];
    result.classes_held = 0;
    result.present_count = 0;
    result.absent_count = 0;

    if (fp == NULL) return result;

    while (fgets(line, sizeof(line), fp) != NULL) {
        char d[11];
        int r;
        char s;
        if (parse_attendance_record_line(line, d, &r, &s)) {
            if (r == roll_number) {
                result.classes_held++;
                if (s == 'P') result.present_count++;
                else result.absent_count++;
            }
        }
    }

    fclose(fp);
    return result;
}

void search_student_report_screen() {
    struct StudentRecord students[MAX_STUDENTS];
    int count = load_students_from_file(students);
    int roll;
    int index;
    struct AttendanceSummary a;
    double percent = 0.0;

    print_cli_section_title("SEARCH STUDENT REPORT");
    if (count == 0) {
        print_cli_status_message("No students available.");
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
        print_cli_status_message("Student not found.");
        wait_for_user_enter();
        return;
    }

    a = get_student_attendance_summary(roll);
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

void show_overall_attendance_screen() {
    struct StudentRecord students[MAX_STUDENTS];
    int count = load_students_from_file(students);
    int i;

    print_cli_section_title("OVERALL ATTENDANCE");
    if (count == 0) {
        print_cli_status_message("No students available.");
        wait_for_user_enter();
        return;
    }

    sort_students_by_roll_number(students, count);
    printf("+--------------------------------------------------------------------------------+\n");
    printf("| %-8s | %-20s | %-7s | %-7s | %-7s | %-10s |\n", "Roll", "Name", "Classes", "Present", "Absent", "Percent");
    printf("+--------------------------------------------------------------------------------+\n");

    for (i = 0; i < count; i++) {
        struct AttendanceSummary a = get_student_attendance_summary(students[i].roll_number);
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
