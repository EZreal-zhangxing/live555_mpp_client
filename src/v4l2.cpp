#include "v4l2.h"
#include "errno.h"

void modeset_page_flip_handler(int fd, uint32_t frame,uint32_t sec, uint32_t usec,void *data){
	// printf("*******************modeset_page_flip_handler*****************************\n");
    uint32_t crtc_id = *(uint32_t *)data;
	int res = drmModePageFlip(fd, crtc_id, buf[bufIdx].fb_id,DRM_MODE_PAGE_FLIP_EVENT, data);
	if(res < 0){
		printf("%s %d \n",strerror(res),res);
	}
}
v4l2Display::v4l2Display(){
	int res = 0;

    ev.version = DRM_EVENT_CONTEXT_VERSION;
	ev.page_flip_handler = modeset_page_flip_handler;

    fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);	//打开card0，card0一般绑定HDMI和LVDS
	resources = drmModeGetResources(fd);	//获取drmModeRes资源,包含fb、crtc、encoder、connector等
	
	crtcId = resources->crtcs[0];			//获取crtc id
	connId = resources->connectors[0];		//获取connector id
	connector = drmModeGetConnector(fd, connId);	//根据connector_id获取connector资源

	/*初始化资源*/
	int display_h = connector->modes[v4l2Idx].hdisplay; // 水平
	int display_v = connector->modes[v4l2Idx].vdisplay; // 垂直

	create_fb(fd,display_h,display_v,buf);
	create_fb(fd,display_h,display_v,buf+1);
	printf(" display size %d %d buf 0 size %d \n",image_width,image_height,buf[0].size);
	printf(" display size %d %d buf 1 size %d \n",image_width,image_height,buf[1].size);

	res = drmModeSetCrtc(fd, crtcId,buf[0].fb_id,0, 0, &connId, 1, &connector->modes[v4l2Idx]);	//初始化和设置crtc，对应显存立即刷新
	printf("%d ,%s \n",res,strerror(res));
	res = drmModePageFlip(fd, crtcId, buf[0].fb_id,DRM_MODE_PAGE_FLIP_EVENT, &crtcId);
	printf("%d ,%s \n",res,strerror(res));
	
    printf("************************************* \n");
    printf("*     init v4l2 display finished !  * \n");
    printf("************************************* \n");

    
}

v4l2Display::v4l2Display(int image_width,int image_height,int image_hor_stride,int image_ver_stride) 
	: image_width(image_width),image_height(image_height),image_hor_stride(image_hor_stride),image_ver_stride(image_ver_stride) {

	int res = 0;
	
	ev.version = DRM_EVENT_CONTEXT_VERSION;
	ev.page_flip_handler = modeset_page_flip_handler;

    fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);	//打开card0，card0一般绑定HDMI和LVDS
	resources = drmModeGetResources(fd);	//获取drmModeRes资源,包含fb、crtc、encoder、connector等
	
	crtcId = resources->crtcs[0];			//获取crtc id
	connId = resources->connectors[0];		//获取connector id
	connector = drmModeGetConnector(fd, connId);	//根据connector_id获取connector资源

	/**
	 * connector 中modes是一个列表 分别对应不同的分辨率。因此要注意在此处选取第7个模式后
	 * 显示器分辨率是1920x1080,而视频解码后的分辨率同样也是1920x1080
	 * 后面 drmModeSetCrtc 设置modes的模式要和这里一样否则会报-28 (分配空间不足)错误
	 * 该列表可以使用modetest -c 查看
	 * 见https://doc.embedfire.com/linux/rk356x/driver/zh/latest/linux_driver/framework_drm.html#id4
  	 * 在使用mpp解码后输出的时候，请确保解码的图像大小和此处显示的大小是一致的！
	 */ 
	/*初始化资源*/
	int display_h = connector->modes[v4l2Idx].hdisplay; // 水平
	int display_v = connector->modes[v4l2Idx].vdisplay; // 垂直

	create_fb(fd,display_h,display_v,buf);
	create_fb(fd,display_h,display_v,buf+1);
	printf(" display size %d %d buf 0 size %d \n",image_width,image_height,buf[0].size);
	printf(" display size %d %d buf 1 size %d \n",image_width,image_height,buf[1].size);

	res = drmModeSetCrtc(fd, crtcId,buf[0].fb_id,0, 0, &connId, 1, &connector->modes[v4l2Idx]);	//初始化和设置crtc，对应显存立即刷新
	printf("%d ,%s \n",res,strerror(res));
	res = drmModePageFlip(fd, crtcId, buf[0].fb_id,DRM_MODE_PAGE_FLIP_EVENT, &crtcId);
	printf("%d ,%s \n",res,strerror(res));
    printf("************************************* \n");
    printf("*     init v4l2 display finished !  * \n");
    printf("************************************* \n");

}



