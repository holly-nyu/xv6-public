#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define NULL ((void*)0)
#define MAX_DEFS 10
#define MAX_LINE_LEN 1024

typedef struct {
    char *var;
    char *val;
} Definition;

// Custom implementation of strlen
int my_strlen(const char *s) {
    int len = 0;
    while (*s++) len++;
    return len;
}

// Custom implementation of strncmp
int my_strncmp(const char *s1, const char *s2, int n) {
    while (n-- > 0) {
        if (*s1 != *s2) {
            return *(unsigned char *)s1 - *(unsigned char *)s2;
        }
        if (*s1 == '\0') {
            return 0;
        }
        s1++;
        s2++;
    }
    return 0;
}

// Custom implementation of strchr
char *my_strchr(const char *s, char c) {
    while (*s) {
        if (*s == c) return (char *)s;
        s++;
    }
    return NULL;
}

// Custom implementation of strstr
char *my_strstr(const char *haystack, const char *needle) {
    int needle_len = my_strlen(needle);
    for (; *haystack; haystack++) {
        if (my_strncmp(haystack, needle, needle_len) == 0) {
            return (char *)haystack;
        }
    }
    return NULL;
}

// Custom implementation of memmove
void *my_memmove(void *dest, const void *src, int n) {
    char *d = dest;
    const char *s = src;
    if (d < s) {
        while (n--) {
            *d++ = *s++;
        }
    } else {
        const char *last_s = s + (n - 1);
        char *last_d = d + (n - 1);
        while (n--) {
            *last_d-- = *last_s--;
        }
    }
    return dest;
}

// Substitute variables in the line
void substitute(char *line, Definition defs[], int def_count) {
    for (int i = 0; i < def_count; i++) {
        char *pos;
        while ((pos = my_strstr(line, defs[i].var)) != NULL) {
            int len_before = pos - line;
            int var_len = my_strlen(defs[i].var);
            int val_len = my_strlen(defs[i].val);

            char new_line[MAX_LINE_LEN];
            my_memmove(new_line, line, len_before); // Copy up to the match
            my_memmove(new_line + len_before, defs[i].val, val_len); // Copy the value
            my_memmove(new_line + len_before + val_len, pos + var_len, my_strlen(pos + var_len) + 1); // Copy the rest

            my_memmove(line, new_line, my_strlen(new_line) + 1); // Update original line
        }
    }
}

// Parse command line definitions
int parse_definitions(int argc, char *argv[], Definition defs[]) {
    int def_count = 0;
    for (int i = 2; i < argc; i++) {
        if (my_strncmp(argv[i], "-D", 2) == 0) {
            char *def = argv[i] + 2;
            char *equal_sign = my_strchr(def, '=');
            if (equal_sign) {
                *equal_sign = '\0';  // Split the string
                defs[def_count].var = def;
                defs[def_count].val = equal_sign + 1;
                def_count++;
            }
        }
    }
    return def_count;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(2, "Usage: %s <input_file> -D<var>=<val> ...\n", argv[0]);
        exit();
    }

    // Open the input file
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        printf(2, "Error opening file %s\n", argv[1]);
        exit();
    }

    Definition defs[MAX_DEFS];
    int def_count = parse_definitions(argc, argv, defs);

    char line[MAX_LINE_LEN];
    int n;
    
    while ((n = read(fd, line, sizeof(line) - 1)) > 0) {
        line[n] = '\0';  // Null-terminate
        substitute(line, defs, def_count);  // Replace variables with values
        printf(1, "%s", line);  // Print the modified line
    }

    if (n < 0) {
        printf(2, "Error reading file\n");
    }

    close(fd);
    exit();
}
