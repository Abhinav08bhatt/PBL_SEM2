# py_app_prototype

Prototype of the `CODE/c_core` attendance app with:
- Python frontend (`tkinter`)
- C backend (reusing logic from `CODE/c_core/student.c` and `CODE/c_core/attendance.c`)

## Structure
- `backend/backend_api.c`: command-style C backend API layer
- `backend/Makefile`: build backend binary
- `frontend/app.py`: Python GUI frontend

## Build Backend
From project root:

```bash
cd py_app_prototype/backend
make
```

This produces `backend/backend_app`.

## Run Frontend
From project root:

```bash
python3 py_app_prototype/frontend/app.py
```

The frontend auto-builds backend binary if missing.

## Notes
- No files in `CODE/` are modified.
- Data files are created in `py_app_prototype/` as:
  - `student_data.txt`
  - `attendance_data.txt`
