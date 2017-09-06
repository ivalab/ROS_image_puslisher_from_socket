#include "socket_server.hpp"

#include <unistd.h> // close
#include <sys/types.h>
#include <sys/stat.h>
#include <iomanip>


//#include <opencv2/imgcodecs.hpp>
#include "opencv2/opencv.hpp"

SocketServer::SocketServer(int port, std::string out_path) :
    image_dims_(cv::Size2i(0, 0)),
    out_path_(out_path),
    client_len_(0),
    server_addr_size_(sizeof(server_addr_)),
    port_(port),
    pic_count_(0),
    sock_fdesc_init_(0),
    sock_fdesc_conn_(0) {
  client_len_ = server_addr_size_;
}

void SocketServer::ConnectToNetwork() {

  // Initialize Socket
  sock_fdesc_init_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fdesc_init_ == -1) {
    close(sock_fdesc_init_);
    perror("Couldn't create socket!\n");
    exit(1);
  }

  // Zero out server address struct
  memset((char*)&server_addr_, 0, server_addr_size_);

  // Set server address struct
  server_addr_.sin_family = AF_INET;
  server_addr_.sin_addr.s_addr = INADDR_ANY;
  server_addr_.sin_port = htons(port_);

  // Assign server address to initial socket file descriptor
  if (bind(sock_fdesc_init_, (struct sockaddr*)&server_addr_,
           server_addr_size_) == -1) {
    perror("Couldn't bind initial socket file descriptor!");
    printf("Trying again after killing existing process on port %d...\n",
           port_);
    close(sock_fdesc_init_);
    if (bind(sock_fdesc_init_, (struct sockaddr*)&server_addr_,
             server_addr_size_) == -1) {
      perror("Couldn't bind initial socket file descriptor after retry!");
      exit(1);
    }
    printf("Successful bind to port %d after killing previous process\n",
           port_);
  }

  // Enable listening on initial socket file descriptor
  listen(sock_fdesc_init_, 5);

  // Block process until connection with client has been established.
  // 'client_fdesc' set as new file descriptor to be used for communication
  sock_fdesc_conn_ = accept(sock_fdesc_init_, (struct sockaddr*)&client_addr_,
                            &client_len_);
  if (sock_fdesc_conn_ == -1) {
    perror("ERROR! Client couldn't connect!");
    exit(1);
  }
}

void SocketServer::ReceiveImageDims() {

  ssize_t bytes_sent = 0;
  size_t dims_size = 0;

  int cols = 0;
  int rows = 0;

  size_t sizeof_dims = sizeof(image_dims_.height);

  if (bytes_sent = recv(sock_fdesc_conn_, (char*)&cols, sizeof_dims, 0) == -1) {
    printf("ERROR!: recv failed\n"
           "sock_fdesc: %d\n"
           "image_size: %zu\n"
           "bytes_sent: %zu\n", sock_fdesc_conn_, dims_size, bytes_sent);
    exit(1);
  }
  else {
      printf("Received rows: %d, cols: %d\n", rows, cols);
    }

  if (bytes_sent = recv(sock_fdesc_conn_, (char*)&rows, sizeof_dims, 0) == -1) {
    printf("ERROR!: recv failed\n"
           "sock_fdesc: %d\n"
           "image_size: %zu\n"
           "bytes_sent: %zu\n", sock_fdesc_conn_, dims_size, bytes_sent);
    exit(1);
  }
  image_dims_ = cv::Size2i(cols, rows);
  printf("Image dimensions: [%dx%d]\n", cols, rows);
}

