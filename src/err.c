/**
 * err.c
 *
 * Contains a formal API to create, initialize, get, reset, and free errors
 * among different backends.
 */

#include "err.h"

#include "assert.h"
#include "redismodule.h"
#include "stdlib.h"
#include "string.h"

char *RAI_Chomp(const char *src) {
    char *str = RedisModule_Strdup(src);
    size_t len = strlen(src);
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '\n' || str[i] == '\r') {
            str[i] = ' ';
        }
    }
    return str;
}

const char *RAI_GetError(RAI_Error *err) { return err->detail; }

const char *RAI_GetErrorOneLine(RAI_Error *err) { return err->detail_oneline; }

RAI_ErrorCode RAI_GetErrorCode(RAI_Error *err) { return err->code; }

void RAI_SetError(RAI_Error *err, RAI_ErrorCode code, const char *detail) {
    if (!err) {
        return;
    }
    if (err->code != RAI_OK) {
        return;
    }
    assert(!err->detail);
    err->code = code;

    if (detail) {
        err->detail = RedisModule_Strdup(detail);
    } else {
        err->detail = RedisModule_Strdup("ERR Generic error");
    }
    err->detail_oneline = RAI_Chomp(err->detail);
}

/**
 * Allocate the memory and initialise the RAI_Error.
 * @param result Output parameter to capture allocated RAI_Error.
 * @return 0 on success, or 1 if the allocation
 * failed.
 */
int RAI_InitError(RAI_Error **result) {
    RAI_Error *err;
    err = (RAI_Error *)RedisModule_Calloc(1, sizeof(RAI_Error));
    err->code = 0;
    err->detail = NULL;
    err->detail_oneline = NULL;
    *result = err;
    return REDISMODULE_OK;
}

void RAI_ClearError(RAI_Error *err) {
    if (err) {
        if (err->detail) {
            RedisModule_Free(err->detail);
            err->detail = NULL;
        }
        if (err->detail_oneline) {
            RedisModule_Free(err->detail_oneline);
            err->detail_oneline = NULL;
        }
        err->code = RAI_OK;
    }
}

void RAI_FreeError(RAI_Error *err) {
    if (err) {
        RAI_ClearError(err);
        RedisModule_Free(err);
    }
}
