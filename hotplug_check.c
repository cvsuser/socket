#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <limits.h>
#include <linux/input.h>
#include <linux/types.h>
#include <mntent.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
//#include <sys/mount.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <linux/netlink.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>

#include <ctype.h>
#include <sys/klog.h>
#include <syslog.h>
#include <pthread.h>

#define PID_FILE                "./usbmounter.pid"
#define PRINT_ON 1
#ifdef PRINT_ON
#define ALOGD(x...) printf(x)
#else
#define ALOGD(x...)
#endif

int init_socket(void);
int process_data(int sockfd);

int init_socket(void)
{
	int sock = 0;
    struct sockaddr_nl snl;
	//setlinebuf(stdout);
    memset(&snl, 0x00, sizeof(struct sockaddr_nl));
    snl.nl_family = 16;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;

    sock = socket(16, SOCK_DGRAM, 15);
    if (sock == -1)
    {
        ALOGD("error getting socket: %s", strerror(errno));
        return -1;
    }

    int ret = bind(sock, (struct sockaddr *) &snl, sizeof(struct sockaddr_nl));
    if (ret < 0) {
        ALOGD("bind failed: %s", strerror(errno));
        close(sock);
        sock = -1;
    }
    return sock;
}
#define DEVICENAME "rt10up"
#define ADDDEVICES "add@/devices"
#define REMOVEDEVICES "remove@/devices/"
int test(){
	int try_times = 3;
	int fd;
	while((access("/dev/rt10up", F_OK) != 0))
	{  
		try_times--;
		sleep(1);
	}
	if(try_times == 0)
	{
		printf("error  <%d>\n",__LINE__);
		return -1;
	}
	if ((fd = open("/dev/rt10up", O_RDWR)) < 0)
	{
		printf("error  <%d>\n",__LINE__);
		return -1;
	}else{
		printf("/dev/rt10up test OK!\n");
		close(fd);
	}
	return 0;
}
int process_data(int fd)
{
	char buf[4096] = {0};
	int len1 = strlen(ADDDEVICES);
	int len2 = strlen(REMOVEDEVICES);
	
	read(fd, &buf, sizeof(buf));
	ALOGD("Received: %s\n", buf);
	
	if (!strncasecmp(buf, ADDDEVICES,len1) && strstr(buf, DEVICENAME)) {
		ALOGD("ADD %s\n", DEVICENAME);
		test();
	} else if (!strncasecmp(buf, REMOVEDEVICES,len2) && strstr(buf, DEVICENAME)) {
		//extract_dev_no(buf, devName);
		ALOGD("REMOVE %s\n", DEVICENAME);
	}
	
	return 0;
}
#include <cutils/properties.h>
int main(int argc,char *argv[])
{
	int rc, socket_usb;	
	int fd,fdtablesize;
	int fp;
	test();
	char g_dump_file_path[1024] = {0};
	if (property_get("check.rt10up", g_dump_file_path, NULL)) {
		ALOGD("1 g_dump_file_path: [%s]\n",g_dump_file_path);
		//return NU_PLAYER;
	}
	ALOGD("2 g_dump_file_path: [%s]\n",g_dump_file_path);
	/*
	rc = log_pid();
	switch (rc) {
		case 1: 
			fprintf(stderr, "%s is runing, kill it and remove %s ...\n", argv[0], PID_FILE);
			exit(1);
		break;
		case -1: 
			fprintf(stderr, "Cannot save %s\n", PID_FILE);
			exit(-1);
		break;
	}
	*/
	//if ((argc < 2) || strncmp(argv[1], "-d", 2)) 
	if(0){
	// Run in deamon mode
	//屏蔽一些有关控制终端操作的信号。防止在守护进程没有正常运转起来时，控制终端受到
	//干扰 退出或挂起，此处忽略了终端I/O信号、STOP信号
		 signal(SIGTTOU,SIG_IGN);
		 signal(SIGTTIN,SIG_IGN);
		 signal(SIGTSTP,SIG_IGN);
		 signal(SIGHUP,SIG_IGN);
	//由于子进程会继承父进程的某些特性，如控制终端、登录会话、进程组等，而守护进程
	//最终要脱离控制终端到后台去运行，所以必须把父进程杀掉，以确保子进程不是进程组长，
	//这也是成功调用setsid()所要求的。
		 if (fork() != 0)
		 {
			syslog(LOG_USER|LOG_INFO,"Run to Line %d\n", __LINE__);
			exit(1);
		 }
	//脱离了控制终端还要脱离登录会话和进程组，这里可以调用setsid()函数，调用成功后
	//进程成为新的会话组长和新的进程组长，并与原来的登录会话和进程组脱离，由于会话过程
	//对控制终端的独占性，进程同时与控制终端脱离。
	
		 //if 
		 setsid();
		 {
			syslog(LOG_USER|LOG_INFO,"Run to Line %d\n", __LINE__);
//			exit(1);
		 }

		 //要达到setsid()函数的功能也可以用如下处理方法。"/dev/tty"是一个流设备，也是终端映射，调用close()函数将终端关闭。
		 if ((fp = open("/dev/tty", O_RDWR)) >= 0)
		 {
			 ioctl(fp, TIOCNOTTY, NULL);
			 close(fp);
		 }

		/*从父进程继承过来的当前工作目录可能在一个装配的文件系统中。因为守护进程通常在系统重启之前是一直存在的，
		所以如果守护进程的当前工作目录在一个装配文件系统中，那么该文件系统就不能被卸载。
		比如说从父进程继承的当前目录是/mnt下面的一个被挂载的目录。
		*/
		 if (chdir("/tmp") == -1) {
			syslog(LOG_USER|LOG_INFO, "Run to Line %d\n", __LINE__);
			exit(1);
		 }
		 /*关闭打开的文件描述符，或重定向标准输入、标准输出和标准错误输出的文件描述符。
		 进程从创建它的父进程那里继承了打开的文件描述符。如果不关闭，将会浪费系统资源，
		 引起无法预料的错误。getdtablesize()返回某个进程所能打开的最大的文件数。*/
		for (fd=0, fdtablesize = getdtablesize(); fd < fdtablesize; fd++)
			close(fd);

		 //有的程序有些特殊的需求，还需要将这三者重新定向。
	/*
		 error=open("/tmp/error",O_WRONLY|O_CREAT,0600);
		 dup2(error,2);
		 close(error);
		 in=open("/tmp/in",O_RDONLY|O_CREAT,0600);
		 if(dup2(in,0)==-1) perror("in");
		 close(in);
		 out=open("/tmp/out",O_WRONLY|O_CREAT,0600);
		 if(dup2(out,1)==-1) perror("out");
		 close(out);
	*/
		 /* 由继承得来的文件方式创建的屏蔽字可能会拒绝设置某些权限，所以要重新赋于所有权限。 */
		 umask(0);

		/*如果父进程不等待子进程结束，子进程将成为僵尸进程（zombie）从而占用系统资源，
		如果父进程等待子进程结束，将增加父进程的负担，影响服务器进程的并发性能。因此需要对SIGCHLD信号做出处理，
		回收僵尸进程的资源，避免造成不必要的资源浪费。 */
		signal(SIGCHLD,SIG_IGN);
	}


	socket_usb = init_socket();

	while (1) {
		process_data(socket_usb);
	}
	close(socket_usb);
	unlink(PID_FILE);
	return 0;
}
