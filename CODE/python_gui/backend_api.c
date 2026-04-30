#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../c_core/student.h"
#include "../c_core/attendance.h"

void print_error_message(const char *message_text) {
    printf("ERR|%s\n", message_text);
}

void print_ok_message(const char *message_text) {
    printf("OK|%s\n", message_text);
}

int convert_text_to_positive_roll_number(const char *text_value, int *number_output) {
    int current_index = 0;
    int final_number = 0;

    if (text_value[0] == '\0') {
        return 0;
    }

    while (text_value[current_index] != '\0') {
        if (!isdigit((unsigned char)text_value[current_index])) {
            return 0;
        }
        current_index++;
    }

    final_number = atoi(text_value);
    if (final_number <= 0) {
        return 0;
    }

    *number_output = final_number;
    return 1;
}

int command_list_students(int argument_count, char *argument_values[]) {
    struct StudentRecord all_students[MAX_SECTION_STUDENTS];
    int total_students;
    int student_index;

    if (argument_count != 4) {
        print_error_message("Usage: list_students <course> <section>");
        return 1;
    }

    total_students = load_students_from_file(argument_values[2], argument_values[3], all_students);
    sort_students_by_roll_number(all_students, total_students);

    for (student_index = 0; student_index < total_students; student_index++) {
        printf("%d|%s\n", all_students[student_index].roll_number, all_students[student_index].name);
    }

    return 0;
}

int command_add_student(int argument_count, char *argument_values[]) {
    struct StudentRecord all_students[MAX_SECTION_STUDENTS];
    struct StudentRecord new_student_record;
    char full_name_text[MAX_NAME_LENGTH];
    int total_students;
    int current_index;

    if (argument_count < 6) {
        print_error_message("Usage: add_student <course> <section> <roll> <name>");
        return 1;
    }

    if (!convert_text_to_positive_roll_number(argument_values[4], &new_student_record.roll_number)) {
        print_error_message("Invalid roll number");
        return 1;
    }

    full_name_text[0] = '\0';

    for (current_index = 5; current_index < argument_count; current_index++) {
        if (strlen(full_name_text) > 0) {
            strncat(full_name_text, " ", sizeof(full_name_text) - strlen(full_name_text) - 1);
        }
        strncat(full_name_text, argument_values[current_index], sizeof(full_name_text) - strlen(full_name_text) - 1);
    }

    if (strlen(full_name_text) == 0 || is_blank_string(full_name_text)) {
        print_error_message("Name is empty");
        return 1;
    }

    strncpy(new_student_record.name, full_name_text, sizeof(new_student_record.name) - 1);
    new_student_record.name[sizeof(new_student_record.name) - 1] = '\0';

    total_students = load_students_from_file(argument_values[2], argument_values[3], all_students);

    if (total_students >= MAX_SECTION_STUDENTS) {
        print_error_message("Section student limit reached");
        return 1;
    }

    if (find_student_index_by_roll(all_students, total_students, new_student_record.roll_number) != -1) {
        print_error_message("Roll number already exists in this section");
        return 1;
    }

    all_students[total_students] = new_student_record;
    total_students++;
    sort_students_by_roll_number(all_students, total_students);

    if (!save_students_to_file(argument_values[2], argument_values[3], all_students, total_students)) {
        print_error_message("Could not save student file");
        return 1;
    }

    print_ok_message("Student added");
    return 0;
}

