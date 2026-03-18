# 📘 ERP System — Git Rules & Workflow Guide

> A clean, disciplined Git workflow for a 2-person team building a Teacher–Student ERP in C (core logic) + Python (GUI)

---

## Project Context

* **Language Split**

  * C → core logic, structures, data handling
  * Python → GUI, interaction layer

* **Team Split (expected)**

  * Sagar → C modules (functions, structs, logic)
  * Abhinav → main.c + Python GUI

---

# 1. Branch Structure (Simple but Powerful)

We will use ONLY these branches:

```
main
develop
feature/*
fix/*
```

---

## `main`

* Always stable
* Always working
* Represents "final working ERP"

❌ Never code directly here
✔ Only receives merges from `develop`

---

## `develop`

* Active development branch
* All features merge here first

✔ Daily working base
✔ Must compile/run

---

## `feature/*`

* One feature = one branch
* Created from `develop`

Examples:

```
feature/login-system
feature/student-records
feature/marks-module
feature/gui-dashboard
feature/file-handling-c
```

---

## `fix/*`

* Bug fixes

Examples:

```
fix/login-crash
fix/memory-leak
```

---

# 2. Golden Rules (NON-NEGOTIABLE)

```
① Never code directly on main
② Always create a branch for work
③ Pull before starting work
④ Push daily (even incomplete work)
⑤ Keep commits small and meaningful
⑥ Don’t break develop
⑦ Don’t work on same file without communication
```

---

# 3. Project Structure (Recommended)

```
erp-system/
│
├── c_core/
│   ├── student.c
│   ├── teacher.c
│   ├── file_ops.c
│   ├── utils.c
│   └── erp.h
│
├── python_gui/
│   ├── app.py
│   ├── login.py
│   ├── dashboard.py
│   └── utils.py
│
├── main.c
├── README.md
└── .gitignore
```

---

# 4. Daily Workflow (STRICT ROUTINE)

## START OF DAY

```bash
git checkout develop
git pull origin develop
git checkout feature/your-branch
git merge develop
```

---

## DURING WORK

```bash
git status
git add .
git commit -m "feat(module): short message"
git push
```

✔ Commit every time something works
✔ Don’t wait till end

---

## END OF DAY

```bash
git add .
git commit -m "wip(module): partial progress"
git push
```

---

# 5. Starting a Feature

```bash
git checkout develop
git pull origin develop
git checkout -b feature/student-module
git push -u origin feature/student-module
```

---

# 6. Finishing a Feature

```bash
git checkout develop
git pull origin develop
git merge feature/student-module
git push origin develop

git branch -d feature/student-module
git push origin --delete feature/student-module
```

---

# 7. Collaboration Rules (VERY IMPORTANT)

## Case 1: Partner needs your code

### Option A (Simple)

* You merge to develop
* Partner pulls develop

```bash
git checkout develop
git pull origin develop
```

---

### Option B (Advanced — faster)

Partner directly pulls your branch:

```bash
git fetch origin
git checkout feature/your-branch
```

---

## Case 2: Same file conflict

👉 Talk before editing same file

If conflict happens:

```bash
# fix file manually
git add .
git commit -m "fix: resolved merge conflict"
```

---

# 8. Commit Message Format

```
type(scope): message
```

### Types:

```
feat     → new feature
fix      → bug fix
refactor → code cleanup
docs     → documentation
chore    → setup/config
wip      → incomplete work
```

---

### Examples:

```
feat(student): add student struct and initialization
feat(gui): create login screen layout
fix(file): prevent file overwrite bug
refactor(core): split logic into modules
wip(gui): dashboard UI not finished
```

---

# 9. Conflict Handling

Example conflict:

```
<<<<<<< HEAD
printf("A");
=======
printf("B");
>>>>>>> branch
```

### Fix:

1. Edit manually
2. Remove markers
3. Save

```bash
git add .
git commit -m "fix: resolved conflict"
```

---

# 10. Emergency Fix (Bug)

```bash
git checkout develop
git checkout -b fix/crash-login

# fix code

git add .
git commit -m "fix(login): prevent crash on empty input"

git checkout develop
git merge fix/crash-login
git push origin develop
```

---

# 11. .gitignore (IMPORTANT)

```
# C
*.o
*.out
*.exe

# Python
__pycache__/
*.pyc

# IDE
.vscode/
.idea/

# OS
.DS_Store
```

---

# 12. Quick Commands Cheat Sheet

```
START WORK
git checkout develop
git pull
git checkout feature/name

COMMIT
git add .
git commit -m "message"
git push

NEW FEATURE
git checkout develop
git pull
git checkout -b feature/name

MERGE
git checkout develop
git merge feature/name
git push

FIX CONFLICT
(edit)
git add .
git commit
```

---

# 13. Team Agreement (IMPORTANT)

Both must agree:

* No direct commits to main
* Always use branches
* Always pull before coding
* Communicate before editing shared files
* Keep commits readable

---

# 14. Final Workflow Summary

```
develop → base
feature → work
merge → develop
repeat
main → final stable
```

---
