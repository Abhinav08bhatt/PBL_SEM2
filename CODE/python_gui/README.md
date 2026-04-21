# Python GUI

This folder contains a `tkinter` GUI for the attendance system.

It uses:
- `backend_api.c` as a small command backend
- `app.py` as the GUI
- `../c_core/student.c` and `../c_core/attendance.c` through their headers

The GUI reads and writes the same data files used by the C CLI:
- `CODE/c_core/student_data.txt`
- `CODE/c_core/attendance_data.txt`

## Build backend

```bash
cd CODE/python_gui
make
```

## Run GUI

```bash
python3 CODE/python_gui/app.py
```

The app auto-builds the backend if `backend_app` is missing.
