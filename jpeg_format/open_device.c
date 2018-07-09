#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL.h>


#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>

void v4l_capability(int fd){
  int N=100;
  printf("Quering camera capabilities...\n");
  struct v4l2_capability vid_cap;
  if(ioctl(fd, VIDIOC_QUERYCAP, &vid_cap) >= 0) {
    printf("  Driver: %s\n", vid_cap.driver);
    printf("  Card: %s\n", vid_cap.card);
    printf("  Bus Info: %s\n", vid_cap.bus_info);
    printf("  Version: %2d\n", vid_cap.version);
    printf("  Capabilities: \n");

    if (vid_cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
      printf("    Video capture support: Yes\n");
    } else {
      printf("    Video capture support: No\n");
    }
    if (vid_cap.capabilities & V4L2_CAP_VIDEO_OUTPUT) {
      printf("    Video output support: Yes\n");
    } else {
      printf("    Video output support: No\n");
    }
    if (vid_cap.capabilities & V4L2_CAP_VIDEO_OVERLAY) {
      printf("    Video overlay support: Yes\n");
    } else {
      printf("    Video overlay support: No\n");
    }
    if (vid_cap.capabilities & V4L2_CAP_VBI_CAPTURE) {
      printf("    Raw VBI capture support: Yes\n");
    } else {
      printf("    Raw VBI capture support: No\n");
    }
    if (vid_cap.capabilities & V4L2_CAP_VBI_OUTPUT) {
      printf("    RAW VBI output support: Yes\n");
    } else {
      printf("    RAW VBI output support: No\n");
    }
    if (vid_cap.capabilities & V4L2_CAP_SLICED_VBI_CAPTURE){
      printf("    Sliced VBI capture support: Yes\n");
    } else {
      printf("    Sliced VBI capture support: No\n");
    }
    if (vid_cap.capabilities & V4L2_CAP_SLICED_VBI_OUTPUT){
      printf("    Sliced VBI capture support: Yes\n");
    } else {
      printf("    Sliced VBI capture support: No\n");
    }
    if (vid_cap.capabilities & V4L2_CAP_RDS_CAPTURE){
      printf("    RDS capture support: Yes\n");
    } else {
      printf("    RDS capture support: No\n");
    }
    if (vid_cap.capabilities & V4L2_CAP_TUNER){
      printf("    Device has a tuner: Yes\n");
    } else {
      printf("    Device has a tuner: No\n");
    }
    if (vid_cap.capabilities & V4L2_CAP_AUDIO){
      printf("    Audio support: Yes\n");
    } else {
      printf("    Audio support: No\n");
    }
    if (vid_cap.capabilities & V4L2_CAP_RADIO){
      printf("    Device has radio support: Yes\n");
    } else {
      printf("    Device has radio support: No\n");
    }
    if (vid_cap.capabilities & V4L2_CAP_READWRITE){
      printf("    Supports read/write system calls: Yes\n");
    } else {
      printf("    Supports read/write system calls: No\n");
    }
    if (vid_cap.capabilities & V4L2_CAP_ASYNCIO){
      printf("    Supports async i/o: Yes\n");
    } else {
      printf("    Supports async i/o: No\n");
    }
    if (vid_cap.capabilities & V4L2_CAP_STREAMING){
      printf("    Supports streaming: Yes\n");
    } else {
      printf("    Supports streaming: No\n");
    }
  }
  else {
    perror("VIDEO_QUERY");
    exit(1);
  }

  printf("\n\n\n");

  printf("Discovering camera controls...\n");
  struct v4l2_queryctrl queryctrl;
  struct v4l2_querymenu querymenu;
  memset (&queryctrl, 0, sizeof (queryctrl));
  for (queryctrl.id = V4L2_CID_BASE; queryctrl.id < V4L2_CID_BASE+N; queryctrl.id++) {
    if (ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl) >= 0) {
      if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	continue;
      printf("VIDIOC_QUERYCTRL(V4L2_CID_BASE+%d)\n", queryctrl.id-V4L2_CID_BASE);
      printf("   id: %d\n", queryctrl.id);

      switch (queryctrl.type){
      case V4L2_CTRL_TYPE_INTEGER:
	printf("   type: INTEGER\n");
	break;
      case V4L2_CTRL_TYPE_BOOLEAN:
	printf("   type: BOOLEAN\n");
	break;
      case V4L2_CTRL_TYPE_MENU:
	printf("   type: MENU\n");
	querymenu.id = queryctrl.id;
	for (querymenu.index = queryctrl.minimum; querymenu.index < queryctrl.maximum; querymenu.index++){
	  printf("      menu id: %d\n", querymenu.index);
	  printf("      menu name: %s\n", querymenu.name);
	}
	break;
      case V4L2_CTRL_TYPE_BUTTON:
	printf("   type: BUTTON\n");
	break;
      } /* end switch */
      printf("   name: %s\n", queryctrl.name);
      printf("   minimum: %d\n", queryctrl.minimum);
      printf("   maximum: %d\n", queryctrl.maximum);
      printf("   step: %d\n", queryctrl.step);
      printf("   default_value: %d\n", queryctrl.default_value);
      printf("   flags: %d\n", queryctrl.flags);
    } else {
      if (errno == EINVAL)
	continue;
      perror("VIDIOC_QUERYCTRL");
      break;
    } /* end if*/
  } /* end for */

}


