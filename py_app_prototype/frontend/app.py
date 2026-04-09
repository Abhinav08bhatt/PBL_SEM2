import os
import subprocess
import tkinter as tk
from datetime import date
from tkinter import messagebox, ttk

BASE_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
BACKEND_SRC = os.path.join(BASE_DIR, "backend", "backend_api.c")
BACKEND_BIN = os.path.join(BASE_DIR, "backend", "backend_app")


def ensure_backend_built():
    if os.path.exists(BACKEND_BIN):
        return

    cmd = ["gcc", "-std=c99", "-Wall", "-Wextra", BACKEND_SRC, "-o", BACKEND_BIN]
    result = subprocess.run(cmd, capture_output=True, text=True, cwd=BASE_DIR)
    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip() or "Failed to compile backend")


def run_backend(*args):
    ensure_backend_built()
    cmd = [BACKEND_BIN, *args]
    result = subprocess.run(cmd, capture_output=True, text=True, cwd=BASE_DIR)
    out = result.stdout.strip()

    if result.returncode != 0:
        err = out if out else (result.stderr.strip() or "Backend command failed")
        raise RuntimeError(err)
    return out


def parse_students(output):
    rows = []
    if not output:
        return rows
    for line in output.splitlines():
        parts = line.split("|", 1)
        if len(parts) == 2:
            rows.append((parts[0], parts[1]))
    return rows


def parse_overall(output):
    rows = []
    if not output:
        return rows
    for line in output.splitlines():
        parts = line.split("|")
        if len(parts) == 6:
            rows.append(parts)
    return rows


