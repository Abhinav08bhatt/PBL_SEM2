# c_core Guide (Human-Friendly Version)

This guide explains the current C code in `CODE/c_core` in plain language.
It is written for your team so everyone clearly understands:

- what each file does
- what each function does
- who worked on what
- what to prepare for the future Python frontend

---

## Project Purpose

This is a basic CLI (terminal) student attendance system.

It does 3 main things:

1. Stores student data (`roll|name`)
2. Stores attendance data (`date|roll|P/A`)
3. Shows reports from that data

There is no database. Everything is file-based using `.txt`.

---

## Files and Responsibilities

### `menu.c`
Main entry and CLI navigation.

- prints menu
- takes command `0-6`
- calls the correct screen function
- handles clear-screen flow between commands

### `student.c`
Student operations + shared helper utilities.

- add student
- remove student
- show all students
- helper functions for input handling, sorting, formatting, and date checks

### `attendance.c`
Attendance operations and reports.

- mark attendance for one date
- search one student report
- show overall attendance table
- attendance file rewrite logic for overwrite/remove cases

### `student_data.txt`
Student storage file.  
Each line format:

`roll_number|student_name`

### `attendance_data.txt`
Attendance storage file.  
Each line format:

`YYYY-MM-DD|roll_number|P_or_A`

---

## Program Flow (Simple)

1. `main()` starts in `menu.c`
2. menu is printed
3. user enters command
4. selected function runs
5. result is shown
6. user presses Enter
7. screen clears and menu comes back
8. command `0` exits

This is a loop until user exits.

---

## Function-by-Function Explanation

## `menu.c`

### `print_main_menu_screen()`
Prints the boxed CLI menu with options `0` to `6`.

### `read_main_menu_command()`
Reads menu command from input.
- returns command number
- returns `-1` if invalid input
- clears leftover input buffer

### `show_command_finish_message(int command_number)`
Prints a small "Command X finished" message after a command ends.

### `main()`
Main loop and command router.

- `1` -> `add_student_screen()`
- `2` -> `remove_student_screen()`
- `3` -> `show_all_students_screen()`
- `4` -> `mark_attendance_screen()`
- `5` -> `search_student_report_screen()`
- `6` -> `show_overall_attendance_screen()`
- `0` -> exit

Also calls screen clear between command cycles.

---

## `student.c`

### `clear_terminal_screen()`
Clears terminal using:
- `cls` on Windows
- `clear` on Linux/macOS

### `clear_input_buffer()`
Consumes leftover characters until newline/EOF.
Important for avoiding skipped input.

### `wait_for_user_enter()`
Shows "Press ENTER to continue..." and waits properly.

### `read_line_input(char *destination, int size)`
Reads full text line safely with `fgets` (used for names).

### `print_cli_section_title(const char *title_text)`
Prints a boxed section header.

### `print_cli_status_message(const char *message_text)`
Prints one status line in CLI box style.

### `wait_for_user_and_clear_screen()`
Waits for Enter, then clears screen.
(helper exists, currently not central in menu flow)

### `is_valid_date_format(const char *date_value)`
Checks date in `YYYY-MM-DD` format.
This is format-level validation (simple and basic).

### `is_blank_string(const char *text)`
Returns true if string is empty/whitespace only.

### `sort_students_by_roll_number(struct StudentRecord students[], int student_count)`
Bubble sorts student array by roll number.

### `parse_student_line(const char *line, struct StudentRecord *student)`
Reads one line from student file and converts it to struct.

### `load_students_from_file(struct StudentRecord students[])`
Loads all students from `student_data.txt` into array.
Returns count.

### `save_students_to_file(struct StudentRecord students[], int student_count)`
Writes all students back to `student_data.txt`.
Returns success/failure.

### `find_student_index_by_roll(struct StudentRecord students[], int student_count, int roll_number)`
Linear search by roll number.
Returns index or `-1`.

### `remove_attendance_for_roll(int roll_number)`
Rewrites attendance file excluding records of one roll number.
Used when deleting a student.

### `show_all_students_screen()`
Displays sorted students in a formatted table.

### `add_student_screen()`
Input + validation + save for adding a student.

### `remove_student_screen()`
Input + lookup + delete + save.
Also removes matching attendance records.

