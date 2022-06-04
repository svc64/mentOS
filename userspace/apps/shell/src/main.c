#include <stdlib.h>
#include <io.h>
#include <stdbool.h>
#include <stdint.h>
#include <proc.h>
#include "cmd.h"
#include "alloc.h"

// str MUST begin with `q`, else bad things will go down
// (caller will make sure of that anyways)
// returns NULL when the quotation mark is unterminated
// len: length of the quotation string (str) til it's terminated
char *read_quotation_str(char *str, char q, size_t *len) {
    *len = 0;
    size_t out_len = 0;
    char *out = malloc_noerror(out_len + 1);
    size_t str_len = strlen(str);
    bool terminated = false;
    for (int i = 1; i < str_len; i++) {
        if (str[i] == '\\') { // '\'
            // does the next character need to be escaped?
            if ((i + 1) < str_len) {
                if (str[i + 1] == '\\' || str[i + 1] == q) {
                    i++;
                    (*len)++;
                    out[out_len] = str[i];
                    out_len++;
                    out = realloc_noerror(out, out_len + 1);
                } else {
                    out[out_len] = str[i];
                    out_len++;
                    out = realloc_noerror(out, out_len + 1);
                }
            } else {
                free(out);
                return NULL; // unterminated
            }
        } else if (str[i] == q) {
            terminated = true;
            break;
        } else {
            out[out_len] = str[i];
            out_len++;
            out = realloc_noerror(out, out_len + 1);
        }
        (*len)++;
    }
    if (!terminated) {
        // unterminated quotation mark :(
        free(out);
        return NULL;
    }
    out[out_len] = '\0';
    (*len) += 2;
    return out;
}

void parse_run_cmd(char *cmdline) {
    struct command *cmd = cmd_new();
    // skip all spaces
    while (*cmdline == ' ') {
        cmdline++;
    }
    char *cmdline_original = cmdline;
    // while we didn't hit a null
    while (*cmdline != '\0') {
        // skip all spaces
        while (*cmdline == ' ') {
            cmdline++;
        }
        char *c = cmdline;
        size_t arg_str_len = 0;
        char *arg_str = malloc_noerror(arg_str_len + 1);
        // go to the end of this part (space / '&' / null)
        while (*c != ' ' && *c != '\0' && *c != '&') {
            if (*c == '"' || *c == '\'') {
                size_t q_len = 0;
                char *quote_str = read_quotation_str(c, *c, &q_len);
                if (!quote_str) {
                    // unterminated quotation mark
                    uintptr_t err_idx = c - cmdline_original;
                    print("unterminated quotation mark (\" %c \"):\n", *c);
                    print("%s\n", cmdline_original);
                    for (int i = 0; i < err_idx; i++) {
                        print(" ");
                    }
                    print("^\n");
                    return;
                }
                arg_str[arg_str_len] = '\0';
                arg_str_len += q_len;
                arg_str = realloc_noerror(arg_str, arg_str_len + 1);
                strcpy(arg_str + strlen(arg_str), quote_str);
                free(quote_str);
                c += q_len;
            } else {
                arg_str_len++;
                arg_str = realloc_noerror(arg_str, arg_str_len + 1);
                arg_str[arg_str_len - 1] = *c;
                c++;
            }
        }
        arg_str[arg_str_len] = '\0';
        if (*c == '\0') {
            cmd_add_arg(cmd, arg_str);
            free(arg_str);
            break;
        }
        cmd_add_arg(cmd, arg_str);
        free(arg_str);
        cmdline = c;
        // skip spaces
        while (*cmdline == ' ') {
            cmdline++;
        }
        // check for & or &&
        if (cmdline[0] == '&') {
            char *cmd_end = cmdline;
            while (*cmd_end != ' ' && *cmd_end != '\0' && *cmd_end != '&') {
                cmd_end++;
            }
            // skip spaces
            int and_count = 0;
            while ((*cmd_end == ' ' || *cmd_end == '&') && and_count <= 2) {
                if (*cmd_end == '&') {
                    and_count++;
                }
                cmd_end++;
            }
            if (and_count > 2 || (*cmd_end == '\0'  && and_count >= 2)) {
                uintptr_t err_idx = cmd_end - cmdline_original;
                if (and_count > 2) {
                    print("unexpected token \"&\":\n");
                    err_idx += 2;
                } else {
                    print("unexpected \"&\" token(s):\n");
                    and_count = 1;
                }
                print("%s\n", cmdline_original);
                for (int i = 0; i < err_idx - and_count; i++) {
                    print(" ");
                }
                print("^");
                print("\n");
                return;
            }
            if (cmdline[1] == '&') {
                cmd->background = false;
            } else {
                cmd->background = true;
            }
            cmd_run(cmd);
            cmd_free(cmd);
            cmd = cmd_new();
            cmdline += and_count;
        }
    }
    if (cmd->executable) {
        cmd_run(cmd);
    }
    cmd_free(cmd);
}

char *get_input(void) {
    size_t idx = 0;
    size_t alloc = 16;
    char *str = malloc_noerror(alloc);
    char c;
    do {
        size_t len = input_read(&c, sizeof(c));
        if (idx >= alloc) {
            alloc += 16;
        }
        char *new_str = realloc_noerror(str, alloc);
        str = new_str;
        str[idx++] = c;
    } while (c != '\n');
    str[idx - 1] = '\0';
    return str;
}

int main(int argc, const char * argv[]) {
    block_sigint();
    print("hi from shell!\n");
    while (true) {
        print("$ ");
        char *input = get_input();
        if (strlen(input)) {
            parse_run_cmd(input);
        }
        free(input);
    }
    return 0;
}
