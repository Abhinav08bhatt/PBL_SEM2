# 1. Command Line Arguments (CLA)

```c id="yo6mwy"
int main(int argc, char *argv[])
```

### Meaning

| Part     | Meaning                            |
| -------- | ---------------------------------- |
| `argc`   | Argument count                     |
| `argv[]` | Argument vector (array of strings) |

### Example

```bash id="x5xpjv"
./a.out hello 123
```

Then:

```c id="rpf17x"
argc = 3

argv[0] = "./a.out"
argv[1] = "hello"
argv[2] = "123"
```

---

# 2. String Input WITHOUT Space

Uses `scanf()`

```c id="d2yq5k"
scanf("%s", str);
```

### Meaning

| Part  | Meaning         |
| ----- | --------------- |
| `%s`  | Read string     |
| `str` | Character array |

### Input

```text id="fj4dsm"
Hello
```

Works fine.

### Problem

```text id="7f2n4h"
Hello World
```

Only stores:

```text id="p76eah"
Hello
```

because `scanf()` stops at space.

---

# 3. String Input WITH Space

Uses `fgets()`

```c id="kfeqjt"
fgets(str, sizeof(str), stdin);
```

### Meaning

| Part          | Meaning        |
| ------------- | -------------- |
| `str`         | String array   |
| `sizeof(str)` | Maximum size   |
| `stdin`       | Keyboard input |

### Example Input

```text id="jzcw22"
Hello World
```

Stores full line.

---

# 4. Common `string.h` Functions

Include:

```c id="8nys3x"
#include <string.h>
```

---

## (a) strlen()

```c id="km8uf8"
strlen(str);
```

### Meaning

Returns length of string.

Example:

```c id="a3hjta"
strlen("Hello")
```

returns:

```text id="rl5s4v"
5
```

---

## (b) strcpy()

```c id="6drf6u"
strcpy(dest, source);
```

### Meaning

Copies source string into destination.

Example:

```c id="u6fxnh"
strcpy(str2, str1);
```

---

## (c) strcat()

```c id="v7ftfi"
strcat(str1, str2);
```

### Meaning

Concatenates (joins) strings.

Example:

```c id="mzcdon"
strcat("Hello", "World");
```

Result:

```text id="86m8k5"
HelloWorld
```

---

## (d) strcmp()

```c id="63x79w"
strcmp(str1, str2);
```

### Meaning

Compares two strings.

Returns:

| Return | Meaning       |
| ------ | ------------- |
| `0`    | Equal         |
| `<0`   | First smaller |
| `>0`   | First greater |

---

# 5. Write in File Line by Line

Uses `fputs()`

```c id="m37xyu"
fputs("Hello\n", fp);
```

### Meaning

| Part        | Meaning              |
| ----------- | -------------------- |
| `fputs()`   | Write string to file |
| `"Hello\n"` | Data to write        |
| `fp`        | File pointer         |

---

# 6. Read File Line by Line

Uses `fgets()`

```c id="4h0mt9"
fgets(str, sizeof(str), fp);
```

### Meaning

| Part          | Meaning        |
| ------------- | -------------- |
| `str`         | Store line     |
| `sizeof(str)` | Max characters |
| `fp`          | File pointer   |

Usually used in loop:

```c id="7tye6q"
while(fgets(str, sizeof(str), fp) != NULL)
```

---

# 7. Write Character by Character

Uses `fputc()`

```c id="iq7y6w"
fputc(ch, fp);
```

### Meaning

| Part | Meaning            |
| ---- | ------------------ |
| `ch` | Character to write |
| `fp` | File pointer       |

Example:

```c id="2qx5cs"
fputc('A', fp);
```

---

# 8. Read Character by Character

Uses `fgetc()`

```c id="xiz8q0"
ch = fgetc(fp);
```

### Meaning

| Part      | Meaning             |
| --------- | ------------------- |
| `fgetc()` | Reads one character |
| `fp`      | File pointer        |

Usually:

```c id="1b7b5m"
while((ch = fgetc(fp)) != EOF)
```

### Meaning

| Part        | Meaning                  |
| ----------- | ------------------------ |
| `fgetc(fp)` | Read one char            |
| `EOF`       | End of file              |
| `!= EOF`    | Continue until file ends |

---

# 9. Important File Syntax

## Open File

```c id="4g6j68"
fp = fopen("data.txt", "r");
```

### Modes

| Mode  | Meaning |
| ----- | ------- |
| `"r"` | Read    |
| `"w"` | Write   |
| `"a"` | Append  |

---

## Close File

```c id="crx2hb"
fclose(fp);
```

Closes file safely.

---

# 10. Most Important Declaration

```c id="czm8tb"
FILE *fp;
```

### Meaning

| Part   | Meaning        |
| ------ | -------------- |
| `FILE` | File structure |
| `*fp`  | File pointer   |

---

# Golden Concept

## Strings in C

```c id="b39qtf"
char str[100];
```

is internally:

```c id="2ok0el"
array of characters ending with '\0'
```

Example:

```text id="5n09z4"
H e l l o \0
```
