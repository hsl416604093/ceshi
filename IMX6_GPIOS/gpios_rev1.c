#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>						/*delay*/
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                        	/*kmalloc*/
#include <linux/vmalloc.h>                    	/*vmalloc*/
#include <linux/types.h>                       	/*ssize_t*/
#include <linux/fs.h>                         	/*file_operaiotns*/
#include <linux/gpio_keys.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm-generic/ioctl.h>
#include <asm-generic/errno-base.h>


/************Ӳ�����*************/
#include <mach/iomux-mx6dl.h>	
/*Ƶ�ϵ�ԪIO*/
#define DS_CS1 			IMX_GPIO_NR(6,11)		/*NANDF_CS0*/
#define DS_CS2 			IMX_GPIO_NR(6,14)		/*NANDF_CS1*/
#define DS_RES1 		IMX_GPIO_NR(6,15)		/*NANDF_CS2*/
#define DS_RES2			IMX_GPIO_NR(1,16)		/*SD1_DAT0*/
#define DS_DIN			IMX_GPIO_NR(7,8)		/*SD3_RST*/
#define DS_CLK			IMX_GPIO_NR(7,6)		/*SD3_DAT2*/
#define DS_IOUPDATE1	IMX_GPIO_NR(3,20)		/*EIM_D20*/
#define DS_IOUPDATE2	IMX_GPIO_NR(7,7)		/*SD3_DAT3*/

/*�����ڴ�ռ��С*/
#define WRITE_MALLOC_SIZE 4096
/**���豸�źʹ��豸��**/
#define DEVICE_MAJOR 104
#define DEVICE_MINOR 0

/*����GPIO�ĸ���*/
#define gpio_num 8

/*ioctl��CMD����*/
#define MY_GPIO_MAGIC	'x'
#define DS_CS1_W 		_IOW(MY_GPIO_MAGIC,0,unsigned long)	/*dir 2bits,type 8bits,nr 8bits,size 14bits*/
#define DS_CS1_R 		_IOR(MY_GPIO_MAGIC,0,unsigned long)

#define DS_CS2_W 		_IOW(MY_GPIO_MAGIC,1,unsigned long)
#define DS_CS2_R 		_IOR(MY_GPIO_MAGIC,1,unsigned long)

#define DS_RES1_W 		_IOW(MY_GPIO_MAGIC,2,unsigned long)
#define DS_RES1_R 		_IOR(MY_GPIO_MAGIC,2,unsigned long)

#define DS_RES2_W 		_IOW(MY_GPIO_MAGIC,3,unsigned long)
#define DS_RES2_R 		_IOR(MY_GPIO_MAGIC,3,unsigned long)

#define DS_DIN_W 		_IOW(MY_GPIO_MAGIC,4,unsigned long)
#define DS_DIN_R 		_IOR(MY_GPIO_MAGIC,4,unsigned long)

#define DS_CLK_W 		_IOW(MY_GPIO_MAGIC,5,unsigned long)
#define DS_CLK_R 		_IOR(MY_GPIO_MAGIC,5,unsigned long)

#define DS_IOUPDATE1_W 	_IOW(MY_GPIO_MAGIC,6,unsigned long)
#define DS_IOUPDATE1_R 	_IOR(MY_GPIO_MAGIC,6,unsigned long)

#define DS_IOUPDATE2_W 	_IOW(MY_GPIO_MAGIC,7,unsigned long)
#define DS_IOUPDATE2_R 	_IOR(MY_GPIO_MAGIC,7,unsigned long)

/*������ָ�룬ָ���ڴ���*/
static char *gpio_spvm;
/*��/sysĿ¼����һ����*/
static struct class *gpio_class;	
/*��������£�����һ���豸�ڵ�*/
static struct cdev *gpio_class_dev;

typedef unsigned int unit;
int gpio_index;

/*����gpio�ṹ��*/
struct gpio_desc
{
	unit num;			/*Ϊ�˲鿴����*/
	unit gpio;
	const char *desc;
};

struct gpio_desc gpios[] = 
{
	/*Ƶ�ʺϳɵ�ԪIO*/
	{0	,DS_CS1			,	"NANDF_CS0"	},		
	{1	,DS_CS2			,	"NANDF_CS1"	},		
	{2	,DS_RES1		,	"NANDF_CS2"	},		
	{3	,DS_RES2		,	"SD1_DAT0"	},		
	{4	,DS_DIN			,	"SD3_RST"	},		
	{5	,DS_CLK			,	"SD3_DATA2"	},		
	{6	,DS_IOUPDATE1	,	"EIM_D20"	},		
	{7	,DS_IOUPDATE2	,	"SD3_DATA3"	},
};

