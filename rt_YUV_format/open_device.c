#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#include "camera_constantes.h"
#include "open_device.h"



void  open_fd(){
  //Open video0 in both read and write modes.
  //device's name should be adjusted.
  if((fd = open("/dev/video1", O_RDWR)) < 0){
    perror("open");
    exit(1);
  }

}

void close_fd(int * fd){
  if(!close(*fd)){
    perror("close");
    exit(1);   
  }
}

void set_video_format(){
  format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  format.fmt.pix.width = 640;
  format.fmt.pix.height = 480;
 
  if(ioctl(fd, VIDIOC_S_FMT, &format) < 0){
    perror("VIDIOC_S_FMT");
    exit(1);
  }
}

/* inform the device about the future buffers */
void buffers_initialization(struct v4l2_requestbuffers *  bufrequest){
  bufrequest->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  bufrequest->memory = V4L2_MEMORY_MMAP;
  bufrequest->count = 1;
 
  if(ioctl(fd, VIDIOC_REQBUFS, bufrequest) < 0){
    perror("VIDIOC_REQBUFS");
    exit(1);
  }
}

/* check how many memory do the device need */
void mem_needed_device(int fd,  struct v4l2_buffer * bufferinfo){

  memset(bufferinfo, 0, sizeof(*bufferinfo));
 
  bufferinfo->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  bufferinfo->memory = V4L2_MEMORY_MMAP;
  bufferinfo->index = 0;
 
  if(ioctl(fd, VIDIOC_QUERYBUF, bufferinfo) < 0){
    perror("VIDIOC_QUERYBUF");
    exit(1);
  }

 
}

/* map the memory for the buffer */
void allocate_mem(int fd, struct v4l2_buffer *  bufferinfo, void ** buffer_start){
  *buffer_start = mmap(
		      NULL,
		      bufferinfo->length,
		      PROT_READ | PROT_WRITE,
		      MAP_SHARED,
		      fd,
		      bufferinfo->m.offset
		      );
  
  if(*buffer_start == MAP_FAILED){
    perror("mmap");
    exit(1);
  }
}

void deallocate_mem(void ** buffer_start, struct v4l2_buffer * bufferinfo){
  
  if(!munmap(*buffer_start,bufferinfo->length)){
    perror("munmap :");
  }
}


void init_stream(SDL_Overlay ** frame, SDL_Rect * position, SDL_Surface ** screen ){

  // Initialise everything.
  SDL_Init(SDL_INIT_VIDEO);

  printf("before setvideo \n");
  // Get the screen's surface.
  *screen = SDL_SetVideoMode(
			     format.fmt.pix.width,
			     format.fmt.pix.height,
			     32,
			     SDL_HWSURFACE);

  printf("after setvideo \n");

     
  // Activate streaming
  int type = bufferinfo.type;
  if(ioctl(fd, VIDIOC_STREAMON, &type) < 0){
    perror("VIDIOC_STREAMON");
    exit(1);
  }

  //create an YUV overlay and the rect that "contains" the frame
  position->x=0;
  position->y=0;
  position->w=format.fmt.pix.width;
  position->h=format.fmt.pix.height;
  

  printf("before create \n");
  *frame = SDL_CreateYUVOverlay(format.fmt.pix.width,
			       format.fmt.pix.height,
			       SDL_YUY2_OVERLAY,
			       *screen);
  printf("after create \n");




}

void handler_signal(int sig){

  // Deactivate streaming
  if(ioctl(fd, VIDIOC_STREAMOFF, &(bufferinfo.type)) < 0){
    perror("VIDIOC_STREAMOFF");
    exit(1);
  }
  

  // Free everything, and unload SDL.
  SDL_FreeYUVOverlay(frame);
  SDL_Quit();
  deallocate_mem( &buffer_start,& bufferinfo);
  exit(1);
}

