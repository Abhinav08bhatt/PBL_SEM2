#include <stdlib.h>

void clearTerminal() {
#ifdef _WIN32
    system("cls");   // Windows
#else
    system("clear"); // Linux & macOS
#endif
}