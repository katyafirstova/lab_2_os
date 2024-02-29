#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#define DEBUGFS_DIR_NAME "pmap_debugfs"
#define DEBUGFS_FILE_NAME "pmap_info"

typedef struct module_init module_init;
typedef struct module_exit module_exit;
static struct dentry *dir;
static struct dentry *file;

/*  извлекает карту памяти для текущего процесса и записывает ее в буфер */
static ssize_t pmap_info_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
    struct task_struct *task;
    struct mm_struct *mm;
    char *info;
    int len = 0;

    task = get_current();
    mm = task->mm;

    if (!mm) {
    printk(KERN_INFO "No memory map available for current process\n");
    return 0;
    }

    len += snprintf(NULL, 0, "Memory map for PID %d:\n", task->pid);
    len += snprintf(NULL, 0, "Start Address\tEnd Address\tPermissions\tOffset\t\tDevice\t\t\tInode\t\t\tPath\n");

    info = kmalloc(len + 1, GFP_KERNEL);
    if (!info) {
    printk(KERN_ERR "Failed to allocate memory for info\n");
    return -ENOMEM;

    snprintf(info, len + 1, "Memory map for PID %d:\n", task->pid);
    snprintf(info + strlen(info), len + 1 - strlen(info), "Start Address\tEnd Address\tPermissions\tOffset\t\tDevice\t\t\tInode\t\t\tPath\n");

    printk(KERN_INFO "%s", info);

    return simple_read_from_buffer(buf, count, f_pos, info, len);
}

static const struct file_operations pmap_info_fops = {
        .read = pmap_info_read,
};

static int __init pmap_debugfs_init(void)
{
    dir = debugfs_create_dir(DEBUGFS_DIR_NAME, NULL);
    if (!dir) {
        printk(KERN_ERR "Failed to create debugfs directory\n");
        return -ENODEV;
    }

    file = debugfs_create_file(DEBUGFS_FILE_NAME, 0444, dir, NULL, &pmap_info_fops);
    if (!file) {
        printk(KERN_ERR "Failed to create debugfs file\n");
        debugfs_remove(dir);
        return -ENODEV;
    }

    return 0;
}

static void __exit pmap_debugfs_exit(void)
{
    debugfs_remove_recursive(dir);
}

module_init(pmap_debugfs_init);
module_exit(pmap_debugfs_exit);




