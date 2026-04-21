#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include "student.h"

/* Used to store attendance totals for one student. */
struct AttendanceSummary {
    int classes_held;
    int present_count;
    int absent_count;
};

/* Attendance functions and report functions. */
int parse_attendance_record_line(const char *line, char *date, int *roll, char *status);
int date_already_has_attendance(const char *target_date);
int rewrite_attendance_file_without_date(const char *target_date);
void mark_attendance_screen(void);
struct AttendanceSummary get_student_attendance_summary(int roll_number);
void search_student_report_screen(void);
void show_overall_attendance_screen(void);

#endif
