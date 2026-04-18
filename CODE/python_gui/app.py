import os
import subprocess
import tkinter as tk
from datetime import date
from tkinter import messagebox, ttk

python_gui_root = os.path.abspath(os.path.dirname(__file__))
c_core_root = os.path.abspath(os.path.join(python_gui_root, "..", "c_core"))
backend_binary_path = os.path.join(python_gui_root, "backend_app")


def build_backend_if_missing():
    if os.path.exists(backend_binary_path):
        return

    compile_command = [
        "gcc",
        "-std=c99",
        "-Wall",
        "-Wextra",
        os.path.join(python_gui_root, "backend_api.c"),
        os.path.join(c_core_root, "student.c"),
        os.path.join(c_core_root, "attendance.c"),
        "-o",
        backend_binary_path,
    ]

    result = subprocess.run(
        compile_command,
        capture_output=True,
        text=True,
        cwd=python_gui_root,
    )

    if result.returncode != 0:
        error_text = result.stderr.strip() or "Failed to compile GUI backend"
        raise RuntimeError(error_text)


def run_backend_command(*arguments):
    build_backend_if_missing()

    result = subprocess.run(
        [backend_binary_path, *arguments],
        capture_output=True,
        text=True,
        cwd=c_core_root,
    )

    stdout_text = result.stdout.strip()
    if result.returncode != 0:
        if stdout_text.startswith("ERR|"):
            raise RuntimeError(stdout_text.split("|", 1)[1])
        error_text = result.stderr.strip() or stdout_text or "Backend command failed"
        raise RuntimeError(error_text)

    return stdout_text