int command_remove_student(int argument_count, char *argument_values[]) {
    struct StudentRecord all_students[MAX_SECTION_STUDENTS];
    int roll_number_to_remove;
    int total_students;
    int found_index;
    int current_index;

    if (argument_count != 5) {
        print_error_message("Usage: remove_student <course> <section> <roll>");
        return 1;
    }

    if (!convert_text_to_positive_roll_number(argument_values[4], &roll_number_to_remove)) {
        print_error_message("Invalid roll number");
        return 1;
    }

    total_students = load_students_from_file(argument_values[2], argument_values[3], all_students);
    found_index = find_student_index_by_roll(all_students, total_students, roll_number_to_remove);

    if (found_index == -1) {
        print_error_message("Student not found");
        return 1;
    }

    for (current_index = found_index; current_index < total_students - 1; current_index++) {
        all_students[current_index] = all_students[current_index + 1];
    }

    total_students--;

    if (!save_students_to_file(argument_values[2], argument_values[3], all_students, total_students)) {
        print_error_message("Could not save student file");
        return 1;
    }

    if (!remove_attendance_for_roll(argument_values[2], argument_values[3], roll_number_to_remove)) {
        print_error_message("Student removed but attendance cleanup failed");
        return 1;
    }

    print_ok_message("Student removed");
    return 0;
}

int read_one_status_token(const char *token_text, int *roll_number_output, char *status_output) {
    char roll_number_text[32];
    int current_index = 0;
    int roll_text_index = 0;

    while (token_text[current_index] != '\0' && token_text[current_index] != ':') {
        if (roll_text_index >= (int)sizeof(roll_number_text) - 1) {
            return 0;
        }

        roll_number_text[roll_text_index] = token_text[current_index];
        roll_text_index++;
        current_index++;
    }

    if (token_text[current_index] != ':') {
        return 0;
    }

    roll_number_text[roll_text_index] = '\0';

    if (!convert_text_to_positive_roll_number(roll_number_text, roll_number_output)) {
        return 0;
    }

    if (token_text[current_index + 1] == '\0' || token_text[current_index + 2] != '\0') {
        return 0;
    }

    *status_output = (char)toupper((unsigned char)token_text[current_index + 1]);

    if (*status_output != 'P' && *status_output != 'A') {
        return 0;
    }

    return 1;
}

int command_mark_attendance(int argument_count, char *argument_values[]) {
    struct StudentRecord all_students[MAX_SECTION_STUDENTS];
    char status_for_each_student[MAX_SECTION_STUDENTS];
    int total_students;
    int current_index;
    FILE *attendance_file_pointer;
    char filepath[256];

    if (argument_count < 6) {
        print_error_message("Usage: mark_attendance <course> <section> <YYYY-MM-DD> <roll:P|A> ...");
        return 1;
    }

    if (!is_valid_date_format(argument_values[4])) {
        print_error_message("Invalid date format (expected YYYY-MM-DD)");
        return 1;
    }

    total_students = load_students_from_file(argument_values[2], argument_values[3], all_students);

    if (total_students == 0) {
        print_error_message("No students found in this section");
        return 1;
    }

    sort_students_by_roll_number(all_students, total_students);

    for (current_index = 0; current_index < total_students; current_index++) {
        status_for_each_student[current_index] = '\0';
    }

    for (current_index = 5; current_index < argument_count; current_index++) {
        int roll_number_from_token;
        char attendance_status;
        int matching_student_index;

        if (!read_one_status_token(argument_values[current_index], &roll_number_from_token, &attendance_status)) {
            print_error_message("Invalid status token (expected roll:P or roll:A)");
            return 1;
        }

        matching_student_index = find_student_index_by_roll(all_students, total_students, roll_number_from_token);

        if (matching_student_index == -1) {
            print_error_message("Roll in status token not found");
            return 1;
        }

        status_for_each_student[matching_student_index] = attendance_status;
    }

    for (current_index = 0; current_index < total_students; current_index++) {
        if (status_for_each_student[current_index] != 'P' && status_for_each_student[current_index] != 'A') {
            print_error_message("Missing attendance status for one or more students");
            return 1;
        }
    }

    if (date_already_has_attendance(argument_values[2], argument_values[3], argument_values[4])) {
        if (!rewrite_attendance_file_without_date(argument_values[2], argument_values[3], argument_values[4])) {
            print_error_message("Could not rewrite existing attendance for date");
            return 1;
        }
    }

    get_data_file_path(filepath, "attendance", argument_values[2], argument_values[3]);
    attendance_file_pointer = fopen(filepath, "a");

    if (attendance_file_pointer == NULL) {
        print_error_message("Could not open attendance file");
        return 1;
    }

    for (current_index = 0; current_index < total_students; current_index++) {
        fprintf(
            attendance_file_pointer,
            "%s|%d|%c\n",
            argument_values[4],
            all_students[current_index].roll_number,
            status_for_each_student[current_index]
        );
    }

    fclose(attendance_file_pointer);
    print_ok_message("Attendance saved");
    return 0;
}

