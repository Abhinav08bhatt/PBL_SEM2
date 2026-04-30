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
                # 5 subjects * 3 components = 15 values
                marks = []
                for _ in range(5):
                    marks.append(str(random.randint(15, 45))) # Theory (max 50)
                    marks.append(str(random.randint(10, 25))) # Practical (max 25)
                    marks.append(str(random.randint(10, 25))) # Internal (max 25)
                
                subprocess.run([backend_binary, "add_grades", course, section, exam, str(roll)] + marks, cwd="../c_core")
