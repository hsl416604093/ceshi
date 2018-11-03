#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>
#include <asm-generic/ioctl.h>

static const char *IMX6_PROT_device = "/dev/gpios";
int fd_IMX6_PORT_INIT;			//IMX6_PORT_INIT()函数开启设备的文件描述符

/*ioctl的CMD参数*/
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





#define DS_CS1_ON			IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_CS1_W,1)
#define DS_CS1_OFF			IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_CS1_W,0)
#define DS_CS2_ON			IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_CS2_W,1)
#define DS_CS2_OFF			IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_CS2_W,0)
#define DS_RES1_ON			IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_RES1_W,1)
#define DS_RES1_OFF			IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_RES1_W,0)
#define DS_RES2_ON			IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_RES2_W,1)
#define DS_RES2_OFF			IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_RES2_W,0)
#define DS_DIN_ON			IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_DIN_W,1)
#define DS_DIN_OFF			IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_DIN_W,0)
#define DS_CLK_ON			IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_CLK_W,1)
#define DS_CLK_OFF			IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_CLK_W,0)
#define DS_IOUPDATE1_ON		IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_IOUPDATE1_W,1)
#define DS_IOUPDATE1_OFF	IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_IOUPDATE1_W,0)
#define DS_IOUPDATE2_ON		IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_IOUPDATE2_W,1)
#define DS_IOUPDATE2_OFF	IMX6_GPIO_WRITE(fd_IMX6_PORT_INIT,DS_IOUPDATE2_W,0)

#define DS_CS1_READ			IMX6_GPIO_READ(fd_IMX6_PORT_INIT,DS_CS1_R)
#define DS_CS2_READ			IMX6_GPIO_READ(fd_IMX6_PORT_INIT,DS_CS2_R)
#define DS_RES1_READ		IMX6_GPIO_READ(fd_IMX6_PORT_INIT,DS_RES1_R)
#define DS_RES2_READ		IMX6_GPIO_READ(fd_IMX6_PORT_INIT,DS_RES2_R)
#define DS_DIN_READ			IMX6_GPIO_READ(fd_IMX6_PORT_INIT,DS_DIN_R)
#define DS_CLK_READ			IMX6_GPIO_READ(fd_IMX6_PORT_INIT,DS_CLK_R)
#define DS_IOUPDATE1_READ	IMX6_GPIO_READ(fd_IMX6_PORT_INIT,DS_IOUPDATE1_R)
#define DS_IOUPDATE2_READ	IMX6_GPIO_READ(fd_IMX6_PORT_INIT,DS_IOUPDATE2_R)

void IMX6_PORT_INIT(void);																//板载GPIO初始化
void IMX6_GPIO_WRITE(unsigned long fd,unsigned long cmd,unsigned char arg);				//板载GPIO写操作
int IMX6_GPIO_READ(unsigned long fd,unsigned long cmd);									//板载GPIO读操作


void IMX6_PORT_INIT(void)
{	
	fd_IMX6_PORT_INIT = open(IMX6_PROT_device,O_RDWR);	
	if(fd_IMX6_PORT_INIT < 0)	
	{		
		printf("can't open IMX6_PORT_INIT`s device\n");	
	}
}

void IMX6_GPIO_WRITE(unsigned long fd,unsigned long cmd,unsigned char arg)
{	
	ioctl(fd,cmd,arg);
}

int IMX6_GPIO_READ(unsigned long fd,unsigned long cmd)
{	
	int read_val;	
	ioctl(fd,cmd,&read_val);	
	return	read_val;
}


int main(int args, char *argv[])
{
	int val;
	IMX6_PORT_INIT();
	while(1)
	{
		DS_CS1_ON;
		printf("DS_CS1 = %d\n",DS_CS1_READ);
		sleep(1);
		DS_CS1_OFF;
		printf("DS_CS1 = %d\n",DS_CS1_READ);
		sleep(1);
		DS_CS2_ON;
		printf("DS_CS2 = %d\n",DS_CS2_READ);
		sleep(1);
		DS_CS2_OFF;
		printf("DS_CS2 = %d\n",DS_CS2_READ);
		sleep(1);
	}
    return 0;
}

