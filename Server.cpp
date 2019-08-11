#include<boost/filesystem.hpp>
#include<iostream>
#include"httplib.h"
namespace bf = boost::filesystem;

#define SHARE_PATH "Share"
using namespace httplib;
class P2PServer{

  public:
    //附近主机配对请求处理
    static void PairHandler(const Request& req,Response& rsp){
      //服务端主机所有网卡接受配对请求
      rsp.status = 200;
    }
    //文件列表请求处理
    static void ListHandler(const Request& req,Response& rsp){
      //首先在工作路径下创建目录
      if(!bf::exists(SHARE_PATH)){
        bf::create_directory(SHARE_PATH);
      }
      //展示文件列表
      bf::directory_iterator item_begin(SHARE_PATH);
      bf::directory_iterator item_end;
      while(item_begin!=item_end){
        //判断当前文件是否为文件夹
        if(bf::is_directory(item_begin->status())){
          item_begin++;
          continue;
        }
        //获取文件路径
        bf::path p = item_begin->path();
        //获取文件名称
        std::string name = p.filename().string();
        rsp.body += name + "\n";
        item_begin++;  
      }
      rsp.status = 200;
    }
    //文件下载请求处理
    static void DownloadHandler(const Request& req,Response& rsp){
      //得到文件路径        
      bf::path p(req.path);
      //得到文件路径
      std::string tmp = "/";
      std::string name = SHARE_PATH + tmp + p.filename().string();
      //将此路径下的文件以二进制的方式写入到内存当中
      std::ifstream file(name,std::ios::binary);
      //判断文件是否正常打开
      if(!file.is_open()){
        rsp.status = 404;
        return;
      }else{
        //首先获取文件的大小
        size_t fsize = bf::file_size(name);
        //給body开空间
        rsp.body.resize(fsize);
        //将硬盘中的文件写入到body当中
        file.read(&rsp.body[0],fsize);
        //判断是否读取成功
        if(!file.good()){
          //服务器内部错误
          rsp.status = 500;
          return;
        }else{
          //文件下载
          rsp.set_header("content-type","aplication/octet-stream");
          rsp.status = 200;
          //关闭文件
          file.close();
        } 
      }
    }
    bool Start(uint16_t port){
      
      srv.bind_to_any_port("0,0,0,0");
      srv.listen("0,0,0,0",port);
      srv.Get("/list",ListHandler);
      srv.Get("/list/(.*)",DownloadHandler);
      srv.Get("/hostpair",PairHandler);


    }
  private:
    Server srv;
};

int main(){
  P2PServer p;
  p.Start(9000);
  return 0;
}
