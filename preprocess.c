#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define MAX_DEFS 10
#define MAX_VAR_LEN 32
#define MAX_VAL_LEN 128
#define MAX_LINE_LEN 1024

typedef struct {
    char var[MAX_VAR_LEN];
    char val[MAX_VAL_LEN];
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

// Custom implementation of strncpy
char *my_strncpy(char *dest, const char *src, int n) {
    int i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

// Parse command line definitions
int parse_definitions(int argc, char *argv[], Definition defs[]) {
    int def_count = 0;
    for (int i = 2; i < argc && def_count < MAX_DEFS; i++) {
        if (my_strncmp(argv[i], "-D", 2) == 0) {
            char *def = argv[i] + 2;
            char *equal_sign = strchr(def, '=');
            if (equal_sign) {
                int var_len = equal_sign - def;
                if (var_len < MAX_VAR_LEN) {
                    my_strncpy(defs[def_count].var, def, var_len);
                    defs[def_count].var[var_len] = '\0';
                    
                    // Copy the value, including spaces
                    int val_len = 0;
                    char *val_start = equal_sign + 1;
                    while (val_start[val_len] != '\0' && val_len < MAX_VAL_LEN - 1) {
                        defs[def_count].val[val_len] = val_start[val_len];
                        val_len++;
                    }
                    defs[def_count].val[val_len] = '\0';
                    
                    // If the value continues in the next argument
                    while (i + 1 < argc && argv[i + 1][0] != '-') {
                        i++;
                        if (val_len < MAX_VAL_LEN - 1) {
                            defs[def_count].val[val_len++] = ' ';
                        }
                        char *next_val = argv[i];
                        while (*next_val != '\0' && val_len < MAX_VAL_LEN - 1) {
                            defs[def_count].val[val_len++] = *next_val++;
                        }
                        defs[def_count].val[val_len] = '\0';
                    }
                    
                    def_count++;
                }
            }
        }
    }
    return def_count;
}

// Substitute variables in the line
void substitute(char *line, Definition defs[], int def_count) {
    char new_line[MAX_LINE_LEN];
    int new_line_index = 0;
    int line_len = my_strlen(line);

    for (int i = 0; i < line_len && new_line_index < MAX_LINE_LEN - 1; i++) {
        int substituted = 0;
        for (int j = 0; j < def_count; j++) {
            int var_len = my_strlen(defs[j].var);
            if (my_strncmp(&line[i], defs[j].var, var_len) == 0) {
                // Check if it's a whole word match
                if ((i == 0 || !((line[i-1] >= 'a' && line[i-1] <= 'z') || 
                                 (line[i-1] >= 'A' && line[i-1] <= 'Z') || 
                                 (line[i-1] >= '0' && line[i-1] <= '9') || 
                                 line[i-1] == '_')) &&
                    (line[i+var_len] == '\0' || !((line[i+var_len] >= 'a' && line[i+var_len] <= 'z') || 
                                                  (line[i+var_len] >= 'A' && line[i+var_len] <= 'Z') || 
                                                  (line[i+var_len] >= '0' && line[i+var_len] <= '9') || 
                                                  line[i+var_len] == '_'))) {
                    int val_len = my_strlen(defs[j].val);
                    if (new_line_index + val_len < MAX_LINE_LEN - 1) {
                        my_strncpy(&new_line[new_line_index], defs[j].val, val_len);
                        new_line_index += val_len;
                        i += var_len - 1;  // -1 because the outer loop will increment i
                        substituted = 1;
                        break;
                    }
                }
            }
        }
        if (!substituted && new_line_index < MAX_LINE_LEN - 1) {
            new_line[new_line_index++] = line[i];
        }
    }
    new_line[new_line_index] = '\0';
    my_strncpy(line, new_line, MAX_LINE_LEN - 1);
    line[MAX_LINE_LEN - 1] = '\0';
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

    // Debug print
    for (int i = 0; i < def_count; i++) {
        printf(2, "Definition %d: %s = %s\n", i, defs[i].var, defs[i].val);
    }

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