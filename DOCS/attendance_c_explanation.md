# 📘 Deep Dive: `attendance.c` Explanation

This guide explains the inner workings of `attendance.c`. While `student.c` manages *who* is in the class, `attendance.c` manages *when* they showed up. It handles parsing dates, marking P/A, overwriting days, and generating percentages.

---

## 1. How We Store Multiple Days of Attendance

Unlike a traditional database that uses complex tables, we track multiple days using a very simple **"Append"** strategy in a text file (`data/BSc_CS_A_attendance.txt`).

**The Format:**
Every single row records one student's status on one specific day:
`YYYY-MM-DD|Roll_Number|Status`

**Example:**
```text
2026-04-01|1|P
2026-04-01|2|A
2026-04-02|1|P
2026-04-02|2|P
```
**How it works:** The file just grows longer over time. Because every row has a date stamped on it, the C code can easily filter through the file to find exactly what it needs.

---

## 2. Reading the File: `parse_attendance_record_line()`

Because our data has three parts separated by pipes (`|`), we need a precise way to extract them.

**The Code Logic:**
```c
sscanf(line, "%10[^|]|%d|%c", date, &roll, &status)
```
- **`%10[^|]`**: This is a clever C trick. It means "read exactly 10 characters, or until you hit a `|` symbol". This grabs our date `2026-04-01` securely.
- **`%d`**: Grabs the integer Roll Number.
- **`%c`**: Grabs the single character Status ('P' or 'A').
- The function then forces the status character to be uppercase using `toupper()` just in case the user accidentally typed a lowercase 'p'.

---

## 3. Saving New Attendance: `mark_attendance_screen()`

When the teacher marks attendance at the end of the day, we need to save it.

**The Code Logic:**
1. The function asks the user for the Date.
2. It calls `date_already_has_attendance()`. This helper function scans the attendance text file line by line. If it sees the inputted date anywhere in the file, it warns the user: *"Attendance already exists. Overwrite?"*
3. If the user is proceeding, we open the text file using `fopen(filepath, "a")`.
   - *Crucial Detail:* The `"a"` stands for **Append Mode**. Unlike `"w"` (which deletes the file), `"a"` safely adds new text to the very bottom of the file without touching the old data.
4. The program loops through the 20 students in the RAM array, asks the user for `P` or `A`, and immediately `fprintf()`s that line into the file.

---

## 4. The Overwrite Trick: `rewrite_attendance_file_without_date()`

What happens if the teacher makes a mistake and needs to re-do yesterday's attendance? We can't just delete lines in C!

We use the **Temp File Copying Mechanism** (just like in student deletion):
1. We create a blank `data/attendance_tmp.txt` file.
2. We read the original attendance file line by line.
3. We look at the date on the line. If the date **does not match** the date the teacher is trying to overwrite, we copy it to the temp file.
4. If the date **does match**, we skip it! (Essentially throwing it away).
5. Once finished, we delete the original file and rename the temp file to take its place.
6. Now the file is totally clean of that specific date, and the program can cleanly `Append` the new corrected attendance at the bottom!

---

## 5. Calculating Percentages: `get_student_attendance_summary()`

To generate a report card for a specific student, we need to crunch the numbers.

**The Code Logic:**
1. We initialize a `struct AttendanceSummary` report card in RAM. We set `classes_held = 0`, `present_count = 0`, and `absent_count = 0`.
2. We open the section's attendance file in Read mode (`"r"`).
3. We scan the file line by line using a `while` loop.
4. We extract the Roll Number from the line. 
5. **The Filter:** If the Roll Number on the line matches the student we are searching for, we proceed. (If it doesn't match, we ignore it).
6. We add `+1` to `classes_held`.
7. We look at the Status. If it's `'P'`, we add `+1` to `present_count`. If it's `'A'`, we add `+1` to `absent_count`.
8. Once the end of the file is reached, we return the finished report card struct.

**Displaying it:** 
In `search_student_report_screen()`, we take that report card and run the math:
```c
percent = ((double)a.present_count * 100.0) / (double)a.classes_held;
```
*(Note: We cast the integers to `double` so C performs decimal division correctly instead of rounding down to zero!)*
