#include "mpp_decoder.h"
#include "stdio.h"
#include<opencv2/opencv.hpp>
#include <pthread.h>
#include <signal.h>

#define CHECK_MPP(RES) {              \
    if(RES != MPP_OK){                \
        printf("[ERROR] Mpp %d",RES); \
    }                                 \
}                                     \

extern long seconds(){
    timeval t;
    gettimeofday(&t,NULL);
    return t.tv_sec * 1e6 + t.tv_usec;
}
/**
 * init mpp decoder
*/
mppDecoder::mppDecoder():display(nullptr),frameData(nullptr){
    MPP_RET res = MPP_OK;

    res = mpp_create(&mppCtx,&mppApi);

    // res = mpp_buffer_group_get_external(&bufferGroup,MPP_BUFFER_TYPE_DRM);

    res = mppApi->control(mppCtx,MPP_DEC_SET_PARSER_SPLIT_MODE,&need_split);

    // res = mppApi->control(mppCtx,MPP_DEC_SET_ENABLE_FAST_PLAY,&enable_fast);
    MppFrameFormat out = MPP_FMT_YUV420P;
    
    res = mpp_init(mppCtx,MPP_CTX_DEC,MPP_VIDEO_CodingAVC); /**H264解码器*/
    
    res = mppApi->control(mppCtx,MPP_DEC_SET_OUTPUT_FORMAT,&out); /**输出格式 YUV420P*/

    // res = mppApi->control(mppCtx,MPP_DEC_SET_EXT_BUF_GROUP,bufferGroup);

    // res = mppApi->control(mppCtx, MPP_DEC_SET_IMMEDIATE_OUT, &immediate_out); /**立即输出解码图像*/

    // res = mppApi->control(mppCtx,MPP_DEC_SET_DISABLE_ERROR,NULL);
    
    res = mpp_dec_cfg_init(&mppCfg);
    res = mppApi->control(mppCtx,MPP_DEC_GET_CFG,mppCfg);
    
    // mpp_dec_cfg_set_u32(mppCfg, "base:split_parse", need_split);

    res = mppApi->control(mppCtx,MPP_DEC_SET_CFG,mppCfg);
    // sdl_init_process();
    
    res = mpp_frame_init(&yuvMppFrame);
    printf("************************************* \n");
    printf("*     init mpp decode finished !    * \n");
    printf("************************************* \n");

    // frameData = new ConcurrenceQueue<frameDataFromLive555>();
}

mppDecoder::~mppDecoder(){
    if(h264Packet){
        mpp_packet_deinit(&h264Packet);
        printf("[INFO] destroy mpp packet \n");
    }
        
    if(yuvMppFrame){
        mpp_frame_deinit(&yuvMppFrame);
        printf("[INFO] destroy mpp frame \n");
    }

    if(bufferGroup){
        mpp_buffer_group_put(bufferGroup);
        printf("[INFO] destroy mpp buffer group \n");
        bufferGroup = NULL;
    }

    if(mppCfg){
        mpp_dec_cfg_deinit(mppCfg);
        printf("[INFO] destroy mpp config \n");
    }
    
    if(mppCtx){
        printf("[INFO] %x destroy mpp context \n",mppCtx);
        mpp_destroy(mppCtx);
        mppCtx = NULL;
    }

    if(display){
        delete display;
        display = NULL;
    }
}

void mppDecoder::push_data(void * data,size_t byteSize,unsigned long int presentationTime){
    frameDataFromLive555 frame555;
    frame555.data = (uint8_t * )malloc(byteSize);
    frame555.bytes = byteSize;
    frame555.presentationTime = presentationTime;
    frameData->push(frame555);
}

frameDataFromLive555* mppDecoder::pop_data(){
    return frameData->pop().get();
}

MPP_RET mppDecoder::decoder_process(void * data,size_t byteSize,unsigned long int presentationTime){
    MPP_RET res = MPP_OK;
    long start = seconds();
    /**
     * 数据封装进mppPacket
    */
    mpp_frame_init(&yuvMppFrame);

    res = mpp_packet_init(&h264Packet,data,byteSize);
    
    mpp_packet_set_data(h264Packet,data);
    mpp_packet_set_size(h264Packet,byteSize);
    mpp_packet_set_pos(h264Packet,data);
    mpp_packet_set_length(h264Packet,byteSize);
    mpp_packet_set_pts(h264Packet,presentationTime*1000);
    int flag = 1;
    int frameEos=0;
    int width,height;
    res = mppApi->decode_put_packet(mppCtx,h264Packet);

    res = mppApi->decode_get_frame(mppCtx,&yuvMppFrame);
    // cv::Mat yuvImage,rgbImage;
    // printf("\n decode get frame %d yuvMpp Frame address %x pack size %u \n",res,yuvMppFrame,byteSize);
    if(yuvMppFrame){
        if(mpp_frame_get_info_change(yuvMppFrame)){
            /**
             * 初始化解码器，获取帧的长宽，以及stride信息。
             * 计算所需要解码的尺寸大小。重新设置bufferGroup
            */
            width = mpp_frame_get_width(yuvMppFrame);
            height = mpp_frame_get_height(yuvMppFrame);
            int hor_stride = mpp_frame_get_hor_stride(yuvMppFrame);
            int ver_stride = mpp_frame_get_ver_stride(yuvMppFrame);
            int bufferSize = mpp_frame_get_buf_size(yuvMppFrame);
            printf("mpp frame get info change :\n");
            printf("Frame Width : %d \n",width);
            printf("Frame Height : %d \n",height);
            printf("Frame hor_stride : %d \n",hor_stride);
            printf("Frame ver_stride : %d \n",ver_stride);
            printf("Frame buffer size : %d \n",bufferSize);
            
            // 初始化显示控件
            if(display != NULL){
                delete display;
            }
            display = new v4l2Display(width,height,hor_stride,ver_stride);
            // yuvImage = cv::Mat(cv::Size(width,height),CV_8UC2);
            // 重置buffer Group 并重新设置buffer的大小和个数 此处只能使用internal bufferGroup使用extr会报错
            res = mpp_buffer_group_get_internal(&bufferGroup,MPP_BUFFER_TYPE_DRM);
            CHECK_MPP(res);
            res = mpp_buffer_group_limit_config(&bufferGroup, bufferSize, 24);
            // res = mpp_buffer_group_clear(bufferGroup); 
            CHECK_MPP(res);
            // res = mpp_buffer_group_limit_config(bufferGroup,bufferSize,24);
            // CHECK_MPP(res);
            mppApi->control(mppCtx, MPP_DEC_SET_INFO_CHANGE_READY, NULL);
            // flag = 0;
        }else{
            
            RK_U32 errinfo = mpp_frame_get_errinfo(yuvMppFrame);
            RK_U32 discard = mpp_frame_get_discard(yuvMppFrame);

            if (errinfo || discard) {
                printf("errod %x discard %x \n",errinfo,discard);
            }
            // 显示该帧
            if(display == nullptr){
                printf("[ERROR] display is uninit \n");
                return MPP_ERR_UNKNOW;
            }
            MppBuffer yuvMppBuffer = mpp_frame_get_buffer(yuvMppFrame);
            size_t bytes = mpp_buffer_get_size(yuvMppBuffer);
            // printf("copy date %lu \n",bytes);

            uint8_t * data = (uint8_t * )mpp_buffer_get_ptr(yuvMppBuffer);
            // int fd = open("./frame.yuv",O_CREAT | O_RDWR,777);
            // write(fd,data,bytes);
            // close(fd);
            // yuvImage = cv::Mat(cv::Size(width,height),CV_8UC2);
            display->display(data,bytes);
            // long end = seconds();
            // long t = 40 - ((end - presentationTime) / 1000);
            // if(t > 0){
            //     printf("sleep [%d] ms \n",t);
            //     usleep(t * 1000);
            // }
            // memcpy(yuvImage.data,data,bytes);
            // cv::cvtColor(yuvImage,rgbImage,cv::COLOR_YUV2BGR);
            // // cv::imshow("TV",yuvImage);
            // cv::imwrite("./rgbImage.jpg",rgbImage);
            // cv::waitKey(1);

            // flag = 0;

        }
        frameEos = mpp_frame_get_eos(yuvMppFrame);
        mpp_frame_deinit(&yuvMppFrame);
    }
        
        // if(frameEos){
        //     flag = 0;
        // }
    mpp_packet_deinit(&h264Packet);
}


