#include <io.h>
#include <files.h>
#include <errors.h>

char *app_name = "strfile";

int main(int argc, const char *argv[]) {
    if (argc != 3) {
        if (argc) {
            app_name = argv[0];
        }
        print("usage: %s [file1] [string]\n", app_name);
        return 1;
    }
    char *filename = argv[1];
    char *str = argv[2];
    int fd = open(filename, O_CREATE | O_WRITE);
    if (fd < 0) {
        print("%s: %s\n", err_str(fd), filename);
        return 1;
    }
    size_t str_size = strlen(str) + 1;
    ssize_t written = write(fd, str, str_size);
    if (written < 0) {
        close(fd);
        print("%s: %s\n", err_str(written), filename);
        return 1;
    }
    if (written != str_size) {
        print("warning: wrote %d bytes instead of %d bytes, out of space?\n", written, str_size);
    }
    close(fd);
    return 0;
}
