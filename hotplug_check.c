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
	//����һЩ�йؿ����ն˲������źš���ֹ���ػ�����û��������ת����ʱ�������ն��ܵ�
	//���� �˳�����𣬴˴��������ն�I/O�źš�STOP�ź�
		 signal(SIGTTOU,SIG_IGN);
		 signal(SIGTTIN,SIG_IGN);
		 signal(SIGTSTP,SIG_IGN);
		 signal(SIGHUP,SIG_IGN);
	//�����ӽ��̻�̳и����̵�ĳЩ���ԣ�������նˡ���¼�Ự��������ȣ����ػ�����
	//����Ҫ��������ն˵���̨ȥ���У����Ա���Ѹ�����ɱ������ȷ���ӽ��̲��ǽ����鳤��
	//��Ҳ�ǳɹ�����setsid()��Ҫ��ġ�
		 if (fork() != 0)
		 {
			syslog(LOG_USER|LOG_INFO,"Run to Line %d\n", __LINE__);
			exit(1);
		 }
	//�����˿����ն˻�Ҫ�����¼�Ự�ͽ����飬������Ե���setsid()���������óɹ���
	//���̳�Ϊ�µĻỰ�鳤���µĽ����鳤������ԭ���ĵ�¼�Ự�ͽ��������룬���ڻỰ����
	//�Կ����ն˵Ķ�ռ�ԣ�����ͬʱ������ն����롣
	
		 //if 
		 setsid();
		 {
			syslog(LOG_USER|LOG_INFO,"Run to Line %d\n", __LINE__);
//			exit(1);
		 }

		 //Ҫ�ﵽsetsid()�����Ĺ���Ҳ���������´�������"/dev/tty"��һ�����豸��Ҳ���ն�ӳ�䣬����close()�������ն˹رա�
		 if ((fp = open("/dev/tty", O_RDWR)) >= 0)
		 {
			 ioctl(fp, TIOCNOTTY, NULL);
			 close(fp);
		 }

		/*�Ӹ����̼̳й����ĵ�ǰ����Ŀ¼������һ��װ����ļ�ϵͳ�С���Ϊ�ػ�����ͨ����ϵͳ����֮ǰ��һֱ���ڵģ�
		��������ػ����̵ĵ�ǰ����Ŀ¼��һ��װ���ļ�ϵͳ�У���ô���ļ�ϵͳ�Ͳ��ܱ�ж�ء�
		����˵�Ӹ����̼̳еĵ�ǰĿ¼��/mnt�����һ�������ص�Ŀ¼��
		*/
		 if (chdir("/tmp") == -1) {
			syslog(LOG_USER|LOG_INFO, "Run to Line %d\n", __LINE__);
			exit(1);
		 }
		 /*�رմ򿪵��ļ������������ض����׼���롢��׼����ͱ�׼����������ļ���������
		 ���̴Ӵ������ĸ���������̳��˴򿪵��ļ���������������رգ������˷�ϵͳ��Դ��
		 �����޷�Ԥ�ϵĴ���getdtablesize()����ĳ���������ܴ򿪵������ļ�����*/
		for (fd=0, fdtablesize = getdtablesize(); fd < fdtablesize; fd++)
			close(fd);

		 //�еĳ�����Щ��������󣬻���Ҫ�����������¶���
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
		 /* �ɼ̳е������ļ���ʽ�����������ֿ��ܻ�ܾ�����ĳЩȨ�ޣ�����Ҫ���¸�������Ȩ�ޡ� */
		 umask(0);

		/*��������̲��ȴ��ӽ��̽������ӽ��̽���Ϊ��ʬ���̣�zombie���Ӷ�ռ��ϵͳ��Դ��
		��������̵ȴ��ӽ��̽����������Ӹ����̵ĸ�����Ӱ����������̵Ĳ������ܡ������Ҫ��SIGCHLD�ź���������
		���ս�ʬ���̵���Դ��������ɲ���Ҫ����Դ�˷ѡ� */
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