void v4l2Display::create_fb(int fd,uint32_t width,uint32_t height,framebuffer * buf){
	struct drm_mode_create_dumb create = {};
 	struct drm_mode_map_dumb map = {};
	/**YUV*/
    create.width = image_hor_stride;
	create.height = image_ver_stride * 2;
	create.bpp = 8;

    drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &create);	//创建显存,返回一个handle

    uint32_t handles[4] = {0}, pitches[4] = {0}, offsets[4] = {0};
	uint32_t fb_id;

	/*Y*/
    handles[0] = create.handle;
    pitches[0] = image_hor_stride;
    offsets[0] = 0;

	/*UV*/
    handles[1] = create.handle;
    pitches[1] = image_hor_stride;
    offsets[1] = image_hor_stride * image_ver_stride;

	// drmModeAddFB(fd, create.width, create.height, 24, 32, create.pitch,create.handle, &fb_id); 
	drmModeAddFB2(fd,width,height,DRM_FORMAT_NV12,handles,pitches,offsets,&fb_id,0);

	map.handle = create.handle;
	drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &map);	//显存绑定fd，并根据handle返回offset
 
	//通过offset找到对应的显存(framebuffer)并映射到用户空间
	uint8_t *vaddr = (uint8_t *)mmap(0, create.size, PROT_READ | PROT_WRITE,MAP_SHARED, fd, map.offset);	

 
	// for (i = 0; i < (create.size / 4); i++)
	// 	vaddr[i] = color;

	buf->vaddr=vaddr;
	buf->fb_id=fb_id;
    buf->size = create.size;
    buf->handle = create.handle;
}

/**
 * Thread 4 "main" received signal SIGSEGV, Segmentation fault.
	[Switching to Thread 0x7ff60091c0 (LWP 3460)]
	__memcpy_generic () at ../sysdeps/aarch64/multiarch/../memcpy.S:200
	200     ../sysdeps/aarch64/multiarch/../memcpy.S: No such file or directory.
	(gdb) bt
	#0  __memcpy_generic () at ../sysdeps/aarch64/multiarch/../memcpy.S:200
	#1  0x0000005555570558 in v4l2Display::display (this=0x7ff0002070, data=0x7ff4615000, byteSize=4177920) at /home/firefly/mpp/live_client_mpp/src/v4l2.cpp:135
	#2  0x0000005555569cd8 in mppDecoder::decoder_queue (this=0x55556134a0) at /home/firefly/mpp/live_client_mpp/src/mpp_decoder.cpp:296
	#3  0x0000005555569d78 in thread_decoder (mpp=0x55556134a0) at /home/firefly/mpp/live_client_mpp/src/mpp_decoder.cpp:312
	#4  0x0000007ff7dc4624 in start_thread (arg=0x5555569d40 <thread_decoder(void*)>) at pthread_create.c:477
	#5  0x0000007ff77c549c in thread_start () at ../sysdeps/unix/sysv/linux/aarch64/clone.S:78

	TODO
*/
void v4l2Display::display(void * data,size_t byteSize){
	bufIdx ^= 1;
    uint8_t * addr = buf[bufIdx].vaddr;
    memmove(addr,data,byteSize);

	int res = 0;
	
	// 事件轮询等待 但是失效
	res = drmHandleEvent(fd,&ev);
	// if(!is_start){
		// ENAVAIL
		// res = drmModePageFlip(fd, crtcId, buf[bufIdx].fb_id,DRM_MODE_PAGE_FLIP_EVENT, &crtcId);
	// 	is_start = 1;
	// }
	if(res < 0){
		printf("%s %d \n",strerror(res),res);
	}
	
}

void v4l2Display::release_fb(int fd, framebuffer *buf)
{
	struct drm_mode_destroy_dumb destroy = {};
	destroy.handle = buf->handle;
 
	drmModeRmFB(fd, buf->fb_id);
	munmap(buf->vaddr, buf->size);
	drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy);
}

v4l2Display::~v4l2Display(){
	printf("[INFO] destroty v4l2Display \n");
	release_fb(fd, &buf[0]);
    release_fb(fd, &buf[1]);
    drmModeFreeConnector(connector);
    drmModeFreeResources(resources);
	memset(buf,0,sizeof(framebuffer));
	memset(buf+1,0,sizeof(framebuffer));
	close(fd);
}


