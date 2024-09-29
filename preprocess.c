#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "param.h"
#include "syscall.h"

#define NULL ((void*)0)
#define MAX_DEFS 10
#define MAX_LINE_LEN 4096
#define INITIAL_BUFFER_SIZE 8192
#define BUFFER_GROWTH_FACTOR 2
#define PGSIZE 4096
#define PRINT_CHUNK_SIZE 256


typedef struct {
    char *var;
    char *val;
} Definition;

char* my_sbrk(int n) {
    return sbrk(n);
}

void* my_malloc(uint size) {
    char* p = my_sbrk(size);
    if (p == (char*)-1)
        return 0;
    return p;
}

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

int isalnum(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}


// Custom implementation of strstr
char *my_strstr(const char *haystack, const char *needle) {
    int needle_len = my_strlen(needle);
    for (; *haystack; haystack++) {
        if ((!haystack || !isalnum(*(haystack-1))) &&
            (my_strncmp(haystack, needle, needle_len) == 0) &&
            (!*(haystack + needle_len) || !isalnum(*(haystack + needle_len)))) {
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

void substitute(char **buffer_ptr, Definition defs[], int def_count, int *total_size, int *buffer_size) {
    char *buffer = *buffer_ptr;
    for (int i = 0; i < def_count; i++) {
        char *pos = buffer;
        while ((pos = my_strstr(pos, defs[i].var)) != NULL) {
            int var_len = my_strlen(defs[i].var);
            int val_len = my_strlen(defs[i].val);
            int rest_len = my_strlen(pos + var_len);

            // Check if buffer needs to grow
            if (*total_size + (val_len - var_len) > *buffer_size) {
                *buffer_size *= BUFFER_GROWTH_FACTOR;
                char *new_buffer = my_malloc(*buffer_size);
                if (!new_buffer) {
                    printf(2, "Memory allocation failed during substitution\n");
                    exit();
                }
                my_memmove(new_buffer, buffer, *total_size);
                pos = new_buffer + (pos - buffer);  // Update pos to point to the new buffer
                buffer = new_buffer;
                *buffer_ptr = buffer;  // Update the original buffer pointer
            }

            if (val_len != var_len) {
                my_memmove(pos + val_len, pos + var_len, rest_len + 1);
            }
            my_memmove(pos, defs[i].val, val_len);
            pos += val_len;
            *total_size += (val_len - var_len);
        }
    }
}

// Parse command line definitions
int parse_definitions(int argc, char *argv[], Definition defs[]) {
    int def_count = 0;
    for (int i = 2; i < argc && def_count < MAX_DEFS; i++) {
        if (my_strncmp(argv[i], "-D", 2) == 0) {
            char *def = argv[i] + 2;
            char *equal_sign = my_strchr(def, '=');
            if (equal_sign) {
                *equal_sign = '\0';  // Split the string
                defs[def_count].var = def;
                defs[def_count].val = equal_sign + 1;
                
                // Remove quotes if present
                int len = my_strlen(defs[def_count].val);
                if (len > 1 && defs[def_count].val[0] == '"' && defs[def_count].val[len-1] == '"') {
                    defs[def_count].val[len-1] = '\0';
                    defs[def_count].val++;
                }
                
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

    char *buffer = my_malloc(INITIAL_BUFFER_SIZE);
    if (!buffer) {
        printf(2, "Initial memory allocation failed\n");
        close(fd);
        exit();
    }

    int buffer_size = INITIAL_BUFFER_SIZE;
    int total_read = 0;
    int n;

    while ((n = read(fd, buffer + total_read, buffer_size - total_read)) > 0) {
        total_read += n;
        if (total_read + PRINT_CHUNK_SIZE > buffer_size) {
            buffer_size *= BUFFER_GROWTH_FACTOR;
            char *new_buffer = my_malloc(buffer_size);
            if (!new_buffer) {
                printf(2, "Memory allocation failed\n");
                close(fd);
                exit();
            }
            my_memmove(new_buffer, buffer, total_read);
            buffer = new_buffer;
        }
    }

    if (n < 0) {
        printf(2, "Error reading file\n");
        close(fd);
        exit();
    }

    buffer[total_read] = '\0';  // Null-terminate the entire input

    // Process the entire buffer
    substitute(&buffer, defs, def_count, &total_read, &buffer_size);    
    // Debug output
    // printf(2, "Total bytes read: %d\n", total_read);

    // Print the processed content in one go
    int printed = 0;
    while (printed < total_read) {
        n = write(1, buffer + printed, total_read - printed);
        if (n <= 0) {
            printf(2, "Error writing output\n");
            break;
        }
        printed += n;
    }

    // Debug output
    // printf(2, "\nTotal bytes printed: %d\n", printed);

    close(fd);
    exit();
}