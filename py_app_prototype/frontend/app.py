import os
import subprocess
import tkinter as tk
from datetime import date
from tkinter import messagebox, ttk

project_prototype_root_folder_path = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
backend_c_source_file_full_path = os.path.join(project_prototype_root_folder_path, "backend", "backend_api.c")
backend_compiled_binary_full_path = os.path.join(project_prototype_root_folder_path, "backend", "backend_app")


def build_backend_if_not_already_built():
    backend_binary_exists_already = os.path.exists(backend_compiled_binary_full_path)
    if backend_binary_exists_already:
        return

    compile_command_list = [
        "gcc",
        "-std=c99",
        "-Wall",
        "-Wextra",
        backend_c_source_file_full_path,
        "-o",
        backend_compiled_binary_full_path,
    ]

    compile_process_result = subprocess.run(
        compile_command_list,
        capture_output=True,
        text=True,
        cwd=project_prototype_root_folder_path,
    )

    if compile_process_result.returncode != 0:
        backend_compile_error_message = compile_process_result.stderr.strip()
        if backend_compile_error_message == "":
            backend_compile_error_message = "Failed to compile C backend"
        raise RuntimeError(backend_compile_error_message)


def execute_backend_command_and_get_text_output(*backend_command_arguments):
    build_backend_if_not_already_built()

    full_command_to_run = [backend_compiled_binary_full_path]
    for each_argument_item in backend_command_arguments:
        full_command_to_run.append(each_argument_item)

    run_result_object = subprocess.run(
        full_command_to_run,
        capture_output=True,
        text=True,
        cwd=project_prototype_root_folder_path,
    )

    backend_standard_output_text = run_result_object.stdout.strip()

    if run_result_object.returncode != 0:
        if backend_standard_output_text != "":
            raise RuntimeError(backend_standard_output_text)
        fallback_backend_error_message = run_result_object.stderr.strip()
        if fallback_backend_error_message == "":
            fallback_backend_error_message = "Backend command failed"
        raise RuntimeError(fallback_backend_error_message)

    return backend_standard_output_text


