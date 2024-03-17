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
#define BUFFER_SIZE 1024

typedef struct module_init module_init;
typedef struct module_exit module_exit;
static struct dentry *dir;
static struct dentry *file;

/*  копирует данные из пользовательского пространства в ядро */
static ssize_t pmap_info_write(struct file *filp, const char __user *buffer, size_t count, loff_t *f_pos) {
    char user_data[BUFFER_SIZE];
    long pid;

    if (count >= BUFFER_SIZE) {
        printk(KERN_ERR "Buffer size exceeded\n");
        return -EINVAL;
    }

    if (copy_from_user(user_data, buffer, count)) {
        printk(KERN_ERR "Failed to copy data from user space\n");
        return -EFAULT;
    }

    sscanf(user_data, "pid: %ld", &pid);

    struct task_struct *task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        printk(KERN_ERR "Process with PID %lu not found\n", pid);
        return -EINVAL;
    }

    return strlen(user_data);
}


/*  извлекает карту памяти для текущего процесса и записывает ее в буфер */
static ssize_t pmap_info_read(struct file *filp, char __user *buffer, size_t count, loff_t *f_pos) {
    long pid;
    struct mm_struct *mm;
    struct vm_area_struct *vma;
    char *info;
    int len = 0;
    int total_kb = 0;

    struct task_struct *task = get_pid_task(find_vpid(pid), PIDTYPE_PID);;
    if (!task) {
        len += sprintf(buffer, "Process with PID %d not found\n", pid);
    } else {
        mm = task->mm;
        len += snprintf(NULL, 0, "Memory map for PID %d:\n", task->pid);
        len += snprintf(NULL, 0, "Address:\n", task->pid);
    }

    /* после получения информлции о процессе, запускается цикл по областям виртуальной памяти:
     *  вычисляем адрес начала, размер в кб, режим доступа, смещение, устройство, и отображение */

    len += sprintf(buffer, "Address           Kbytes     RSS   Dirty Mode   Mapping\n");
    for (vma = mm->mmap; vma; vma = vma->vm_next) {
        len += sprintf(buffer, "Address           Kbytes Mode  Offset           Device    Mapping\n");
        for (vma = mm->mmap; vma; vma = vma->vm_next) {
            len += sprintf(buffer+len, "%016lx %8ld %4s  %016lx %10d %s\n",
                           vma->vm_start,
                           (vma->vm_end - vma->vm_start) >> 10,
                           (vma->vm_flags & VM_READ) ? "r" : "-",
                           vma->vm_pgoff << PAGE_SHIFT,
                           (vma->vm_flags & VM_MAYSHARE) ? -1 : 0,
                           (vma->vm_file) ? vma->vm_file->f_path.dentry->d_name.name : "[anon]"
            );
            total_kb += (vma->vm_end - vma->vm_start) >> 10;
        }

        len += sprintf(buf+len, "----------------  ------  ----  ----------------  ----------  -------------------------------\n");
        len += sprintf(buf+len, "total kB         %6d\n", total_kb);

    // Возвращаем результат чтения в буфер, переданный функции
    if (copy_to_user(buffer, buf, len)) {
        return -EFAULT;
    }
}

static const struct file_operations pmap_info_fops = {
        .read = pmap_info_read,
        .write = pmap_info_write,
};

static int __init pmap_debugfs_init(void) {
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

static void __exit mod_exit(void) {
    debugfs_remove_recursive(debug_dir);
}




