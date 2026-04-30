# 📘 Deep Dive: `grades.h` & `grades.c` Explanation

This guide explains the most complex mathematical and string-parsing part of our C Backend: the **Grades and SGPA Engine**. It handles parsing huge rows of numbers, mapping grades to a 10-point scale, calculating the final SGPA, and evaluating pass/fail status. 

---

## 1. The Blueprint: `grades.h`

The header file defines the structures required to handle a multi-component grading system.

### `struct SubjectGrades`
Instead of just tracking one final number for a subject, we track three distinct components per subject. We created a struct specifically for this:
```c
struct SubjectGrades {
    int theory;     /* Max 50 */
    int practical;  /* Max 25 */
    int internal;   /* Max 25 */
};
```
*Why this matters:* This means a single "Subject" is actually a cluster of three integers. This allows the GUI to display highly detailed report cards rather than just a total.

### `struct StudentGrades`
This is the master struct. It acts as the ultimate report card for the student. It holds:
- Their `roll_number`.
- An **array of 5** `SubjectGrades` for Mid-Sem (`mid_marks[5]`).
- An **array of 5** `SubjectGrades` for End-Sem (`end_marks[5]`).
- The dynamically calculated statistics: `sgpa`, `passed_subjects`, `failed_subjects`, and `overall_grade`.

**Viva Answer Tip:** If the teacher asks *"Why do you have structs inside of structs?"*, you can confidently reply: *"It reflects the real-world hierarchy of data. A Student has multiple Exams (Mid/End). An Exam consists of multiple Subjects. And each Subject consists of multiple Marks (Theory, Practical, Internal). Struct embedding makes our code incredibly organized and highly scalable!"*

---

## 2. Flat-File Storage Strategy: 17-Element Strings

We store the grades in isolated files like `data/BSc_CS_A_grades.txt`. Because our grades are highly detailed, each row in the text file is massively long.

**The Format:**
`Roll_Number|Exam_Type(MID/END)|T1|P1|I1|T2|P2|I2|T3|P3|I3|T4|P4|I4|T5|P5|I5`

*(T = Theory, P = Practical, I = Internal. There are 5 subjects, meaning 5 * 3 = 15 marks per line!)*

**The Parsing Trick:**
To read this giant string back into the C structures safely, we use a giant `sscanf` block:
```c
sscanf(line, "%d|%9[^|]|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d", 
       &roll, type, 
       &m[0].theory, &m[0].practical, &m[0].internal,
       ... // (and so on for all 15 elements)
```
Notice `%9[^|]`. This is a regular expression trick in C. It tells `sscanf` to read a string of text up to 9 characters long, **but stop immediately if it sees a pipe `|` character**. This safely extracts the `"MID"` or `"END"` string without accidentally grabbing the numbers next to it!

---

## 3. Saving Grades: The `save_student_grades()` Function

Saving grades uses the exact same **Temp File Overwrite Trick** we used for deleting students and overwriting attendance.

1. It creates a blank `data/grades_tmp.txt` file.
2. It loops through the existing grades file. 
3. If it sees a row that matches the `roll_number` AND the `exam_type` we are saving, it **replaces** that row by printing the new 15-mark array into the temp file.
4. If it's a different roll number or different exam type, it just copies the old row as-is.
5. If the file ends and it *never* found the roll number, it simply appends the brand new 15-mark row at the bottom!
6. Finally, it deletes the old file and renames the temp file.

---

## 4. The Math Engine: `calculate_student_grade_stats()`

This function is the mathematical heart of the project. It takes the 15 Mid-Sem marks and 15 End-Sem marks, crunches them, and outputs Pass/Fail, SGPA, and the final Grade Character.

### Step A: The 200-to-100 Scaling
Since a student can get up to 100 marks in the Mid-Sem (50+25+25) and up to 100 marks in the End-Sem, the total per subject is out of 200.
To convert this to a standard 100-percent scale:
```c
int scaled_total = (mid_total + end_total) / 2;
```

### Step B: Pass / Fail Check
We define the passing mark as 40%.
```c
if (scaled_total >= 40) (*passed)++;
else (*failed)++;
```

### Step C: Grade Points for SGPA (10-Point Scale)
SGPA (Semester Grade Point Average) is calculated by converting the raw percentage into a 10-point scale per subject.
- `>= 90`: 10 points
- `>= 80`: 9 points
- `>= 70`: 8 points
- `>= 60`: 7 points
- `>= 40`: 6 points
- `< 40`: 0 points (Fail)

It adds up the points for all 5 subjects and divides by 5.0 to get the exact floating-point SGPA:
```c
*sgpa = (float)total_points / 5.0f;
```

### Step D: The Final Character Grade
It converts the calculated SGPA back into an approximate percentage (`SGPA * 10.0`), and assigns the final alphabetical grade:
- **A**: 90%+
- **B**: 80%+
- **C**: 70%+
- **D**: 60%+
- **E**: 40%+
- **F**: Below 40%

**Viva Tip:** *"We separated the raw data storage (the `save` function) from the calculations (the `calculate_stats` function). By keeping the math independent, the system calculates the SGPA "On-The-Fly" when the Python GUI asks for a report, rather than saving redundant static SGPA numbers into the database. This prevents data corruption and ensures calculations are always 100% accurate!"*
