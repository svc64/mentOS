#include <stdlib.h>
#include <stddef.h>
#include <io.h>
#include <files.h>
#include <errors.h>

#define DEFAULT_BUFFER_SIZE 4096
char *app_name = "cat";

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        if (argc) {
            app_name = argv[0];
        }
        print("usage: %s [file1] [file2] ...\n", app_name);
        return 1;
    }
    uint8_t *buf = malloc(DEFAULT_BUFFER_SIZE);
    if (!buf) {
        print("failed to allocate %d bytes for file reading\n", DEFAULT_BUFFER_SIZE);
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        char *filename = argv[i];
        int fd = open(filename, O_READ);
        if (fd < 0) {
            print("%s: %s\n", err_str(fd), filename);
            return 1;
        }
        ssize_t bytes_read;
        do {
            bytes_read = read(fd, buf, DEFAULT_BUFFER_SIZE);
            if (bytes_read < 0) {
                print("%s: %s\n", err_str(bytes_read), filename);
                return 1;
            }
            for (int x = 0; x < bytes_read; x++) {
                putc(buf[x]);
            }
        } while (bytes_read > 0);
        close(fd);
    }
    return 0;
}
