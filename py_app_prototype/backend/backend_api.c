#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Reuse the existing C core logic directly from CODE/c_core without editing it.
 * This file adds a command-driven API layer for the Python frontend.
 */
#include "../../CODE/c_core/student.c"
#include "../../CODE/c_core/attendance.c"

static void print_error(const char *message) {
    printf("ERR|%s\n", message);
}

static void print_ok(const char *message) {
    printf("OK|%s\n", message);
}

static int parse_positive_int(const char *text, int *value_out) {
    char *endptr = NULL;
    long value = strtol(text, &endptr, 10);
    if (endptr == text || *endptr != '\0' || value <= 0 || value > 2147483647L) {
        return 0;
    }
    *value_out = (int)value;
    return 1;
}

static int cmd_list_students(void) {
    struct StudentRecord students[MAX_STUDENTS];
    int i;
    int count = load_students_from_file(students);

    sort_students_by_roll_number(students, count);
    for (i = 0; i < count; i++) {
        printf("%d|%s\n", students[i].roll_number, students[i].name);
    }
    return 0;
}

static int cmd_add_student(int argc, char *argv[]) {
    struct StudentRecord students[MAX_STUDENTS];
    struct StudentRecord new_student;
    int count;
    char name[MAX_NAME_LENGTH];

    if (argc < 4) {
        print_error("Usage: add_student <roll> <name>");
        return 1;
    }

    if (!parse_positive_int(argv[2], &new_student.roll_number)) {
        print_error("Invalid roll number");
        return 1;
    }

    name[0] = '\0';
    for (int i = 3; i < argc; i++) {
        if (strlen(name) > 0) {
            strncat(name, " ", sizeof(name) - strlen(name) - 1);
        }
        strncat(name, argv[i], sizeof(name) - strlen(name) - 1);
    }

    if (strlen(name) == 0 || is_blank_string(name)) {
        print_error("Name is empty");
        return 1;
    }

    strncpy(new_student.name, name, sizeof(new_student.name) - 1);
    new_student.name[sizeof(new_student.name) - 1] = '\0';

    count = load_students_from_file(students);
    if (count >= MAX_STUDENTS) {
        print_error("Student limit reached");
        return 1;
    }

    if (find_student_index_by_roll(students, count, new_student.roll_number) != -1) {
        print_error("Roll number already exists");
        return 1;
    }

    students[count++] = new_student;
    sort_students_by_roll_number(students, count);

    if (!save_students_to_file(students, count)) {
        print_error("Could not save student file");
        return 1;
    }

    print_ok("Student added");
    return 0;
}

static int cmd_remove_student(int argc, char *argv[]) {
    struct StudentRecord students[MAX_STUDENTS];
    int roll;
    int count;
    int idx;

    if (argc != 3) {
        print_error("Usage: remove_student <roll>");
        return 1;
    }

    if (!parse_positive_int(argv[2], &roll)) {
        print_error("Invalid roll number");
        return 1;
    }

    count = load_students_from_file(students);
    idx = find_student_index_by_roll(students, count, roll);
    if (idx == -1) {
        print_error("Student not found");
        return 1;
    }

    for (int i = idx; i < count - 1; i++) {
        students[i] = students[i + 1];
    }
    count--;

    if (!save_students_to_file(students, count)) {
        print_error("Could not save student file");
        return 1;
    }

    if (!remove_attendance_for_roll(roll)) {
        print_error("Student removed but attendance cleanup failed");
        return 1;
    }

    print_ok("Student removed");
    return 0;
}

static int parse_status_token(const char *token, int *roll, char *status) {
    char *colon = strchr(token, ':');
    char roll_part[32];
    if (colon == NULL) return 0;

    size_t len = (size_t)(colon - token);
    if (len == 0 || len >= sizeof(roll_part)) return 0;

    memcpy(roll_part, token, len);
    roll_part[len] = '\0';

    if (!parse_positive_int(roll_part, roll)) return 0;

    if (*(colon + 1) == '\0' || *(colon + 2) != '\0') return 0;
    *status = (char)toupper((unsigned char)*(colon + 1));
    if (*status != 'P' && *status != 'A') return 0;
    return 1;
}

