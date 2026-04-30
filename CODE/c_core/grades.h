#ifndef GRADES_H
#define GRADES_H

#include "student.h"

/* Structure to hold components for a single subject */
struct SubjectGrades {
    int theory;     /* Max 50 */
    int practical;  /* Max 25 */
    int internal;   /* Max 25 */
};

/* Structure to hold all grades for a single student */
struct StudentGrades {
    int roll_number;
    struct SubjectGrades mid_marks[5];
    struct SubjectGrades end_marks[5];
    float sgpa;
    int passed_subjects;
    int failed_subjects;
    char overall_grade;
};

/* Grade functions */
int save_student_grades(const char *course, const char *section, int roll, const char *exam_type, struct SubjectGrades marks[5]);
int get_student_grades(const char *course, const char *section, int roll, struct SubjectGrades mid_marks[5], struct SubjectGrades end_marks[5]);
void calculate_student_grade_stats(struct SubjectGrades mid_marks[5], struct SubjectGrades end_marks[5], float *sgpa, int *passed, int *failed, char *overall_grade);

/* CLI screen functions (stubbed for GUI mostly) */
void mark_grades_screen(void);

#endif
