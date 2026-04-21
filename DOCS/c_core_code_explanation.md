# c_core Viva Notes (Detailed)

## 1. What this module is

This project is a student attendance system built mainly in C, with a Python GUI on top.

Main parts:

- `main.c`:
  - root launcher file
  - starts the Python GUI app
- `CODE/c_core`:
  - original C attendance logic
  - works in terminal/CLI style
  - stores data in plain text files
- `CODE/python_gui`:
  - `tkinter` GUI
  - uses a small C backend
  - reuses the same C core functions

Data storage is still simple text files:

- `student_data.txt` stores students as: `roll|name`
- `attendance_data.txt` stores attendance as: `YYYY-MM-DD|roll|P/A`

Important point:

- the GUI and the C core both use the same files inside `CODE/c_core`
- this means there is one shared source of data

## 2. File-by-file architecture

### Root level

### `main.c`

- very small launcher file
- runs `python3 CODE/python_gui/app.py`
- makes the project start from one main entry point

### `Makefile`

- convenience file for building the launcher binary
- `make app` builds the root launcher
- `make run` builds and runs it

Note:

- `Makefile` is useful, but not required
- the same build can be done manually with `gcc`

## 3. C core folder architecture

### `CODE/c_core/menu.c`

- CLI program entry point for the old terminal version
- prints the menu
- reads user command
- calls the needed student or attendance screen function

### `CODE/c_core/student.h`

Contains declarations for:

- constants like `MAX_STUDENTS`
- file name constants
- `struct StudentRecord`
- helper functions
- student-related screen functions

Purpose:

- allows other files to use student functions without including `student.c` directly

### `CODE/c_core/attendance.h`

Contains declarations for:

- `struct AttendanceSummary`
- attendance helper functions
- report functions
- attendance screen functions

Purpose:

- allows proper sharing of attendance functions between files

### `CODE/c_core/student.c`

Contains:

- input helper functions
- formatting helper functions
- blank-string check
- date-format helper used by attendance logic
- student file load/save logic
- student add/remove/show screens
- attendance cleanup when a student is removed

### `CODE/c_core/attendance.c`

Contains:

- attendance line parsing
- date overwrite logic
- mark attendance screen
- student attendance summary logic
- single-student report
- full overall report

### `CODE/c_core/Makefile`

- convenience file for compiling the CLI C app
- builds `menu_app`

## 4. Python GUI folder architecture

### `CODE/python_gui/app.py`

- the main GUI program
- built with `tkinter`
- shows three tabs:
  - Students
  - Attendance
  - Reports

It does not directly edit the text files itself.

Instead:

- it calls a small C backend program
- that backend uses the same core logic as the C version

### `CODE/python_gui/backend_api.c`

- command-based C backend for the GUI
- links with `student.c` and `attendance.c`
- accepts commands like:
  - `list_students`
  - `add_student`
  - `remove_student`
  - `mark_attendance`
  - `get_report`
  - `get_overall`

Why this file exists:

- Python GUI cannot directly call C functions in this project structure
- so this backend acts like a bridge between Python and C

### `CODE/python_gui/Makefile`

- convenience file for building the backend binary
- builds `backend_app`

## 5. Data structures and constants

### `struct StudentRecord` (`student.h`)

Fields:

- `int roll_number`
  - unique id for each student
  - must be greater than `0`
- `char name[MAX_NAME_LENGTH]`
  - stores the student name
  - maximum length is controlled by `MAX_NAME_LENGTH`

### `struct AttendanceSummary` (`attendance.h`)

Fields:

- `classes_held`
  - total attendance entries found for one student
- `present_count`
  - total `P`
- `absent_count`
  - total `A`

### Important constants (`student.h`)

- `STUDENT_DATA_FILE = "student_data.txt"`
- `ATTENDANCE_DATA_FILE = "attendance_data.txt"`
- `MAX_STUDENTS = 300`
- `MAX_NAME_LENGTH = 100`

These values are shared through the header, so all linked files use the same limits.

## 6. Execution flow (project level)

### A) Normal app flow

1. user runs the root program
2. root `main.c` starts the Python GUI
3. Python GUI opens `CODE/python_gui/app.py`
4. GUI builds backend if needed
5. GUI runs backend commands when user clicks buttons
6. backend command uses C core functions
7. C core reads/writes the shared text files in `CODE/c_core`
8. GUI refreshes tables and reports