static int cmd_mark_attendance(int argc, char *argv[]) {
    struct StudentRecord students[MAX_STUDENTS];
    char statuses[MAX_STUDENTS];
    int count;
    FILE *fp;

    if (argc < 4) {
        print_error("Usage: mark_attendance <YYYY-MM-DD> <roll:P|A> ...");
        return 1;
    }

    if (!is_valid_date_format(argv[2])) {
        print_error("Invalid date format (expected YYYY-MM-DD)");
        return 1;
    }

    count = load_students_from_file(students);
    if (count == 0) {
        print_error("No students found");
        return 1;
    }
    sort_students_by_roll_number(students, count);

    for (int i = 0; i < count; i++) {
        statuses[i] = '\0';
    }

    for (int i = 3; i < argc; i++) {
        int roll;
        char status;
        int idx;

        if (!parse_status_token(argv[i], &roll, &status)) {
            print_error("Invalid status token (expected roll:P or roll:A)");
            return 1;
        }

        idx = find_student_index_by_roll(students, count, roll);
        if (idx == -1) {
            print_error("Roll in status token not found");
            return 1;
        }

        statuses[idx] = status;
    }

    for (int i = 0; i < count; i++) {
        if (statuses[i] != 'P' && statuses[i] != 'A') {
            print_error("Missing attendance status for one or more students");
            return 1;
        }
    }

    if (date_already_has_attendance(argv[2])) {
        if (!rewrite_attendance_file_without_date(argv[2])) {
            print_error("Could not rewrite existing attendance for date");
            return 1;
        }
    }

    fp = fopen(ATTENDANCE_DATA_FILE, "a");
    if (fp == NULL) {
        print_error("Could not open attendance file");
        return 1;
    }

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s|%d|%c\n", argv[2], students[i].roll_number, statuses[i]);
    }

    fclose(fp);
    print_ok("Attendance saved");
    return 0;
}

static int cmd_get_report(int argc, char *argv[]) {
    struct StudentRecord students[MAX_STUDENTS];
    int roll;
    int count;
    int idx;
    struct AttendanceSummary a;
    double percent = 0.0;

    if (argc != 3) {
        print_error("Usage: get_report <roll>");
        return 1;
    }

    if (!parse_positive_int(argv[2], &roll)) {
        print_error("Invalid roll number");
        return 1;
    }

    count = load_students_from_file(students);
    idx = find_student_index_by_roll(students, count, roll);
    if (idx == -1) {
        print_error("Student not found");
        return 1;
    }

    a = get_student_attendance_summary(roll);
    if (a.classes_held > 0) {
        percent = ((double)a.present_count * 100.0) / (double)a.classes_held;
    }

    printf("%d|%s|%d|%d|%d|%.2f\n",
           students[idx].roll_number,
           students[idx].name,
           a.classes_held,
           a.present_count,
           a.absent_count,
           percent);

    return 0;
}

static int cmd_get_overall(void) {
    struct StudentRecord students[MAX_STUDENTS];
    int count = load_students_from_file(students);

    sort_students_by_roll_number(students, count);
    for (int i = 0; i < count; i++) {
        struct AttendanceSummary a = get_student_attendance_summary(students[i].roll_number);
        double percent = 0.0;
        if (a.classes_held > 0) {
            percent = ((double)a.present_count * 100.0) / (double)a.classes_held;
        }

        printf("%d|%s|%d|%d|%d|%.2f\n",
               students[i].roll_number,
               students[i].name,
               a.classes_held,
               a.present_count,
               a.absent_count,
               percent);
    }

    return 0;
}

static int cmd_get_attendance_by_date(int argc, char *argv[]) {
    FILE *fp;
    char line[300];

    if (argc != 3) {
        print_error("Usage: get_attendance_by_date <YYYY-MM-DD>");
        return 1;
    }

    if (!is_valid_date_format(argv[2])) {
        print_error("Invalid date format (expected YYYY-MM-DD)");
        return 1;
    }

    fp = fopen(ATTENDANCE_DATA_FILE, "r");
    if (fp == NULL) return 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        char d[11];
        int r;
        char s;
        if (parse_attendance_record_line(line, d, &r, &s) && strcmp(d, argv[2]) == 0) {
            printf("%d|%c\n", r, s);
        }
    }

    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_error("Usage: backend_app <command> [args]");
        return 1;
    }

    if (strcmp(argv[1], "list_students") == 0) {
        return cmd_list_students();
    }
    if (strcmp(argv[1], "add_student") == 0) {
        return cmd_add_student(argc, argv);
    }
    if (strcmp(argv[1], "remove_student") == 0) {
        return cmd_remove_student(argc, argv);
    }
    if (strcmp(argv[1], "mark_attendance") == 0) {
        return cmd_mark_attendance(argc, argv);
    }
    if (strcmp(argv[1], "get_report") == 0) {
        return cmd_get_report(argc, argv);
    }
    if (strcmp(argv[1], "get_overall") == 0) {
        return cmd_get_overall();
    }
    if (strcmp(argv[1], "get_attendance_by_date") == 0) {
        return cmd_get_attendance_by_date(argc, argv);
    }

    print_error("Unknown command");
    return 1;
}