/*open������ʵ��*/
static int gpio_open(struct inode *inode, struct file *file)
{
	unit error,i;
	/*����gpio*/
	for(i = 0;i < gpio_num;i++)
	{
		gpio_free(gpios[i].gpio);
		error = gpio_request(gpios[i].gpio,gpios[i].desc);
		if(error < 0)
		{
			printk(KERN_ALERT"failed to request %s\n",gpios[i].desc);
			goto fail1;
		}
	}
	return 0;
fail1:
	return error;
}

static long gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{	
	unit error;
	int write_val,read_val,index;
	switch(cmd)
	{
		case DS_CS1_W:
			index = 0;
			error = gpio_direction_output(gpios[index].gpio,0);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			write_val = arg;
			__gpio_set_value(gpios[_IOC_NR(DS_CS1_W)].gpio,write_val);
			break;
		case DS_CS1_R:
			index = 0;
			error = gpio_direction_input(gpios[index].gpio);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			read_val = __gpio_get_value(gpios[_IOC_NR(DS_CS1_R)].gpio);
			if(copy_to_user((int __user*)arg,&read_val,sizeof(read_val)))
				return ENOTTY;
			break;
		case DS_CS2_W:
			index = 1;
			error = gpio_direction_output(gpios[index].gpio,0);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			write_val = arg;
			__gpio_set_value(gpios[_IOC_NR(DS_CS2_W)].gpio,write_val);
			break;
		case DS_CS2_R:
			index = 1;
			error = gpio_direction_input(gpios[index].gpio);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			read_val = __gpio_get_value(gpios[_IOC_NR(DS_CS2_R)].gpio);
			if(copy_to_user((int __user*)arg,&read_val,sizeof(read_val)))
				return ENOTTY;
			break;
		case DS_RES1_W:
			index = 2;
			error = gpio_direction_output(gpios[index].gpio,0);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			write_val = arg;
			__gpio_set_value(gpios[_IOC_NR(DS_RES1_W)].gpio,write_val);
			break;
		case DS_RES1_R:
			index = 2;
			error = gpio_direction_input(gpios[index].gpio);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			read_val = __gpio_get_value(gpios[_IOC_NR(DS_RES1_R)].gpio);
			if(copy_to_user((int __user*)arg,&read_val,sizeof(read_val)))
				return ENOTTY;
			break;
		case DS_RES2_W:
			index = 3;
			error = gpio_direction_output(gpios[index].gpio,0);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			write_val = arg;
			__gpio_set_value(gpios[_IOC_NR(DS_RES2_W)].gpio,write_val);
			break;
		case DS_RES2_R:
			index = 3;
			error = gpio_direction_input(gpios[index].gpio);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			read_val = __gpio_get_value(gpios[_IOC_NR(DS_RES2_R)].gpio);
			if(copy_to_user((int __user*)arg,&read_val,sizeof(read_val)))
				return ENOTTY;
			break;
		case DS_DIN_W:
			index = 4;
			error = gpio_direction_output(gpios[index].gpio,0);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			write_val = arg;
			__gpio_set_value(gpios[_IOC_NR(DS_DIN_W)].gpio,write_val);
			break;
		case DS_DIN_R:
			index = 4;
			error = gpio_direction_input(gpios[index].gpio);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			read_val = __gpio_get_value(gpios[_IOC_NR(DS_DIN_R)].gpio);
			if(copy_to_user((int __user*)arg,&read_val,sizeof(read_val)))
				return ENOTTY;
			break;
		case DS_CLK_W:
			index = 5;
			error = gpio_direction_output(gpios[index].gpio,0);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			write_val = arg;
			__gpio_set_value(gpios[_IOC_NR(DS_CLK_W)].gpio,write_val);
			break;
		case DS_CLK_R:
			index = 5;
			error = gpio_direction_input(gpios[index].gpio);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			read_val = __gpio_get_value(gpios[_IOC_NR(DS_CLK_R)].gpio);
			if(copy_to_user((int __user*)arg,&read_val,sizeof(read_val)))
				return ENOTTY;
			break;
		case DS_IOUPDATE1_W:
			index = 6;
			error = gpio_direction_output(gpios[index].gpio,0);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			write_val = arg;
			__gpio_set_value(gpios[_IOC_NR(DS_IOUPDATE1_W)].gpio,write_val);
			break;
		case DS_IOUPDATE1_R:
			index = 6;
			error = gpio_direction_input(gpios[index].gpio);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			read_val = __gpio_get_value(gpios[_IOC_NR(DS_IOUPDATE1_R)].gpio);
			if(copy_to_user((int __user*)arg,&read_val,sizeof(read_val)))
				return ENOTTY;
			break;
		case DS_IOUPDATE2_W:
			index = 7;
			error = gpio_direction_output(gpios[index].gpio,0);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			write_val = arg;
			__gpio_set_value(gpios[_IOC_NR(DS_IOUPDATE2_W)].gpio,write_val);
			break;
		case DS_IOUPDATE2_R:
			index = 7;
			error = gpio_direction_input(gpios[index].gpio);
			if (error < 0)
			{
				printk(KERN_ALERT"failed to configure direction for %s \n",gpios[index].desc);
				goto fail1;
			}
			read_val = __gpio_get_value(gpios[_IOC_NR(DS_IOUPDATE2_R)].gpio);
			if(copy_to_user((int __user*)arg,&read_val,sizeof(read_val)))
				return ENOTTY;
			break;
		default:break;
	}
	return 0;
fail1:
	gpio_free(gpios[index].gpio);
}

