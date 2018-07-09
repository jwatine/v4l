

#include <SDL/SDL_video.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/videodev2.h>


#define MILLISEC 0.001
#define DEBUG



void handler_signal(int sig);
void init_stream(SDL_Overlay ** frame, SDL_Rect * position, SDL_Surface ** screen );
void deallocate_mem(void ** buffer_start, struct v4l2_buffer * bufferinfo);
void allocate_mem(int fd, struct v4l2_buffer * bufferinfo, void ** buffer_start);
void mem_needed_device(int fd,  struct v4l2_buffer * bufferinfo);
void buffers_initialization(struct v4l2_requestbuffers *  bufrequest);
void set_video_format();
void close_fd(int * fd);
void  open_fd();
