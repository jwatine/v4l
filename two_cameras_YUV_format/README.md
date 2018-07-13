
based on the work done by jwhsmith, details on
http://jwhsmith.net/2014/12/capturing-a-webcam-stream-using-v4l2/ 

Bash commands to know the formats supported by the device 
v4l2-ctl -d /dev/video0 --list-formats-ext  

more references about v4l2 at
https://linuxtv.org/downloads/v4l-dvb-apis/uapi/v4l/extended-controls.html#jpeg-control-reference

Execution
-------
program to control at most 2 cameras, beware of affecting the two cameras
on 2 hubs, otherwise you might have memory problem.
Just give the two paths to the device you want to capture.

