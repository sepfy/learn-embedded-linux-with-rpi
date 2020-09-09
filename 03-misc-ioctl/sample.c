#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h> // for open
#include <unistd.h> // for close

#define IOCTL_MISCDEV_SET 0x00
#define IOCTL_MISCDEV_GET 0x01

struct miscdev_data {
    int val;
    char data[64];
};

int main(void) {

    int fd, ret;
    char buf[128] = {0};
    struct miscdev_data data;

    fd = open("/dev/misc_dev", O_RDWR);
    if(fd < 0)
        perror("open");


    sprintf(buf, "User says hello\n");
    write(fd, buf, 128);

    read(fd, buf, 128);
    printf("Read data frm kenrel: %s\n", buf);

    memset(&data, 0, sizeof(data));
    data.val = 10;
    sprintf(data.data, "User says hi");

    ret = ioctl(fd, IOCTL_MISCDEV_SET, &data);
    if(ret < 0) {
        perror("ioctl set");
    }

    ret = ioctl(fd, IOCTL_MISCDEV_GET, &data);
    if(ret < 0) {
        perror("ioctl get");
    }
    
    printf("Get data: miscdata_data.val = %d, miscdata_data.data = %s\n", data.val, data.data);

#if 0
        cmd.val = 0xCC;
        ret = ioctl(fd, IOCTL_VALSET, &cmd);
        if (ret == -1) {
                printf("errno %d\n", errno);
                perror("ioctl");
        }

 ret = ioctl(fd, IOCTL_VALGET, &cmd);

        if (ret == -1) {
                printf("errno %d\n", errno);
                perror("ioctl");
        }
        printf("val %x\n", cmd.val);


 num = 100;
 ret = ioctl(fd, IOCTL_VALSET_NUM, num);
        if (ret == -1) {
                printf("errno %d\n", errno);
                perror("ioctl");
        }

 ret = ioctl(fd, IOCTL_VALGET_NUM, &num);

        if (ret == -1) {
                printf("errno %d\n", errno);
                perror("ioctl");
        }
        printf("num %d\n", num);
#endif

    if(fd > 0)
        close(fd);

    return 0;
}