int command_get_report(int argument_count, char *argument_values[]) {
    struct StudentRecord all_students[MAX_SECTION_STUDENTS];
    struct AttendanceSummary student_attendance_summary;
    int roll_number_to_search;
    int total_students;
    int found_student_index;
    double attendance_percentage = 0.0;

    if (argument_count != 5) {
        print_error_message("Usage: get_report <course> <section> <roll>");
        return 1;
    }

    if (!convert_text_to_positive_roll_number(argument_values[4], &roll_number_to_search)) {
        print_error_message("Invalid roll number");
        return 1;
    }

    total_students = load_students_from_file(argument_values[2], argument_values[3], all_students);
    found_student_index = find_student_index_by_roll(all_students, total_students, roll_number_to_search);

    if (found_student_index == -1) {
        print_error_message("Student not found");
        return 1;
    }

    student_attendance_summary = get_student_attendance_summary(argument_values[2], argument_values[3], roll_number_to_search);

    if (student_attendance_summary.classes_held > 0) {
        attendance_percentage =
            ((double)student_attendance_summary.present_count * 100.0) /
            (double)student_attendance_summary.classes_held;
    }

    printf(
        "%d|%s|%d|%d|%d|%.2f\n",
        all_students[found_student_index].roll_number,
        all_students[found_student_index].name,
        student_attendance_summary.classes_held,
        student_attendance_summary.present_count,
        student_attendance_summary.absent_count,
        attendance_percentage
    );

    return 0;
}

int command_get_overall(int argument_count, char *argument_values[]) {
    struct StudentRecord all_students[MAX_SECTION_STUDENTS];
    int total_students;
    int student_index;

    if (argument_count != 4) {
        print_error_message("Usage: get_overall <course> <section>");
        return 1;
    }

    total_students = load_students_from_file(argument_values[2], argument_values[3], all_students);
    sort_students_by_roll_number(all_students, total_students);

    for (student_index = 0; student_index < total_students; student_index++) {
        struct AttendanceSummary current_student_summary;
        double attendance_percentage = 0.0;

        current_student_summary = get_student_attendance_summary(argument_values[2], argument_values[3], all_students[student_index].roll_number);

        if (current_student_summary.classes_held > 0) {
            attendance_percentage =
                ((double)current_student_summary.present_count * 100.0) /
                (double)current_student_summary.classes_held;
        }

        printf(
            "%d|%s|%d|%d|%d|%.2f\n",
            all_students[student_index].roll_number,
            all_students[student_index].name,
            current_student_summary.classes_held,
            current_student_summary.present_count,
            current_student_summary.absent_count,
            attendance_percentage
        );
    }

    return 0;
}

int main(int argument_count, char *argument_values[]) {
    if (argument_count < 2) {
        print_error_message("Usage: backend_app <command> [args]");
        return 1;
    }

    if (strcmp(argument_values[1], "list_students") == 0) {
        return command_list_students(argument_count, argument_values);
    }

    if (strcmp(argument_values[1], "add_student") == 0) {
        return command_add_student(argument_count, argument_values);
    }

    if (strcmp(argument_values[1], "remove_student") == 0) {
        return command_remove_student(argument_count, argument_values);
    }

    if (strcmp(argument_values[1], "mark_attendance") == 0) {
        return command_mark_attendance(argument_count, argument_values);
    }

    if (strcmp(argument_values[1], "get_report") == 0) {
        return command_get_report(argument_count, argument_values);
    }

    if (strcmp(argument_values[1], "get_overall") == 0) {
        return command_get_overall(argument_count, argument_values);
    }

    print_error_message("Unknown command");
    return 1;
}
