#ifndef MPP_DECODER
#define MPP_DECODER

#include <rockchip/mpp_buffer.h>
#include <rockchip/rk_mpi.h>
#include <rockchip/mpp_frame.h>
#include "v4l2.h"
#include "safely_queue.h"

struct frameDataFromLive555{
    uint8_t *data;
    size_t bytes;
    unsigned long int presentationTime;
};

class mppDecoder{
public:
    mppDecoder();

    MPP_RET decoder_process(void * data,size_t byteSize,unsigned long int presentationTime);

    MPP_RET decoder_queue();

    void push_data(void * data,size_t byteSize,unsigned long int presentationTime);

    frameDataFromLive555* pop_data();

    void Start();

    int queue_length(){
        return frameData->length();
    };

    ~mppDecoder();

    void decoderClose();

    volatile int decoder_live = 1;
private:
    MppApi * mppApi;
    MppCtx mppCtx;
    MppDecCfg mppCfg;
    
    MppBuffer h264Buffer;
    MppFrame yuvMppFrame;
    MppPacket h264Packet;

    MppBufferGroup bufferGroup;

    v4l2Display * display;

    int need_split = 1;     /**是否需要编码器自行分帧*/
    int enable_fast = 1;    /**是否支持快速输出模式*/
    int immediate_out = 1;

    pthread_t thread;
    ConcurrenceQueue<frameDataFromLive555> *frameData; 
};

#endif