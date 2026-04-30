#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "grades.h"

int get_student_grades(const char *course, const char *section, int roll, struct SubjectGrades mid_marks[5], struct SubjectGrades end_marks[5]) {
    char filepath[256];
    FILE *fp;
    char line[500];
    int i;
    
    // Initialize marks to 0
    for(i=0; i<5; i++) {
        mid_marks[i].theory = 0; mid_marks[i].practical = 0; mid_marks[i].internal = 0;
        end_marks[i].theory = 0; end_marks[i].practical = 0; end_marks[i].internal = 0;
    }

    get_data_file_path(filepath, "grades", course, section);
    fp = fopen(filepath, "r");
    if (fp == NULL) return 0; // No grades yet

    while (fgets(line, sizeof(line), fp) != NULL) {
        int r;
        char type[10];
        struct SubjectGrades m[5];
        if (sscanf(line, "%d|%9[^|]|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d", 
                   &r, type, 
                   &m[0].theory, &m[0].practical, &m[0].internal,
                   &m[1].theory, &m[1].practical, &m[1].internal,
                   &m[2].theory, &m[2].practical, &m[2].internal,
                   &m[3].theory, &m[3].practical, &m[3].internal,
                   &m[4].theory, &m[4].practical, &m[4].internal) == 17) {
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

int save_student_grades(const char *course, const char *section, int roll, const char *exam_type, struct SubjectGrades marks[5]) {
    char filepath[256];
    char temp_filepath[256];
    FILE *input;
    FILE *temp;
    char line[500];
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
            struct SubjectGrades m[5];
            if (sscanf(line, "%d|%9[^|]|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d", 
                       &r, type, 
                       &m[0].theory, &m[0].practical, &m[0].internal,
                       &m[1].theory, &m[1].practical, &m[1].internal,
                       &m[2].theory, &m[2].practical, &m[2].internal,
                       &m[3].theory, &m[3].practical, &m[3].internal,
                       &m[4].theory, &m[4].practical, &m[4].internal) == 17) {
                if (r == roll && strcmp(type, exam_type) == 0) {
                    fprintf(temp, "%d|%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d\n", roll, exam_type, 
                            marks[0].theory, marks[0].practical, marks[0].internal,
                            marks[1].theory, marks[1].practical, marks[1].internal,
                            marks[2].theory, marks[2].practical, marks[2].internal,
                            marks[3].theory, marks[3].practical, marks[3].internal,
                            marks[4].theory, marks[4].practical, marks[4].internal);
                    found = 1;
                } else {
                    fprintf(temp, "%d|%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d\n", r, type, 
                            m[0].theory, m[0].practical, m[0].internal,
                            m[1].theory, m[1].practical, m[1].internal,
                            m[2].theory, m[2].practical, m[2].internal,
                            m[3].theory, m[3].practical, m[3].internal,
                            m[4].theory, m[4].practical, m[4].internal);
                }
            }
        }
        fclose(input);
    }

    if (!found) {
        fprintf(temp, "%d|%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d\n", roll, exam_type, 
                marks[0].theory, marks[0].practical, marks[0].internal,
                marks[1].theory, marks[1].practical, marks[1].internal,
                marks[2].theory, marks[2].practical, marks[2].internal,
                marks[3].theory, marks[3].practical, marks[3].internal,
                marks[4].theory, marks[4].practical, marks[4].internal);
    }

    fclose(temp);
    remove(filepath);
    if (rename(temp_filepath, filepath) != 0) return 0;
    return 1;
}

void calculate_student_grade_stats(struct SubjectGrades mid_marks[5], struct SubjectGrades end_marks[5], float *sgpa, int *passed, int *failed, char *overall_grade) {
    int i;
    int total_points = 0;
    int has_mid_and_end = 1;

    *passed = 0;
    *failed = 0;

    for (i = 0; i < 5; i++) {
        int mid_total = mid_marks[i].theory + mid_marks[i].practical + mid_marks[i].internal;
        int end_total = end_marks[i].theory + end_marks[i].practical + end_marks[i].internal;

        if (mid_total == 0 && end_total == 0) {
            has_mid_and_end = 0;
        }

        int scaled_total = (mid_total + end_total) / 2;

        if (scaled_total >= 40) (*passed)++;
        else (*failed)++;

        int points = 0;
        if (scaled_total >= 90) points = 10;
        else if (scaled_total >= 80) points = 9;
        else if (scaled_total >= 70) points = 8;
        else if (scaled_total >= 60) points = 7;
        else if (scaled_total >= 40) points = 6;
        else points = 0;
        
        total_points += points;
    }

    if (!has_mid_and_end) {
        *sgpa = 0.0f;
        *overall_grade = '-';
        return;
    }

    *sgpa = (float)total_points / 5.0f;
    float overall_percent = (*sgpa) * 10.0f;

    if (overall_percent >= 90.0f) *overall_grade = 'A';
    else if (overall_percent >= 80.0f) *overall_grade = 'B';
    else if (overall_percent >= 70.0f) *overall_grade = 'C';
    else if (overall_percent >= 60.0f) *overall_grade = 'D';
    else if (overall_percent >= 40.0f) *overall_grade = 'E';
    else *overall_grade = 'F';
}

void mark_grades_screen(void) {
    print_cli_section_title("MARK GRADES");
    print_cli_status_message("Please use the Python GUI for this feature.");
    wait_for_user_enter();
}
