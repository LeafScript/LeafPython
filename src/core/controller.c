/*
 * MIT License
 *
 * Copyright (c) 2024 AZhao yuzhaowei2002@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "controller.h"
#include "leafpy_cfg.h"
#include "error.h"
#include "log.h"
#include "mm.h"
#include "test_parser.h"
#include "test_vm.h"
#include "test_core.h"
#include "test_frame.h"

int controller_init(void)
{
    int ret;
#if (ENABLE_CORE_TESTCORE == 1) && (LEAFPY_DEFAULT_CORE_TYPE == CORE_TYPE_TESTCORE)
    ret = test_vm_init(DEFAULT_CORE_ID);
    if (ret != EC_OK) {
        core_printf("[ctrl] test_vm init failed, ret[%d]\n", ret);
        return ret;
    }

    ret = test_core_init(DEFAULT_CORE_ID);
    if (ret != EC_OK) {
        core_printf("[ctrl] test_core init failed, ret[%d]\n", ret);
        return ret;
    }
#endif
    return EC_OK;
}

int controller_run_code(const char *code, u32 code_len, char *result, u32 result_len)
{
    if (code == NULL || code_len == 0 || result == NULL) {
        return EC_PARAM_INVALID;
    }

#if (ENABLE_CORE_TESTCORE == 1) && (LEAFPY_DEFAULT_CORE_TYPE == CORE_TYPE_TESTCORE)
    int ret;
    test_frame_s *frame = test_frame_init(1);
    if (frame == NULL) {
        core_printf("[ctrl] test_frame init failed\n");
        return EC_ALLOC_FAILED;
    }

    ret = test_parser_code_gen_frame(code, code_len, frame);
    if (ret != EC_OK) {
        core_printf("[ctrl] test_parser code gen frame failed, ret[%d]\n", ret);
        mm_free(frame->bc_list);
        mm_free(frame);
        return ret;
    }

    ret = test_vm_run_frame(DEFAULT_CORE_ID, frame);
    if (ret != EC_OK) {
        core_printf("[ctrl] test_vm run code frame failed, ret[%d]\n", ret);
        mm_free(frame->bc_list);
        mm_free(frame);
        return ret;
    }
#endif

    return EC_OK;
}

int controller_run_bytecode(const char *bytecode, u32 bytecode_len, char *result, u32 result_len)
{
    if (bytecode == NULL || bytecode_len == 0 || result == NULL) {
        return EC_PARAM_INVALID;
    }

#if (ENABLE_CORE_TESTCORE == 1) && (LEAFPY_DEFAULT_CORE_TYPE == CORE_TYPE_TESTCORE)
    int ret;
    test_frame_s *frame = test_frame_init(1);
    if (frame == NULL) {
        core_printf("[ctrl] test_frame init failed\n");
        return EC_ALLOC_FAILED;
    }

    ret = test_parser_bc_gen_frame(bytecode, bytecode_len, frame);
    if (ret != EC_OK) {
        core_printf("[ctrl] test_parser bc gen frame failed, ret[%d]\n", ret);
        mm_free(frame->bc_list);
        mm_free(frame);
        return ret;
    }

    ret = test_vm_run_frame(DEFAULT_CORE_ID, frame);
    if (ret != EC_OK) {
        core_printf("[ctrl] test_vm run bc frame failed, ret[%d]\n", ret);
        mm_free(frame->bc_list);
        mm_free(frame);
        return ret;
    }
#endif
    return EC_OK;
}

int controller_add_core(u8 core_id)
{
#if (ENABLE_CORE_TESTCORE == 1) && (LEAFPY_DEFAULT_CORE_TYPE == CORE_TYPE_TESTCORE)
    int ret;
    ret = test_vm_add(core_id);
    if (ret != EC_OK) {
        core_printf("[ctrl] test_vm add failed, ret[%d]\n", ret);
        goto error_exit;
    }

    ret = test_core_add(core_id);
    if (ret != EC_OK) {
        core_printf("[ctrl] test_core add failed, ret[%d]\n", ret);
        goto error_exit;
    }
    return EC_OK;

error_exit:
    if (ret == EC_ALLOC_FAILED) {
        test_vm_free(core_id);
        test_vm_free(core_id);
    }
    return ret;
#endif
}