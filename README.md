# Scanning 3D tool for Kinect v2

### This app came out of our investigation on how scan objects with Kinect v2 and get an unified pointcloud.

![3D scanning interface](/scan-portrait.png)

#### Dependencies:
* [ofxKinectForWindows2](https://github.com/elliotwoods/ofxKinectForWindows2): A wrapper of Kinect 2 SDK for Openframeworks made by @elliotwoods
* [ofxDatGui](https://github.com/braitsch/ofxDatGui): Gui library

#### Instructions
This apps gets the pointcloud of an object on each side. Then it unifies meshes and exports it into a PLY file.

In order to get a good scanning, you have to place it in front of the Kinect (the center point is marked with the intersection of two yellow lines).
Its also important to set the scanning area and the distance to object by the interface.

During the photoshoot, you have to rotate the object a certain angle for each photo.

> angle = 360 / n

> n = number of photos to take
