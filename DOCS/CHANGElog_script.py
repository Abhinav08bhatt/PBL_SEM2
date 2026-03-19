#!/usr/bin/env python3
"""
Generate a reliable project changelog snapshot for:
1) Everything under CODE/
2) main.c at repository root

Output sections:
- Empty files
- Modified/new files (with full readable content)
- Non-modified files
- Deleted files
"""

from __future__ import annotations

import argparse
import subprocess
from datetime import datetime
from pathlib import Path
from typing import Iterable


BASE_DIR = Path(__file__).resolve().parent.parent
DEFAULT_OUTPUT = BASE_DIR / "DOCS" / "CHANGElog.md"
TRACK_DIR = BASE_DIR / "CODE"
TRACK_FILE = BASE_DIR / "main.c"
MAX_READABLE_BYTES = 1_000_000  # 1 MB safety cap per file

LANGUAGE_BY_SUFFIX = {
    ".c": "c",
    ".h": "c",
    ".cpp": "cpp",
    ".cc": "cpp",
    ".cxx": "cpp",
    ".hpp": "cpp",
    ".hh": "cpp",
    ".py": "python",
    ".dart": "dart",
    ".java": "java",
    ".js": "javascript",
    ".ts": "typescript",
    ".tsx": "tsx",
    ".jsx": "jsx",
    ".go": "go",
    ".rs": "rust",
    ".kt": "kotlin",
    ".swift": "swift",
    ".php": "php",
    ".rb": "ruby",
    ".sh": "bash",
    ".zsh": "bash",
    ".md": "markdown",
    ".json": "json",
    ".yaml": "yaml",
    ".yml": "yaml",
    ".toml": "toml",
    ".xml": "xml",
    ".html": "html",
    ".css": "css",
    ".sql": "sql",
    ".txt": "text",
}


def run_git(*args: str) -> str:
    result = subprocess.run(
        ["git", *args],
        cwd=BASE_DIR,
        capture_output=True,
        text=True,
        check=False,
    )
    return result.stdout.strip()


def repo_relative(path: Path) -> str:
    return path.relative_to(BASE_DIR).as_posix()


def in_scope_path(path_str: str) -> bool:
    path = (BASE_DIR / path_str).resolve()
    if TRACK_DIR.exists():
        try:
            path.relative_to(TRACK_DIR.resolve())
            return True
        except ValueError:
            pass
    return path == TRACK_FILE.resolve()


def collect_scope_files() -> list[Path]:
    files: set[Path] = set()
    if TRACK_DIR.exists():
        for entry in TRACK_DIR.rglob("*"):
            if entry.is_file():
                files.add(entry.resolve())
    if TRACK_FILE.exists() and TRACK_FILE.is_file():
        files.add(TRACK_FILE.resolve())
    return sorted(files, key=lambda p: repo_relative(p))


def parse_status() -> dict[str, set[str]]:
    """
    Parse `git status --porcelain=1 --untracked-files=all` into repo-relative paths.
    """
    modified: set[str] = set()
    deleted: set[str] = set()
    untracked: set[str] = set()

    for line in run_git("status", "--porcelain=1", "--untracked-files=all").splitlines():
        if not line:
            continue

        if line.startswith("?? "):
            path = line[3:]
            if in_scope_path(path):
                untracked.add(path)
            continue
        if line.startswith("!! "):
            continue

        x, y = line[0], line[1]
        raw_path = line[3:]
        if " -> " in raw_path:
            _, raw_path = raw_path.split(" -> ", 1)

        if not in_scope_path(raw_path):
            continue

        if x == "D" or y == "D":
            deleted.add(raw_path)
        else:
            modified.add(raw_path)

    return {
        "modified": modified,
        "deleted": deleted,
        "untracked": untracked,
    }


def is_probably_text(data: bytes) -> bool:
    if b"\x00" in data:
        return False
    if not data:
        return True
    # Allow common whitespace control chars, reject excessive binary noise.
    allowed_controls = {9, 10, 13}
    suspicious = 0
    for byte in data:
        if byte < 32 and byte not in allowed_controls:
            suspicious += 1
    return (suspicious / len(data)) < 0.05


def read_text_file(path: Path, max_bytes: int) -> tuple[str, str]:
    """
    Returns (status, content)
    status in {"ok", "empty", "binary", "too_large", "error"}.
    """
    try:
        raw = path.read_bytes()
    except OSError as exc:
        return "error", f"[Unreadable file: {exc}]"

    if len(raw) == 0:
        return "empty", ""
    if len(raw) > max_bytes:
        return "too_large", f"[Skipped: file larger than {max_bytes} bytes]"
    if not is_probably_text(raw):
        return "binary", "[Skipped: binary/non-text file]"

    try:
        text = raw.decode("utf-8")
    except UnicodeDecodeError:
        text = raw.decode("utf-8", errors="replace")
    return "ok", text


def code_fence_language(path: Path) -> str:
    return LANGUAGE_BY_SUFFIX.get(path.suffix.lower(), "text")


