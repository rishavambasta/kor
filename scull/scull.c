#include <linux/types.h>


loff_t scull_llseek (struct file *filp, loff_t off, int whence)
{
	struct scull_dev *dev = filp -> private_data;
	loff_t newpos;

	switch(whence)
	{
		case 0:
			newpos = off;
			break;
		case 1:
			newpos = filp -> f_pos +off;
			break;
		case 2:
			newpos = dev -> size + off;
			break;

		default:
			return -EINVAL;
	}

	if (newpos < 0) 
		return -EINVAL;
	filp -> f_pos = newpos;
	return newpos;
}




struct file_operations scull_fops = 
{
	.owner = THIS_MODULE,
	.llseek = scull_llseek,
	.read = scull_read,
	.write = scull_write,
	.unlock_ioctl = NULL,//scull_ioctl,
	.open = scull_open,
	.release = scull_release,
};


void scull_cleanup_module(void)
{
	int i;
	dev_t deno = MKDEV(scull_major,scull_minor);

	if (scull_devices)
	{
		for (i=0; i < scull_nr_devs; i++)
		{
			scull_trim (scull_devices + i);
			cdev_dev(&scull_devices[i].cdev);
		}
		kfree(scull_devices);
	}

	unregister_chrdev_region(devno, scull_nr_devs);
	scull_p_cleanup();
	scull_access_cleanup();
}


static void scull_setup_cdev (struct scull_dev *dev, inti index)
{
	int err,devno = MKDEV(scull_major,scull_minor + index);
	cdev_init(&dev->cdev, &scull_fops);
	dev -> cdev.owner = THIS_MODULE;
	dev -> cdev.ops = &scull_fops;
	err = cdev_add(&dev -> cdev, devno, 1);

	if (err)
		printk (KERN_NOTICE "Error %d adding scull%d",err,index);
}


int scull_init_module (void)
{
	int result, i;
	dev_t dev = 0;


	if (scull_major)
	{
		dev = MKDEV (scull_major, scull_minor);
		result = register_chrdev_region(dev, scull_nr_devs, "scull");
	}
	else
	{
		result = alloc_chrdev_region(&dev, scull_minor,scull_nr_devs,"scull");
		scull_major = MAJOR(dev);
	}

	if (result < 0 )
	{
		printk(KERN_WARNING "scull: can't get major %d\n",scull_major);
		return result;
	}
	scull_devices = kmalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
	if (!scull_devices)
	{
		result = -ENOMEM;
		goto fail;
	}
	memset(scull_devices,0,scull_nr_devs * sizeof(struct scull_dev));

	for (i=0;i<scull_nr_devs;i++)
	{
		scull_devices[i].quantum = scull_quantum;
		scull_devices[i].qset = scull_qset;
		sema_init(&scull_devies[i].sem,1);
		scull_setup_cdev(&scull_devices[i],i);
	}
	
	dev = MKDEV(scull_major, scull_minor + scull_nr_devs);
	dev += scull_p_init(dev);
	dev += scull_access_init(dev);

	return 0;

fail:
	scull_cleanup_module();
	return result;

	module_init(scull_init_module);
	module_exit(scull_cleanup_module);

	