void  open_fd(int * fd){

  char device_path[50],c;
    
  printf("*** Enter your device's path : ***\n");
  scanf("%s",device_path);

  printf("Press backspace to continue\n");

  struct termios info;
  struct termios info_past;
  if(tcgetattr(0, &info)){       /* get current terminal attributes; 0 is the file descriptor for stdin */
    perror("tcgetattr");
  }   

  info_past.c_lflag = info.c_lflag;/*save previous configuration*/
  info_past.c_cc[VMIN]=info.c_cc[VMIN];
  info_past.c_cc[VTIME]=info.c_cc[VTIME];
    
  info.c_lflag &= ~ICANON;      /* disable canonical mode */
  info.c_cc[VMIN] = 1;          /* wait until at least one keystroke available */
  info.c_cc[VTIME] = 0;         /* no timeout */

  if(tcsetattr(0, TCSANOW, &info)){ /* set immediately */
    perror("tcsetattr");
  }

  while((c=getchar())!=32); /* press backspace  to continue... ! */
   
  
  
  info.c_lflag = info_past.c_lflag;/*save previous configuration*/
  info.c_cc[VMIN]=info_past.c_cc[VMIN];
  info.c_cc[VTIME]=info_past.c_cc[VTIME];

  if(tcsetattr(0, TCSANOW, &info)){ 
    perror("tcsetattr");
  }

 
  //Open video0 in both read and write modes.
  //device's name should be adjusted.
  if((*fd = open("/dev/video0", O_RDWR)) < 0){
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

void set_video_format(int fd,  struct v4l2_format * format){
  format->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  format->fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
  format->fmt.pix.width = 800;
  format->fmt.pix.height = 600;
 
  if(ioctl(fd, VIDIOC_S_FMT, format) < 0){
    perror("VIDIOC_S_FMT");
    exit(1);
  }
}

/* inform the device about the future buffers */
void buffers_initialization(int fd,struct v4l2_requestbuffers *  bufrequest){
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

void allocate_mem(int fd,  struct v4l2_buffer * bufferinfo,void * buffer_start){
  buffer_start = mmap(
		      NULL,
		      bufferinfo->length,
		      PROT_READ | PROT_WRITE,
		      MAP_SHARED,
		      fd,
		      bufferinfo->m.offset
		      );
  
  if(buffer_start == MAP_FAILED){
    perror("mmap");
    exit(1);
  }

}


void stream(int fd,  struct v4l2_format format,void * buffer_start,  struct v4l2_buffer * bufferinfo){

  buffer_start = mmap(
		      NULL,
		      bufferinfo->length,
		      PROT_READ | PROT_WRITE,
		      MAP_SHARED,
		      fd,
		      bufferinfo->m.offset
		      );
  
  if(buffer_start == MAP_FAILED){
    perror("mmap");
    exit(1);
  }

  // Initialise everything.
  SDL_Init(SDL_INIT_VIDEO);
  IMG_Init(IMG_INIT_JPG);
 
  // Get the screen's surface.
  SDL_Surface* screen = SDL_SetVideoMode(
					 format.fmt.pix.width,
					 format.fmt.pix.height,
					 32, SDL_HWSURFACE
					 );
 
  SDL_RWops* buffer_stream;
  SDL_Surface* frame;
  SDL_Rect position = {.x = 0, .y = 0};

  // Create a stream based on our buffer.
  
  if((buffer_stream = SDL_RWFromMem(buffer_start, bufferinfo->length))==NULL){
    printf("SDL_RWFromMem failed \n");
  }
  
  
  /* // Put the buffer in the incoming queue. */
  /* //if it doesn't work supress that call and reverse the dequeue and queue in the loop */
  /* if(ioctl(fd, VIDIOC_QBUF, bufferinfo) < 0){ */
  /*   perror("VIDIOC_QBUF"); */
  /*   exit(1); */
  /* } */
 
  // Activate streaming
  int type = bufferinfo->type;
  if(ioctl(fd, VIDIOC_STREAMON, &type) < 0){
    perror("VIDIOC_STREAMON");
    exit(1);
  }

  bufferinfo->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  bufferinfo->memory = V4L2_MEMORY_MMAP;
  bufferinfo->index = 0;
  int exit_prog=0;
  while(exit_prog<=10){

    // Queue the next one.
    if(ioctl(fd, VIDIOC_QBUF, bufferinfo) < 0){
      perror("VIDIOC_QBUF");
      exit(1);
    }
    
    // Dequeue the buffer.
    if(ioctl(fd, VIDIOC_DQBUF, bufferinfo) < 0){
      perror("VIDIOC_QBUF");
      exit(1);
    }


    // Create a surface using the data coming out of the above stream.
    frame = IMG_Load_RW(buffer_stream, 0);
    
    // Blit the surface and flip the screen.
    SDL_BlitSurface(frame, NULL, screen, &position);
    SDL_Flip(screen);

    sleep(1);
    exit_prog++;
  }
 
  // Deactivate streaming
  if(ioctl(fd, VIDIOC_STREAMOFF, &type) < 0){
    perror("VIDIOC_STREAMOFF");
    exit(1);
  }

 

  // Free everything, and unload SDL & Co.
  SDL_FreeSurface(frame);
  SDL_RWclose(buffer_stream);
  IMG_Quit();
  SDL_Quit();



}



int main(void){


  int fd;
  /* open the device, this function handles errors */
  open_fd(&fd);

  /* check for the device's capability */
  v4l_capability(fd);
  
  /* Bash commands to know the formats supported by the device */
  /* v4l2-ctl -d /dev/video0 --list-formats-ext */  
  /* the function should be adjusted for the type of device you're using */
  struct v4l2_format  format;
  set_video_format(fd,&format);

  /* inform the device about the buffer */
  struct v4l2_requestbuffers  bufrequest;
  buffers_initialization(fd, &bufrequest);

  /* ask to the device the size of the buffer it needs */
  struct v4l2_buffer  bufferinfo;
  mem_needed_device(fd,&bufferinfo);


  /*allocate memory to the buffer*/
  /*declare a pointer that should point to the buffer's memory area */
  void * buffer_start;
  //  allocate_mem(fd,&bufferinfo,buffer_start);

  
  
  /* get a frame from the device and display it ( most important part ! )  */
  stream(fd,format,buffer_start,&bufferinfo);
  
  /* close the file descriptor */
  close_fd(&fd);


  
  
  return EXIT_SUCCESS;
}
