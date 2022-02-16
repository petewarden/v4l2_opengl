#ifndef INCLUDE_CAPTURE_MAIN_H
#define INCLUDE_CAPTURE_MAIN_H

#include <stdbool.h>
#include <stdint.h>

void *capture_main(void *cookie);

bool get_latest_capture(int *width, int *height, uint8_t **rgba_buffer);

#endif // INCLUDE_CAPTURE_MAIN_H