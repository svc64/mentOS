#include <files.h>
#include <io.h>
#include <errors.h>

char *app_name = "mkdir";

int make_dir(char *path) {
    int err = mkdir(path);
    if (err < 0) {
        char *str = err_str(err);
        if (str) {
            print("%s: %s\n", str, path);
        } else {
            print("Unknown error %d: %s\n", err, path);
        }
        return 1;
    }
    return 0;
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        if (argc) {
            app_name = argv[0];
        }
        print("usage: %s [dir1] [dir2] [dir3] ...\n", app_name);
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        int ret = make_dir(argv[i]);
        if (ret) {
            return ret;
        }
    }
    return 0;
}