void SocketServer::ReceiveImage(cv::Mat& image) {

  int num_bytes = 0;
  int image_ptr = 0;
  int image_size = 0;

  // Reset image
  //image = cv::Mat::zeros(image_dims_, CV_8UC4);
  image_dims_ = cv::Size2i(320, 240);
  image_dims_ = cv::Size2i(640, 480);
  image = cv::Mat::zeros(image_dims_, CV_8UC3);
  //cv::Mat image = cv::Mat::zeros( 480, 640, CV_8UC3);

  // Get image size
  image_size = image.total() * image.elemSize();

  // Allocate space for image buffer
  uchar sock_data[image_size];

  // Save image data to buffer
  for (int i = 0; i < image_size; i += num_bytes) {
    num_bytes = recv(sock_fdesc_conn_, sock_data + i, image_size - i, 0);
    if (num_bytes == -1) {
      printf("ERROR!: recv failed\n"
             "i: %d\n"
             "sock_fdesc: %d\n"
             "image_size: %d\n"
             "num_bytes: %d\n", i, sock_fdesc_conn_, image_size, num_bytes);
      exit(1);
    }
    std::cout<<"bytes received: "<<num_bytes<<std::endl;
  }


         
  for (int i = 0;  i < image.rows; i++) {
      for (int j = 0; j < image.cols; j++) {                                     
          image.at<cv::Vec3b>(i,j) = cv::Vec3b(sock_data[image_ptr+ 0],sock_data[image_ptr+1],sock_data[image_ptr+2]);
          //image_ptr=image_ptr+3;
          if(image_ptr < 10){
            std::cout<<(int)(sock_data[image_ptr])<<std::endl;
            std::cout<<(int)(sock_data[image_ptr+1])<<std::endl;
            std::cout<<(int)(sock_data[image_ptr+2])<<std::endl;
          }
          image_ptr=image_ptr+3;
      }
  }
  cv::namedWindow("rgb");
  cv::imshow("rgb",image);
  int k=cv::waitKey(10);




  std::ostringstream oss;
  //oss << out_path_ << "/pic_" << std::to_string(pic_count_++) << ".png";
  oss << out_path_ << "/pic_" << std::setw(4) << std::setfill('0') << pic_count_++ << ".png";
  pic_filename_ = oss.str();
}


void SocketServer::ReceiveImage_d(cv::Mat& image) {

  int num_bytes = 0;
  int image_ptr = 0;
  int image_size = 0;

  image_dims_ = cv::Size2i(320, 240);
  image = cv::Mat::zeros(image_dims_, CV_16U);

  // Get image size
  image_size = image.total() * image.elemSize();

  // Allocate space for image buffer
  uchar sock_data[image_size];

  // Save image data to buffer
  for (int i = 0; i < image_size; i += num_bytes) {
    num_bytes = recv(sock_fdesc_conn_, sock_data + i, image_size - i, 0);
    if (num_bytes == -1) {
      printf("ERROR!: recv failed\n"
             "i: %d\n"
             "sock_fdesc: %d\n"
             "image_size: %d\n"
             "num_bytes: %d\n", i, sock_fdesc_conn_, image_size, num_bytes);
      exit(1);
    }
    std::cout<<"bytes received: "<<num_bytes<<std::endl;
  }


         
  for (int i = 0;  i < image.rows; i++) {
      for (int j = 0; j < image.cols; j++) {          
          unsigned short p = (sock_data[image_ptr + 1] << 8) | sock_data[image_ptr];                           
          image.at<unsigned short>(i,j) = p;
          image_ptr=image_ptr+2;
      }
  }
  cv::namedWindow("depth");
  cv::imshow("depth",image);
  int k=cv::waitKey(10);




  std::ostringstream oss;
  //oss << out_path_ << "/pic_" << std::to_string(pic_count_++) << ".png";
  oss << out_path_ << "/pic_" << std::setw(4) << std::setfill('0') << pic_count_d++ << "_d.png";
  pic_filename_d = oss.str();
}

void SocketServer::WriteImage(cv::Mat& image) {
  cv::imwrite(pic_filename_, image);
}

void SocketServer::WriteImage_d(cv::Mat& image) {
  cv::imwrite(pic_filename_d, image);
}

void SocketServer::WriteImage_saveKey(cv::Mat& image, cv::Mat& image_d) {
  char k;
  k=cvWaitKey(10);
  if(k == 's') {
    cv::imwrite(pic_filename_, image);
    cv::imwrite(pic_filename_d, image_d);
  }
}

int SocketServer::GetWidth() {
  return image_dims_.width;
}

int SocketServer::GetHeight() {
  return image_dims_.height;
}