def build_untracked_diff(rel_path: str, path: Path, max_bytes: int) -> str:
    status_name, content = read_text_file(path, max_bytes=max_bytes)
    if status_name == "empty":
        return (
            f"diff --git a/{rel_path} b/{rel_path}\n"
            "new file mode 100644\n"
            f"--- /dev/null\n+++ b/{rel_path}\n"
            "@@ -0,0 +1,0 @@"
        )
    if status_name in {"binary", "too_large", "error"}:
        return (
            f"diff --git a/{rel_path} b/{rel_path}\n"
            "new file mode 100644\n"
            f"--- /dev/null\n+++ b/{rel_path}\n"
            f"[{status_name}: full literal diff unavailable]"
        )

    file_lines = content.splitlines()
    header = [
        f"diff --git a/{rel_path} b/{rel_path}",
        "new file mode 100644",
        "--- /dev/null",
        f"+++ b/{rel_path}",
        f"@@ -0,0 +1,{len(file_lines)} @@",
    ]
    body = [f"+{line}" for line in file_lines]
    return "\n".join(header + body)


def get_literal_diff(rel_path: str, is_untracked: bool, max_bytes: int) -> str:
    path = BASE_DIR / rel_path
    if is_untracked:
        return build_untracked_diff(rel_path, path, max_bytes=max_bytes)
    diff = run_git("diff", "--no-color", "HEAD", "--", rel_path)
    return diff or "[No textual diff available]"


def section_list(title: str, items: Iterable[str]) -> str:
    sorted_items = sorted(set(items))
    lines = [f"## {title}"]
    if not sorted_items:
        lines.append("- None")
    else:
        lines.extend(f"- `{item}`" for item in sorted_items)
    return "\n".join(lines)


def build_report(max_bytes: int) -> str:
    now = datetime.now().astimezone()
    branch = run_git("branch", "--show-current") or "(detached HEAD)"
    last_commit = run_git("log", "-1", "--pretty=format:%h - %s (%an)")
    recent_commits = run_git("log", "-5", "--oneline")
    short_status = run_git("status", "--short")
    status = parse_status()

    scope_files = collect_scope_files()
    scope_rel = {repo_relative(p): p for p in scope_files}

    changed = status["modified"] | status["untracked"]
    deleted = status["deleted"]
    non_modified = sorted(path for path in scope_rel if path not in changed and path not in deleted)
    empty_files = sorted(path for path, p in scope_rel.items() if p.stat().st_size == 0)

    lines: list[str] = []
    lines.append("# CHANGELOG Snapshot")
    lines.append("")
    lines.append(f"- Generated: `{now.strftime('%Y-%m-%d %H:%M:%S %Z')}`")
    lines.append(f"- Branch: `{branch}`")
    lines.append(f"- Scope: `CODE/**` and `main.c`")
    lines.append("")
    lines.append("## Last Commit")
    lines.append(last_commit or "N/A")
    lines.append("")
    lines.append("## Recent Commits")
    lines.append("```text")
    lines.append(recent_commits or "No commits found.")
    lines.append("```")
    lines.append("")
    lines.append("## Git Status (Short)")
    lines.append("```text")
    lines.append(short_status or "Working tree clean.")
    lines.append("```")
    lines.append("")
    lines.append(section_list("Empty Files", empty_files))
    lines.append("")
    lines.append(section_list("Deleted Files", deleted))
    lines.append("")
    lines.append(section_list("Non-Modified Files", non_modified))
    lines.append("")
    lines.append(section_list("Modified/New Files", changed))
    lines.append("")
    lines.append("## Modified/New Files - Literal Changes (Diff)")
    if not changed:
        lines.append("- None")
    else:
        for rel_path in sorted(changed):
            lines.append("")
            lines.append(f"### `{rel_path}`")
            lines.append("```diff")
            lines.append(
                get_literal_diff(
                    rel_path,
                    is_untracked=rel_path in status["untracked"],
                    max_bytes=max_bytes,
                ).rstrip("\n")
            )
            lines.append("```")

    lines.append("")
    lines.append("## Modified/New Files - Full Content")

    if not changed:
        lines.append("- None")
    else:
        for rel_path in sorted(changed):
            path = scope_rel.get(rel_path, (BASE_DIR / rel_path))
            lines.append("")
            lines.append(f"### `{rel_path}`")

            if not path.exists():
                lines.append("_File path no longer exists on disk._")
                continue

            status_name, content = read_text_file(path, max_bytes=max_bytes)
            if status_name == "empty":
                lines.append("_Empty file._")
                continue
            if status_name in {"binary", "too_large", "error"}:
                lines.append(content)
                continue

            language = code_fence_language(path)
            lines.append(f"```{language}")
            lines.append(content.rstrip("\n"))
            lines.append("```")

    lines.append("")
    return "\n".join(lines)


def write_report(output_path: Path, content: str, mode: str) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    file_mode = "a" if mode == "append" else "w"
    with output_path.open(file_mode, encoding="utf-8") as f:
        if file_mode == "a" and output_path.stat().st_size > 0:
            f.write("\n\n---\n\n")
        f.write(content)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Generate a clean changelog snapshot for CODE/ and main.c."
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=DEFAULT_OUTPUT,
        help="Output markdown file path (default: DOCS/CHANGElog.md)",
    )
    parser.add_argument(
        "--mode",
        choices=["overwrite", "append"],
        default="append",
        help="append: add a new snapshot block, overwrite: rewrite file cleanly",
    )
    parser.add_argument(
        "--max-readable-bytes",
        type=int,
        default=MAX_READABLE_BYTES,
        help="Skip full-content dump for files larger than this byte size.",
    )
    args = parser.parse_args()

    report = build_report(max_bytes=args.max_readable_bytes)
    write_report(args.output, report, mode=args.mode)
    print(f"CHANGELOG written to: {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
