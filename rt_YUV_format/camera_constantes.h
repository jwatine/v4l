#include <SDL/SDL.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/videodev2.h>

#define BUFF_SIZE 614400

//device's file descriptor
int fd;

//that structure 
struct v4l2_buffer  bufferinfo;

//format of the video, this should be set according to
//the camera capabilities.
struct v4l2_format  format;

//the buffer that is filled up by the device
void * buffer_start;

//the structures used to display the video
SDL_Overlay * frame;
SDL_Rect position;
SDL_Surface * screen;

