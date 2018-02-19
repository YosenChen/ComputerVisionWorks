My Personal Computer Vision Works


MultiCamMultiHumanTrackingSystem:
It's the code of my NTU master thesis: Efficient Consistent Labeling in Visual Surveillance System with Multiple Cameras, which is published at 2010. A complete multi-camera multi-human tracking system is implemented in a distributed architecture. Local trackers perform local object detection and face tracking, upload object info for the human labeling and database update on the central server. The server solves geometic correspondence across multiple cameras, then compare the color similarity for each body block using the calculated human height info. The object database is used to handle the human reentry problem. Local trackers handle object occlusion using the object depth order determined by projecting the 3D standing positions to the camera image 2D plane.
Slides: https://www.slideshare.net/YuShengYosenChen/mcmhts-oral-present
Presentation: https://www.youtube.com/watch?v=ilVOrZavUZE
"Multi-humans tracking(backup CAM2 201009210636).rar" is the archived file at that time.

Single camera object tracking:
4-dimensional object info (xy position, size, rotation) are estimated in real-time. The complete interaction service is implemented as a finite state machine which includes 4 states, interaction trigger detection, object info extraction, tracking & interaction, object losing handling.
Slides: https://www.slideshare.net/YuShengYosenChen/presentationvisualtracking
Youtube:
face tracking: https://www.youtube.com/watch?v=5Q6INpA-QQE
object tracking: https://www.youtube.com/watch?v=UcFLDoMPPiI
Interaction system: https://www.youtube.com/watch?v=wIpE84abbeY
"Google325(20100102)_noDumpPhoto.rar" is the archived file at that time.

KalmanFilterTracker:
A simple implementation of Kalman filtering object tracker
Youtube: https://www.youtube.com/watch?v=9CBDH1Y4Iw0

ABM_data:
It's my implementation of adaptive background model code, which is referenced to Chris Stauffer's paper in CVPR98. I implemented it when I was a NTU master student in 2009-2010.

OpenCV Face Detection with 2-Eye Position Check:
It was a simple work about the OpenCV sample code. Detected eye positions are checked with the face info, then used to filter out unreasonable eye-face geometrics.

VideoSyncer:
A video tool which can sycn up the playback speed of 2 videos by adjusting the timestamp with linear interpolation

