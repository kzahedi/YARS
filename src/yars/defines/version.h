#ifndef __VERSIONS_H__
#define __VERSIONS_H__

XmlChangeLog::add(0, 7, 1, "Removed <environments>, replaced by <environment>", true);
XmlChangeLog::add(0, 7, 2, "<pose> is now optional for <box>, <sphere>, <cylinder>, and <cappedCylinder>", false);
XmlChangeLog::add(0, 7, 3, "term and pairwise are now optional for logger <gnuplot>", false);
XmlChangeLog::add(0, 7, 4, "precision attribute in logging modules <object>, <sensor>, <actuator>, and <controller> are now non-negative integers", true);
XmlChangeLog::add(0, 7, 5, "changed position of <pose> and <controller> in <movable>", true);
XmlChangeLog::add(0, 7, 6, "camera now has the option to use look at & up vs. angles", false);
XmlChangeLog::add(0, 7, 7, "optional parameters for fog settings", false);
XmlChangeLog::add(0, 7, 8, "OSD fonts configurable for each screen", false);
XmlChangeLog::add(0, 7, 9, "cappedCylinder renamed to capsule", true);
XmlChangeLog::add(0, 7, 10, "texture definitions of box and capsule changed", true);
XmlChangeLog::add(0, 7, 11, "changed sky tag", true);
XmlChangeLog::add(0, 7, 12, "removed shadow tag", true);
XmlChangeLog::add(0, 7, 13, "changed trace tag", true);
XmlChangeLog::add(0, 7, 14, "added particles", false);
XmlChangeLog::add(0, 7, 15, "added billboard", false);
XmlChangeLog::add(0, 7, 16, "changed decay to final in trace line tag", false);
XmlChangeLog::add(0, 7, 17, "changed parametrisation of trace lines", false);
XmlChangeLog::add(0, 7, 18, "changed physics parametrisation", true);
XmlChangeLog::add(0, 7, 19, "global texture definition added", false);
XmlChangeLog::add(0, 7, 20, "moved force and velocity to header of joint definition", false);
XmlChangeLog::add(0, 7, 21, "joints now take pose instead of axis and anchor", true);
XmlChangeLog::add(0, 7, 22, "<ground> now requires <visualisation>", true);
XmlChangeLog::add(0, 7, 23, "changed order of <actuators> now <sensors> in <movable>", true);
XmlChangeLog::add(0, 7, 24, "moved global <pose> tag", true);
XmlChangeLog::add(0, 7, 25, "renamed <movable> to <robot> and <movables> to <robots>", true);
XmlChangeLog::add(0, 7, 26, "moved behaviour frequency definition from the simulator tag to the controller tag", true);
XmlChangeLog::add(0, 7, 27, "simulator frequency is now an attribute of <simulator>", true);
XmlChangeLog::add(0, 7, 28, "velocity can now be negative for the actuators", false);
XmlChangeLog::add(0, 7, 29, "added <resolution> tag for screen resolution", false);
XmlChangeLog::add(0, 7, 30, "added <recording> tag for automatically timed recording", false);
XmlChangeLog::add(0, 7, 31, "renamed autoCapture autoFollow autoShow", true);
XmlChangeLog::add(0, 7, 32, "added blender logger", false);
XmlChangeLog::add(0, 7, 33, "added csv logger", false);
XmlChangeLog::add(0, 7, 34, "added new parameter for csv and file logger", false);
XmlChangeLog::add(0, 8, 35, "changed force and velocity specification for hinge and slider actuators", true);
XmlChangeLog::add(0, 8, 36, "added additional logging information to actuator logger", false);
XmlChangeLog::add(0, 8, 37, "added tag for gravitation", false);
XmlChangeLog::add(0, 8, 38, "moved force and velocity tags for <generic> actuator", true);
XmlChangeLog::add(0, 8, 39, "generic spring actuator included", false);
XmlChangeLog::add(0, 8, 40, "orbit speed is now given in degree per second", false);
#endif // __VERSIONS_H__
