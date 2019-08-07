#include<vector>
#include<string>
#include<unistd.h>
#include<iostream>
#include<ifaddrs.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"httplib.h"
#include<boost/filesystem.hpp>
#include<boost/system>
#include<boost/algorithm/string>

namespace bf = boost::filesystem;
using namespace httplib;
class P2PClient{

  public:
    bool ShowOnlineHost(){
     struct ifaddrs *addrs,*ifa = NULL;
     //获取本机网卡信息
     getifaddrs(&addrs);
     //循环获取本机所有的网卡信息
     for(ifa=addrs;ifa != NULL;ifa=ifa->ifa_next){
       if(ifa->ifa_addr == NULL){
         continue;
       }
       //进行类型强转
       struct sockaddr_in* addr = (struct sockaddr_in*)ifa->ifa_addr;
       struct sockaddr_in* mask = (struct sockaddr_in*)ifa->ifa_netmask;
       //本地回环网卡除外
       //inet_addr 将点分十进制的字符串IP地址转化成32位的网络字节序IP地址
       if(addr->sin_addr.s_addr == inet_addr("127.0.0.1")){
         continue;
       }
       if(addr->sin_addr.s_addr == inet_addr("192.168.122.1")){
         continue;
       }
       //将网络字节序转换位主机字节序
       if(addr->sin_family == AF_INET){
         uint32_t ip = ntohl(addr->sin_addr.s_addr);
         uint32_t mk = ntohl(mask->sin_addr.s_addr);
         //求出网络号和主机位数
         uint32_t network = ip & mk;
         uint32_t num = ~mk;
         uint32_t i;
         for(i = 1; i < num; ++i){
           uint32_t ip_host = network + i;
           uint32_t ip_net = htonl(ip_host);
           char ip_buffer[32];
           inet_ntop(AF_INET,&ip_net,ip_buffer,32);
           _host_list.push_back(ip_buffer);
         }
       }
     }
     freeifaddrs(addrs);
     return true;
    }
    bool PairNearbyHost(){
      for(auto& e : _host_list){
        Client client(&e[0],9000);
        auto rsp = client.Get("/hostpair");
        if(rsp!=NULL){
          _online_list.push_back(e);
        }
      }
      return true;
    }
    bool ShowNearbyHost(){
      int i = 0;
      for(auto& e : _online_list){
        std::cout << i++ << e << std::endl;
      }
      std::cout << std::endl;
      return true;
    }
    //显示指定主机的
    bool GetShareList(){
      std::cout << "输入你要获取的主机号：";
      fflush(stdout);
      std::cin >> _host_id;
      Client client(&_online_list[_host_id][0],9000);
      auto rsp = client.Get("/list");
      std::string filename;
      boost::split
      
    } 
  private:
    int _host_id;
    std::vector<std::string>  _online_list;
    std::vector<std::string>  _file_list;
    std::vector<std::string>  _host_list;
};


int main(){
  P2PClient c;
  c.ShowOnlineHost();
}
