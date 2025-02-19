/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "param_linker_section.h"

// 获取section的首位地址
#if defined(__ARMCC_VERSION) /* ARM C Compiler */
// 如果不在sct文件定义section，则根据代码自动创建
extern const int     NextpilotParamTab$$Base;
extern const int     NextpilotParamTab$$Limit;
static param_info_t *__param_section_start_ptr = (param_info_t *)&NextpilotParamTab$$Base;
static param_info_t *__param_section_end_ptr   = (param_info_t *)&NextpilotParamTab$$Limit;
static param_info_t *__param_table__           = nullptr;
static uint16_t      __param_count__           = UINT16_MAX; // NextpilotParamTab$$Length
#elif defined(__ICCARM__) || defined(__ICCRX__)              /* for IAR Compiler */
#pragma section = "NextpilotParamTab"
static param_info_t *__param_section_start_ptr = (param_info_t *)__section_begin("NextpilotParamTab");
static param_info_t *__param_section_end_ptr   = (param_info_t *)__section_end("NextpilotParamTab");
static param_info_t *__param_table__           = nullptr;
static uint16_t      __param_count__           = UINT16_MAX;
#elif defined(__GNUC__) || defined(__TI_COMPILER_VERSION__) || defined(__TASKING__)
// 请在lds文件中定义好nextpilot_param_tab section
extern const int     __nextpilot_param_tab_start;
extern const int     __nextpilot_param_tab_end;
static param_info_t *__param_section_start_ptr = (param_info_t *)&__nextpilot_param_tab_start;
static param_info_t *__param_section_end_ptr   = (param_info_t *)&__nextpilot_param_tab_end;
static param_info_t *__param_table__           = nullptr;
static uint16_t      __param_count__           = UINT16_MAX;
#elif defined(__ADSPBLACKFIN__) /* for VisaulDSP++ Compiler*/
// 如何定义section
extern "asm" int     __nextpilot_param_tab_start;
extern "asm" int     __nextpilot_param_tab_end;
static param_info_t *__param_section_start_ptr = (param_info_t *)&__nextpilot_param_tab_start;
static param_info_t *__param_section_end_ptr   = (param_info_t *)&__nextpilot_param_tab_end;
static param_info_t *__param_table__           = nullptr;
static uint16_t      __param_count__           = UINT16_MAX;
#elif defined(_MSC_VER)
#pragma section("NextpilotParamTab$a", read)
#pragma section("NextpilotParamTab$p", read)
#pragma section("NextpilotParamTab$z", read)
__declspec(allocate("NextpilotParamTab$a")) static param_info_t __nextpilot_param_tab_start = {
    .ptr = nullptr,
};
__declspec(allocate("NextpilotParamTab$z")) static param_info_t __nextpilot_param_tab_end = {
    .ptr = nullptr,
};
static param_info_t *__param_section_start_ptr = &__nextpilot_param_tab_start;
static param_info_t *__param_section_end_ptr   = &__nextpilot_param_tab_end;
static param_info_t  __param_table__[1000];
static uint16_t      __param_count__ = UINT16_MAX;
#endif

static bool param_init() {
#if defined(_MSC_VER)
    unsigned int *ptr_begin = (unsigned int *)(__param_section_start_ptr + 1);
    unsigned int *ptr_end   = (unsigned int *)__param_section_end_ptr;
    uint16_t      count     = 0;

    // 由于msv的section是不连续的，因此先将所有的param拷贝到一个table里面
    // table数组的size是1000
    while (ptr_begin < ptr_end && count <= 1000) {
        if (*ptr_begin != 0) {
            rt_memcpy(__param_table__ + count, ptr_begin, sizeof(param_info_t));
            count++;
            ptr_begin += sizeof(param_info_t) / sizeof(unsigned int);
        } else {
            ptr_begin++;
        }
    }
    __param_count__ = count;
#else
    __param_table__ = __param_section_start_ptr;
    __param_count__ = __param_section_end_ptr - __param_section_start_ptr;
#endif
    return true;
}

static uint16_t param_get_count() {
    if (__param_count__ == UINT16_MAX) {
        param_int();
    }

    return __param_count__;
}

static bool param_in_range(param_t idx) {
    return idx < param_get_count();
}

static int param_get_info(param_t idx, param_info_t *info) {
    if (!param_in_range(idx) || !info) {
        return -1;
    }

    info->name  = __param_table__[idx].ptr->name;
    info->type  = __param_table__[idx].ptr->type;
    info->value = __param_table__[idx].ptr->value;
    info->falg  = __param_table__[idx].ptr->flag;

    return 0;
}

static int param_find(const char *name) {
    return 0;
}

static int param_reset(param_t idx) {
    return 0;
}

param_interface_ops_t _ops = {
    .init       = param_init,
    .find       = find,
    .reset      = reset,
    .get_count  = get_count,
    .get_info   = get_info,
    .get_value  = get_value,
    .set_value  = set_value,
    .get_status = get_status,
    .set_status = set_status,
};
