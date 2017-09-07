# ROS_image_puslisher_from_socket
#### a publisher and receiver for rgb and depth image through socket
---------------------------------------------
This is a modification of [official ROS image_common pacakge](https://github.com/ros-perception/image_common). This code combines the [opencv socket server](https://github.com/alexanderkoumis/toy-opencv-mat-socket-server) feature so it can receive color/depth socket and publish it to a ROS topic

## usage:

1. start ros
```
roscore
```
2. start publisher 
```
rosrun image_transport_tutorial my_publisher 2325 ../data
```
2325: the port number
../data: folder path

3. start subscriber
```
rosrun image_transport_tutorial my_subscriber
```

Then you should be able to see a window pop out

-----------------------------------------------
## subscribe rgb image instead of depth image

in `my_publisher.cpp` and `my_subscriber.cpp`  
comment out any line ending // depth  
uncomment and line ending //color

-----------------------------------------------
## try a single image, list of images, webcam 

1. image: simply replace `my_publisher.cpp.image` to `my_publisher.cpp`
```
rosrun image_transport_tutorial my_publisher 2325 path/to/the/image.png
```
2. list: simply replace `my_publisher.cpp.folder` to `my_publisher.cpp`  
remember to change the path to the folder in `my_publisher.cpp`
```
rosrun image_transport_tutorial my_publisher 2325 
```
3. webcam: simply replace `my_publisher.cpp.webcam` to `my_publisher.cpp`
```
rosrun image_transport_tutorial my_publisher 2325 0
```
