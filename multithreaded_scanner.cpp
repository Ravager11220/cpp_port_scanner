#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <vector>

std::mutex cout_mutex; 

void scanport(const std::string& ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return; 
    }

    sockaddr_in targetAddress;
    std::memset(&targetAddress, 0, sizeof(targetAddress));
    targetAddress.sin_family = AF_INET;
    targetAddress.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &targetAddress.sin_addr) <= 0) {
        std::cerr << "Invalid IP address\n";
        close(sock);
        return;
    }

   
    if (connect(sock, (struct sockaddr*)&targetAddress, sizeof(targetAddress)) == 0) {
      
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "[+] PORT " << port << " is OPEN\n";
    }
    
   
    close(sock);
}

void scan_range(const std::string& ip, int startport, int endport) {
    for (int port = startport; port <= endport; port++) {
        scanport(ip, port); 
    }
}

int main() {
    int start_port = 80;
    int end_port = 100000000;
    std::string addr = "127.0.0.1";
    if (end_port > 65535) {
    std::cout << "ERROR: Port range cannot exceed 65535. Capping range.\n";
        end_port = 65535;
    }
    
    
    int num_threads=std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;
    int total_ports = end_port - start_port + 1;
    if (num_threads > total_ports) {
        num_threads = total_ports;
    }
    int ports_per_thread=(end_port-start_port+1)/num_threads;
    std::cout << "SCANNING PORTS........\n";
    std::vector<std::thread> threads;
    for(int i=0;i<num_threads;i++){
        int current_start=start_port+(i*ports_per_thread);
        int current_end=current_start+ports_per_thread-1;
        if (i == num_threads - 1) {
            current_end = end_port;
        }
        threads.push_back(std::thread(scan_range,addr,current_start,current_end));
    }
    for (auto& i : threads){
        if(i.joinable()){
            i.join();
        }
    }
  
   

    std::cout << "SCAN OVER........\n";
    
    return 0;
}
