#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STUDENT_DATA_FILE "student_data.txt"
#define ATTENDANCE_DATA_FILE "attendance_data.txt"
#define MAX_STUDENTS 300
#define MAX_NAME_LENGTH 100

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

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void wait_for_user_enter() {
    printf("\n| Press ENTER to continue...");
    fflush(stdout);
    clear_input_buffer();
    printf("\n");
}

void read_line_input(char *destination, int size) {
    if (fgets(destination, size, stdin) == NULL) {
        destination[0] = '\0';
        return;
    }
    destination[strcspn(destination, "\n")] = '\0';
}

void print_cli_section_title(const char *title_text) {
    printf("\n+==========================================================+\n");
    printf("| %-56s |\n", title_text);
    printf("+==========================================================+\n");
}

void print_cli_status_message(const char *message_text) {
    printf("| %-56s |\n", message_text);
}

void wait_for_user_and_clear_screen() {
    wait_for_user_enter();
    clear_terminal_screen();
}

int is_valid_date_format(const char *date_value) {
    int i;
    if (strlen(date_value) != 10) return 0;
    if (date_value[4] != '-' || date_value[7] != '-') return 0;

    for (i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)date_value[i])) return 0;
    }
    return 1;
}

int is_blank_string(const char *text) {
    int i = 0;
    while (text[i] != '\0') {
        if (!isspace((unsigned char)text[i])) return 0;
        i++;
    }
    return 1;
}

void sort_students_by_roll_number(struct StudentRecord students[], int student_count) {
    int i, j;
    for (i = 0; i < student_count - 1; i++) {
        for (j = 0; j < student_count - i - 1; j++) {
            if (students[j].roll_number > students[j + 1].roll_number) {
                struct StudentRecord temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
}

int parse_student_line(const char *line, struct StudentRecord *student) {
    int roll;
    char name[MAX_NAME_LENGTH];
    if (sscanf(line, "%d|%99[^\n]", &roll, name) != 2) return 0;
    if (roll <= 0) return 0;
    if (is_blank_string(name)) return 0;
    student->roll_number = roll;
    strcpy(student->name, name);
    return 1;
}

int load_students_from_file(struct StudentRecord students[]) {
    FILE *fp = fopen(STUDENT_DATA_FILE, "r");
    char line[300];
    int count = 0;
    if (fp == NULL) return 0;

    while (fgets(line, sizeof(line), fp) != NULL && count < MAX_STUDENTS) {
        struct StudentRecord s;
        if (parse_student_line(line, &s)) {
            students[count] = s;
            count++;
        }
    }
    fclose(fp);
    return count;
}

int save_students_to_file(struct StudentRecord students[], int student_count) {
    FILE *fp = fopen(STUDENT_DATA_FILE, "w");
    int i;
    if (fp == NULL) return 0;

    for (i = 0; i < student_count; i++) {
        fprintf(fp, "%d|%s\n", students[i].roll_number, students[i].name);
    }
    fclose(fp);
    return 1;
}

int find_student_index_by_roll(struct StudentRecord students[], int student_count, int roll_number) {
    int i;
    for (i = 0; i < student_count; i++) {
        if (students[i].roll_number == roll_number) return i;
    }
    return -1;
}

int remove_attendance_for_roll(int roll_number) {
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
        char date[20];
        int roll;
        char status;
        if (sscanf(line, "%19[^|]|%d|%c", date, &roll, &status) == 3) {
            if (roll != roll_number) {
                fprintf(temp, "%s|%d|%c\n", date, roll, status);
            }
        }
    }

    fclose(input);
    fclose(temp);
    remove(ATTENDANCE_DATA_FILE);
    if (rename("attendance_tmp.txt", ATTENDANCE_DATA_FILE) != 0) return 0;
    return 1;
}

void add_student_screen() {
    struct StudentRecord students[MAX_STUDENTS];
    struct StudentRecord new_student;
    int count = load_students_from_file(students);

    print_cli_section_title("ADD STUDENT");
    if (count >= MAX_STUDENTS) {
        print_cli_status_message("Cannot add more students.");
        wait_for_user_enter();
        return;
    }

    printf("| Enter roll number: ");
    if (scanf("%d", &new_student.roll_number) != 1 || new_student.roll_number <= 0) {
        clear_input_buffer();
        print_cli_status_message("Invalid roll number.");
        wait_for_user_enter();
        return;
    }
    clear_input_buffer();

    if (find_student_index_by_roll(students, count, new_student.roll_number) != -1) {
        print_cli_status_message("Roll number already exists.");
        wait_for_user_enter();
        return;
    }

    printf("| Enter name: ");
    read_line_input(new_student.name, MAX_NAME_LENGTH);
    if (strlen(new_student.name) == 0 || is_blank_string(new_student.name)) {
        print_cli_status_message("Name is empty.");
        wait_for_user_enter();
        return;
    }

    students[count] = new_student;
    count++;
    sort_students_by_roll_number(students, count);
    if (!save_students_to_file(students, count)) {
        print_cli_status_message("Could not save file.");
        wait_for_user_enter();
        return;
    }
    print_cli_status_message("Student added successfully.");
    wait_for_user_enter();
}

void remove_student_screen() {
    struct StudentRecord students[MAX_STUDENTS];
    int count = load_students_from_file(students);
    int roll;
    int i, index;

    print_cli_section_title("REMOVE STUDENT");
    if (count == 0) {
        print_cli_status_message("No students to remove.");
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
    
    for (i = index; i < count - 1; i++) {
        students[i] = students[i + 1];
    }
    count--;
    
    if (!save_students_to_file(students, count)) {
        print_cli_status_message("Could not save student file.");
        wait_for_user_enter();
        return;
    }
    remove_attendance_for_roll(roll);
    print_cli_status_message("Student removed successfully.");
    wait_for_user_enter();
}


void show_all_students_screen() {
    struct StudentRecord students[MAX_STUDENTS];
    int count = load_students_from_file(students);
    int i;
    sort_students_by_roll_number(students, count);

    print_cli_section_title("ALL STUDENTS");
    if (count == 0) {
        print_cli_status_message("No students found.");
        wait_for_user_enter();
        return;
    }

    printf("| %-10s | %-41s |\n", "Roll No", "Name");
    printf("+------------+-------------------------------------------+\n");
    for (i = 0; i < count; i++) {
        printf("| %-10d | %-41s |\n", students[i].roll_number, students[i].name);
    }
    printf("+------------+-------------------------------------------+\n");
    wait_for_user_enter();
}