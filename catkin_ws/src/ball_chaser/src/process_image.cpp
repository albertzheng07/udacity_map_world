#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

ros::ServiceClient client;

void drive_robot(float lin_x, float ang_z)
{
    ball_chaser::DriveToTarget srv;

    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if(!client.call(srv))
    {
        ROS_ERROR("Fail to call service Drive to Target");
    }
}

void process_image_callback(const sensor_msgs::Image img)
{
    int white_pixel = 255;

    int count_left = 0;
    int count_middle = 0;
    int count_right = 0;
    for (int i = 0; i < img.height; i++)
    {
        for (int j = 0; j < img.step; j+=3)
        {
            int index = i*img.step + j;
  
            if (img.data[index] == white_pixel && img.data[index+1] == white_pixel && img.data[index+2] == white_pixel)
            {
                if (j <= img.step/3)
                {   
                    count_left++;
                }
                else if (j > img.step/3 && j <= img.step*2/3)
                {
                    count_middle++;
                }
                else if (j > img.step*2/3)
                {
                    count_right++;
                }
            }
        }
    }

    //ROS_INFO_STREAM("Count left is " + std::to_string(count_left));
    //ROS_INFO_STREAM("Count middle is " + std::to_string(count_middle));
    //ROS_INFO_STREAM("Count right is " + std::to_string(count_right));

    if ( (count_left+count_middle+count_right) == 0 || (count_middle > count_left && count_middle > count_right)) // drive straight
    {
        drive_robot(0.05, 0.0);
    }
    else if ( count_left > count_middle && count_left > count_right)
    {
        drive_robot(0.05, 0.1); // drive left
    }
    else if ( count_right > count_middle && count_right > count_left)
    {
        drive_robot(0.05, -0.1); // drive right
    } 
    
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    ros::spin();

    return 0;
}