import os
import subprocess
import tkinter as tk
from datetime import date
from tkinter import messagebox
from tkinter import ttk

# Folder paths used by the GUI and backend.
python_gui_folder_path = os.path.abspath(os.path.dirname(__file__))
c_core_folder_path = os.path.abspath(os.path.join(python_gui_folder_path, "..", "c_core"))
backend_binary_full_path = os.path.join(python_gui_folder_path, "backend_app")


def build_backend_if_needed():
    if os.path.exists(backend_binary_full_path):
        return

    # Build the small C backend if it is not already built.
    compile_command = [
        "gcc",
        "-std=c99",
        "-Wall",
        "-Wextra",
        os.path.join(python_gui_folder_path, "backend_api.c"),
        os.path.join(c_core_folder_path, "student.c"),
        os.path.join(c_core_folder_path, "attendance.c"),
        "-o",
        backend_binary_full_path,
    ]

    compile_result = subprocess.run(
        compile_command,
        capture_output=True,
        text=True,
        cwd=python_gui_folder_path,
    )

    if compile_result.returncode != 0:
        error_message = compile_result.stderr.strip()
        if error_message == "":
            error_message = "Failed to compile backend"
        raise RuntimeError(error_message)


def run_backend_command(*command_arguments):
    build_backend_if_needed()

    run_result = subprocess.run(
        [backend_binary_full_path, *command_arguments],
        capture_output=True,
        text=True,
        cwd=c_core_folder_path,
    )

    output_text = run_result.stdout.strip()

    if run_result.returncode != 0:
        if output_text.startswith("ERR|"):
            raise RuntimeError(output_text.split("|", 1)[1])

        error_message = run_result.stderr.strip()
        if error_message == "":
            if output_text != "":
                error_message = output_text
            else:
                error_message = "Backend command failed"

        raise RuntimeError(error_message)

    return output_text


