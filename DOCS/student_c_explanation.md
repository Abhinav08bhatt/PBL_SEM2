# 📘 Deep Dive: `student.c` Explanation

This guide is designed for your Viva preparation. It breaks down the exact logic, tricks, and mechanisms used inside the `student.c` file. This file is the "Manager" for all student data (adding, removing, reading, and writing).

---

## 1. The Core Idea: Managing Data in RAM vs. Hard Drive
In C, reading from and writing to text files continuously is slow and error-prone. 
To make our program fast and safe, we use the **"Load -> Modify -> Save"** pattern.

1. **Load:** We open the text file and copy all 20 students into an **Array of Structs** in the active memory (RAM).
2. **Modify:** We add, delete, or sort the students inside the RAM array. It is extremely fast to modify arrays.
3. **Save:** Once we are done modifying the array, we open the text file again in `"w"` (write) mode, completely wipe the old text file clean, and write the updated array into the file from scratch.

---

## 2. Dynamic File Paths: The `get_data_file_path()` Trick

We don't use just one file for students. We use isolated files for each section (like `BSc_CS_A_students.txt`). But how does C know which file to open? 

**The Code Logic:**
```c
void get_data_file_path(char *filepath, const char *type, const char *course, const char *section)
```
1. The function takes the `course` name (e.g., `"BSc CS"`) and the `section` (e.g., `"A"`).
2. It loops through the `course` string character by character. If it sees a space (`' '`), it replaces it with an underscore (`'_'`). `"BSc CS"` becomes `"BSc_CS"`.
3. Finally, it uses `sprintf()` to glue the strings together:
   `sprintf(filepath, "data/%s_%s_%s.txt", clean_course, section, type);`
4. The result is stored in `filepath`, giving us exactly `"data/BSc_CS_A_students.txt"`.

**Viva Tip:** *"This dynamic path generator allows us to support infinite courses and sections without writing hardcoded file names for every single class!"*

---

## 3. Reading from Files: `load_students_from_file()`

When we need to view students, we must read the text file.

**The Code Logic:**
1. We use `get_data_file_path()` to get the correct file address.
2. We open the file using `fopen(filepath, "r")`. The `"r"` stands for **Read Mode**.
3. We use a `while` loop with `fgets()` to read the file **line by line** until the end.
4. For each line (e.g., `12|Rahul Sharma`), we pass it to a helper function called `parse_student_line()`.
5. `parse_student_line()` uses `sscanf(line, "%d|%99[^\n]", &roll, name)` to split the line at the `|` pipe character. It pulls out the integer `12` and the string `"Rahul Sharma"`.
6. These are placed into a `StudentRecord` struct and added to our RAM array.

---

## 4. Writing to Files: `save_students_to_file()`

After we add or delete a student in our RAM array, we must update the permanent file.

**The Code Logic:**
1. We open the file using `fopen(filepath, "w")`. The `"w"` stands for **Write Mode**. 
   - *Crucial Detail:* Opening a file in `"w"` mode immediately deletes all existing text inside it. We are starting with a blank slate!
2. We use a `for` loop to go through our RAM array of students.
3. For each student, we use `fprintf(fp, "%d|%s\n", students[i].roll_number, students[i].name)` to write the student back into the file in our specific `Roll|Name` format.
4. We `fclose()` the file to save it safely.

---

## 5. Adding and Sorting Students

### `add_student_screen()`
1. We ask the user for a Roll Number and Name.
2. We use a simple `for` loop to check if the roll number already exists in our RAM array. If it does, we reject it (to prevent duplicates).
3. If it's valid, we append the new student to the end of the array.

### `sort_students_by_roll_number()`
We use the **Bubble Sort** algorithm.
1. It compares adjacent students in the array. 
2. If the first student has a larger roll number than the second student, it swaps their positions in the array using a temporary variable (`struct StudentRecord temp`).
3. It repeats this until the entire array is in perfect numerical order.
4. *Then* we trigger the save function, guaranteeing the text file is always written perfectly sorted.

---

## 6. The Deletion Trick: `remove_attendance_for_roll()`

If we delete a student, we also need to delete their historical attendance records so they don't pollute our data. But **C does not let you delete a line from the middle of a text file.**

**The "Temp File" Trick:**
1. We open the original attendance file in `"r"` (Read) mode.
2. We create a brand new, empty file called `data/attendance_tmp.txt` in `"w"` (Write) mode.
3. We read the original file line by line.
4. **The Magic:** If the line belongs to the student we are deleting, we ignore it. If the line belongs to ANY OTHER student, we `fprintf()` it into the new `temp` file.
5. Once we finish, we close both files.
6. We use the C function `remove(filepath)` to completely delete the original attendance file.
7. We use `rename("data/attendance_tmp.txt", filepath)` to change the name of our temporary file to the original file's name.

**Viva Tip:** *"This temp-file copying mechanism is the safest and most standard way to delete specific records from a flat-file database in C."*
