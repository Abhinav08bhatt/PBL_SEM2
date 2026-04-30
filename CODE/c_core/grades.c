#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "grades.h"

int get_student_grades(const char *course, const char *section, int roll, int mid_marks[5], int end_marks[5]) {
    char filepath[256];
    FILE *fp;
    char line[300];
    int i;
    
    // Initialize marks to 0
    for(i=0; i<5; i++) {
        mid_marks[i] = 0;
        end_marks[i] = 0;
    }

    get_data_file_path(filepath, "grades", course, section);
    fp = fopen(filepath, "r");
    if (fp == NULL) return 0; // No grades yet

    while (fgets(line, sizeof(line), fp) != NULL) {
        int r;
        char type[10];
        int m[5];
        if (sscanf(line, "%d|%9[^|]|%d|%d|%d|%d|%d", &r, type, &m[0], &m[1], &m[2], &m[3], &m[4]) == 7) {
            if (r == roll) {
                if (strcmp(type, "MID") == 0) {
                    for(i=0; i<5; i++) mid_marks[i] = m[i];
                } else if (strcmp(type, "END") == 0) {
                    for(i=0; i<5; i++) end_marks[i] = m[i];
                }
            }
        }
    }
    fclose(fp);
    return 1;
}

int save_student_grades(const char *course, const char *section, int roll, const char *exam_type, int marks[5]) {
    char filepath[256];
    char temp_filepath[256];
    FILE *input;
    FILE *temp;
    char line[300];
    int found = 0;

    get_data_file_path(filepath, "grades", course, section);
    sprintf(temp_filepath, "data/grades_tmp.txt");

    input = fopen(filepath, "r");
    temp = fopen(temp_filepath, "w");

    if (temp == NULL) {
        if (input != NULL) fclose(input);
        return 0;
    }

    if (input != NULL) {
        while (fgets(line, sizeof(line), input) != NULL) {
            int r;
            char type[10];
            int m[5];
            if (sscanf(line, "%d|%9[^|]|%d|%d|%d|%d|%d", &r, type, &m[0], &m[1], &m[2], &m[3], &m[4]) == 7) {
                if (r == roll && strcmp(type, exam_type) == 0) {
                    fprintf(temp, "%d|%s|%d|%d|%d|%d|%d\n", roll, exam_type, marks[0], marks[1], marks[2], marks[3], marks[4]);
                    found = 1;
                } else {
                    fprintf(temp, "%d|%s|%d|%d|%d|%d|%d\n", r, type, m[0], m[1], m[2], m[3], m[4]);
                }
            }
        }
        fclose(input);
    }

    if (!found) {
        fprintf(temp, "%d|%s|%d|%d|%d|%d|%d\n", roll, exam_type, marks[0], marks[1], marks[2], marks[3], marks[4]);
    }

    fclose(temp);
    remove(filepath);
    if (rename(temp_filepath, filepath) != 0) return 0;
    return 1;
}

float calculate_sgpa(int mid_marks[5], int end_marks[5]) {
    int i;
    int total_points = 0;
    for (i = 0; i < 5; i++) {
        int total = mid_marks[i] + end_marks[i]; // max 100
        int points = 0;
        if (total >= 90) points = 10;
        else if (total >= 80) points = 9;
        else if (total >= 70) points = 8;
        else if (total >= 60) points = 7;
        else if (total >= 50) points = 6;
        else if (total >= 40) points = 5;
        else points = 0;
        
        total_points += points;
    }
    return (float)total_points / 5.0f;
}

void mark_grades_screen(void) {
    print_cli_section_title("MARK GRADES");
    print_cli_status_message("Please use the Python GUI for this feature.");
    wait_for_user_enter();
}
