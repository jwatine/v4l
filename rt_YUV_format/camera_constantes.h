#include <SDL/SDL.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/videodev2.h>

#define BUFF_SIZE 614400

int fd;
struct v4l2_buffer  bufferinfo;
SDL_Rect position;
struct v4l2_format  format;
void * buffer_start;
SDL_Overlay * frame;
SDL_Rect position;
SDL_Surface * screen;

