# c_core Viva Notes (Detailed)

## 1. What this module is

`CODE/c_core` is a terminal-based student attendance system written in C.
It uses plain text files as storage (no database):

- `student_data.txt` stores students as: `roll|name`
- `attendance_data.txt` stores attendance as: `YYYY-MM-DD|roll|P/A`

The executable flow starts from `menu.c`, which includes `student.c` and `attendance.c` directly.

## 2. File-by-file architecture

### `menu.c`

- Program entry point (`main()`)
- Prints menu and reads command
- Routes command to student/attendance screens
- Handles invalid command handling and screen clear loop

### `student.c`

Contains:

- shared constants (`MAX_STUDENTS`, file names)
- `struct StudentRecord`
- reusable helper functions (input handling, formatting, string validation, sorting)
- student operations: add, remove, show
- utility that removes attendance entries for a removed student

### `attendance.c`

Contains:

- attendance line parser and validator
- overwrite-by-date mechanism
- attendance marking flow
- single-student summary and overall report logic

## 3. Data structures and constants

### `struct StudentRecord` (`student.c`)

Fields:

- `int roll_number`:
  - unique identity used everywhere
  - must be `> 0`
- `char name[MAX_NAME_LENGTH]`:
  - student name buffer
  - max stored length controlled by `MAX_NAME_LENGTH` (`100`)

### `struct AttendanceSummary` (`attendance.c`)

Fields:

- `classes_held`: total attendance records found for one roll
- `present_count`: number of `P`
- `absent_count`: number of `A`

### Important constants

- `STUDENT_DATA_FILE = "student_data.txt"`
- `ATTENDANCE_DATA_FILE = "attendance_data.txt"`
- `MAX_STUDENTS = 300`
- `MAX_NAME_LENGTH = 100`

These constants centralize limits and filenames, so changing one value updates behavior globally.

## 4. Execution flow (end-to-end)

1. `main()` clears screen and enters infinite loop.
2. Menu is printed (`print_main_menu_screen`).
3. Input command is read (`read_main_menu_command`).
4. `switch` dispatches:
   - `1` add student
   - `2` remove student
   - `3` show all students
   - `4` mark attendance
   - `5` search report
   - `6` overall attendance
   - `0` exit
5. After each valid command, a “command finished” message and screen clear happen.
6. Invalid command prints error and waits for ENTER before clearing.

## 5. Detailed function explanations

## A) `menu.c`

### `print_main_menu_screen()`

What it does:

- prints static boxed menu text

Logic/conditions:

- no branching
- pure output function

### `read_main_menu_command()`

Variables:

- `int menu_choice = -1`: default sentinel for invalid

Logic:

1. prints prompt
2. calls `scanf("%d", &menu_choice)`
3. if scan fails (`!= 1`):
   - clears input buffer
   - returns `-1`
4. otherwise clears buffer and returns scanned number

Why buffer clear matters:

- prevents leftover `\n` or junk from breaking next input read

### `show_command_finish_message(int command_number)`

What it does:

- prints completion line using passed command id

### `main()`

Core variables:

- `selected_menu_choice`: stores parsed command

Logic:

1. clears terminal once before loop
2. in each loop iteration:
   - print menu
   - read command
   - route by `switch`

`switch` branch behavior:

- `case 1..6`: calls corresponding screen function, prints done message, clears screen
- `case 0`: prints closing box and returns `0`
- `default`: invalid command flow + wait/clear helper

Design note:

- `menu.c` directly includes `.c` files (`#include "student.c"`, `#include "attendance.c"`), which works for small academic projects but is not ideal for larger projects (normally use `.h` declarations + separate compilation).

## B) `student.c`

### `clear_terminal_screen()`

Logic:

- preprocessor condition:
  - on Windows (`_WIN32`) runs `system("cls")`
  - otherwise runs `system("clear")`

Purpose:

- cross-platform screen clear

### `clear_input_buffer()`

Variables:

- `int c`: receives one char at a time from `getchar()`

Logic:

- repeatedly reads until newline (`'\n'`) or EOF

Purpose:

- flushes extra user input so next scan starts clean

### `wait_for_user_enter()`

Logic:

1. prints “Press ENTER…” prompt
2. `fflush(stdout)` to ensure prompt appears immediately
3. calls `clear_input_buffer()` to consume until ENTER

### `read_line_input(char *destination, int size)`

Parameters:

- `destination`: output buffer
- `size`: buffer length to protect from overflow

Logic:

1. calls `fgets(destination, size, stdin)`
2. if `NULL` (input error/EOF), sets empty string and returns
3. removes trailing newline using `strcspn(destination, "\n")`

Key point:

- safe line input compared to `gets` (which is unsafe)

### `print_cli_section_title(const char *title_text)`

- prints boxed title line with formatting width

### `print_cli_status_message(const char *message_text)`

- prints one formatted status line

