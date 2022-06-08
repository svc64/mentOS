#include <stdlib.h>
#include <stdbool.h>
#include <io.h>
#include <files.h>
#include <errors.h>

bool r = false;
bool f = false;

void *malloc_noerror(size_t size) {
    void *ret = malloc(size);
    if (!ret) {
        print("allocation of size %d failed!\n", size);
        exit(1);
    }
    return ret;
}

void *realloc_noerror(void *ptr, size_t size) {
    void *ret = realloc(ptr, size);
    if (!ret) {
        print("reallocation to size %d failed!\n", size);
        exit(1);
    }
    return ret;
}

char *join_paths(const char *p1, const char *p2) {
    size_t p1_len = strlen(p1);
    char *new_str = malloc_noerror(p1_len + strlen(p2) + 2);
    strcpy(new_str, p1);
    if (p1[p1_len - 1] != '/' && *p2 != '/') {
        strcat(new_str, "/");
    }
    strcat(new_str, p2);
    return new_str;
}

void rm_file(char *path) {
    int err = unlink(path);
    if (err < 0) {
        if (!f) {
            print("%s: %s\n", err_str(err), path);
        }
    }
}

void rmtree(char *path) {
    int dir = opendir(path);
    if (dir < 0) {
        print("%s: %s\n", err_str(dir), path);
        return;
    }
    struct dirent ent;
    int err = 0;
    while (err != E_OOB) {
        err = read_dir(dir, &ent);
        if (err < 0 && err != E_OOB) {
            closedir(dir);
            print("%s: %s\n", err_str(dir), path);
            return;
        } else if (err != E_OOB) {
            if (ent.type == DT_DIR) {
                rmtree(join_paths(path, ent.name));
            } else {
                rm_file(join_paths(path, ent.name));
            }
        }
    }
    rm_file(path);
    closedir(dir);
}

int main(int argc, const char *argv[]) {
    char *ents[argc];
    size_t ent_count = 0;
    if (argc < 2) {
        print("usage:\nrm [file]\nrm [dir]\n");
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        if (*(argv[i]) == '-' && *(argv[i+1])) {
            char *args = argv[i] + 1;
            for (int x = 0; x < strlen(args); x++) {
                switch (args[x]) {
                    case 'r':
                        r = true;
                        break;
                    case 'f':
                        f = true;
                        break;
                    default:
                        print("unknown argument: %c\n", args[x]);
                        return 1;
                }
            }
        } else {
            ents[ent_count++] = argv[i];
        }
    }
    struct stat s; 
    for (int i = 0; i < ent_count; i++) {
        if (r) {
            int err = stat(ents[i], &s);
            if (err < 0) {
                if (!f) {
                    print("%s: %s\n", err_str(err), ents[i]);
                }
                continue;
            }
            if (s.type == DT_DIR) {
                rmtree(ents[i]);
                rm_file(ents[i]);
            } else {
                rm_file(ents[i]);
            }
        } else {
            int err = stat(ents[i], &s);
            if (err < 0) {
                if (!f) {
                    print("%s: %s\n", err_str(err), ents[i]);
                }
                continue;
            }
            if (s.type == DT_DIR) {
                print("%s: Is a directory\n", ents[i]);
            } else {
                rm_file(ents[i]);
            }
        }
    }
    return 0;
}
