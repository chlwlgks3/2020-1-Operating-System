#include <linux/module.h> //included for all kernel modules
#include <linux/cdev.h> //struct cdev사용위한 헤더파일
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

#define GPIO_MAJOR	243 //커널에서 디바이스 드라이버를 구분/연결하는데 사용 0~255사이의 값
#define GPIO_MINOR	0 //디바이스 드라이버 내에 장치를 구분하기위해 사용
#define GPIO_DEVICE 	"ledtest"
#define GPIO_NUM	17

static int gpio_open(struct inode*, struct file*); //디바이스 드라이버가 처음 열렸을때 하드웨어 초기화, 에러체크
static int gpio_release(struct inode*, struct file*); //open이 flip->private data에 할당한 데이터의 할당삭제
static ssize_t gpio_read(struct file*, char*, size_t, loff_t*); //디바이스에서 자료를 읽는데 사용한다
static ssize_t gpio_write(struct file*, const char*, size_t, loff_t*); //자료를 디바이스로 보낸다

volatile unsigned *gpio; //gpio의주소로 항상 volatile pointer를 사용해야한다->변할여지가있으므로 최적화x
static char msg[BLOCK_SIZE] = {0};
struct cdev gpio_cdev;

//file_operation: 문자 디바이스 드라이버와 응용프로그램을 연결하는 고리
static struct file_operations gpio_fop ={
	.owner = THIS_MODULE, //파일동작의 소유자 THIS_MODULE은 linux/module.h에 정의되어있는 매크로이고 이를 사용해 초기화
	.open=gpio_open,  //디바이스 드라이버가 처음 열렸을때 하드웨어 초기화, 에러체크
	.release=gpio_release, //open이 flip->private data에 할당한 데이터의 할당삭제
	.read=gpio_read,
	.write=gpio_write, 
};

int start_module(void){ //insmod명령어를 통해 커널에 로딩될때 호출됨
	unsigned int cnt=1;
	static void* map;
	int add;
	dev_t devno;
	
	printk(KERN_INFO "START MODULE\n"); //커널의 logging 메커니즘이 수행되며 수행한 함수의 정보를 기록하거나 경고를 알림(우선순위 통해제공)
	devno=MKDEV(GPIO_MAJOR, GPIO_MINOR); //MKDEV()로 주번호, 부번호를 생성
	register_chrdev_region(devno, 1, GPIO_DEVICE); //디바이스에서 사용할 영역을 할당한다.즉 주,부번호에 따른 struct char_device_struct생성
	cdev_init(&gpio_cdev, &gpio_fop); //cdev_add함수를 사용하기 위해 cdev구조체 초기화
	gpio_cdev.owner = THIS_MODULE;
	
	add=cdev_add(&gpio_cdev, devno, cnt); //cdev구조체를 커널에 등록한다. 실제디바이스 구조체와 디바이스 번호연결
	map=ioremap(GPIO_BASE, GPIO_SIZE); //mmap과 같이 물리메모리에 직접적으로 접근할 수 있도록 함
	gpio=(volatile unsigned int*)map;
	
	GPIO_IN(GPIO_NUM);
	GPIO_OUT(GPIO_NUM);
	
	return 0;
}

void end_module(void){ //rmmod명령어를 통해 모듈이 종료되기 전에(커널에서 제거될때) 호출된다.
	dev_t devno = MKDEV(GPIO_MAJOR,GPIO_MINOR);
	unregister_chrdev_region(devno, 1);	//사용중인 디바이스 번호 해제
	cdev_del(&gpio_cdev); 
	
	if(gpio)	
		iounmap(gpio); //물리메모리에 직접 접근하여 해제한다
	printk(KERN_INFO "END MODULE\n");
}
static int gpio_open(struct inode *inode, struct file *file){
	try_module_get(THIS_MODULE); //디바이스 드라이버를 작성할때 사용횟수를 직접관리해야하는경우 사용횟수 증가를 위한 함수
	printk("OPEN - gpio device\n");
	return 0;
}

static int gpio_release(struct inode* inode, struct file* file){
	module_put(THIS_MODULE); //디바이스 드라이버를 작성할때 사용횟수를 직접관리해야하는경우 사용횟수 감소를 위한 함수
	printk("CLOSE - gpio device\n");
	return 0;
}

static ssize_t gpio_read(struct file* file, char* buf, size_t len, loff_t* off){ //디바이스에서 자료읽음
	int cnt;
	//int copy_to_user(void user* to,const void* from, unsigned long n):
	//from이 가리키는 주소의 커널메모리 블록데이터를 to가 가리키는 사용자 메모리블록 데이터에 n 만큼 써넣음
	// 써넣은 공간의 유효성 검사 수행
	cnt = copy_to_user(buf, msg, strlen(msg)+1); 
	printk("GPIO device READ: %s \n", msg);
	return cnt;
}

static ssize_t gpio_write(struct file* file, const char* buf, size_t len, loff_t* off){ //자료를 디바이스로 보냄
	short cnt;
	memset(msg, 0, BLOCK_SIZE); //메모리를 특정값으로 초기화하는 함수
	//void* memset(void* s,int c,size_t n) :c문자로 메모리블록의 n바이트를 지정 

	//int copy_from_user(void* to,const void user* from,unsigned long n):
	// from이 가리키는 주소의 사용자 메모리블록데이터를 to가 가리키는 커널메모리블록데이터에 n 만큼 써넣음
	cnt = copy_from_user(msg, buf, len);
	(!strcmp(msg, "0")) ? GPIO_CLR(GPIO_NUM):GPIO_SET(GPIO_NUM);
	printk("gpio device WRITE: %s \n", msg);
	return cnt;
}

 
MODULE_LICENSE("GPL"); //커널에게 모듈이 어떤 라이센스 하에서 커널을 이용하게 되는지를 알려주는 역할,라이센스에 따라서 사용할수있는 변수등에 제한생김
module_init(start_module); //커널에게 모듈이 로딩되었을때 호출되어야하는 함수를 알려주는역할->모듈이 하게되는 모든일들이 이 초기화함수가 호출하는 함수에 의해 처리됨
module_exit(end_module); //exit키워드가 커널에게 모듈의 언로딩시에 이 함수를 한번만 호출해야함을 알려준다