class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Student Attendance Prototype (Python + C)")
        self.geometry("980x680")

        self.students_cache = []

        notebook = ttk.Notebook(self)
        notebook.pack(fill="both", expand=True, padx=10, pady=10)

        self.students_tab = ttk.Frame(notebook)
        self.attendance_tab = ttk.Frame(notebook)
        self.reports_tab = ttk.Frame(notebook)

        notebook.add(self.students_tab, text="Students")
        notebook.add(self.attendance_tab, text="Attendance")
        notebook.add(self.reports_tab, text="Reports")

        self.build_students_tab()
        self.build_attendance_tab()
        self.build_reports_tab()

        self.refresh_students()
        self.refresh_overall()

    def build_students_tab(self):
        form = ttk.Frame(self.students_tab)
        form.pack(fill="x", padx=10, pady=10)

        ttk.Label(form, text="Roll:").grid(row=0, column=0, sticky="w")
        self.roll_entry = ttk.Entry(form, width=15)
        self.roll_entry.grid(row=0, column=1, padx=6)

        ttk.Label(form, text="Name:").grid(row=0, column=2, sticky="w")
        self.name_entry = ttk.Entry(form, width=40)
        self.name_entry.grid(row=0, column=3, padx=6)

        ttk.Button(form, text="Add", command=self.add_student).grid(row=0, column=4, padx=6)
        ttk.Button(form, text="Remove by Roll", command=self.remove_student).grid(row=0, column=5, padx=6)
        ttk.Button(form, text="Refresh", command=self.refresh_students).grid(row=0, column=6, padx=6)

        cols = ("roll", "name")
        self.students_tree = ttk.Treeview(self.students_tab, columns=cols, show="headings", height=20)
        self.students_tree.heading("roll", text="Roll")
        self.students_tree.heading("name", text="Name")
        self.students_tree.column("roll", width=140, anchor="center")
        self.students_tree.column("name", width=700)
        self.students_tree.pack(fill="both", expand=True, padx=10, pady=10)

    def build_attendance_tab(self):
        top = ttk.Frame(self.attendance_tab)
        top.pack(fill="x", padx=10, pady=10)

        ttk.Label(top, text="Date (YYYY-MM-DD):").pack(side="left")
        self.date_entry = ttk.Entry(top, width=15)
        self.date_entry.insert(0, date.today().isoformat())
        self.date_entry.pack(side="left", padx=8)

        ttk.Button(top, text="Load Students", command=self.load_attendance_grid).pack(side="left", padx=6)
        ttk.Button(top, text="Submit Attendance", command=self.submit_attendance).pack(side="left", padx=6)

        self.attendance_canvas = tk.Canvas(self.attendance_tab)
        self.attendance_canvas.pack(side="left", fill="both", expand=True, padx=10, pady=10)

        scrollbar = ttk.Scrollbar(self.attendance_tab, orient="vertical", command=self.attendance_canvas.yview)
        scrollbar.pack(side="right", fill="y")
        self.attendance_canvas.configure(yscrollcommand=scrollbar.set)

        self.attendance_frame = ttk.Frame(self.attendance_canvas)
        self.attendance_canvas.create_window((0, 0), window=self.attendance_frame, anchor="nw")
        self.attendance_frame.bind(
            "<Configure>",
            lambda _: self.attendance_canvas.configure(scrollregion=self.attendance_canvas.bbox("all")),
        )

        self.status_widgets = {}

    def build_reports_tab(self):
        top = ttk.Frame(self.reports_tab)
        top.pack(fill="x", padx=10, pady=10)

        ttk.Label(top, text="Roll:").pack(side="left")
        self.report_roll_entry = ttk.Entry(top, width=15)
        self.report_roll_entry.pack(side="left", padx=8)
        ttk.Button(top, text="Get Student Report", command=self.get_report).pack(side="left", padx=6)
        ttk.Button(top, text="Refresh Overall", command=self.refresh_overall).pack(side="left", padx=6)

        self.report_text = tk.Text(self.reports_tab, height=6)
        self.report_text.pack(fill="x", padx=10, pady=(0, 10))

        cols = ("roll", "name", "classes", "present", "absent", "percent")
        self.overall_tree = ttk.Treeview(self.reports_tab, columns=cols, show="headings", height=16)
        for col in cols:
            self.overall_tree.heading(col, text=col.capitalize())
        self.overall_tree.column("roll", width=90, anchor="center")
        self.overall_tree.column("name", width=260)
        self.overall_tree.column("classes", width=90, anchor="center")
        self.overall_tree.column("present", width=90, anchor="center")
        self.overall_tree.column("absent", width=90, anchor="center")
        self.overall_tree.column("percent", width=90, anchor="center")
        self.overall_tree.pack(fill="both", expand=True, padx=10, pady=10)

    def refresh_students(self):
        try:
            output = run_backend("list_students")
        except Exception as exc:
            messagebox.showerror("Backend Error", str(exc))
            return

        self.students_cache = parse_students(output)

        for item in self.students_tree.get_children():
            self.students_tree.delete(item)
        for row in self.students_cache:
            self.students_tree.insert("", "end", values=row)

        self.load_attendance_grid()

    def add_student(self):
        roll = self.roll_entry.get().strip()
        name = self.name_entry.get().strip()
        if not roll or not name:
            messagebox.showwarning("Input", "Roll and name are required")
            return

        try:
            run_backend("add_student", roll, name)
        except Exception as exc:
            messagebox.showerror("Add Failed", str(exc))
            return

        self.roll_entry.delete(0, tk.END)
        self.name_entry.delete(0, tk.END)
        self.refresh_students()

    def remove_student(self):
        roll = self.roll_entry.get().strip()
        if not roll:
            messagebox.showwarning("Input", "Enter roll in the roll field")
            return

        try:
            run_backend("remove_student", roll)
        except Exception as exc:
            messagebox.showerror("Remove Failed", str(exc))
            return

        self.roll_entry.delete(0, tk.END)
        self.refresh_students()
        self.refresh_overall()

    def load_attendance_grid(self):
        for widget in self.attendance_frame.winfo_children():
            widget.destroy()
        self.status_widgets = {}

        if not self.students_cache:
            ttk.Label(self.attendance_frame, text="No students found.").grid(row=0, column=0, sticky="w")
            return

        ttk.Label(self.attendance_frame, text="Roll", width=12).grid(row=0, column=0, padx=8, pady=4)
        ttk.Label(self.attendance_frame, text="Name", width=40).grid(row=0, column=1, padx=8, pady=4)
        ttk.Label(self.attendance_frame, text="Status", width=10).grid(row=0, column=2, padx=8, pady=4)

        for idx, (roll, name) in enumerate(self.students_cache, start=1):
            ttk.Label(self.attendance_frame, text=roll, width=12).grid(row=idx, column=0, padx=8, pady=3)
            ttk.Label(self.attendance_frame, text=name, width=40).grid(row=idx, column=1, padx=8, pady=3, sticky="w")

            status = ttk.Combobox(self.attendance_frame, values=["P", "A"], width=8, state="readonly")
            status.set("P")
            status.grid(row=idx, column=2, padx=8, pady=3)
            self.status_widgets[roll] = status

    def submit_attendance(self):
        attendance_date = self.date_entry.get().strip()
        if not attendance_date:
            messagebox.showwarning("Input", "Date is required")
            return
        if not self.students_cache:
            messagebox.showwarning("Input", "No students to mark")
            return

        args = ["mark_attendance", attendance_date]
        for roll, _ in self.students_cache:
            status = self.status_widgets[roll].get().strip().upper()
            if status not in {"P", "A"}:
                messagebox.showwarning("Input", f"Invalid status for roll {roll}")
                return
            args.append(f"{roll}:{status}")

        try:
            run_backend(*args)
        except Exception as exc:
            messagebox.showerror("Attendance Failed", str(exc))
            return

        messagebox.showinfo("Success", "Attendance saved")
        self.refresh_overall()

    def get_report(self):
        roll = self.report_roll_entry.get().strip()
        if not roll:
            messagebox.showwarning("Input", "Roll is required")
            return

        try:
            output = run_backend("get_report", roll)
        except Exception as exc:
            messagebox.showerror("Report Failed", str(exc))
            return

        parts = output.split("|")
        if len(parts) != 6:
            messagebox.showerror("Report Failed", "Unexpected backend format")
            return

        text = (
            f"Roll: {parts[0]}\n"
            f"Name: {parts[1]}\n"
            f"Classes: {parts[2]}\n"
            f"Present: {parts[3]}\n"
            f"Absent: {parts[4]}\n"
            f"Attendance: {parts[5]}%\n"
        )
        self.report_text.delete("1.0", tk.END)
        self.report_text.insert(tk.END, text)

    def refresh_overall(self):
        try:
            output = run_backend("get_overall")
        except Exception as exc:
            messagebox.showerror("Overall Failed", str(exc))
            return

        rows = parse_overall(output)
        for item in self.overall_tree.get_children():
            self.overall_tree.delete(item)
        for row in rows:
            self.overall_tree.insert("", "end", values=row)


if __name__ == "__main__":
    try:
        ensure_backend_built()
    except Exception as exc:
        root = tk.Tk()
        root.withdraw()
        messagebox.showerror("Startup Error", str(exc))
        raise SystemExit(1)

    app = App()
    app.mainloop()
