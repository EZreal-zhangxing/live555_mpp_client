#include "rtsp_client.h"
#include "signal.h"
#include <iostream>



int main(int argc,char * argv[]){
    if(argc != 2){
        std::cout << "only one parameter is rtsp url like this [rtsp://<username>:<password>@<ip address>/<your>/<defined>/<path>]" << std::endl;
    }
    rtsp_client_process(argv[0],argv[1]);
    return 0;
}