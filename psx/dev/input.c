#include "input.h"

#include <string.h>
#include <stdlib.h>

psx_input_t* psx_input_create(void) {
    return (psx_input_t*)malloc(sizeof(psx_input_t));
}

void psx_input_init(psx_input_t* input) {
    memset(input, 0, sizeof(psx_input_t));
}

void psx_input_destroy(psx_input_t* input) {
    free(input);
}
