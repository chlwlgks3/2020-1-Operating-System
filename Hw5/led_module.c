#include <linux/module.h> //included for all kernel modules
#include <linux/cdev.h> //struct cdev������� �������
#include <linux/uaccess.h>
#include <linux/fs.h> //included for file_operations structure
#include <linux/io.h> 
#define GPIO_BASE	0xfe200000 //GPIO_BASE address
#define GPIO_SIZE	256
#define GPIO_SET(g)	(*(gpio+7)=1<<g) //sets bits which are 1 ignores bits which are 0
#define GPIO_CLR(g)	(*(gpio+10)=1<<g) //clears bits which are 1 ignores bits which are 0
#define GPIO_GET(g)	(*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH
// GPIO setup macros. Always use GPIO_IN(g) before using GPIO_OUT(g)
#define GPIO_IN(g)	(*(gpio+((g)/10)) &=~(7<<(((g)%10)*3))) 
#define GPIO_OUT(g)	(*(gpio+((g)/10)) |=(1<<(((g)%10)*3)))

#define GPIO_MAJOR	243 //Ŀ�ο��� ����̽� ����̹��� ����/�����ϴµ� ��� 0~255������ ��
#define GPIO_MINOR	0 //����̽� ����̹� ���� ��ġ�� �����ϱ����� ���
#define GPIO_DEVICE 	"ledtest"
#define GPIO_NUM	17

static int gpio_open(struct inode*, struct file*); //����̽� ����̹��� ó�� �������� �ϵ���� �ʱ�ȭ, ����üũ
static int gpio_release(struct inode*, struct file*); //open�� flip->private data�� �Ҵ��� �������� �Ҵ����
static ssize_t gpio_read(struct file*, char*, size_t, loff_t*); //����̽����� �ڷḦ �дµ� ����Ѵ�
static ssize_t gpio_write(struct file*, const char*, size_t, loff_t*); //�ڷḦ ����̽��� ������

volatile unsigned *gpio; //gpio���ּҷ� �׻� volatile pointer�� ����ؾ��Ѵ�->���ҿ����������Ƿ� ����ȭx
static char msg[BLOCK_SIZE] = {0};
struct cdev gpio_cdev;

//file_operation: ���� ����̽� ����̹��� �������α׷��� �����ϴ� ��
static struct file_operations gpio_fop ={
	.owner = THIS_MODULE, //���ϵ����� ������ THIS_MODULE�� linux/module.h�� ���ǵǾ��ִ� ��ũ���̰� �̸� ����� �ʱ�ȭ
	.open=gpio_open,  //����̽� ����̹��� ó�� �������� �ϵ���� �ʱ�ȭ, ����üũ
	.release=gpio_release, //open�� flip->private data�� �Ҵ��� �������� �Ҵ����
	.read=gpio_read,
	.write=gpio_write, 
};

int start_module(void){ //insmod��ɾ ���� Ŀ�ο� �ε��ɶ� ȣ���
	unsigned int cnt=1;
	static void* map;
	int add;
	dev_t devno;
	
	printk(KERN_INFO "START MODULE\n"); //Ŀ���� logging ��Ŀ������ ����Ǹ� ������ �Լ��� ������ ����ϰų� ��� �˸�(�켱���� ��������)
	devno=MKDEV(GPIO_MAJOR, GPIO_MINOR); //MKDEV()�� �ֹ�ȣ, �ι�ȣ�� ����
	register_chrdev_region(devno, 1, GPIO_DEVICE); //����̽����� ����� ������ �Ҵ��Ѵ�.�� ��,�ι�ȣ�� ���� struct char_device_struct����
	cdev_init(&gpio_cdev, &gpio_fop); //cdev_add�Լ��� ����ϱ� ���� cdev����ü �ʱ�ȭ
	gpio_cdev.owner = THIS_MODULE;
	
	add=cdev_add(&gpio_cdev, devno, cnt); //cdev����ü�� Ŀ�ο� ����Ѵ�. ��������̽� ����ü�� ����̽� ��ȣ����
	map=ioremap(GPIO_BASE, GPIO_SIZE); //mmap�� ���� �����޸𸮿� ���������� ������ �� �ֵ��� ��
	gpio=(volatile unsigned int*)map;
	
	GPIO_IN(GPIO_NUM);
	GPIO_OUT(GPIO_NUM);
	
	return 0;
}