### `wait_for_user_and_clear_screen()`

- helper composition:
  - wait for enter
  - clear terminal

### `is_valid_date_format(const char *date_value)`

Validation rules:

1. length must be exactly `10`
2. index `4` and `7` must be `'-'`
3. all other positions must be digits (`isdigit`)

Returns:

- `1` valid format, `0` invalid

Important viva point:

- this validates format only, not calendar correctness (example: `2026-99-99` passes format test)

### `is_blank_string(const char *text)`

Logic:

- loops through all chars
- if any non-whitespace char exists, returns `0`
- if all whitespace or empty, returns `1`

Used for:

- rejecting names like `"   "`

### `sort_students_by_roll_number(struct StudentRecord students[], int student_count)`

Algorithm:

- bubble sort ascending by `roll_number`

Variables:

- outer loop `i`, inner loop `j`, and temp record for swap

Complexity:

- time `O(n^2)`, space `O(1)` extra

### `parse_student_line(const char *line, struct StudentRecord *student)`

Input format expected:

- `roll|name`

Logic:

1. `sscanf(line, "%d|%99[^\n]", &roll, name)`
   - reads integer roll
   - reads name until newline
2. if parse fields not exactly 2 -> invalid
3. if `roll <= 0` -> invalid
4. if `name` blank/whitespace -> invalid
5. copies parsed values into output struct

Returns:

- `1` valid line parsed, `0` invalid

### `load_students_from_file(struct StudentRecord students[])`

Variables:

- `FILE *fp`
- `char line[300]` input line buffer
- `int count`

Logic:

1. open student file read mode
2. if open fails, return `0`
3. read each line while file has lines and `count < MAX_STUDENTS`
4. parse line with `parse_student_line`
5. only valid lines are stored and counted
6. close file and return count

Important behavior:

- corrupted lines are silently skipped

### `save_students_to_file(struct StudentRecord students[], int student_count)`

Logic:

1. open student file in write mode (`"w"`) - truncates old file
2. if open fails return `0`
3. write all records as `roll|name`
4. close file and return `1`

### `find_student_index_by_roll(struct StudentRecord students[], int student_count, int roll_number)`

Logic:

- linear search over array
- returns index if found else `-1`

Complexity:

- `O(n)`

### `remove_attendance_for_roll(int roll_number)`

Purpose:

- maintain consistency when student is deleted

Logic:

1. open attendance file for read
2. if file missing, return success (`1`) because nothing to remove
3. open temp file `attendance_tmp.txt` for write
4. for each parseable attendance line:
   - if `roll != target_roll`, write line to temp
   - else skip (deletes that record)
5. close files
6. delete original attendance file
7. rename temp file to original name
8. return success/failure based on rename

Common viva question:

- Why temp file? Because deleting selective lines in-place is hard in text files; rewrite is safer.

### `add_student_screen()`

Variables:

- `students[MAX_STUDENTS]`
- `new_student`
- `count`

Flow with conditions:

1. load existing students
2. if at max capacity, reject
3. input roll via `scanf`
4. if invalid scan or roll <= 0, reject
5. if roll already exists (`find_student_index_by_roll != -1`), reject
6. read name with `read_line_input`
7. if empty/blank name, reject
8. append new student, increment count
9. sort array by roll
10. save full file
11. if save fails, show error
12. else show success

### `remove_student_screen()`

Variables:

- `students[]`, `count`, `roll`, `index`, loop variable `i`

Flow:

1. load students
2. if none, reject
3. read target roll
4. validate roll input
5. find index of student
6. if not found, reject
7. shift array left from `index` to `count-2` (overwrites removed item)
8. decrement `count`
9. save updated student list
10. remove attendance for that roll
11. show success

### `show_all_students_screen()`

Flow:

1. load students
2. sort by roll
3. if count 0, show “No students found”
4. print table header
5. loop print each row

## C) `attendance.c`

### `parse_attendance_record_line(const char *line, char *date, int *roll, char *status)`

Expected format:

- `YYYY-MM-DD|roll|P/A`

Logic:

1. parse 3 tokens with `sscanf`
2. normalize status to uppercase (`toupper`)
3. validate roll > 0
4. validate status is only `P` or `A`
5. validate date format with `is_valid_date_format`
6. return valid/invalid

### `date_already_has_attendance(const char *target_date)`

Purpose:

- detects duplicate date entry before marking attendance

Logic:

1. open attendance file read mode
2. if file missing -> return `0` (no duplicate)
3. parse each valid line
4. if parsed date equals target date (`strcmp == 0`), return `1`
5. else after full scan return `0`

### `rewrite_attendance_file_without_date(const char *target_date)`

Purpose:

- implement overwrite behavior for existing date

Logic:

1. open attendance file input
2. if missing, return success (`1`)
3. open temp output file
4. copy all parsed lines except those with target date
5. replace original with temp via `remove + rename`
6. return success/failure

