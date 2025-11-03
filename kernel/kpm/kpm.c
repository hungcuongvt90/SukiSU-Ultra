/* SPDX-License-Identifier: GPL-2.0-or-later */
/* 
 * Copyright (C) 2025 Liankong (xhsw.new@outlook.com). All Rights Reserved.
 * 本代码由GPL-2授权
 * 
 * 适配KernelSU的KPM 内核模块加载器兼容实现
 * 
 * 集成了 ELF 解析、内存布局、符号处理、重定位（支持 ARM64 重定位类型）
 * 并参照KernelPatch的标准KPM格式实现加载和控制
 */
#include <linux/export.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kernfs.h>
#include <linux/file.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <linux/elf.h>
#include <linux/kallsyms.h>
#include <linux/version.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/rcupdate.h>
#include <asm/elf.h>    /* 包含 ARM64 重定位类型定义 */
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <asm/cacheflush.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/set_memory.h>
#include <linux/version.h>
#include <linux/export.h>
#include <linux/slab.h>
#include <asm/insn.h>
#include <linux/kprobes.h>
#include <linux/stacktrace.h>
#include <linux/kallsyms.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,0,0) && defined(CONFIG_MODULES)
#include <linux/moduleloader.h> // 需要启用 CONFIG_MODULES
#endif
#include "kpm.h"
#include "compact.h"

#ifndef NO_OPTIMIZE
#if defined(__GNUC__) && !defined(__clang__)
    #define NO_OPTIMIZE __attribute__((optimize("O0")))
#elif defined(__clang__)
    #define NO_OPTIMIZE __attribute__((optnone))
#else
    #define NO_OPTIMIZE
#endif
#endif

// ============================================================================================

noinline
NO_OPTIMIZE
static void sukisu_kpm_load_module_path(const char* path, const char* args, void* ptr, void __user* result) {
    // This is a KPM module stub.
    int res = -1;
    printk("KPM: Stub function called (sukisu_kpm_load_module_path). path=%s args=%s ptr=%p\n", path, args, ptr);
    __asm__ volatile("nop");  // 精确控制循环不被优化
    if(copy_to_user(result, &res, sizeof(res)) < 1) printk("KPM: Copy to user failed.");
}

noinline
NO_OPTIMIZE
static void sukisu_kpm_unload_module(const char* name, void* ptr, void __user* result) {
    // This is a KPM module stub.
    int res = -1;
    printk("KPM: Stub function called (sukisu_kpm_unload_module). name=%s ptr=%p\n", name, ptr);
    __asm__ volatile("nop");  // 精确控制循环不被优化
    if(copy_to_user(result, &res, sizeof(res)) < 1) printk("KPM: Copy to user failed.");
}

noinline
NO_OPTIMIZE
static void sukisu_kpm_num(void __user* result) {
    // This is a KPM module stub.
    int res = 0;
    printk("KPM: Stub function called (sukisu_kpm_num).\n");
    __asm__ volatile("nop");  // 精确控制循环不被优化
    if(copy_to_user(result, &res, sizeof(res)) < 1) printk("KPM: Copy to user failed.");
}

noinline
NO_OPTIMIZE
static void sukisu_kpm_info(const char* name, void __user* out, void __user* result) {
    // This is a KPM module stub.
    int res = -1;
    printk("KPM: Stub function called (sukisu_kpm_info). name=%s buffer=%p\n", name, out);
    __asm__ volatile("nop");  // 精确控制循环不被优化
    if(copy_to_user(result, &res, sizeof(res)) < 1) printk("KPM: Copy to user failed.");
}

noinline
NO_OPTIMIZE
static void sukisu_kpm_list(void __user* out, unsigned int bufferSize, void __user* result) {
    // This is a KPM module stub.
    int res = -1;
    printk("KPM: Stub function called (sukisu_kpm_list). buffer=%p size=%d\n", out, bufferSize);
    if(copy_to_user(result, &res, sizeof(res)) < 1) printk("KPM: Copy to user failed.");
}

