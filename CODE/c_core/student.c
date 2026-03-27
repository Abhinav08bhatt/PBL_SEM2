#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STUDENT_DATA_FILE "student_data.txt"
#define ATTENDANCE_DATA_FILE "attendance_data.txt"
#define MAX_STUDENTS 1000
#define MAX_NAME_LENGTH 100
#define MAX_LINE_LENGTH 256

struct StudentRecord {
    int roll_number;
    char name[MAX_NAME_LENGTH];
};

void clear_terminal_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void flush_stdin_buffer() {
    int character;
    while ((character = getchar()) != '\n' && character != EOF) {
    }
}

void wait_for_enter_key() {
    printf("\nPress ENTER to continue...");
    fflush(stdout);
    flush_stdin_buffer();
}

void read_text_input(char *destination, int destination_size) {
    if (fgets(destination, destination_size, stdin) == NULL) {
        destination[0] = '\0';
        return;
    }

    destination[strcspn(destination, "\n")] = '\0';
}

int is_valid_date_format(const char *date_value) {
    if (strlen(date_value) != 10) {
        return 0;
    }

    if (date_value[4] != '-' || date_value[7] != '-') {
        return 0;
    }

    for (int index = 0; index < 10; index++) {
        if (index == 4 || index == 7) {
            continue;
        }
        if (!isdigit((unsigned char)date_value[index])) {
            return 0;
        }
    }

    int month_value = (date_value[5] - '0') * 10 + (date_value[6] - '0');
    int day_value = (date_value[8] - '0') * 10 + (date_value[9] - '0');

    if (month_value < 1 || month_value > 12) {
        return 0;
    }
    if (day_value < 1 || day_value > 31) {
        return 0;
    }

    return 1;
}

void sort_student_records_by_roll(struct StudentRecord students[], int student_count) {
    for (int outer_index = 0; outer_index < student_count - 1; outer_index++) {
        for (int inner_index = 0; inner_index < student_count - outer_index - 1; inner_index++) {
            if (students[inner_index].roll_number > students[inner_index + 1].roll_number) {
                struct StudentRecord temporary_record = students[inner_index];
                students[inner_index] = students[inner_index + 1];
                students[inner_index + 1] = temporary_record;
            }
        }
    }
}

int load_all_students(struct StudentRecord students[]) {
    FILE *student_file_pointer = fopen(STUDENT_DATA_FILE, "r");
    if (student_file_pointer == NULL) {
        return 0;
    }

    int student_count = 0;
    while (student_count < MAX_STUDENTS &&
           fscanf(student_file_pointer, "%d|%99[^\n]\n",
                  &students[student_count].roll_number,
                  students[student_count].name) == 2) {
        student_count++;
    }

    fclose(student_file_pointer);
    return student_count;
}

int save_all_students(struct StudentRecord students[], int student_count) {
    FILE *student_file_pointer = fopen(STUDENT_DATA_FILE, "w");
    if (student_file_pointer == NULL) {
        printf("\nError: unable to write student data file.\n");
        return 0;
    }

    for (int student_index = 0; student_index < student_count; student_index++) {
        fprintf(student_file_pointer, "%d|%s\n",
                students[student_index].roll_number,
                students[student_index].name);
    }

    fclose(student_file_pointer);
    return 1;
}

int find_student_index_by_roll_number(struct StudentRecord students[], int student_count, int roll_number) {
    for (int student_index = 0; student_index < student_count; student_index++) {
        if (students[student_index].roll_number == roll_number) {
            return student_index;
        }
    }
    return -1;
}

