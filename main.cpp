#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void scanport(const std::string& ip, int port){
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if (sock<0){
        std::cerr<<"socket not connected";
        return;
    }
    

    sockaddr_in targetAddress;
    std::memset(&targetAddress,0,sizeof(targetAddress));
    targetAddress.sin_family = AF_INET;
    targetAddress.sin_port = htons(port);

    if(inet_pton(AF_INET,ip.c_str(),&targetAddress.sin_addr)<=0){
        std::cerr<<"invalid ip address";
        close(sock);
        return;
    }
    if (connect(sock,(struct sockaddr*)&targetAddress,sizeof(targetAddress))==0){
        std::cout<<"[+] PORT " <<port<<" is OPEN"<<std::endl;
        close(sock);
    }
    close(sock);
    
}
int main(){
    int start_port=80;
    int end_port=30000;
    std::string addr="127.0.0.1";
    for (int port=start_port;port<=end_port;port++){
        scanport(addr,port);
    }
    
    return 0;
}