void end_module(void){ //rmmod��ɾ ���� ����� ����Ǳ� ����(Ŀ�ο��� ���ŵɶ�) ȣ��ȴ�.
	dev_t devno = MKDEV(GPIO_MAJOR,GPIO_MINOR);
	unregister_chrdev_region(devno, 1);	//������� ����̽� ��ȣ ����
	cdev_del(&gpio_cdev); 
	
	if(gpio)	
		iounmap(gpio); //�����޸𸮿� ���� �����Ͽ� �����Ѵ�
	printk(KERN_INFO "END MODULE\n");
}
static int gpio_open(struct inode *inode, struct file *file){
	try_module_get(THIS_MODULE); //����̽� ����̹��� �ۼ��Ҷ� ���Ƚ���� ���������ؾ��ϴ°�� ���Ƚ�� ������ ���� �Լ�
	printk("OPEN - gpio device\n");
	return 0;
}

static int gpio_release(struct inode* inode, struct file* file){
	module_put(THIS_MODULE); //����̽� ����̹��� �ۼ��Ҷ� ���Ƚ���� ���������ؾ��ϴ°�� ���Ƚ�� ���Ҹ� ���� �Լ�
	printk("CLOSE - gpio device\n");
	return 0;
}

static ssize_t gpio_read(struct file* file, char* buf, size_t len, loff_t* off){ //����̽����� �ڷ�����
	int cnt;
	//int copy_to_user(void user* to,const void* from, unsigned long n):
	//from�� ����Ű�� �ּ��� Ŀ�θ޸� ��ϵ����͸� to�� ����Ű�� ����� �޸𸮺�� �����Ϳ� n ��ŭ �����
	// ����� ������ ��ȿ�� �˻� ����
	cnt = copy_to_user(buf, msg, strlen(msg)+1); 
	printk("GPIO device READ: %s \n", msg);
	return cnt;
}

static ssize_t gpio_write(struct file* file, const char* buf, size_t len, loff_t* off){ //�ڷḦ ����̽��� ����
	short cnt;
	memset(msg, 0, BLOCK_SIZE); //�޸𸮸� Ư�������� �ʱ�ȭ�ϴ� �Լ�
	//void* memset(void* s,int c,size_t n) :c���ڷ� �޸𸮺���� n����Ʈ�� ���� 

	//int copy_from_user(void* to,const void user* from,unsigned long n):
	// from�� ����Ű�� �ּ��� ����� �޸𸮺�ϵ����͸� to�� ����Ű�� Ŀ�θ޸𸮺�ϵ����Ϳ� n ��ŭ �����
	cnt = copy_from_user(msg, buf, len);
	(!strcmp(msg, "0")) ? GPIO_CLR(GPIO_NUM):GPIO_SET(GPIO_NUM);
	printk("gpio device WRITE: %s \n", msg);
	return cnt;
}

 
MODULE_LICENSE("GPL"); //Ŀ�ο��� ����� � ���̼��� �Ͽ��� Ŀ���� �̿��ϰ� �Ǵ����� �˷��ִ� ����,���̼����� ���� ����Ҽ��ִ� ����� ���ѻ���
module_init(start_module); //Ŀ�ο��� ����� �ε��Ǿ����� ȣ��Ǿ���ϴ� �Լ��� �˷��ִ¿���->����� �ϰԵǴ� ����ϵ��� �� �ʱ�ȭ�Լ��� ȣ���ϴ� �Լ��� ���� ó����
module_exit(end_module); //exitŰ���尡 Ŀ�ο��� ����� ��ε��ÿ� �� �Լ��� �ѹ��� ȣ���ؾ����� �˷��ش�


