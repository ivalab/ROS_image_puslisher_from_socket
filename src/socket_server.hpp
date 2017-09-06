#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP

#include <string>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <cctype> // isdigit
#include <sys/socket.h>
#include <netinet/in.h>

//#include <opencv2/core.hpp>
#include "opencv2/opencv.hpp"

class SocketServer {
 public:
  SocketServer(int port, std::string out_path);
  void ConnectToNetwork();
  void ReceiveImageDims();
  void ReceiveImage(cv::Mat& image);
  void ReceiveImage_d(cv::Mat& image);
  void WriteImage(cv::Mat& image);
  void WriteImage_d(cv::Mat& image);
  void WriteImage_saveKey(cv::Mat& image, cv::Mat& image_d);
  int GetWidth();
  int GetHeight();

 private:
  cv::Size2i image_dims_;
  struct sockaddr_in server_addr_;
  struct sockaddr_in client_addr_;
  std::string pic_filename_;
  std::string pic_filename_d;
  std::string out_path_;
  socklen_t client_len_;
  size_t server_addr_size_;
  int port_;
  int pic_count_;
  int pic_count_d;
  int sock_fdesc_init_;
  int sock_fdesc_conn_;
};

#endif