noinline
NO_OPTIMIZE
static void sukisu_kpm_control(void __user* name, void __user* args, void __user* result) {
    // This is a KPM module stub.
    int res = -1;
    printk("KPM: Stub function called (sukisu_kpm_control). name=%p args=%p\n", name, args);
    __asm__ volatile("nop");  // 精确控制循环不被优化
    if(copy_to_user(result, &res, sizeof(res)) < 1) printk("KPM: Copy to user failed.");
}

noinline
NO_OPTIMIZE
static void sukisu_kpm_version(void __user* out, unsigned int bufferSize, void __user* result) {
    int res = -1;
    printk("KPM: Stub function called (sukisu_kpm_version). buffer=%p size=%d\n", out, bufferSize);
    if(copy_to_user(result, &res, sizeof(res)) < 1) printk("KPM: Copy to user failed.");
}

EXPORT_SYMBOL(sukisu_kpm_load_module_path);
EXPORT_SYMBOL(sukisu_kpm_unload_module);
EXPORT_SYMBOL(sukisu_kpm_num);
EXPORT_SYMBOL(sukisu_kpm_info);
EXPORT_SYMBOL(sukisu_kpm_list);
EXPORT_SYMBOL(sukisu_kpm_version);
EXPORT_SYMBOL(sukisu_kpm_control);

int do_kpm_load(void __user *arg)
{
    struct ksu_kpm_load_cmd cmd;

    if (copy_from_user(&cmd, arg, sizeof(cmd)))
        return -EFAULT;

    sukisu_kpm_load_module_path(cmd.path, cmd.args, NULL, (void __user *)&cmd.result);

    if (copy_to_user(arg, &cmd, sizeof(cmd)))
        return -EFAULT;

    return 0;
}

int do_kpm_unload(void __user *arg)
{
    struct ksu_kpm_unload_cmd cmd;

    if (copy_from_user(&cmd, arg, sizeof(cmd)))
        return -EFAULT;

    sukisu_kpm_unload_module(cmd.name, NULL, (void __user *)&cmd.result);

    if (copy_to_user(arg, &cmd, sizeof(cmd)))
        return -EFAULT;

    return 0;
}

int do_kpm_num(void __user *arg)
{
    struct ksu_kpm_num_cmd cmd;

    sukisu_kpm_num((void __user *)&cmd.num);

    if (copy_to_user(arg, &cmd, sizeof(cmd))) {
        pr_err("kpm_num: copy_to_user failed\n");
        return -EFAULT;
    }

    return 0;
}

int do_kpm_list(void __user *arg)
{
    struct ksu_kpm_list_cmd cmd;

    if (copy_from_user(&cmd, arg, sizeof(cmd)))
        return -EFAULT;

    sukisu_kpm_list((void __user *)cmd.buffer, cmd.size, (void __user *)&cmd.result);

    if (copy_to_user(arg, &cmd, sizeof(cmd)))
        return -EFAULT;

    return 0;
}

int do_kpm_info(void __user *arg)
{
    struct ksu_kpm_info_cmd cmd;

    if (copy_from_user(&cmd, arg, sizeof(cmd)))
        return -EFAULT;

    sukisu_kpm_info(cmd.name, (void __user *)cmd.buffer, (void __user *)&cmd.result);

    if (copy_to_user(arg, &cmd, sizeof(cmd)))
        return -EFAULT;

    return 0;
}

int do_kpm_control(void __user *arg)
{
    struct ksu_kpm_control_cmd cmd;

    if (copy_from_user(&cmd, arg, sizeof(cmd)))
        return -EFAULT;

    sukisu_kpm_control((void __user *)cmd.name, (void __user *)cmd.args, (void __user *)&cmd.result);

    if (copy_to_user(arg, &cmd, sizeof(cmd)))
        return -EFAULT;

    return 0;
}

int do_kpm_version(void __user *arg)
{
    struct ksu_kpm_version_cmd cmd;

    if (copy_from_user(&cmd, arg, sizeof(cmd)))
        return -EFAULT;

    sukisu_kpm_version((void __user *)cmd.buffer, sizeof(cmd.buffer), (void __user *)&cmd.result);

    if (copy_to_user(arg, &cmd, sizeof(cmd)))
        return -EFAULT;

    return 0;
}