class AttendanceGui(tk.Tk):
    def __init__(self):
        super().__init__()

        self.title("Student Attendance GUI")
        self.geometry("980x680")

        self.students = []
        self.status_widgets = {}

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

        self.refresh_students()
        self.refresh_overall_report()

    def create_students_tab(self):
        form_frame = ttk.Frame(self.students_tab)
        form_frame.pack(fill="x", padx=10, pady=10)

        ttk.Label(form_frame, text="Roll:").grid(row=0, column=0, sticky="w")
        self.roll_entry = ttk.Entry(form_frame, width=15)
        self.roll_entry.grid(row=0, column=1, padx=6)

        ttk.Label(form_frame, text="Name:").grid(row=0, column=2, sticky="w")
        self.name_entry = ttk.Entry(form_frame, width=40)
        self.name_entry.grid(row=0, column=3, padx=6)

        ttk.Button(form_frame, text="Add", command=self.add_student).grid(row=0, column=4, padx=6)
        ttk.Button(form_frame, text="Remove by Roll", command=self.remove_student).grid(row=0, column=5, padx=6)
        ttk.Button(form_frame, text="Refresh", command=self.refresh_students).grid(row=0, column=6, padx=6)

        self.students_table = ttk.Treeview(
            self.students_tab,
            columns=("roll", "name"),
            show="headings",
            height=20,
        )
        self.students_table.heading("roll", text="Roll")
        self.students_table.heading("name", text="Name")
        self.students_table.column("roll", width=140, anchor="center")
        self.students_table.column("name", width=700)
        self.students_table.pack(fill="both", expand=True, padx=10, pady=10)

    def create_attendance_tab(self):
        top_frame = ttk.Frame(self.attendance_tab)
        top_frame.pack(fill="x", padx=10, pady=10)

        ttk.Label(top_frame, text="Date (YYYY-MM-DD):").pack(side="left")
        self.date_entry = ttk.Entry(top_frame, width=15)
        self.date_entry.insert(0, date.today().isoformat())
        self.date_entry.pack(side="left", padx=8)

        ttk.Button(top_frame, text="Load Students", command=self.build_attendance_form).pack(side="left", padx=6)
        ttk.Button(top_frame, text="Submit Attendance", command=self.submit_attendance).pack(side="left", padx=6)

        self.canvas = tk.Canvas(self.attendance_tab)
        self.canvas.pack(side="left", fill="both", expand=True, padx=10, pady=10)

        scrollbar = ttk.Scrollbar(self.attendance_tab, orient="vertical", command=self.canvas.yview)
        scrollbar.pack(side="right", fill="y")
        self.canvas.configure(yscrollcommand=scrollbar.set)

        self.attendance_frame = ttk.Frame(self.canvas)
        self.canvas.create_window((0, 0), window=self.attendance_frame, anchor="nw")
        self.attendance_frame.bind(
            "<Configure>",
            lambda _event: self.canvas.configure(scrollregion=self.canvas.bbox("all")),
        )

    def create_reports_tab(self):
        top_frame = ttk.Frame(self.reports_tab)
        top_frame.pack(fill="x", padx=10, pady=10)

        ttk.Label(top_frame, text="Roll:").pack(side="left")
        self.report_roll_entry = ttk.Entry(top_frame, width=15)
        self.report_roll_entry.pack(side="left", padx=8)

        ttk.Button(top_frame, text="Get Student Report", command=self.get_student_report).pack(side="left", padx=6)
        ttk.Button(top_frame, text="Refresh Overall", command=self.refresh_overall_report).pack(side="left", padx=6)

        self.single_report_text = tk.Text(self.reports_tab, height=6)
        self.single_report_text.pack(fill="x", padx=10, pady=(0, 10))

        self.overall_table = ttk.Treeview(
            self.reports_tab,
            columns=("roll", "name", "classes", "present", "absent", "percent"),
            show="headings",
            height=16,
        )
        self.overall_table.heading("roll", text="Roll")
        self.overall_table.heading("name", text="Name")
        self.overall_table.heading("classes", text="Classes")
        self.overall_table.heading("present", text="Present")
        self.overall_table.heading("absent", text="Absent")
        self.overall_table.heading("percent", text="Percent")

        self.overall_table.column("roll", width=90, anchor="center")
        self.overall_table.column("name", width=260)
        self.overall_table.column("classes", width=90, anchor="center")
        self.overall_table.column("present", width=90, anchor="center")
        self.overall_table.column("absent", width=90, anchor="center")
        self.overall_table.column("percent", width=90, anchor="center")
        self.overall_table.pack(fill="both", expand=True, padx=10, pady=10)

    def refresh_students(self):
        try:
            output_text = run_backend_command("list_students")
        except Exception as error:
            messagebox.showerror("Backend Error", str(error))
            return

        self.students = []
        for line in output_text.splitlines():
            parts = line.split("|", 1)
            if len(parts) == 2:
                self.students.append((parts[0], parts[1]))

        for item in self.students_table.get_children():
            self.students_table.delete(item)
        for student in self.students:
            self.students_table.insert("", "end", values=student)

        self.build_attendance_form()

    def add_student(self):
        roll = self.roll_entry.get().strip()
        name = self.name_entry.get().strip()

        if not roll or not name:
            messagebox.showwarning("Input", "Roll and name are required")
            return

        try:
            run_backend_command("add_student", roll, name)
        except Exception as error:
            messagebox.showerror("Add Failed", str(error))
            return

        self.roll_entry.delete(0, tk.END)
        self.name_entry.delete(0, tk.END)
        self.refresh_students()
        self.refresh_overall_report()

    def remove_student(self):
        roll = self.roll_entry.get().strip()

        if not roll:
            messagebox.showwarning("Input", "Enter roll in the roll field")
            return

        try:
            run_backend_command("remove_student", roll)
        except Exception as error:
            messagebox.showerror("Remove Failed", str(error))
            return

        self.roll_entry.delete(0, tk.END)
        self.refresh_students()
        self.refresh_overall_report()

    def build_attendance_form(self):
        for widget in self.attendance_frame.winfo_children():
            widget.destroy()

        self.status_widgets = {}

        if not self.students:
            ttk.Label(self.attendance_frame, text="No students found.").grid(row=0, column=0, sticky="w")
            return

        ttk.Label(self.attendance_frame, text="Roll", width=12).grid(row=0, column=0, padx=8, pady=4)
        ttk.Label(self.attendance_frame, text="Name", width=40).grid(row=0, column=1, padx=8, pady=4)
        ttk.Label(self.attendance_frame, text="Status", width=10).grid(row=0, column=2, padx=8, pady=4)

        row_number = 1
        for roll, name in self.students:
            ttk.Label(self.attendance_frame, text=roll, width=12).grid(row=row_number, column=0, padx=8, pady=3)
            ttk.Label(self.attendance_frame, text=name, width=40).grid(
                row=row_number,
                column=1,
                padx=8,
                pady=3,
                sticky="w",
            )

            status_box = ttk.Combobox(self.attendance_frame, values=["P", "A"], width=8, state="readonly")
            status_box.set("P")
            status_box.grid(row=row_number, column=2, padx=8, pady=3)
            self.status_widgets[roll] = status_box
            row_number += 1

    def submit_attendance(self):
        attendance_date = self.date_entry.get().strip()

        if not attendance_date:
            messagebox.showwarning("Input", "Date is required")
            return

        if not self.students:
            messagebox.showwarning("Input", "No students to mark")
            return

        command_arguments = ["mark_attendance", attendance_date]
        for roll, _name in self.students:
            status_box = self.status_widgets.get(roll)
            if status_box is None:
                messagebox.showwarning("Input", f"Missing status widget for roll {roll}")
                return

            status = status_box.get().strip().upper()
            if status not in {"P", "A"}:
                messagebox.showwarning("Input", f"Invalid status for roll {roll}")
                return

            command_arguments.append(f"{roll}:{status}")

        try:
            run_backend_command(*command_arguments)
        except Exception as error:
            messagebox.showerror("Attendance Failed", str(error))
            return

        messagebox.showinfo("Success", "Attendance saved")
        self.refresh_overall_report()

    def get_student_report(self):
        roll = self.report_roll_entry.get().strip()

        if not roll:
            messagebox.showwarning("Input", "Roll is required")
            return

        try:
            report_text = run_backend_command("get_report", roll)
        except Exception as error:
            messagebox.showerror("Report Failed", str(error))
            return

        parts = report_text.split("|")
        if len(parts) != 6:
            messagebox.showerror("Report Failed", "Unexpected backend format")
            return

        display_text = (
            f"Roll: {parts[0]}\n"
            f"Name: {parts[1]}\n"
            f"Classes: {parts[2]}\n"
            f"Present: {parts[3]}\n"
            f"Absent: {parts[4]}\n"
            f"Attendance: {parts[5]}%\n"
        )

        self.single_report_text.delete("1.0", tk.END)
        self.single_report_text.insert(tk.END, display_text)

    def refresh_overall_report(self):
        try:
            output_text = run_backend_command("get_overall")
        except Exception as error:
            messagebox.showerror("Overall Failed", str(error))
            return

        for item in self.overall_table.get_children():
            self.overall_table.delete(item)

        for line in output_text.splitlines():
            parts = line.split("|")
            if len(parts) == 6:
                self.overall_table.insert("", "end", values=parts)


if __name__ == "__main__":
    try:
        build_backend_if_missing()
    except Exception as error:
        hidden_root = tk.Tk()
        hidden_root.withdraw()
        messagebox.showerror("Startup Error", str(error))
        raise SystemExit(1)

    application = AttendanceGui()
    application.mainloop()
