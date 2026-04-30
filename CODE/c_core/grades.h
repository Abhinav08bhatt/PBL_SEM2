#ifndef GRADES_H
#define GRADES_H

#include "student.h"

/* Structure to hold all grades for a single student */
struct StudentGrades {
    int roll_number;
    int mid_marks[5];
    int end_marks[5];
    float sgpa;
};

/* Grade functions */
int save_student_grades(const char *course, const char *section, int roll, const char *exam_type, int marks[5]);
int get_student_grades(const char *course, const char *section, int roll, int mid_marks[5], int end_marks[5]);
float calculate_sgpa(int mid_marks[5], int end_marks[5]);

/* CLI screen functions (stubbed for GUI mostly) */
void mark_grades_screen(void);

#endif
