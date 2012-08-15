/////////////////////////////////////////////////////////
ofxKinectMeshRecorder
/////////////////////////////////////////////////////////

OpenFrameworks App
by welovecode

This is a simple app that records/playbacks the Kinect point cloud into/from a serie
of txt files.

It also includes a Blender script that creates an animated Mesh inside blender from those txt files
very useful for 3d/video post production. From Blender you can export the animation into a sequence
of ".obj" files and use it in other 3d programs or After Effects.

Please remember this is a limited first version we developed for internal work in a project.

welovecode
hola@welovecode.net


Interesting Related Pojects:
-------------------------------------------------------

Moullinex / Catalina music video
Source code and workflow on the same process but using Kinect + Processing + Cinema 4D + After Effects.
http://moullinex.tumblr.com/post/3180520798/catalina-music-video

kinectToTexturedOBJ
class to save the data from the kinect as a textured OBJ file.
https://github.com/roymacdonald/kinectToTexturedOBJ


This app depends on the following addons:
-------------------------------------------------------
OF version: 0.71

- ofxKinect
openFrameworks wrapper for the xbox kinect
https://github.com/ofTheo/ofxKinect


Changes:
-------------------------------------------------------
15-06-12: First version with Blender import scripts.