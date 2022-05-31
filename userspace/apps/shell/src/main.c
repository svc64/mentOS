#include <stdlib.h>
#include <io.h>
#include <stdbool.h>
#include <proc.h>

#define INPUT_SIZE 4096

int main(int argc, const char *argv[]) {
    print("Hi!\n");
    exec("/exec_me", false);
    char *input = malloc(INPUT_SIZE);
    if (!input) {
        print("failed to allocate memory for input!\n");
        exit(1);
    }
    size_t read = input_read(input, sizeof(input) - 1);
    input[read] = '\0';
    print("Read input: %s\n", input);
    free(input);
    exit(0);
    return 4141;
}
