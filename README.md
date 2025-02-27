# waypoint_rviz_plugin
This Rviz's plugin allows you to set and save waypoints in the RViz interface and publish them to a ROS topic. For example, it can be used with a [simple planner](https://github.com/datledoan/simple_planner) to define a sequence of waypoints for a robot to follow, enabling basic path planning and navigation.

# Publish topic
geometry_msgs::PoseArray /waypoints

# Installation
* Clone code to your workspace and build
    ```sh
    cd your_ws/src
    git clone https://github.com/datledoan/waypoint_rviz_plugin.git
    catkin build
    ```

# Usage
* Open rviz
    ```sh
    source your_ws/devel/setup.bash
    rviz
    ```


https://github.com/user-attachments/assets/7cc5251b-071c-43e9-9090-d671d0ad6057

