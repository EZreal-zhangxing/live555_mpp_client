#include "rtsp_client.h"
#include "signal.h"
#include <iostream>
#include <unistd.h>

int main(int argc,char * argv[]){
    int t = 0;
    int protocol = 0; // 0 :udp 1: tcp
    int multicast = 0; // 1 activate multicast
    char * rtsp_url = 0;
    if(argc < 3){
        printf("Usage: %s [OPTION]...\n", argv[0]);
        printf("Options:\n");
        printf("-p:\t the protocol in [tcp,udp] default is udp\n");
        printf("-u:\t the url like this [rtsp://<username>:<password>@<ip address>/<your>/<defined>/<path>] \n");
        printf("-m:\t the stream use multicast \n");
        return 0;
    }
    while((t = getopt(argc,argv,"p:u:m")) != -1){
        switch (t)
        {
        case 'p':{
            if(std::string(optarg) == std::string("tcp")){
                protocol = 1;
            }else if(std::string(optarg) == std::string("udp")){
                protocol = 0;
            }else{
                printf("-p:\t the protocol mush in [tcp,udp] default is udp\n");
            }
            break;
        }
        case 'u':
            rtsp_url = optarg;
            break;
        case 'm':
            multicast = 1;
            break;
        case '?':
        case ':':
        default:
            printf("Invalid option: %c\n", optopt);
            printf("Usage: %s [OPTION]...\n", argv[0]);
            printf("Options:\n");
            printf("-p:\t the protocol in [tcp,udp] default is udp\n");
            printf("-u:\t the url like this [rtsp://<username>:<password>@<ip address>/<your>/<defined>/<path>] \n");
            printf("-m:\t the stream use multicast \n");
            break;
        }
    }
    if(rtsp_url != 0){
        printf("protocol : %s\n",protocol ? "tcp" : "udp");
        printf("multicast : %d\n",multicast);
        rtsp_client_process(argv[0],rtsp_url,protocol,multicast);
    }
    
    return 0;
}