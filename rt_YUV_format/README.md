
based on the work done by jwhsmith,more information on
http://jwhsmith.net/2014/12/capturing-a-webcam-stream-using-v4l2/ 

more references about v4l2 at
https://linuxtv.org/downloads/v4l-dvb-apis/uapi/v4l/extended-controls.html#jpeg-control-reference

REAL-TIME
---------
You need to install Prelude and Ptask on your computer to run this program.

PTASK   : https://github.com/glipari/ptask
PRELUDE : http://www.lifl.fr/~forget/prelude.html

REAL-TIME CAMERA CONTROL
------------------------
This program offers a real-time control of a camera using YUV format.
It can be changed in order to handle more video's format.
the operation_buffer_data function located in camera_nodes.c
could be changed to display some psychedelic images! 


COMPILATION
-----------
make -f ptask_wrapper.make

EXECUTION
---------
sudo ./no-admission.sh
sudo ./camera
Beware of entering the good path to the device.

