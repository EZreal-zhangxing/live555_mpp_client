#include "rtsp_client_live555.h"

#include "signal.h"

char eventLoopWatchVariable = 0;

void processSigInt(int sigint){
    eventLoopWatchVariable = 1;
}

// We need at least one "rtsp://" URL argument:
int rtsp_client_process(char * programName,char * rtspUrl) {

    signal(SIGINT,processSigInt);

    // Begin by setting up our usage environment:
    TaskScheduler* scheduler = BasicTaskScheduler::createNew();
    UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

    
    // openURL(*env, programName, rtspUrl);
    RTSPClient* rtspClient = ourRTSPClient::createNew(*env, rtspUrl, RTSP_CLIENT_VERBOSITY_LEVEL, programName);
    openURL(*env,rtspClient);

    // All subsequent activity takes place within the event loop:
    env->taskScheduler().doEventLoop(&eventLoopWatchVariable);
        // This function call does not return, unless, at some point in time, "eventLoopWatchVariable" gets set to something non-zero.
    
    *env << " stop Stream " << rtspClient->url() << " \n";

    shutdownStream(rtspClient,1);

    
    env->reclaim(); env = NULL;
    delete scheduler; scheduler = NULL;

    return 0;

    // If you choose to continue the application past this point (i.e., if you comment out the "return 0;" statement above),
    // and if you don't intend to do anything more with the "TaskScheduler" and "UsageEnvironment" objects,
    // then you can also reclaim the (small) memory used by these objects by uncommenting the following code:
    /*
        env->reclaim(); env = NULL;
        delete scheduler; scheduler = NULL;
    */
}