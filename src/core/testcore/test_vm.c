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
#include "test_vm.h"
#include "error_code.h"
#include "list.h"
#include "log.h"
#include "mm.h"
#include "test_frame.h"
#include "test_bc.h"

static int test_vm_run_bc(test_vm_s *vm, test_bc_s *bc)
{
    int ret;
    ret = test_bc_proc(bc);
    if (ret != EC_OK) {
        core_log("[test_vm] run bc failed, ret[%d]\n", ret);
    }
    return ret;
}

static int test_vm_run_bc_list(test_vm_s *vm, test_frame_s *frame)
{
    int i, ret;
    for (i = 0; i < frame->bc_num; i++) {
        ret = test_vm_run_bc(vm, &frame->bc_list[i]);
        if (ret != EC_OK) {
            core_log("[test_vm] run bc list failed, num[%d] ret[%d]\n", i, ret);
            return ret;
        }
    }
    return EC_OK;
}

static LIST_HEAD(g_vm_list);

int test_vm_run_frame(u8 core_id, test_frame_s *frame)
{
    int i, ret;
    test_vm_s *vm = NULL;
    list_for_each_entry(vm, &g_vm_list, node) {
        if (vm->core_id == core_id) {
            return test_vm_run_bc_list(vm, frame);
        }
    }
    core_log("[test_vm] run frame core_id[%u] not found\n", core_id);
    return EC_CORE_ID_INVALID;
}

int test_vm_init(u8 core_id, u8 frame_q_id)
{
    test_frame_callback_s frame_cb = {
        .run_frame = test_vm_run_frame,
    };
    test_vm_s *vm = NULL;
    int ret;

    vm = mm_malloc(sizeof(test_vm_s));
    if (vm == NULL) {
        core_log("[test_vm] init alloc vm failed, core_id[%u]\n", core_id);
        return EC_ALLOC_FAILED;
    }
    vm->core_id = core_id;
    list_add_tail(&vm->node, &g_vm_list);
    ret = test_frame_register(frame_q_id, core_id, &frame_cb);
    if (ret != EC_OK) {
        core_log("[test_vm] init register frame failed, core_id[%u]\n", core_id);
        return ret;
    }
    return EC_OK;
}

int test_vm_add(u8 core_id, u8 frame_q_id)
{
    test_vm_s *vm = NULL;
    int ret;

    list_for_each_entry(vm, &g_vm_list, node) {
        if (vm->core_id == core_id) {
            core_log("[test_vm] add core_id[%u] failed, already exist\n", core_id);
            return EC_CORE_ID_INVALID;
        }
    }
    ret = test_vm_init(core_id, frame_q_id);
    if (ret != EC_OK) {
        core_log("[test_vm] add core_id[%u] failed, ret[%d]\n", core_id, ret);
        return ret;
    }
    return EC_OK;
}

int test_vm_free(u8 core_id)
{
    test_vm_s *vm = NULL;
    test_vm_s *next = NULL;
    list_for_each_entry_safe(vm, next, &g_vm_list, node) {
        if (vm->core_id == core_id) {
            list_del(&vm->node);
            mm_free(vm);
            return EC_OK;
        }
    }
    core_log("[test_vm] free core_id[%u] not found\n", core_id);
    return EC_CORE_ID_INVALID;
}

void test_vm_free_all(void)
{
    test_vm_s *vm = NULL;
    test_vm_s *next = NULL;
    list_for_each_entry_safe(vm, next, &g_vm_list, node) {
        list_del(&vm->node);
        mm_free(vm);
    }
}