### `mark_attendance_screen()`

Variables:

- `students[]`, `count`, `date[11]`, `i`

Flow:

1. load students
2. if none, stop
3. sort students
4. clear buffer and read date string
5. validate date format
6. if date already exists:
   - ask `Y/N` overwrite
   - if not `Y`, cancel flow
   - if `Y`, rewrite file excluding old date entries
7. open attendance file append mode
8. for each student:
   - repeatedly ask `P/A` until valid
   - uppercase normalize
   - write line `date|roll|status`
9. close file and success message

Key condition details:

- inner `while(1)` ensures only valid attendance values are accepted
- overwrite is explicit user-controlled; no accidental overwrite

### `get_student_attendance_summary(int roll_number)`

Variables:

- local `result` struct initialized to zeros
- parsed fields per line: `d`, `r`, `s`

Flow:

1. initialize counters all `0`
2. open attendance file; if unavailable return zeros
3. parse each valid attendance line
4. if line roll matches input roll:
   - `classes_held++`
   - if `P`, `present_count++` else `absent_count++`
5. return result struct

### `search_student_report_screen()`

Variables:

- `roll`, `index`, `AttendanceSummary a`, `double percent`

Flow:

1. load students; if none stop
2. read and validate roll
3. locate student in loaded list
4. if not found stop
5. compute summary using `get_student_attendance_summary`
6. compute percentage if `classes_held > 0`
7. print detailed box with roll, name, counts, percent

Formula:

- `percent = (present_count * 100.0) / classes_held`

### `show_overall_attendance_screen()`

Flow:

1. load students; if none stop
2. sort by roll
3. print table header
4. for each student:
   - call `get_student_attendance_summary`
   - compute percentage (guard divide-by-zero)
   - print row

Complexity note:

- current design re-scans entire attendance file for each student, so if `S` students and `A` attendance lines, time is roughly `O(S * A)`.

## 6. Data format notes from current dataset

From files in this folder:

- students currently appear as strict `roll|name`
- attendance currently appears as strict `YYYY-MM-DD|roll|P/A`
- at least one attendance entry exists for date `2026-04-12` with roll `3`

Possible viva discussion point:

- attendance file may contain rolls not in current student file if data was edited externally; reporting functions still count by roll if queried.

## 7. Input validation summary (important viva topic)

Validated:

- roll must parse as integer and be `> 0`
- name cannot be empty or whitespace only
- attendance status only `P` or `A` (case-insensitive input, stored uppercase)
- date must match `YYYY-MM-DD` pattern

Not fully validated:

- calendar validity of date (month/day ranges not checked)
- duplicate attendance entries for same roll+date are not separately checked if file manually edited
- uniqueness in student file depends on add flow; manual file edits can break assumptions

## 8. Why specific helper functions exist

- `clear_input_buffer`: fixes common `scanf` + line-input issues
- `read_line_input`: safely read names with spaces
- parser functions (`parse_student_line`, `parse_attendance_record_line`): centralize validation logic, avoid repeated parsing bugs
- rewrite functions using temp file: safe filtered delete/update on text files

## 9. Common viva questions and good answers

1. Why bubble sort?
- Simpler to implement/read for small arrays. With `MAX_STUDENTS = 300`, performance is acceptable for this academic project.

2. Why use text files instead of database?
- Project scope is CLI fundamentals and file handling; text files are transparent and easy to inspect manually.

3. How is duplicate student roll prevented?
- Before insertion, `find_student_index_by_roll` checks current list; insertion is rejected if roll exists.

4. How is duplicate attendance date handled?
- `date_already_has_attendance` checks existence. User gets overwrite choice. If overwrite is `Y`, old date records are removed by rewrite.

5. How do you avoid input skipping bugs?
- Every `scanf` path calls `clear_input_buffer`; line input uses `fgets` wrapper.

6. What happens if files do not exist?
- Student load returns `0`, attendance queries return defaults, and operations handle empty state gracefully.

7. Biggest limitation in current design?
- `show_overall_attendance_screen` repeatedly scans attendance file for each student (`O(S*A)`). Also no strict calendar-date validation.

## 10. Improvement points (if teacher asks “what next?”)

- split `.c` includes into header+source (`.h` + separate compilation)
- add strict date validity check (month/day/leap year)
- compute overall attendance in one pass using map/array to reduce repeated scans
- add atomic file replace checks (handle `remove` failure explicitly)
- add unit tests for parser and validation functions

## 11. Quick cheat sheet

- Add student path: load -> validate roll -> unique check -> validate name -> sort -> save
- Remove student path: load -> validate roll -> find -> shift -> save -> cleanup attendance
- Mark attendance path: load students -> validate date -> optional overwrite -> collect P/A per student -> append save
- Report path (single): find student -> scan attendance for that roll -> compute percent
- Report path (overall): for each student -> scan attendance -> print row

