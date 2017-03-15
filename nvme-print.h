#ifndef NVME_PRINT_H
#define NVME_PRINT_H

#include <inttypes.h>
#include "program.h"
#include "nvme.h"

#define max(x, y) (x) > (y) ? (x) : (y)

void print_nvme_status(const char *cmd, int status);
void print_nvme_feature(const char *cmd, __u32 feature_id, __u32 result);

#define CREATE_TSTRUCT
#include "nvme-tstruct.h"

#endif