### B) Old CLI flow

1. user runs `CODE/c_core/menu_app`
2. `menu.c` prints menu
3. command is read
4. `switch` calls student or attendance screens
5. data is read/written to the same text files

Important comparison:

- CLI and GUI are two different frontends
- both use the same storage files

## 7. Detailed function explanations

## A) `main.c`

### `main()`

What it does:

- starts the GUI app using `system()`

Logic:

1. prepares a command to run Python GUI
2. calls `system("python3 CODE/python_gui/app.py")`
3. if launch fails, prints a simple error

Why this is useful:

- project can be started from one file
- easier for demo and submission

## B) `student.c`

### `clear_terminal_screen()`

Logic:

- on Windows uses `cls`
- otherwise uses `clear`

Purpose:

- clears terminal in the CLI version

### `clear_input_buffer()`

Logic:

- keeps reading chars until newline or EOF

Purpose:

- prevents bad leftover input from affecting next scan

### `wait_for_user_enter()`

Logic:

1. prints ENTER prompt
2. flushes output
3. waits for remaining input to finish

### `read_line_input(char *destination, int size)`

Logic:

1. uses `fgets`
2. if input fails, stores empty string
3. removes ending newline

Important point:

- safer than old unsafe input methods

### `print_cli_section_title(const char *title_text)`

- prints a formatted heading box

### `print_cli_status_message(const char *message_text)`

- prints one formatted status line

### `wait_for_user_and_clear_screen()`

- combines wait and clear into one helper

### `is_valid_date_format(const char *date_value)`

Checks:

1. length must be `10`
2. index `4` and `7` must be `-`
3. all other characters must be digits

Returns:

- `1` if format is valid
- `0` otherwise

Viva point:

- this checks format only
- it does not check if the calendar date is real

### `is_blank_string(const char *text)`

Logic:

- returns `1` if text is empty or all spaces
- returns `0` if any visible character exists

Used for:

- rejecting blank student names

### `sort_students_by_roll_number(struct StudentRecord students[], int student_count)`

Algorithm:

- bubble sort

Why bubble sort is okay here:

- data size is small
- easier for students to explain in viva

### `parse_student_line(const char *line, struct StudentRecord *student)`

Reads one line in format:

- `roll|name`

Checks:

- line format is correct
- roll number is positive
- name is not blank

### `load_students_from_file(struct StudentRecord students[])`

Logic:

1. opens `student_data.txt`
2. reads line by line
3. parses valid student lines
4. stores them in array
5. returns total count

### `save_students_to_file(struct StudentRecord students[], int student_count)`

Logic:

1. opens file in write mode
2. writes all students back in `roll|name` format
3. returns success/failure

### `find_student_index_by_roll(...)`

Logic:

- simple linear search through student array

Returns:

- index if found
- `-1` if not found

### `remove_attendance_for_roll(int roll_number)`

Logic:

1. open attendance file
2. create temp file
3. copy only records that do not belong to removed student
4. replace old file with temp file

Purpose:

- keeps attendance data clean after deletion

### `add_student_screen()`

Logic:

1. load current students
2. check limit
3. read roll number
4. reject invalid or duplicate roll
5. read name
6. reject blank name
7. add student
8. sort by roll
9. save file

### `remove_student_screen()`

Logic:

1. load students
2. read roll
3. find matching student
4. shift array left to remove that student
5. save updated student list
6. remove related attendance records

### `show_all_students_screen()`

Logic:

1. load students
2. sort them
3. print them in a table

## C) `attendance.c`

### `parse_attendance_record_line(const char *line, char *date, int *roll, char *status)`

Reads one line in format:

- `YYYY-MM-DD|roll|P/A`

Checks:

- date format
- positive roll number
- status is `P` or `A`

### `date_already_has_attendance(const char *target_date)`

Logic:

- scans attendance file
- if any record has the same date, returns `1`

Purpose:

- helps decide whether attendance for that day should be overwritten

### `rewrite_attendance_file_without_date(const char *target_date)`

Logic:

1. open attendance file
2. create temp file
3. copy all records except the chosen date
4. replace old file

Purpose:

