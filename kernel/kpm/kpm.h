#ifndef ___SUKISU_KPM_H
#define ___SUKISU_KPM_H

#include <linux/types.h>

struct ksu_kpm_load_cmd {
    char path[256];
    char args[256];
    __u32 result;
};

struct ksu_kpm_unload_cmd {
    char name[256];
    __u32 result;
};

struct ksu_kpm_num_cmd {
    __u32 num;
};

struct ksu_kpm_list_cmd {
    char buffer[256];
    unsigned int size;
    __u32 result;
};

struct ksu_kpm_info_cmd {
    char name[256];
    char buffer[256];
    __u32 result;
};

struct ksu_kpm_control_cmd {
    char name[256];
    char args[256];
    __u32 result;
};

struct ksu_kpm_version_cmd {
    char buffer[256];
    __u32 result;
};

#define KSU_IOCTL_KPM_LOAD    _IOC(_IOC_WRITE, 'K', 200, 0)
#define KSU_IOCTL_KPM_UNLOAD  _IOC(_IOC_WRITE, 'K', 201, 0)
#define KSU_IOCTL_KPM_NUM _IOC(_IOC_READ, 'K', 202, 0)
#define KSU_IOCTL_KPM_LIST    _IOC(_IOC_READ|_IOC_WRITE, 'K', 203, 0)
#define KSU_IOCTL_KPM_INFO    _IOC(_IOC_READ|_IOC_WRITE, 'K', 204, 0)
#define KSU_IOCTL_KPM_CONTROL _IOC(_IOC_READ|_IOC_WRITE, 'K', 205, 0)
#define KSU_IOCTL_KPM_VERSION _IOC(_IOC_READ|_IOC_WRITE, 'K', 206, 0)


int do_kpm_load(void __user *arg);
int do_kpm_unload(void __user *arg);
int do_kpm_num(void __user *arg);
int do_kpm_list(void __user *arg);
int do_kpm_info(void __user *arg);
int do_kpm_control(void __user *arg);
int do_kpm_version(void __user *arg);

#endif