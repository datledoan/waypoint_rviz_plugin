#ifndef WAYPOINT_TOOL_H
#define WAYPOINT_TOOL_H

#include <rviz/tool.h>
#include <ros/ros.h>
#include <geometry_msgs/PoseArray.h>
#include <geometry_msgs/Pose.h>

namespace Ogre
{
class SceneNode;
class Vector3;
}

namespace rviz
{
class VectorProperty;
class VisualizationManager;
class ViewportMouseEvent;
}

namespace waypoint_rviz_plugin
{

class WaypointTool: public rviz::Tool
{
Q_OBJECT
public:
  WaypointTool();
  ~WaypointTool();

  virtual void onInitialize();

  virtual void activate();
  virtual void deactivate();

  virtual int processMouseEvent( rviz::ViewportMouseEvent& event );

  virtual void load( const rviz::Config& config );
  virtual void save( rviz::Config config ) const;

private:
  void makeWp( const Ogre::Vector3& position );

  std::vector<Ogre::SceneNode*> wp_nodes_;
  Ogre::SceneNode* moving_wp_node_;
  std::string wp_resource_;
  rviz::VectorProperty* current_wp_property_;
  ros::NodeHandle nh_;
  ros::Publisher waypoint_pub_;
  geometry_msgs::PoseArray waypoints_;
};
// END_TUTORIAL

} // end namespace waypoint_rviz_plugin

#endif // WAYPOINT_TOOL_H
