#include <stddef.h>
#include <stdint.h>
#include "stdlib.h"
#include "alloc.h"
#include "exceptions.h"

void add_component(char *component, size_t len, char ***out, size_t count) {
    if (!(*out)) {
        if (count != 1) {
            panic("add_component: count != 1");
        }
        *out = malloc(count * sizeof(char *));
        if (!(*out)) {
            return;
        }
    } else {
        *out = realloc(*out, count * sizeof(char *));
        if (!(*out)) {
            return;
        }
    }
    (*out)[count - 1] = malloc(len + 1);
    if (!((*out)[count - 1])) {
        for (int i = 0; i < count - 1; i++) {
            free((*out)[i]);
        }
        free(*out);
        *out = NULL;
        return;
    }
    memcpy((*out)[count - 1], component, len);
    ((*out)[count - 1])[len] = '\0';
}

int split_path(char *path, char ***out, size_t *count) {
    *count = 0;
    char *c = path;
    while (*c != '\0') {
        if (*c != '/') {
            char *component = c;
            (*count)++;
            while (*c != '/' && *c != '\0') {
                c++;
            }
            size_t component_size = (size_t)((uintptr_t)c - (uintptr_t)component);
            add_component(component, component_size, out, *count);
            if (!(*out)) {
                // out of memory
                return -1;
            }
        }
        if (*c == '/') {
            (*count)++;
            add_component("/", 1, out, *count);
        }
        while (*c == '/' && *c != '\0') {
            c++;
        }
    }
    return 0;
}

char *join_paths(const char *p1, const char *p2) {
    size_t p1_len = strlen(p1);
    char *new_str = malloc(p1_len + strlen(p2) + 2);
    if (!new_str) {
        return NULL;
    }
    strcpy(new_str, p1);
    if (p1[p1_len - 1] != '/' && *p2 != '/') {
        strcat(new_str, "/");
    }
    strcat(new_str, p2);
    return new_str;
}

void free_split_path(char **out, size_t count) {
    for (int i = 0; i < count; i++) {
        free(out[i]);
    }
}

char *remove_last_component(char *path) {
    char **split = NULL;
    size_t components;
    if (!split_path(path, &split, &components) && !components) {
        char *sanitized_path = malloc(2);
        sanitized_path[0] = '/';
        sanitized_path[1] = '\0';
        return sanitized_path;
    }
    char *sanitized_path = malloc(1);
    if (!sanitized_path) {
        free_split_path(split, components);
        return NULL;
    }
    sanitized_path[0] = '\0';
    for (int i = 0; i < components - 1; i++) {
        if (*(split[i]) != '/') {
            char *new_sanitized_path = join_paths(sanitized_path, split[i]);
            if (!new_sanitized_path) {
                free(sanitized_path);
                free_split_path(split, components);
                return NULL;
            }
            free(sanitized_path);
            sanitized_path = new_sanitized_path;
        }
    }
    free_split_path(split, components);
    return sanitized_path;
}

char *sanitize_path(char *path, char *cwd) {
    char **split = NULL;
    size_t components;
    split_path(path, &split, &components);
    if (!split) {
        return NULL;
    }
    if (!strcmp(split[0], ".")) {
        free(split[0]);
        size_t current_path_len = strlen(cwd) + 1;
        char *current_path = malloc(current_path_len);
        strcpy(current_path, cwd);
        split[0] = current_path;
    }
    char *sanitized_path = malloc(1);
    if (!sanitized_path) {
        free_split_path(split, components);
        return NULL;
    }
    sanitized_path[0] = '\0';
    for (int i = 0; i < components; i++) {
        if (*(split[i]) != '/') {
            if (!strcmp(split[i], "..")) {
                char *new_sanitized_path = remove_last_component(sanitized_path);
                free(sanitized_path);
                sanitized_path = new_sanitized_path;
            } else {
                char *new_sanitized_path = join_paths(sanitized_path, split[i]);
                free(sanitized_path);
                sanitized_path = new_sanitized_path;
            }
        }
    }
    free_split_path(split, components);
    if (!strlen(sanitized_path)) {
        char *slash = realloc(sanitized_path, sizeof("/"));
        if (!slash) {
            free(sanitized_path);
            return NULL;
        }
        strcpy(slash, "/");
        sanitized_path = slash;
    }
    return sanitized_path;
}