/*release������ʵ��*/
static int gpio_close(struct inode *inode, struct file *file)
{	
	unit i;
	/*�ͷ�ռ�õ���Դ*/
	for(i = 0;i < gpio_num;i++)
		gpio_free(gpios[i].gpio);
	/*��ӡ��ʾ�˳���Ϣ*/
//	printk(KERN_ALERT"gpio_dev is closed!\n");
	return 0;
}
/*������ļ���������*/
static const struct file_operations gpio_fops = 
{
	/*����ӵ����*/
	.owner			= THIS_MODULE,
	.open			= gpio_open,
	.unlocked_ioctl	= gpio_ioctl,
	.release 		= gpio_close,
};

/*�����ĳ�ʼ������*/
static int gpio_init(void)
{
	/*�豸��ʼ��*/
	int devno;
	/*�豸�ŵ�����,����*/
	devno = MKDEV(DEVICE_MAJOR,DEVICE_MINOR);
	/*�����豸�ṹ��ĵ�ַ�ռ�*/
	gpio_spvm = (char *)vmalloc(WRITE_MALLOC_SIZE);
	gpio_class_dev = cdev_alloc();
	/*�ַ��豸��ʼ��������ز������豸*/
	cdev_init(gpio_class_dev,&gpio_fops);
	/*�豸��ӵ����*/
	gpio_class_dev->owner = THIS_MODULE;
	/*����豸���ں�*/
	cdev_add(gpio_class_dev,devno,1);
	/*��̬�����豸��*/
	register_chrdev(DEVICE_MAJOR,"gpios",&gpio_fops);	
	/*�����豸�࣬�����Զ������豸�ļ�*/
	gpio_class = class_create(THIS_MODULE, "gpios");	
	/*������ǰ�������豸�࣬�����豸*/
	device_create(gpio_class,NULL,MKDEV(DEVICE_MAJOR,DEVICE_MINOR),NULL,"gpios");
	mxc_iomux_v3_setup_pad(MX6DL_PAD_NANDF_CS0__GPIO_6_11);
	mxc_iomux_v3_setup_pad(MX6DL_PAD_NANDF_CS1__GPIO_6_14);
	mxc_iomux_v3_setup_pad(MX6DL_PAD_NANDF_CS2__GPIO_6_15);
	mxc_iomux_v3_setup_pad(MX6DL_PAD_SD1_DAT0__GPIO_1_16);
	mxc_iomux_v3_setup_pad(MX6DL_PAD_SD3_RST__GPIO_7_8);
	mxc_iomux_v3_setup_pad(MX6DL_PAD_SD3_DAT2__GPIO_7_6);
	mxc_iomux_v3_setup_pad(MX6DL_PAD_EIM_D20__GPIO_3_20);
	mxc_iomux_v3_setup_pad(MX6DL_PAD_SD3_DAT3__GPIO_7_7);
	return 0;
}

/*�˳�����*/
static void gpio_exit(void)
{
	/*�豸ж��*/
	unregister_chrdev(DEVICE_MAJOR,"gpios");								
	device_destroy(gpio_class,MKDEV(DEVICE_MAJOR,DEVICE_MINOR));						
	class_destroy(gpio_class);
}

/*LICENSE��Ϣ*/
MODULE_LICENSE("GPL");
/*ж�غͼ���*/
module_init(gpio_init);
module_exit(gpio_exit);

