#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreEntity.h>

#include <ros/console.h>

#include <rviz/viewport_mouse_event.h>
#include <rviz/visualization_manager.h>
#include <rviz/mesh_loader.h>
#include <rviz/geometry.h>
#include <rviz/properties/vector_property.h>

#include "waypoint_tool.h"

namespace waypoint_rviz_plugin
{

WaypointTool::WaypointTool()
  : moving_wp_node_( NULL )
  , current_wp_property_( NULL )
{
  shortcut_key_ = 'l';
}

WaypointTool::~WaypointTool()
{
  for( unsigned i = 0; i < wp_nodes_.size(); i++ )
  {
    scene_manager_->destroySceneNode( wp_nodes_[ i ]);
  }
}

void WaypointTool::onInitialize()
{
  wp_resource_ = "package://waypoint_rviz_plugin/media/new_flag.dae";

  if( rviz::loadMeshFromResource( wp_resource_ ).isNull() )
  {
    ROS_ERROR( "WaypointTool: failed to load model resource '%s'.", wp_resource_.c_str() );
    return;
  }

  moving_wp_node_ = scene_manager_->getRootSceneNode()->createChildSceneNode();
  Ogre::Entity* entity = scene_manager_->createEntity( wp_resource_ );
  moving_wp_node_->attachObject( entity );
  moving_wp_node_->setVisible( false );
  waypoint_pub_ = nh_.advertise<geometry_msgs::PoseArray>( "waypoints", 1, true );
}

void WaypointTool::activate()
{
  if( moving_wp_node_ )
  {
    moving_wp_node_->setVisible( true );

    current_wp_property_ = new rviz::VectorProperty( "Wp " + QString::number( wp_nodes_.size() ));
    current_wp_property_->setReadOnly( true );
    getPropertyContainer()->addChild( current_wp_property_ );
  }
}

void WaypointTool::deactivate()
{
  if( moving_wp_node_ )
  {
    moving_wp_node_->setVisible( false );
    delete current_wp_property_;
    current_wp_property_ = NULL;
  }
}

int WaypointTool::processMouseEvent( rviz::ViewportMouseEvent& event )
{
  if( !moving_wp_node_ )
  {
    return Render;
  }
  Ogre::Vector3 intersection;
  Ogre::Plane ground_plane( Ogre::Vector3::UNIT_Z, 0.0f );
  if( rviz::getPointOnPlaneFromWindowXY( event.viewport,
                                         ground_plane,
                                         event.x, event.y, intersection ))
  {
    moving_wp_node_->setVisible( true );
    moving_wp_node_->setPosition( intersection );
    current_wp_property_->setVector( intersection );

    if( event.leftDown() )
    {
      makeWp( intersection );
      current_wp_property_ = NULL; 
      return Render | Finished;
    }
  }
  else
  {
    moving_wp_node_->setVisible( false ); 
  }
  return Render;
}

void WaypointTool::makeWp( const Ogre::Vector3& position )
{
  Ogre::SceneNode* node = scene_manager_->getRootSceneNode()->createChildSceneNode();
  Ogre::Entity* entity = scene_manager_->createEntity( wp_resource_ );
  node->attachObject( entity );
  node->setVisible( true );
  node->setPosition( position );
  wp_nodes_.push_back( node );
  geometry_msgs::Pose pose;
    pose.position.x = position.x;
    pose.position.y = position.y;
    pose.position.z = position.z;
    waypoints_.poses.push_back(pose);
    waypoint_pub_.publish(waypoints_);
}

void WaypointTool::save( rviz::Config config ) const
{
  config.mapSetValue( "Class", getClassId() );

  rviz::Config wps_config = config.mapMakeChild( "Wps" );

  rviz::Property* container = getPropertyContainer();
  int num_children = container->numChildren();
  for( int i = 0; i < num_children; i++ )
  {
    rviz::Property* position_prop = container->childAt( i );
    rviz::Config wp_config = wps_config.listAppendNew();
    wp_config.mapSetValue( "Name", position_prop->getName() );
    wp_config.mapSetValue( "X", position_prop->childAt( 0 )->getValue().toDouble() );
    wp_config.mapSetValue( "Y", position_prop->childAt( 1 )->getValue().toDouble() );
    wp_config.mapSetValue( "Z", position_prop->childAt( 2 )->getValue().toDouble() );
    position_prop->save( wp_config );
  }
}

void WaypointTool::load( const rviz::Config& config )
{
  rviz::Config wps_config = config.mapGetChild( "Wps" );
  int num_wps = wps_config.listLength();
  for( int i = 0; i < num_wps; i++ )
  {
    rviz::Config wp_config = wps_config.listChildAt( i );
    QString name = "Wp " + QString::number( i + 1 );
    wp_config.mapGetString( "Name", &name );
    rviz::VectorProperty* prop = new rviz::VectorProperty( name );
    prop->load( wp_config );
    prop->setReadOnly( true );
    getPropertyContainer()->addChild( prop );
    makeWp( prop->getVector() );
  }
}


} // end namespace waypoint_rviz_plugin

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(waypoint_rviz_plugin::WaypointTool,rviz::Tool )
