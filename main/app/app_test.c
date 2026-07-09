#include <stdio.h>

#include "app.h"

#define APP_NAME test

REG_APP {
    printf(params[0]);
}