class AttendancePrototypeWindow(tk.Tk):
    def __init__(self):
        super().__init__()

        self.title("Student Attendance Prototype (Python + C)")
        self.geometry("980x680")

        self.latest_students_roll_and_name_rows = []
        self.student_roll_number_to_status_combobox_widget_map = {}

        self.main_notebook_with_tabs = ttk.Notebook(self)
        self.main_notebook_with_tabs.pack(fill="both", expand=True, padx=10, pady=10)

        self.students_management_tab_frame = ttk.Frame(self.main_notebook_with_tabs)
        self.attendance_marking_tab_frame = ttk.Frame(self.main_notebook_with_tabs)
        self.reports_tab_frame = ttk.Frame(self.main_notebook_with_tabs)

        self.main_notebook_with_tabs.add(self.students_management_tab_frame, text="Students")
        self.main_notebook_with_tabs.add(self.attendance_marking_tab_frame, text="Attendance")
        self.main_notebook_with_tabs.add(self.reports_tab_frame, text="Reports")

        self.bottom_button_area_frame = ttk.Frame(self)
        self.bottom_button_area_frame.pack(fill="x", padx=10, pady=(0, 10))
        self.quit_application_button = ttk.Button(
            self.bottom_button_area_frame,
            text="Quit",
            command=self.destroy,
        )
        self.quit_application_button.pack(side="right")

        self.create_students_tab_widgets()
        self.create_attendance_tab_widgets()
        self.create_reports_tab_widgets()

        self.refresh_students_table_from_backend()
        self.refresh_overall_report_table_from_backend()

    def create_students_tab_widgets(self):
        self.students_tab_top_form_frame = ttk.Frame(self.students_management_tab_frame)
        self.students_tab_top_form_frame.pack(fill="x", padx=10, pady=10)

        ttk.Label(self.students_tab_top_form_frame, text="Roll:").grid(row=0, column=0, sticky="w")
        self.roll_number_input_entry = ttk.Entry(self.students_tab_top_form_frame, width=15)
        self.roll_number_input_entry.grid(row=0, column=1, padx=6)

        ttk.Label(self.students_tab_top_form_frame, text="Name:").grid(row=0, column=2, sticky="w")
        self.student_name_input_entry = ttk.Entry(self.students_tab_top_form_frame, width=40)
        self.student_name_input_entry.grid(row=0, column=3, padx=6)

        self.add_student_button = ttk.Button(
            self.students_tab_top_form_frame,
            text="Add",
            command=self.add_student_button_clicked,
        )
        self.add_student_button.grid(row=0, column=4, padx=6)

        self.remove_student_button = ttk.Button(
            self.students_tab_top_form_frame,
            text="Remove by Roll",
            command=self.remove_student_button_clicked,
        )
        self.remove_student_button.grid(row=0, column=5, padx=6)

        self.refresh_students_button = ttk.Button(
            self.students_tab_top_form_frame,
            text="Refresh",
            command=self.refresh_students_table_from_backend,
        )
        self.refresh_students_button.grid(row=0, column=6, padx=6)

        self.students_table_widget = ttk.Treeview(
            self.students_management_tab_frame,
            columns=("roll", "name"),
            show="headings",
            height=20,
        )
        self.students_table_widget.heading("roll", text="Roll")
        self.students_table_widget.heading("name", text="Name")
        self.students_table_widget.column("roll", width=140, anchor="center")
        self.students_table_widget.column("name", width=700)
        self.students_table_widget.pack(fill="both", expand=True, padx=10, pady=10)

    def create_attendance_tab_widgets(self):
        self.attendance_tab_top_input_frame = ttk.Frame(self.attendance_marking_tab_frame)
        self.attendance_tab_top_input_frame.pack(fill="x", padx=10, pady=10)

        ttk.Label(self.attendance_tab_top_input_frame, text="Date (YYYY-MM-DD):").pack(side="left")
        self.attendance_date_input_entry = ttk.Entry(self.attendance_tab_top_input_frame, width=15)
        self.attendance_date_input_entry.insert(0, date.today().isoformat())
        self.attendance_date_input_entry.pack(side="left", padx=8)

        self.load_students_for_attendance_button = ttk.Button(
            self.attendance_tab_top_input_frame,
            text="Load Students",
            command=self.rebuild_attendance_form_for_all_students,
        )
        self.load_students_for_attendance_button.pack(side="left", padx=6)

        self.submit_attendance_button = ttk.Button(
            self.attendance_tab_top_input_frame,
            text="Submit Attendance",
            command=self.submit_attendance_button_clicked,
        )
        self.submit_attendance_button.pack(side="left", padx=6)

        self.attendance_rows_scroll_canvas = tk.Canvas(self.attendance_marking_tab_frame)
        self.attendance_rows_scroll_canvas.pack(side="left", fill="both", expand=True, padx=10, pady=10)

        self.attendance_rows_vertical_scrollbar = ttk.Scrollbar(
            self.attendance_marking_tab_frame,
            orient="vertical",
            command=self.attendance_rows_scroll_canvas.yview,
        )
        self.attendance_rows_vertical_scrollbar.pack(side="right", fill="y")
        self.attendance_rows_scroll_canvas.configure(yscrollcommand=self.attendance_rows_vertical_scrollbar.set)

        self.attendance_rows_content_frame = ttk.Frame(self.attendance_rows_scroll_canvas)
        self.attendance_rows_scroll_canvas.create_window((0, 0), window=self.attendance_rows_content_frame, anchor="nw")
        self.attendance_rows_content_frame.bind(
            "<Configure>",
            lambda unused_event: self.attendance_rows_scroll_canvas.configure(
                scrollregion=self.attendance_rows_scroll_canvas.bbox("all")
            ),
        )

    def create_reports_tab_widgets(self):
        self.reports_top_controls_frame = ttk.Frame(self.reports_tab_frame)
        self.reports_top_controls_frame.pack(fill="x", padx=10, pady=10)

        ttk.Label(self.reports_top_controls_frame, text="Roll:").pack(side="left")
        self.single_student_report_roll_entry = ttk.Entry(self.reports_top_controls_frame, width=15)
        self.single_student_report_roll_entry.pack(side="left", padx=8)

        self.get_single_report_button = ttk.Button(
            self.reports_top_controls_frame,
            text="Get Student Report",
            command=self.get_single_student_report_button_clicked,
        )
        self.get_single_report_button.pack(side="left", padx=6)

        self.refresh_overall_report_button = ttk.Button(
            self.reports_top_controls_frame,
            text="Refresh Overall",
            command=self.refresh_overall_report_table_from_backend,
        )
        self.refresh_overall_report_button.pack(side="left", padx=6)

        self.single_student_report_text_area = tk.Text(self.reports_tab_frame, height=6)
        self.single_student_report_text_area.pack(fill="x", padx=10, pady=(0, 10))

        self.overall_report_table_widget = ttk.Treeview(
            self.reports_tab_frame,
            columns=("roll", "name", "classes", "present", "absent", "percent"),
            show="headings",
            height=16,
        )
        self.overall_report_table_widget.heading("roll", text="Roll")
        self.overall_report_table_widget.heading("name", text="Name")
        self.overall_report_table_widget.heading("classes", text="Classes")
        self.overall_report_table_widget.heading("present", text="Present")
        self.overall_report_table_widget.heading("absent", text="Absent")
        self.overall_report_table_widget.heading("percent", text="Percent")

        self.overall_report_table_widget.column("roll", width=90, anchor="center")
        self.overall_report_table_widget.column("name", width=260)
        self.overall_report_table_widget.column("classes", width=90, anchor="center")
        self.overall_report_table_widget.column("present", width=90, anchor="center")
        self.overall_report_table_widget.column("absent", width=90, anchor="center")
        self.overall_report_table_widget.column("percent", width=90, anchor="center")

        self.overall_report_table_widget.pack(fill="both", expand=True, padx=10, pady=10)

    def refresh_students_table_from_backend(self):
        try:
            backend_output_text_for_students = execute_backend_command_and_get_text_output("list_students")
        except Exception as backend_error_object:
            messagebox.showerror("Backend Error", str(backend_error_object))
            return

        self.latest_students_roll_and_name_rows = []

        output_lines_list = backend_output_text_for_students.splitlines()
        for each_output_line_text in output_lines_list:
            split_line_parts = each_output_line_text.split("|", 1)
            if len(split_line_parts) == 2:
                student_roll_number_text = split_line_parts[0]
                student_name_text = split_line_parts[1]
                self.latest_students_roll_and_name_rows.append((student_roll_number_text, student_name_text))

        all_existing_row_items = self.students_table_widget.get_children()
        for each_existing_row_item in all_existing_row_items:
            self.students_table_widget.delete(each_existing_row_item)

        for each_student_row in self.latest_students_roll_and_name_rows:
            self.students_table_widget.insert("", "end", values=each_student_row)

        self.rebuild_attendance_form_for_all_students()

    def add_student_button_clicked(self):
        input_roll_number_text = self.roll_number_input_entry.get().strip()
        input_student_name_text = self.student_name_input_entry.get().strip()

        if input_roll_number_text == "" or input_student_name_text == "":
            messagebox.showwarning("Input", "Roll and name are required")
            return

        try:
            execute_backend_command_and_get_text_output(
                "add_student",
                input_roll_number_text,
                input_student_name_text,
            )
        except Exception as add_error_object:
            messagebox.showerror("Add Failed", str(add_error_object))
            return

        self.roll_number_input_entry.delete(0, tk.END)
        self.student_name_input_entry.delete(0, tk.END)
        self.refresh_students_table_from_backend()
        self.refresh_overall_report_table_from_backend()

    def remove_student_button_clicked(self):
        input_roll_number_text = self.roll_number_input_entry.get().strip()
        if input_roll_number_text == "":
            messagebox.showwarning("Input", "Enter roll in the roll field")
            return

        try:
            execute_backend_command_and_get_text_output("remove_student", input_roll_number_text)
        except Exception as remove_error_object:
            messagebox.showerror("Remove Failed", str(remove_error_object))
            return

        self.roll_number_input_entry.delete(0, tk.END)
        self.refresh_students_table_from_backend()
        self.refresh_overall_report_table_from_backend()

    def rebuild_attendance_form_for_all_students(self):
        all_existing_attendance_widgets = self.attendance_rows_content_frame.winfo_children()
        for each_existing_widget in all_existing_attendance_widgets:
            each_existing_widget.destroy()

        self.student_roll_number_to_status_combobox_widget_map = {}

        number_of_students_available = len(self.latest_students_roll_and_name_rows)
        if number_of_students_available == 0:
            ttk.Label(self.attendance_rows_content_frame, text="No students found.").grid(row=0, column=0, sticky="w")
            return

        ttk.Label(self.attendance_rows_content_frame, text="Roll", width=12).grid(row=0, column=0, padx=8, pady=4)
        ttk.Label(self.attendance_rows_content_frame, text="Name", width=40).grid(row=0, column=1, padx=8, pady=4)
        ttk.Label(self.attendance_rows_content_frame, text="Status", width=10).grid(row=0, column=2, padx=8, pady=4)

        current_row_number_for_grid = 1
        for each_roll_number_text, each_student_name_text in self.latest_students_roll_and_name_rows:
            ttk.Label(self.attendance_rows_content_frame, text=each_roll_number_text, width=12).grid(
                row=current_row_number_for_grid,
                column=0,
                padx=8,
                pady=3,
            )
            ttk.Label(self.attendance_rows_content_frame, text=each_student_name_text, width=40).grid(
                row=current_row_number_for_grid,
                column=1,
                padx=8,
                pady=3,
                sticky="w",
            )

            status_choice_combobox = ttk.Combobox(
                self.attendance_rows_content_frame,
                values=["P", "A"],
                width=8,
                state="readonly",
            )
            status_choice_combobox.set("P")
            status_choice_combobox.grid(row=current_row_number_for_grid, column=2, padx=8, pady=3)

            self.student_roll_number_to_status_combobox_widget_map[each_roll_number_text] = status_choice_combobox
            current_row_number_for_grid = current_row_number_for_grid + 1

    def submit_attendance_button_clicked(self):
        input_attendance_date_text = self.attendance_date_input_entry.get().strip()
        if input_attendance_date_text == "":
            messagebox.showwarning("Input", "Date is required")
            return

        if len(self.latest_students_roll_and_name_rows) == 0:
            messagebox.showwarning("Input", "No students to mark")
            return

        backend_argument_list_for_mark_attendance = ["mark_attendance", input_attendance_date_text]

        for each_roll_number_text, _unused_name in self.latest_students_roll_and_name_rows:
            current_status_combobox_widget = self.student_roll_number_to_status_combobox_widget_map.get(each_roll_number_text)
            if current_status_combobox_widget is None:
                messagebox.showwarning("Input", f"Missing status widget for roll {each_roll_number_text}")
                return

            status_text_for_this_roll = current_status_combobox_widget.get().strip().upper()
            if status_text_for_this_roll != "P" and status_text_for_this_roll != "A":
                messagebox.showwarning("Input", f"Invalid status for roll {each_roll_number_text}")
                return

            status_token_text = f"{each_roll_number_text}:{status_text_for_this_roll}"
            backend_argument_list_for_mark_attendance.append(status_token_text)

        try:
            execute_backend_command_and_get_text_output(*backend_argument_list_for_mark_attendance)
        except Exception as attendance_error_object:
            messagebox.showerror("Attendance Failed", str(attendance_error_object))
            return

        messagebox.showinfo("Success", "Attendance saved")
        self.refresh_overall_report_table_from_backend()

    def get_single_student_report_button_clicked(self):
        input_roll_number_for_report_text = self.single_student_report_roll_entry.get().strip()
        if input_roll_number_for_report_text == "":
            messagebox.showwarning("Input", "Roll is required")
            return

        try:
            backend_output_text_for_single_report = execute_backend_command_and_get_text_output(
                "get_report",
                input_roll_number_for_report_text,
            )
        except Exception as report_error_object:
            messagebox.showerror("Report Failed", str(report_error_object))
            return

        split_report_parts_list = backend_output_text_for_single_report.split("|")
        if len(split_report_parts_list) != 6:
            messagebox.showerror("Report Failed", "Unexpected backend format")
            return

        roll_number_text = split_report_parts_list[0]
        student_name_text = split_report_parts_list[1]
        classes_count_text = split_report_parts_list[2]
        present_count_text = split_report_parts_list[3]
        absent_count_text = split_report_parts_list[4]
        attendance_percent_text = split_report_parts_list[5]

        final_report_text_to_show = ""
        final_report_text_to_show = final_report_text_to_show + f"Roll: {roll_number_text}\n"
        final_report_text_to_show = final_report_text_to_show + f"Name: {student_name_text}\n"
        final_report_text_to_show = final_report_text_to_show + f"Classes: {classes_count_text}\n"
        final_report_text_to_show = final_report_text_to_show + f"Present: {present_count_text}\n"
        final_report_text_to_show = final_report_text_to_show + f"Absent: {absent_count_text}\n"
        final_report_text_to_show = final_report_text_to_show + f"Attendance: {attendance_percent_text}%\n"

        self.single_student_report_text_area.delete("1.0", tk.END)
        self.single_student_report_text_area.insert(tk.END, final_report_text_to_show)

    def refresh_overall_report_table_from_backend(self):
        try:
            backend_output_text_for_overall_report = execute_backend_command_and_get_text_output("get_overall")
        except Exception as overall_error_object:
            messagebox.showerror("Overall Failed", str(overall_error_object))
            return

        all_existing_overall_rows = self.overall_report_table_widget.get_children()
        for each_row_item in all_existing_overall_rows:
            self.overall_report_table_widget.delete(each_row_item)

        overall_report_line_list = backend_output_text_for_overall_report.splitlines()
        for each_line in overall_report_line_list:
            split_columns = each_line.split("|")
            if len(split_columns) == 6:
                self.overall_report_table_widget.insert("", "end", values=split_columns)


if __name__ == "__main__":
    try:
        build_backend_if_not_already_built()
    except Exception as startup_error_object:
        temporary_hidden_root_window = tk.Tk()
        temporary_hidden_root_window.withdraw()
        messagebox.showerror("Startup Error", str(startup_error_object))
        raise SystemExit(1)

    beginner_style_prototype_window = AttendancePrototypeWindow()
    beginner_style_prototype_window.mainloop()
