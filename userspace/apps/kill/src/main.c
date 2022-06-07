#include <stdlib.h>
#include <io.h>
#include <signal.h>
#include <errors.h>

char *app_name = "kill";

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        if (argc) {
            app_name = argv[0];
        }
        print("usage:\n");
        print("%s <signal> [pid]\n", app_name);
        print("%s [pid]\n", app_name);
        return 1;
    }
    int signal = SIGKILL;
    for (int i = 1; i < argc; i++) {
        if ((*argv[i]) == '-') {
            if (i == 1) {
                signal = atoi(argv[i]);
            } else {
                print("invalid PID: %d\n", argv[i]);
                return 1;
            }
        } else {
            int pid = atoi(argv[i]);
            int err = kill(pid, signal);
            if (err < 0) {
                switch (err) {
                    case E_NXPROC:
                        print("No such process: %d\n", pid);
                        break;
                    case E_PARAM:
                        print("Invalid signal: %d\n", signal);
                        break;
                    default:
                        print("Unknown error: %s\n", err_str(err));
                        break;
                }
                return 1;
            }
        }
    }
    return 0;
}
