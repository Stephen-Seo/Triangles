#include "state.hpp"

#ifdef _MSC_VER
#include "windows.h"
#include <cstdlib>
#include <cstring>

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
    int argvCapacity = 16;
    int argc = 0;
    char **argv = (char**)calloc(argvCapacity, sizeof(char*));
    int argLength = std::strlen(lpCmdLine);
    int tempCapacity = 128;
    int tempSize = 0;
    char *temp = (char*)malloc(sizeof(char) * tempCapacity);
    for (int i = 0; i < argLength; ++i) {
        if (lpCmdLine[i] == ' ') {
            if (tempSize > 0) {
                if (argc >= argvCapacity) {
                    char **newArgv = (char**)calloc(argvCapacity * 2, sizeof(char*));
                    std::memcpy(newArgv, argv, sizeof(char*) * argvCapacity);
                    free(argv);
                    argv = newArgv;
                    argvCapacity *= 2;
                }
                argv[argc] = (char*)malloc(sizeof(char) * (tempSize + 1));
                std::memcpy(argv[argc], temp, sizeof(char) * tempSize);
                argv[argc][tempSize] = 0;
                ++argc;
                tempSize = 0;
            }
        } else {
            if (tempSize >= tempCapacity) {
                char *newTemp = (char*)malloc(sizeof(char) * tempCapacity * 2);
                std::memcpy(newTemp, temp, sizeof(char) * tempSize);
                free(temp);
                temp = newTemp;
                tempCapacity *= 2;
            }
            temp[tempSize++] = lpCmdLine[i];
        }
    }
    if (tempSize > 0) {
        if (argc >= argvCapacity) {
            char **newArgv = (char**)calloc(argvCapacity * 2, sizeof(char*));
            std::memcpy(newArgv, argv, sizeof(char*) * argvCapacity);
            free(argv);
            argv = newArgv;
            argvCapacity *= 2;
        }
        argv[argc] = (char*)malloc(sizeof(char) * (tempSize + 1));
        std::memcpy(argv[argc], temp, sizeof(char) * tempSize);
        argv[argc][tempSize] = 0;
        ++argc;
        tempSize = 0;
    }
    free(temp);

    // init
    Tri::State state(argc, argv);

    // main loop
    while(state.get_flags().test(1)) {
        state.handle_events();
        state.update();
        state.draw();
    }

    // cleanup

    for (int i = 0; i < argc; ++i) {
        free(argv[i]);
    }
    free(argv);

    return 0;
}
#else
int main(int argc, char **argv) {
    // init
    Tri::State state(argc, argv);

    // main loop
    while(state.get_flags().test(1)) {
        state.handle_events();
        state.update();
        state.draw();
    }

    return 0;
}
#endif
