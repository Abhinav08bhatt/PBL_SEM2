#ifndef STUDENT_H
#define STUDENT_H

/* File names and common limits used in the project. */
#define MAX_STUDENTS 300
#define MAX_NAME_LENGTH 100
#define MAX_SECTION_STUDENTS 20

/* Basic structure for one student. */
struct StudentRecord {
    int roll_number;
    char name[MAX_NAME_LENGTH];
};

/* Small helper functions used in many places. */
void clear_terminal_screen(void);
void clear_input_buffer(void);
void wait_for_user_enter(void);
void read_line_input(char *destination, int size);
void print_cli_section_title(const char *title_text);
void print_cli_status_message(const char *message_text);
void wait_for_user_and_clear_screen(void);
int is_valid_date_format(const char *date_value);
int is_blank_string(const char *text);
void sort_students_by_roll_number(struct StudentRecord students[], int student_count);
void get_data_file_path(char *filepath, const char *type, const char *course, const char *section);
int parse_student_line(const char *line, struct StudentRecord *student);
int load_students_from_file(const char *course, const char *section, struct StudentRecord students[]);
int save_students_to_file(const char *course, const char *section, struct StudentRecord students[], int student_count);
int find_student_index_by_roll(struct StudentRecord students[], int student_count, int roll_number);
int remove_attendance_for_roll(const char *course, const char *section, int roll_number);

/* Screen functions for the CLI version. */
void add_student_screen(void);
void remove_student_screen(void);
void show_all_students_screen(void);

#endif