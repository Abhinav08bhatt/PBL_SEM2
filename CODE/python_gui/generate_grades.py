import subprocess
import random
import os

courses = ["BSc CS", "BSc IT", "BCA", "BCA (AIDS)"]
sections = ["A", "B", "C"]
exam_types = ["MID", "END"]

backend_binary = os.path.abspath("backend_app")

for course in courses:
    for section in sections:
        for roll in range(1, 21):
            for exam in exam_types:
                marks = [str(random.randint(20, 50)) for _ in range(5)]
                subprocess.run([backend_binary, "add_grades", course, section, exam, str(roll)] + marks, cwd="../c_core")
