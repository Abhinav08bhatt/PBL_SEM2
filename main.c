#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int command_result;

    /* Run the Python GUI from the main C file. */
    command_result = system("python3 CODE/python_gui/app.py");

    if (command_result != 0) {
        printf("Could not start the app.\n");
        return 1;
    }

    return 0;
}