MPP_RET mppDecoder::decoder_queue(){
    printf("queue size %d\n",frameData->length());
    MPP_RET res = MPP_OK;
    if(frameData->empty()){
        usleep(10000);
        return MPP_OK;
    }
    frameDataFromLive555* frame555Data = pop_data();
    /**
     * 数据封装进mppPacket
    */
    res = mpp_packet_init(&h264Packet,frame555Data->data,frame555Data->bytes);

    mpp_packet_set_data(h264Packet,frame555Data->data);
    mpp_packet_set_size(h264Packet,frame555Data->bytes);
    mpp_packet_set_pos(h264Packet,frame555Data->data);
    mpp_packet_set_length(h264Packet,frame555Data->bytes);
    mpp_packet_set_pts(h264Packet,frame555Data->presentationTime * 1000);
retry:
    res = mppApi->decode_put_packet(mppCtx,h264Packet);
    if(res != MPP_SUCCESS){
        usleep(100000);
        goto retry;
    }
    printf("\n decode_put_packet %d pack size %u \n",res,frame555Data->bytes);

    res = mppApi->decode_get_frame(mppCtx,&yuvMppFrame);

    printf("\n decode_get_frame %d pack size %u \n",res,frame555Data->bytes);
    if(yuvMppFrame){
        if(mpp_frame_get_info_change(yuvMppFrame)){
            /**
             * 初始化解码器，获取帧的长宽，以及stride信息。
             * 计算所需要解码的尺寸大小。重新设置bufferGroup
            */
            int width = mpp_frame_get_width(yuvMppFrame);
            int height = mpp_frame_get_height(yuvMppFrame);
            int hor_stride = mpp_frame_get_hor_stride(yuvMppFrame);
            int ver_stride = mpp_frame_get_ver_stride(yuvMppFrame);
            int bufferSize = mpp_frame_get_buf_size(yuvMppFrame);
            printf("mpp frame get info change :\n");
            printf("Frame Width : %d \n",width);
            printf("Frame Height : %d \n",height);
            printf("Frame hor_stride : %d \n",hor_stride);
            printf("Frame ver_stride : %d \n",ver_stride);
            printf("Frame buffer size : %d \n",bufferSize);

            // 初始化显示控件
            // if(display != NULL){
            //     delete display;
            // }
            // display = new v4l2Display(width,height,hor_stride,ver_stride);
            // 重置buffer Group 并重新设置buffer的大小和个数
            res = mpp_buffer_group_clear(bufferGroup); 
            CHECK_MPP(res);
            res = mpp_buffer_group_limit_config(bufferGroup,bufferSize,24);
            CHECK_MPP(res);
            res = mppApi->control(mppCtx, MPP_DEC_SET_INFO_CHANGE_READY, NULL);
            CHECK_MPP(res);
        }else{
            
            RK_U32 errinfo = mpp_frame_get_errinfo(yuvMppFrame);
            RK_U32 discard = mpp_frame_get_discard(yuvMppFrame);

            if (errinfo || discard) {
                printf("errod %x discard %x \n",errinfo,discard);
            }
            // 显示该帧
            if(display == nullptr){
                printf("[ERROR] display is uninit \n");
                return MPP_ERR_UNKNOW;
            }
            MppBuffer yuvMppBuffer = mpp_frame_get_buffer(yuvMppFrame);
            size_t bytes = mpp_buffer_get_size(yuvMppBuffer);
            printf("copy date %lu \n",bytes);
            uint8_t * data = (uint8_t * )mpp_buffer_get_ptr(yuvMppBuffer);
            // display->display(data,bytes);
        }
    }
    mpp_packet_deinit(&h264Packet);

    free(frame555Data->data);
}

void mppDecoder::decoderClose(){
    decoder_live = 0;
}

static void * thread_decoder(void * mpp){
    mppDecoder * decoder =  (mppDecoder *)mpp;
    while(decoder->decoder_live){
        decoder->decoder_queue();
    }
    
    return NULL;
}

void mppDecoder::Start(){
    pthread_t thread;
    pthread_create(&thread,NULL,thread_decoder,this);
    pthread_detach(thread);
}