*This project has been created as part of the 42 curriculum by elim-hon.*

# get_next_line

## Description

`get_next_line` is a 42 curriculum project whose goal is to implement a function that returns **one line at a time** from a file descriptor, on each successive call. The function must work for regular files, for standard input, and for any compile-time `BUFFER_SIZE` value. It returns the line that was read (including the terminating `\n` when it exists), or `NULL` when there is nothing left to read, or when an error occurs.

The project has two parts:

- Mandatory - a single static variable holds the reading state.
- Bonus - the same function supports multiple file descriptors at the same time (interleaved calls), while still using only one static variable.

Prototype:

```c
char *get_next_line(int fd);
```
## Instructions

### Compilation

The project must compile both with and without the `-D BUFFER_SIZE=n` flag, always with `cc -Wall -Wextra -Werror`.

Mandatory (default `BUFFER_SIZE` is 42):

```sh
cc -Wall -Wextra -Werror get_next_line.c get_next_line_utils.c -o gnl
```

With an explicit buffer size:

```sh
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c -o gnl
```

Bonus:

```sh
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line_bonus.c get_next_line_utils_bonus.c -o gnl_bonus
```

### Usage

Mandatory - read a file line by line:

```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "get_next_line.h"

int main(void)
{
    int  fd = open("test.txt", O_RDONLY);
    char *line;

    if (fd < 0)
        return (1);
    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);
    }
    close(fd);
    return (0);
}
```

Bonus - interleave reads from several file descriptors:

```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "get_next_line_bonus.h"

int main(void)
{
    int  fd1 = open("a.txt", O_RDONLY);
    int  fd2 = open("b.txt", O_RDONLY);
    char *l1;
    char *l2;

    while (1)
    {
        l1 = get_next_line(fd1);
        l2 = get_next_line(fd2);
        if (!l1 && !l2)
            break;
        if (l1) { printf("fd1: %s", l1); free(l1); }
        if (l2) { printf("fd2: %s", l2); free(l2); }
    }
    close(fd1);
    close(fd2);
    return (0);
}
```

## Algorithm & Technical choices

### Overall strategy

The function reads from the file descriptor in chunks of `BUFFER_SIZE` bytes and appends each chunk to a running string held in a static variable between calls. Reading stops as soon as the stash contains a \n, so we never over-read past the line we are about to return.

Three helpers implement this:

1. `read_to_string` - allocates a `BUFFER_SIZE + 1` buffer, loops over `read()`, joins each chunk with the current string via `ft_strjoin`, and exits the loop when either `ft_strchr(str, '\n')` returns a pointer or `read()` returns `0` (EOF) / `-1` (error). The buffer is freed before returning.

2. `extract_line` - scans the string up to the first `\n`, allocates exactly the needed size (`i + 2` if a `\n` is present, else `i + 1`), copies the line, and returns it as a freshly allocated string.

3. `update_string` - allocates a new string containing everything after the first `\n`, frees the old string, and returns the new one. When no `\n` is found (i.e. the file is exhausted), it frees the old string and returns `NULL`. 

`get_next_line` ties the three together: read -> extract -> update, with error and EOF paths resetting the static string so the next call starts clean.

### Why this design

- Read as little as possible per call. The read loop short-circuits on the first `\n`, satisfying the subject's requirement of not reading the whole file just to process one line.

- One static variable.
- - Mandatory - `static char *str;` - a single pointer.
- - Bonus - `static char *str[FD_MAX];` - a single array (an array of pointers is one variable). Indexing by fd gives each descriptor its own state, so interleaving `get_next_line(fd1)`, `get_next_line(fd2)`, `get_next_line(fd1)` works perfectly.

- Strict `malloc`/`free` discipline. Every allocation is paired with a matching `free`. The `free_and_return` helper frees both the stash and the temporary buffer on every error path inside `read_to_string`, preventing leaks when `read()` or `ft_strjoin()` fails.

- Buffer-size agnostic. Whether `BUFFER_SIZE` is 1, 42, 9999 or 10 000 000, the same code path is used. Small values only make the loop iterate more often, large values simply allocate a bigger temporary buffer. `BUFFER_SIZE <= 0` is rejected up front.

- `lseek()` is not used, no global variables. State lives purely inside the static string.

### Bonus - multiple file descriptors

Replacing the single stash with `static char *str[FD_MAX]` (where `FD_MAX` is the typical soft limit for fds on Linux, `4096`) is enough to support the bonus requirement: each call operates only on `str[fd]`. Calls on different fds therefore never interfere, and there is no need to maintain a linked list of `(fd, stash)` pairs - a single array covers all descriptors with O(1) access.

### Edge cases handled

- `fd < 0`, `fd >= FD_MAX`, or `BUFFER_SIZE <= 0` -> `NULL`.
- Empty file / EOF with an empty string -> `NULL`, and the string is freed and reset.
- File whose last line has no trailing `\n` -> the last chunk is still returned correctly.
- `read()` returning `-1` -> stash and temporary buffer are freed, `NULL` is returned, and the stash is reset for the next call.
- `malloc()` failure at any step -> the previously allocated memory is freed and `NULL` is returned.

## Resources

### Classic references

- `man 2 read`, `man 2 open`, `man 3 malloc`, `man 3 free`, `man 3 close`
- The 42 subject Get Next Line (version 14.3)
- Norminette documentation (42)
- *The C Programming Language*, B. Kernighan & D. Ritchie — pointers, arrays, and the static storage class
- Linux file descriptor limits: `ulimit -n`, `sysconf(_SC_OPEN_MAX)`
- General background on static vaiables and string-based parsing

### AI usage

AI was used as a support tool, never as a code generator, in line with the 42 AI guidelines. Its uses were:

1. Design discussion of the bonus constraint - clarifying that a single `static char *str[FD_MAX]` array counts as one static variable (as opposed to a linked list of (`fd, stash`) pairs), and discussing the trade-offs of each approach.
2. README drafting - structuring the Description / Instructions /
Resources / Algorithm sections required by the subject.

All code was written, understood, tested, and defended personally. AI did not produce the final implementation, as every suggestion was reviewed, adapted, and validated by running the project against the usual gnl test suites (mandatory and bonus, with several buffer sizes).