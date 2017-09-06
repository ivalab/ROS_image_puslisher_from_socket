#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

#include <memory> // unique_ptr
#include <sys/stat.h>
#include "socket_server.hpp"


bool DirExists(const char* path) {
  struct stat info;
  if (stat(path, &info) != 0) {
    perror("Can't access path");
    return 0;
  }
  else if(info.st_mode & S_IFDIR) {
    return 1;
  }
  else return 0;
}

void AssertCond(bool assert_cond, const char* fail_msg) {
  if (!assert_cond) {
    printf("Error: %s\nUsage: ./pic-server <port> <out_path>\n", fail_msg);
    exit(1);
  }
}

void ParseArgs(int argc, char** argv) {
  //AssertCond(argc == 3, "Wrong number of arguments");
  //AssertCond(DirExists(argv[2]), "Supplied directory does not exist");
}


int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_publisher");
  ros::NodeHandle nh;
  image_transport::ImageTransport it(nh);
  image_transport::Publisher pub = it.advertise("camera/image", 1);

  // Convert the passed as command line parameter index for the video device to an integer
  ParseArgs(argc, argv);
  int port = atoi(argv[1]);
  std::unique_ptr<SocketServer> server_ptr(new SocketServer(port, argv[2]));
  server_ptr->ConnectToNetwork();

  sensor_msgs::ImagePtr msg;

  ros::Rate loop_rate(50);
  while (nh.ok()) {
     cv::Mat frame;
     //server_ptr->ReceiveImage(frame);//color
     server_ptr->ReceiveImage_d(frame);//depth
     // Check if grabbed frame is actually full with some content
     if(true) {
       //msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();//color
       msg = cv_bridge::CvImage(std_msgs::Header(), "mono8", frame).toImageMsg();//depth
       pub.publish(msg);
       cv::waitKey(1);
     }

     ros::spinOnce();
     loop_rate.sleep();

  }
}

