#include <io.h>
#include <stdbool.h>
int main(int argc, const char *argv[]) {
    print("Hi!\n");
    char input[400];
    while (true) {
        size_t read = input_read(input, sizeof(input) - 1);
        input[read] = '\0';
        print("Read input: %s\n", input);
    }
    return 4141;
}