- supports overwrite attendance by date

### `mark_attendance_screen()`

Logic:

1. load students
2. ask for date
3. validate date format
4. if date already exists, ask for overwrite
5. open attendance file
6. ask `P/A` for each student
7. save all records

### `get_student_attendance_summary(int roll_number)`

Logic:

1. scan attendance file
2. count only lines of one roll number
3. update present/absent totals

Returns:

- one `AttendanceSummary`

### `search_student_report_screen()`

Logic:

1. read roll
2. find student
3. get summary
4. calculate percentage
5. print one student report

### `show_overall_attendance_screen()`

Logic:

1. load all students
2. sort them
3. for each student, get attendance summary
4. calculate percentage
5. print table

## D) `backend_api.c`

This file is the bridge between Python and C.

### `command_list_students()`

- loads students
- sorts them
- prints each row as `roll|name`

### `command_add_student(...)`

- validates roll number
- joins name words together
- checks duplicate roll
- saves new student

### `command_remove_student(...)`

- validates roll
- removes student from array
- saves file
- removes old attendance for that student

### `read_one_status_token(...)`

Input format:

- `roll:P`
- `roll:A`

Purpose:

- splits one token into roll number and attendance status

### `command_mark_attendance(...)`

Logic:

1. validate date
2. load students
3. read all passed status tokens
4. match each token to a student
5. ensure all students got one status
6. overwrite same date if needed
7. append attendance records

### `command_get_report(...)`

- validates roll
- loads student
- gets summary
- prints one line for GUI use

Output format:

- `roll|name|classes|present|absent|percent`

### `command_get_overall()`

- loops through all students
- gets summary for each one
- prints one line per student

## E) `app.py`

### `build_backend_if_needed()`

Purpose:

- compiles `backend_api.c` if backend binary is missing

### `run_backend_command(*command_arguments)`

Purpose:

- runs backend program from Python
- returns output text
- raises readable error if backend fails

Important point:

- backend runs with `CODE/c_core` as current working directory
- so it uses the same shared data files

### `AttendanceApp`

This is the main GUI window.

Main tabs:

- Students
- Attendance
- Reports

### `create_students_tab()`

Creates:

- roll input
- name input
- add/remove/refresh buttons
- students table

### `create_attendance_tab()`

Creates:

- date input
- load students button
- submit attendance button
- scrollable attendance form

### `create_reports_tab()`

Creates:

- roll input for report search
- report refresh buttons
- single-student report text area
- overall attendance table

### `refresh_students_table()`

Logic:

1. get student list from backend
2. save rows in memory
3. clear old table rows
4. insert new rows
5. rebuild attendance form

### `add_student()`

Logic:

1. read roll and name from entries
2. validate not blank
3. call backend `add_student`
4. clear input fields
5. refresh student and report tables

### `remove_student()`

Logic:

1. read roll number
2. validate input
3. call backend `remove_student`
4. refresh tables

### `build_attendance_form()`

Logic:

1. clear old widgets
2. if no students, show message
3. create roll, name, status columns
4. create one combobox per student

### `submit_attendance()`

Logic:

1. read date
2. ensure students exist
3. collect one `roll:P/A` token for each student
4. call backend `mark_attendance`
5. refresh overall report

### `get_student_report()`

Logic:

1. read roll
2. call backend `get_report`
3. split returned line
4. show readable text in GUI

### `refresh_overall_report_table()`

Logic:

1. call backend `get_overall`
2. clear old rows
3. insert updated rows

## 8. Why headers were added

Earlier version idea:

- one `.c` file directly included another `.c` file

Current version:

- declarations are in `.h` files
- implementations stay in `.c` files
- files are compiled and linked normally

Why this is better:

- cleaner project structure
- easier reuse in GUI backend
- easier viva explanation
- closer to standard C programming practice

## 9. Why this design works well for the project

Advantages:

- simple text-file storage
- easy to understand for first-year project
- C logic is reused instead of rewritten
- GUI and CLI can both work with the same data
- header files make reuse cleaner

Tradeoff:

- text files are simple but not as powerful as a database
- some logic uses basic algorithms like bubble sort and linear search
- backend communication is simple command text, not a full API

For a student project, this is still a good design because it is easy to show, explain, and test.
