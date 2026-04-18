#define _XOPEN_SOURCE 700

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char executable_path[4096];
    char project_root[4096];
    char gui_script_path[4096];

    (void)argc;

    if (realpath(argv[0], executable_path) == NULL) {
        perror("Could not resolve launcher path");
        return 1;
    }

    strncpy(project_root, executable_path, sizeof(project_root) - 1);
    project_root[sizeof(project_root) - 1] = '\0';

    if (snprintf(
            gui_script_path,
            sizeof(gui_script_path),
            "%s/CODE/python_gui/app.py",
            dirname(project_root)) >= (int)sizeof(gui_script_path)) {
        fprintf(stderr, "GUI script path is too long\n");
        return 1;
    }

    execlp("python3", "python3", gui_script_path, (char *)NULL);

    fprintf(stderr, "Failed to launch python3: %s\n", strerror(errno));
    return 1;
}