int remove_attendance_records_for_roll_number(int roll_number) {
    FILE *attendance_file_pointer = fopen(ATTENDANCE_DATA_FILE, "r");
    if (attendance_file_pointer == NULL) {
        return 1;
    }

    FILE *temporary_file_pointer = fopen("attendance_tmp.txt", "w");
    if (temporary_file_pointer == NULL) {
        fclose(attendance_file_pointer);
        return 0;
    }

    char attendance_date[11];
    int attendance_roll_number;
    char attendance_status;

    while (fscanf(attendance_file_pointer, "%10[^|]|%d|%c\n",
                  attendance_date,
                  &attendance_roll_number,
                  &attendance_status) == 3) {
        if (attendance_roll_number != roll_number) {
            fprintf(temporary_file_pointer, "%s|%d|%c\n",
                    attendance_date,
                    attendance_roll_number,
                    attendance_status);
        }
    }

    fclose(attendance_file_pointer);
    fclose(temporary_file_pointer);

    remove(ATTENDANCE_DATA_FILE);
    if (rename("attendance_tmp.txt", ATTENDANCE_DATA_FILE) != 0) {
        return 0;
    }

    return 1;
}

void display_all_students() {
    clear_terminal_screen();

    struct StudentRecord students[MAX_STUDENTS];
    int student_count = load_all_students(students);
    sort_student_records_by_roll(students, student_count);

    printf("\n+=================================================================+");
    printf("\n|                         STUDENT LIST                            |");
    printf("\n+=================================================================+");

    if (student_count == 0) {
        printf("\n| %-63s |", "No students found.");
        printf("\n+=================================================================+\n");
        wait_for_enter_key();
        return;
    }

    printf("\n| %-10s | %-48s |", "ROLL NO", "NAME");
    printf("\n+=================================================================+\n");

    for (int student_index = 0; student_index < student_count; student_index++) {
        printf("| %-10d | %-48s |\n",
               students[student_index].roll_number,
               students[student_index].name);
    }

    printf("+=================================================================+\n");
    wait_for_enter_key();
}

void add_new_student() {
    clear_terminal_screen();

    struct StudentRecord students[MAX_STUDENTS];
    int student_count = load_all_students(students);

    if (student_count >= MAX_STUDENTS) {
        printf("\nStudent limit reached. Cannot add more records.\n");
        wait_for_enter_key();
        return;
    }

    struct StudentRecord new_student;

    printf("\nEnter roll number: ");
    if (scanf("%d", &new_student.roll_number) != 1 || new_student.roll_number <= 0) {
        printf("Invalid roll number.\n");
        wait_for_enter_key();
        return;
    }

    if (find_student_index_by_roll_number(students, student_count, new_student.roll_number) != -1) {
        printf("\nRoll number already exists.\n");
        wait_for_enter_key();
        return;
    }

    flush_stdin_buffer();

    printf("Enter student name: ");
    read_text_input(new_student.name, MAX_NAME_LENGTH);

    if (strlen(new_student.name) == 0) {
        printf("\nStudent name cannot be empty.\n");
        wait_for_enter_key();
        return;
    }

    students[student_count] = new_student;
    student_count++;
    sort_student_records_by_roll(students, student_count);

    if (!save_all_students(students, student_count)) {
        wait_for_enter_key();
        return;
    }

    printf("\nStudent added successfully.\n");
    wait_for_enter_key();
}

void remove_student_by_roll_number() {
    clear_terminal_screen();

    struct StudentRecord students[MAX_STUDENTS];
    int student_count = load_all_students(students);

    if (student_count == 0) {
        printf("\nNo students available to remove.\n");
        wait_for_enter_key();
        return;
    }

    int roll_number_to_remove;
    printf("\nEnter roll number to remove: ");
    if (scanf("%d", &roll_number_to_remove) != 1 || roll_number_to_remove <= 0) {
        printf("Invalid roll number.\n");
        wait_for_enter_key();
        return;
    }

    int remove_index = find_student_index_by_roll_number(students, student_count, roll_number_to_remove);
    if (remove_index == -1) {
        printf("\nStudent not found.\n");
        wait_for_enter_key();
        return;
    }

    for (int student_index = remove_index; student_index < student_count - 1; student_index++) {
        students[student_index] = students[student_index + 1];
    }
    student_count--;

    if (!save_all_students(students, student_count)) {
        wait_for_enter_key();
        return;
    }

    if (!remove_attendance_records_for_roll_number(roll_number_to_remove)) {
        printf("\nStudent removed, but failed to clean attendance records.\n");
        wait_for_enter_key();
        return;
    }

    printf("\nStudent removed successfully.\n");
    wait_for_enter_key();
}
