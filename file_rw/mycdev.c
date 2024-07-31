#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/eventpoll.h>
#include "file.h"
#include <linux/io.h>

#define CNAME 		"mycdev"
#define COUNT		1
struct file_dev{ 
	dev_t devid; /* 设备号 */
	struct cdev cdev; /* cdev */
	struct class *class; /* 类 */
	struct device *device; /* 设备 */
	int major; /* 主设备号 */
	int minor; /* 次设备号 */
	struct mutex lock; /* 互斥体 */
 };
struct file_dev fileRW;

int mycdev_open(struct inode *inode, struct file *filp)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);

	filp->private_data = &fileRW;  /* 私有数据 */
	if (mutex_lock_interruptible(&fileRW.lock))
		return -ERESTARTSYS;

	return 0;
}

long mycdev_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
	int ret;
	myfile file;

	switch(cmd){
		case FILE_R:
			ret = copy_to_user((void *)args, (void *)&buffer, file.fileSize);
			if(ret){
				printk("copy data to user error\n");
				return -EINVAL;
			}
			
			break;
		case FILE_W:
			ret = copy_from_user((void *)&file, (void *)args, sizeof(file));
			buffer = (char*)kmalloc(file.fileSize, GFP_KERNEL);
			memcpy(buffer, file.buf, file.fileSize);

			if(ret){
				printk("copy data from user error\n");
				return -EINVAL;
			}
			
			break;
		default:
			break;
		}
	
	return 0;
}

int mycdev_close(struct inode *inode, struct file *filp)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	struct file_dev *cdev = filp->private_data;
	/* 释放互斥锁 */
	mutex_unlock(&cdev->lock);
	kfree(buffer);
	return 0;
}

const struct file_operations fops = {
	.open = mycdev_open,
	.unlocked_ioctl = mycdev_ioctl,
	.release = mycdev_close,
};

static int __init mycdev_init(void)
{
	/* 1、创建设备号 */
	if (fileRW.major) { /* 定义了设备号 */
		fileRW.devid = MKDEV(fileRW.major, 0);
		register_chrdev_region(fileRW.devid, COUNT, CNAME);
	} else { /* 没有定义设备号 */
		alloc_chrdev_region(&fileRW.devid, 0, COUNT, CNAME); /* 申请设备号 */
		fileRW.major = MAJOR(fileRW.devid); /* 获取主设备号 */
	 	fileRW.minor = MINOR(fileRW.devid); /* 获取次设备号 */
	}
	
	/* 2、初始化 cdev */
	fileRW.cdev.owner = THIS_MODULE;
	cdev_init(&fileRW.cdev, &fops);

	/* 3、添加一个 cdev */
	cdev_add(&fileRW.cdev, fileRW.devid, COUNT);

	/* 4、创建类 */
	fileRW.class = class_create(THIS_MODULE, CNAME);
	if (IS_ERR(fileRW.class)) 
		return PTR_ERR(fileRW.class);

	/* 5、创建设备 */
	fileRW.device = device_create(fileRW.class, NULL, fileRW.devid, NULL, CNAME);
	if (IS_ERR(fileRW.device))
		return PTR_ERR(fileRW.device);

	/* 初始化互斥体 */
	 mutex_init(&fileRW.lock);

	return 0;
}

static void __exit mycdev_exit(void)
{
	/* 注销字符设备 */
	cdev_del(&fileRW.cdev);/* 删除 cdev */
	unregister_chrdev_region(fileRW.devid, COUNT);

	device_destroy(fileRW.class, fileRW.devid);
	class_destroy(fileRW.class);
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");










