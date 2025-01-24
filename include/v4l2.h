#ifndef V4L2_H
#define V4L2_H
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm_fourcc.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

class framebuffer{
public:
	uint32_t size; // 大小
	uint32_t handle; // 句柄
	uint32_t fb_id;
	uint8_t *vaddr;	 // 虚地址
};

static framebuffer buf[2]; // 双缓冲刷屏
static int bufIdx = 0;
/**
 * v4l2 显示
*/
class v4l2Display{
public:
    v4l2Display();
    v4l2Display(int image_width,int image_height,int image_hor_stride,int image_ver_stride);
    ~v4l2Display();
    
    void create_fb(int fd,uint32_t width,uint32_t heifght,framebuffer * buf);
    void display(void * data,size_t byteSize);
    void release_fb(int fd, struct framebuffer *buf);
private:
    int fd;
    uint32_t crtcId;
    uint32_t connId;
    drmEventContext ev;
    drmModeConnector * connector;
    drmModeRes * resources;

    int image_width;
    int image_height;
    int image_hor_stride;
    int image_ver_stride;

    int v4l2Idx = 6; // 根据显示器分辨率的不同请选择不同的分辨率

    int is_start = 0;
};

inline void modeset_page_flip_handler(int fd, uint32_t frame,uint32_t sec, uint32_t usec,void *data); // 翻页处理函数

#endif