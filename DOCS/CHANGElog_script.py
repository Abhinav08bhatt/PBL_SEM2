import subprocess
from datetime import datetime
import os

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(BASE_DIR)

CHANGELOG_FILE = os.path.join("DOCS", "CHANGElog.md")

TRACK_PATHS = ["CODE", "main.c"]
VALID_EXTENSIONS = (".c", ".h", ".py", ".md", ".txt")
MAX_FILE_SIZE = 100000  # 100 KB


def run_git_command(cmd):
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    return result.stdout.strip()


def get_current_branch():
    return run_git_command("git branch --show-current")


def get_last_commit():
    return run_git_command("git log -1 --pretty=format:'%h - %s (%an)'")


def get_recent_commits():
    return run_git_command("git log -5 --oneline")


def get_status_porcelain():
    return run_git_command("git status --porcelain")


def parse_git_status():
    """
    Returns dict:
    {
        "modified": [],
        "added": [],
        "deleted": []
    }
    """
    status = get_status_porcelain().splitlines()
    changes = {"modified": [], "added": [], "deleted": []}

    for line in status:
        code = line[:2].strip()
        file = line[3:]

        if code in ["M", "MM", "AM"]:
            changes["modified"].append(file)
        elif code in ["A"]:
            changes["added"].append(file)
        elif code in ["D"]:
            changes["deleted"].append(file)

    return changes


def read_file(path):
    try:
        if os.path.getsize(path) > MAX_FILE_SIZE:
            return "[Skipped: file too large]\n"
        with open(path, "r", encoding="utf-8", errors="ignore") as f:
            return f.read()
    except:
        return "[Unreadable file]\n"


def get_all_tracked_files():
    files_list = []

    for path in TRACK_PATHS:
        if os.path.isfile(path):
            files_list.append(path)

        elif os.path.isdir(path):
            for root, dirs, files in os.walk(path):
                for file in files:
                    if file.endswith(VALID_EXTENSIONS):
                        files_list.append(os.path.join(root, file))

    return sorted(files_list)


def generate_snapshot():
    changes = parse_git_status()
    all_files = get_all_tracked_files()

    snapshot = ""

    snapshot += "\n## 🔥 Changed Files (Full Content)\n"

    for category in ["added", "modified"]:
        for file in changes[category]:
            if os.path.exists(file) and file.endswith(VALID_EXTENSIONS):
                snapshot += f"\n--- {category.upper()}: {file} ---\n"
                snapshot += read_file(file)

    snapshot += "\n## 🗑 Deleted Files\n"
    for file in changes["deleted"]:
        snapshot += f"{file}\n"

    snapshot += "\n## 📄 Unchanged Files\n"
    changed_set = set(changes["modified"] + changes["added"] + changes["deleted"])

    for file in all_files:
        if file not in changed_set:
            snapshot += f"{file} (unchanged)\n"

    return snapshot


def append_changelog():
    now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    entry = f"""
# 📌 Update: {now}

## 🌿 Branch
{get_current_branch()}

## 🧾 Last Commit
{get_last_commit()}

## 📜 Recent Commits
{get_recent_commits()}

## 📂 Git Status
{run_git_command("git status --short")}

{generate_snapshot()}

------------------------------------------------------------
"""

    # ✅ Ensure git/ folder exists
    os.makedirs(os.path.dirname(CHANGELOG_FILE), exist_ok=True)

    with open(CHANGELOG_FILE, "a", encoding="utf-8") as f:
        f.write(entry)

    print("✅ SMART CHANGELOG UPDATED → git/CHANGElog.md")


if __name__ == "__main__":
    append_changelog()