class AttendanceApp(tk.Tk):
    def __init__(self):
        super().__init__()

        self.title("Student Attendance GUI")
        self.geometry("980x680")

        self.student_rows = []
        self.status_box_by_roll_number = {}

        self.current_course = ""
        self.current_section = ""

        self.create_global_controls()

        self.notebook = ttk.Notebook(self)
        self.notebook.pack(fill="both", expand=True, padx=10, pady=10)

        self.students_tab = ttk.Frame(self.notebook)
        self.attendance_tab = ttk.Frame(self.notebook)
        self.reports_tab = ttk.Frame(self.notebook)

        self.notebook.add(self.students_tab, text="Students")
        self.notebook.add(self.attendance_tab, text="Attendance")
        self.notebook.add(self.reports_tab, text="Reports")

        self.create_students_tab()
        self.create_attendance_tab()
        self.create_reports_tab()

    def create_global_controls(self):
        top_frame = ttk.Frame(self)
        top_frame.pack(fill="x", padx=10, pady=10)

        ttk.Label(top_frame, text="Course:").pack(side="left")
        self.global_course_cb = ttk.Combobox(top_frame, values=["BSc CS", "BSc IT", "BCA", "BCA (AIDS)"], width=15, state="readonly")
        self.global_course_cb.pack(side="left", padx=5)
        if self.global_course_cb["values"]:
            self.global_course_cb.current(0)

        ttk.Label(top_frame, text="Section:").pack(side="left")
        self.global_section_cb = ttk.Combobox(top_frame, values=["A", "B", "C"], width=5, state="readonly")
        self.global_section_cb.pack(side="left", padx=5)
        if self.global_section_cb["values"]:
            self.global_section_cb.current(0)

        ttk.Button(top_frame, text="Load Section Data", command=self.load_section_data).pack(side="left", padx=10)

        self.status_label = ttk.Label(top_frame, text="No section loaded.", foreground="gray")
        self.status_label.pack(side="left", padx=10)

    def load_section_data(self):
        course = self.global_course_cb.get().strip()
        section = self.global_section_cb.get().strip()

        if not course or not section:
            messagebox.showwarning("Input Error", "Please select a Course and Section.")
            return

        self.current_course = course
        self.current_section = section
        
        self.status_label.config(text=f"Loaded: {course} - Section {section}", foreground="green")

        self.refresh_students_table()
        self.refresh_overall_report_table()

    def ensure_section_loaded(self):
        if not self.current_course or not self.current_section:
            messagebox.showwarning("Error", "Please load a section first from the top bar.")
            return False
        return True

    def create_students_tab(self):
        top_form_frame = ttk.Frame(self.students_tab)
        top_form_frame.pack(fill="x", padx=10, pady=10)

        ttk.Label(top_form_frame, text="Roll:").grid(row=0, column=0, sticky="w")
        self.roll_number_entry = ttk.Entry(top_form_frame, width=10)
        self.roll_number_entry.grid(row=0, column=1, padx=4)

        ttk.Label(top_form_frame, text="Name:").grid(row=0, column=2, sticky="w")
        self.student_name_entry = ttk.Entry(top_form_frame, width=30)
        self.student_name_entry.grid(row=0, column=3, padx=4)

        ttk.Button(top_form_frame, text="Add", command=self.add_student).grid(row=0, column=4, padx=4)
        ttk.Button(top_form_frame, text="Remove by Roll", command=self.remove_student).grid(row=0, column=5, padx=4)

        self.students_table = ttk.Treeview(
            self.students_tab,
            columns=("roll", "name"),
            show="headings",
            height=20,
        )
        self.students_table.heading("roll", text="Roll")
        self.students_table.heading("name", text="Name")
        self.students_table.column("roll", width=80, anchor="center")
        self.students_table.column("name", width=600)
        self.students_table.pack(fill="both", expand=True, padx=10, pady=10)

    def create_attendance_tab(self):
        top_input_frame = ttk.Frame(self.attendance_tab)
        top_input_frame.pack(fill="x", padx=10, pady=10)

        ttk.Label(top_input_frame, text="Date:").pack(side="left")
        self.attendance_date_entry = ttk.Entry(top_input_frame, width=12)
        self.attendance_date_entry.insert(0, date.today().isoformat())
        self.attendance_date_entry.pack(side="left", padx=4)

        ttk.Button(top_input_frame, text="Submit Attendance", command=self.submit_attendance).pack(side="left", padx=10)

        self.attendance_canvas = tk.Canvas(self.attendance_tab)
        self.attendance_canvas.pack(side="left", fill="both", expand=True, padx=10, pady=10)

        self.attendance_scrollbar = ttk.Scrollbar(
            self.attendance_tab,
            orient="vertical",
            command=self.attendance_canvas.yview,
        )
        self.attendance_scrollbar.pack(side="right", fill="y")
        self.attendance_canvas.configure(yscrollcommand=self.attendance_scrollbar.set)

        self.attendance_rows_frame = ttk.Frame(self.attendance_canvas)
        self.attendance_canvas.create_window((0, 0), window=self.attendance_rows_frame, anchor="nw")

        self.attendance_rows_frame.bind(
            "<Configure>",
            lambda event: self.attendance_canvas.configure(
                scrollregion=self.attendance_canvas.bbox("all")
            ),
        )

    def create_reports_tab(self):
        top_controls_frame = ttk.Frame(self.reports_tab)
        top_controls_frame.pack(fill="x", padx=10, pady=10)

        ttk.Label(top_controls_frame, text="Roll:").pack(side="left")
        self.report_roll_number_entry = ttk.Entry(top_controls_frame, width=15)
        self.report_roll_number_entry.pack(side="left", padx=8)

        ttk.Button(top_controls_frame, text="Get Student Report", command=self.get_student_report).pack(side="left", padx=6)

        self.single_student_report_text = tk.Text(self.reports_tab, height=7)
        self.single_student_report_text.pack(fill="x", padx=10, pady=(0, 10))

        self.overall_report_table = ttk.Treeview(
            self.reports_tab,
            columns=("roll", "name", "classes", "present", "absent", "percent"),
            show="headings",
            height=16,
        )
        self.overall_report_table.heading("roll", text="Roll")
        self.overall_report_table.heading("name", text="Name")
        self.overall_report_table.heading("classes", text="Classes")
        self.overall_report_table.heading("present", text="Present")
        self.overall_report_table.heading("absent", text="Absent")
        self.overall_report_table.heading("percent", text="Percent")

        self.overall_report_table.column("roll", width=80, anchor="center")
        self.overall_report_table.column("name", width=250)
        self.overall_report_table.column("classes", width=80, anchor="center")
        self.overall_report_table.column("present", width=80, anchor="center")
        self.overall_report_table.column("absent", width=80, anchor="center")
        self.overall_report_table.column("percent", width=80, anchor="center")
        self.overall_report_table.pack(fill="both", expand=True, padx=10, pady=10)

    def refresh_students_table(self):
        if not self.ensure_section_loaded():
            return
            
        try:
            backend_output_text = run_backend_command("list_students", self.current_course, self.current_section)
        except Exception as error_object:
            messagebox.showerror("Backend Error", str(error_object))
            return

        self.student_rows = []

        for each_line_text in backend_output_text.splitlines():
            split_parts = each_line_text.split("|", 1)
            if len(split_parts) == 2:
                self.student_rows.append(tuple(split_parts))

        for each_old_row in self.students_table.get_children():
            self.students_table.delete(each_old_row)

        for each_student_row in self.student_rows:
            self.students_table.insert("", "end", values=each_student_row)

        self.build_attendance_form()

    def add_student(self):
        if not self.ensure_section_loaded():
            return
            
        input_roll_number_text = self.roll_number_entry.get().strip()
        input_student_name_text = self.student_name_entry.get().strip()

        if input_roll_number_text == "" or input_student_name_text == "":
            messagebox.showwarning("Input", "Roll and Name are required")
            return

        try:
            run_backend_command("add_student", self.current_course, self.current_section, input_roll_number_text, input_student_name_text)
        except Exception as error_object:
            messagebox.showerror("Add Failed", str(error_object))
            return

        self.roll_number_entry.delete(0, tk.END)
        self.student_name_entry.delete(0, tk.END)
        self.refresh_students_table()
        self.refresh_overall_report_table()

    def remove_student(self):
        if not self.ensure_section_loaded():
            return
            
        input_roll_number_text = self.roll_number_entry.get().strip()

        if input_roll_number_text == "":
            messagebox.showwarning("Input", "Enter roll in the roll field")
            return

        try:
            run_backend_command("remove_student", self.current_course, self.current_section, input_roll_number_text)
        except Exception as error_object:
            messagebox.showerror("Remove Failed", str(error_object))
            return

        self.roll_number_entry.delete(0, tk.END)
        self.refresh_students_table()
        self.refresh_overall_report_table()

    def build_attendance_form(self):
        for each_widget in self.attendance_rows_frame.winfo_children():
            each_widget.destroy()

        self.status_box_by_roll_number = {}

        if len(self.student_rows) == 0:
            ttk.Label(self.attendance_rows_frame, text="No students found.").grid(row=0, column=0, sticky="w")
            return

        ttk.Label(self.attendance_rows_frame, text="Roll", width=12).grid(row=0, column=0, padx=8, pady=4)
        ttk.Label(self.attendance_rows_frame, text="Name", width=40).grid(row=0, column=1, padx=8, pady=4)
        ttk.Label(self.attendance_rows_frame, text="Status", width=10).grid(row=0, column=2, padx=8, pady=4)

        current_row_number = 1

        for each_roll_number_text, each_student_name_text in self.student_rows:
            ttk.Label(self.attendance_rows_frame, text=each_roll_number_text, width=12).grid(
                row=current_row_number, column=0, padx=8, pady=3,
            )
            ttk.Label(self.attendance_rows_frame, text=each_student_name_text, width=40).grid(
                row=current_row_number, column=1, padx=8, pady=3, sticky="w",
            )
            current_status_box = ttk.Combobox(
                self.attendance_rows_frame, values=["P", "A"], width=8, state="readonly",
            )
            current_status_box.set("P")
            current_status_box.grid(row=current_row_number, column=2, padx=8, pady=3)

            self.status_box_by_roll_number[each_roll_number_text] = current_status_box
            current_row_number += 1

    def submit_attendance(self):
        if not self.ensure_section_loaded():
            return
            
        input_date_text = self.attendance_date_entry.get().strip()

        if input_date_text == "":
            messagebox.showwarning("Input", "Date is required")
            return

        if len(self.student_rows) == 0:
            messagebox.showwarning("Input", "No students to mark")
            return

        backend_command_argument_list = ["mark_attendance", self.current_course, self.current_section, input_date_text]

        for each_roll_number_text, _ in self.student_rows:
            current_status_box = self.status_box_by_roll_number.get(each_roll_number_text)

            if current_status_box is None:
                messagebox.showwarning("Input", f"Missing status widget for roll {each_roll_number_text}")
                return

            current_status_text = current_status_box.get().strip().upper()

            if current_status_text != "P" and current_status_text != "A":
                messagebox.showwarning("Input", f"Invalid status for roll {each_roll_number_text}")
                return

            backend_command_argument_list.append(f"{each_roll_number_text}:{current_status_text}")

        try:
            run_backend_command(*backend_command_argument_list)
        except Exception as error_object:
            messagebox.showerror("Attendance Failed", str(error_object))
            return

        messagebox.showinfo("Success", "Attendance saved")
        self.refresh_overall_report_table()

    def get_student_report(self):
        if not self.ensure_section_loaded():
            return
            
        input_roll_number_text = self.report_roll_number_entry.get().strip()

        if input_roll_number_text == "":
            messagebox.showwarning("Input", "Roll is required")
            return

        try:
            backend_output_text = run_backend_command("get_report", self.current_course, self.current_section, input_roll_number_text)
        except Exception as error_object:
            messagebox.showerror("Report Failed", str(error_object))
            return

        report_parts = backend_output_text.split("|")

        if len(report_parts) != 6:
            messagebox.showerror("Report Failed", "Unexpected backend format")
            return

        readable_report_text = ""
        readable_report_text += f"Roll: {report_parts[0]}\n"
        readable_report_text += f"Name: {report_parts[1]}\n"
        readable_report_text += f"Classes: {report_parts[2]}\n"
        readable_report_text += f"Present: {report_parts[3]}\n"
        readable_report_text += f"Absent: {report_parts[4]}\n"
        readable_report_text += f"Attendance: {report_parts[5]}%\n"

        self.single_student_report_text.delete("1.0", tk.END)
        self.single_student_report_text.insert(tk.END, readable_report_text)

    def refresh_overall_report_table(self):
        if not self.ensure_section_loaded():
            return
            
        try:
            backend_output_text = run_backend_command("get_overall", self.current_course, self.current_section)
        except Exception as error_object:
            messagebox.showerror("Overall Failed", str(error_object))
            return

        for each_old_row in self.overall_report_table.get_children():
            self.overall_report_table.delete(each_old_row)

        for each_line_text in backend_output_text.splitlines():
            split_parts = each_line_text.split("|")
            if len(split_parts) == 6:
                self.overall_report_table.insert("", "end", values=split_parts)


if __name__ == "__main__":
    try:
        build_backend_if_needed()
    except Exception as error_object:
        hidden_window = tk.Tk()
        hidden_window.withdraw()
        messagebox.showerror("Startup Error", str(error_object))
        raise SystemExit(1)

    main_window = AttendanceApp()
    main_window.mainloop()