---

## `attendance.c`

### `parse_attendance_record_line(const char *line, char *date, int *roll, char *status)`
Parses one attendance line and validates values.

### `date_already_has_attendance(const char *target_date)`
Checks whether attendance exists for a date.

### `rewrite_attendance_file_without_date(const char *target_date)`
Removes all records of one date from attendance file.
Used when user chooses overwrite.

### `mark_attendance_screen()`
Main attendance entry flow:
- load students
- enter date
- optionally overwrite same-date data
- enter `P/A` for each student
- append to file

### `get_student_attendance_summary(int roll_number)`
Calculates:
- classes held
- present count
- absent count

for one student.

### `search_student_report_screen()`
Finds student by roll and prints personal attendance summary.

### `show_overall_attendance_screen()`
Shows attendance summary for every student in table form.

---

## Team Contribution Guide

## 1) Abhinav (Attendance + Menu)

You handle:
- `attendance.c`
- `menu.c`

Focus areas to understand deeply:
- command flow in `main()`
- safe input flow in command handling
- attendance overwrite logic
- how reports are calculated from raw attendance lines

When changing code, always check:
- command-to-function mapping still correct
- every attendance line written in correct format
- overwrite/delete logic does not lose unrelated data

---

## 2) Sagar (Student Module)

You handle:
- `student.c`

Focus areas to understand deeply:
- student load/save pipeline
- roll-number uniqueness rules
- remove-student flow and attendance cleanup
- input handling helpers (`clear_input_buffer`, `read_line_input`, wait flow)

When changing code, always check:
- no duplicate roll numbers
- no broken format in `student_data.txt`
- remove flow updates both student + attendance consistency

---

## 3) Bannard (Dummy Data + Test Data Quality)

You handle:
- `student_data.txt`
- `attendance_data.txt`

Focus areas to understand deeply:
- file format must always be exact
- roll numbers in attendance must exist in student data
- status only `P` or `A`
- date format must be `YYYY-MM-DD`

When preparing new datasets:
- keep realistic names
- avoid duplicate roll numbers
- ensure attendance lines are balanced and believable

---

## Data Format Rules (Very Important)

### Student file
Correct:
`12|Aarav Sharma`

Wrong:
- `12, Aarav`
- `roll=12|name=Aarav`

### Attendance file
Correct:
`2026-04-05|12|P`

Wrong:
- `05-04-2026|12|P`
- `2026-04-05|12|Present`

---

## Future Plan: Python Frontend Integration (PyQt/Tkinter)

You said this project will later use a Python frontend.  
That is a good plan. Here is the practical migration path.

### Current state
- UI + logic are mixed in C CLI functions.
- storage is `.txt`.

### Future target
- Python handles UI (PyQt or Tkinter).
- C handles core logic as reusable backend module.
- C exports clean APIs through `.h` files.
- storage may move from `.txt` to `.dat`.
- launch from `main.c` (or packaged `main.exe` on Windows).

### Recommended transition steps

1. **Split declarations into `.h`**
   - `student.h`
   - `attendance.h`
   - `menu.h` (optional if menu remains)

2. **Separate UI and logic**
   - keep pure data functions in C
   - remove direct `printf/scanf` from core APIs

3. **Create function-style APIs**
   Example style:
   - `int add_student(int roll, const char *name);`
   - `int remove_student(int roll);`
   - `int mark_attendance(const char *date, int roll, char status);`
   - `int get_student_summary(int roll, struct AttendanceSummary *out);`

4. **Move file format to `.dat` (if needed)**
   - can still be text-based `.dat` first (simple)
   - later switch to binary if needed

5. **Connect Python to C**
   - easiest: subprocess calls first
   - better: shared library + `ctypes`/`cffi`

6. **Keep `main.c` as test harness**
   - use CLI `main.c` for backend testing
   - Python frontend becomes real app entry

### Why this helps
- Python UI iteration becomes fast
- C logic stays reusable and testable
- team can work in parallel:
  - UI in Python
  - backend in C

---

## Final Notes

This codebase is intentionally simple and readable.
That is good for a student team project.

If you keep data format strict and keep functions small/clear, this will remain stable and easy to move to a Python frontend later.
