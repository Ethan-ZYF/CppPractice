#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void errif(bool cond, const char* err_msg) {
    if (cond) {
        perror(err_msg);
        exit(EXIT_FAILURE);
    }
}