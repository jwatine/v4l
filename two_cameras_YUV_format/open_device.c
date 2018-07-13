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
#include <time.h>

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#define MILLISEC 0.001
#define NANO_TO_MICRO 1000
//#define DEBUG

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

  printf("Press space to continue\n");

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
   
  
  
  info.c_lflag = info_past.c_lflag;/*reload previous configuration*/
  info.c_cc[VMIN]=info_past.c_cc[VMIN];
  info.c_cc[VTIME]=info_past.c_cc[VTIME];

  if(tcsetattr(0, TCSANOW, &info)){ 
    perror("tcsetattr");
  }

 
  //Open video0 in both read and write modes.
  //device's name should be adjusted.
  if((*fd = open(device_path, O_RDWR)) < 0){
    perror("open");
    exit(1);
  }

#ifdef DEBUG
  printf("fd value : %d\n",*fd);
#endif
}

void close_fd(int * fd){
  if(!close(*fd)){
    perror("close");
  }
}

void set_video_format(int fd,  struct v4l2_format * format){
  format->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  format->fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  format->fmt.pix.width = 640;
  format->fmt.pix.height = 480;
 
  if(ioctl(fd, VIDIOC_S_FMT, format) < 0){
    perror("VIDIOC_S_FMT ");
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

/* map the memory for the buffer */
void allocate_mem(int fd,  struct v4l2_buffer * bufferinfo,void ** buffer_start){
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


  printf("taille buffer : %d \n",bufferinfo->length);
}

void deallocate_mem(void ** buffer_start, struct v4l2_buffer * bufferinfo){
  
  if(!munmap(*buffer_start,bufferinfo->length)){
    perror("munmap :");
  }
}


/* some operation on the YUC frame ...  */
void coeff_YUC_buffer_data( void ** buffer_start,struct v4l2_buffer * bufferinfo ){
  int static coefficient =0;
  int i;
  for (i=0; i<bufferinfo->length;i++){
    (*(unsigned char **)buffer_start)[i]+=coefficient;
  }

  coefficient++;
}

void reverse_buffer_data(void ** buffer_start, struct v4l2_buffer * bufferinfo){
  
  int i;
  char tmp;
  for (i =0 ; i<bufferinfo->length/2;i=i+3){
    
    tmp =(*(unsigned char **)buffer_start)[i];
    (*(unsigned char **)buffer_start)[i]=(*(unsigned char **)buffer_start)[bufferinfo->length-i-2];
    (*( unsigned char **)buffer_start)[bufferinfo->length-i-2]=tmp;

    tmp =(*(unsigned char **)buffer_start)[i+1];
    (*(unsigned char **)buffer_start)[i+1]=(*(unsigned char **)buffer_start)[bufferinfo->length-i-1];
    (*( unsigned char **)buffer_start)[bufferinfo->length-i-1]=tmp;

    tmp =(*(unsigned char **)buffer_start)[i+2];
    (*(unsigned char **)buffer_start)[i+2]=(*(unsigned char **)buffer_start)[bufferinfo->length-i];
    (*( unsigned char **)buffer_start)[bufferinfo->length-i]=tmp;
    
  }

  
}


void stream(int fd1,  struct v4l2_format format1,void * buffer_start1,  struct v4l2_buffer * bufferinfo1,int fd2,struct v4l2_format format2,void * buffer_start2,  struct v4l2_buffer * bufferinfo2){

  // Initialise everything.
  SDL_Init(SDL_INIT_VIDEO);
 
  // Get the screen's surface.
  SDL_Surface* screen = SDL_SetVideoMode(
					 format1.fmt.pix.width*2,
					 format1.fmt.pix.height*2,
					 32,
					 SDL_HWSURFACE);
 
     
  // Activate streaming
  int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if(ioctl(fd1, VIDIOC_STREAMON, &type) < 0){
    perror("VIDIOC_STREAMON");
    exit(1);
  }
  if(ioctl(fd2, VIDIOC_STREAMON, &type) < 0){
    perror("VIDIOC_STREAMON");
    exit(1);
  }

  
  //create an YUV overlay and the rect that "contains" the frame
  SDL_Overlay* frame1, *frame2,*frame3,*frame4;
  
  frame1 = SDL_CreateYUVOverlay(format1.fmt.pix.width,
			       format1.fmt.pix.height,
			       SDL_YUY2_OVERLAY,
			       screen);
  frame2 = SDL_CreateYUVOverlay(format2.fmt.pix.width,
			       format2.fmt.pix.height,
			       SDL_YUY2_OVERLAY,
			       screen);
  frame3 = SDL_CreateYUVOverlay(format1.fmt.pix.width,
			       format1.fmt.pix.height,
			       SDL_YUY2_OVERLAY,
			       screen);
  frame4 = SDL_CreateYUVOverlay(format1.fmt.pix.width,
			       format1.fmt.pix.height,
			       SDL_YUY2_OVERLAY,
			       screen);

  SDL_Rect position1 = {.x = 0, .y = 0,.w = format1.fmt.pix.width , .h=format1.fmt.pix.height};
  SDL_Rect position2 = {.x = 640, .y = 480,.w = format1.fmt.pix.width , .h=format2.fmt.pix.height};
  SDL_Rect position3 = {.x = 640, .y = 0,.w = format1.fmt.pix.width , .h=format1.fmt.pix.height};
  SDL_Rect position4 = {.x = 0, .y = 480,.w = format1.fmt.pix.width , .h=format2.fmt.pix.height};

  
  
  /* main loop */
  /* queue the buffer and dequeue it when it's filled */
  /* Display the frame on the screen once the buffer has been filled */
  int exit_prog =0;
  while(exit_prog<=200){


    /*** First task : queue and dequeue the buffer ***/

    // Queue the buffers.
    if(ioctl(fd1, VIDIOC_QBUF, bufferinfo1) < 0){
      perror("VIDIOC_QBUF");
      exit(1);
    }
    if(ioctl(fd2, VIDIOC_QBUF, bufferinfo2) < 0){
      perror("VIDIOC_QBUF");
      exit(1);
    }
        
    // Dequeue the buffers.
    if(ioctl(fd2, VIDIOC_DQBUF, bufferinfo2) < 0){
      perror("VIDIOC_DQBUF");
      exit(1);
    }
    if(ioctl(fd1, VIDIOC_DQBUF, bufferinfo1) < 0){
      perror("VIDIOC_DQBUF");
      exit(1);
    }


    //just reverse the frame... 
    if(exit_prog >100){    
      reverse_buffer_data(&buffer_start1,bufferinfo1);
    }
    else{
      reverse_buffer_data(&buffer_start2,bufferinfo2);
    }

    //cpy the data from the device's buffers to the frame's buffers
    memcpy(frame1->pixels[0],buffer_start1, bufferinfo1->length);
    frame1->pitches[0] = 320;
    memcpy(frame2->pixels[0],buffer_start2, bufferinfo2->length);
    frame2->pitches[0] = 320;

    //some operation on buffers to get some psychedelic images ! 
    coeff_YUC_buffer_data(&buffer_start1,bufferinfo1);
    coeff_YUC_buffer_data(&buffer_start2,bufferinfo2);

    //cpy the operated data to the frame's buffers.
    memcpy(frame3->pixels[0],buffer_start1, bufferinfo1->length);
    frame1->pitches[0] = 320;
    memcpy(frame4->pixels[0],buffer_start2, bufferinfo2->length);
    frame2->pitches[0] = 320;



    /*** third task : display the video  ***/
    //lock the frame so that we can copy new values in the buffer data
    SDL_LockYUVOverlay(frame1);
    SDL_LockYUVOverlay(frame2);
    SDL_LockYUVOverlay(frame3);
    SDL_LockYUVOverlay(frame4);
 
    
    //display effectively the frame to the screen with right position
    if(SDL_DisplayYUVOverlay(frame1,&position1)){
      perror("DisplayYUVOverlay");
    }
    if(SDL_DisplayYUVOverlay(frame2,&position2)){
      perror("DisplayYUVOverlay");
    }
    if(SDL_DisplayYUVOverlay(frame3,&position3)){
      perror("DisplayYUVOverlay");
    }
    if(SDL_DisplayYUVOverlay(frame4,&position4)){
      perror("DisplayYUVOverlay");
    }


    //unlock...
    SDL_UnlockYUVOverlay(frame1);
    SDL_UnlockYUVOverlay(frame2);
    SDL_UnlockYUVOverlay(frame3);
    SDL_UnlockYUVOverlay(frame4);
    
    /*Delay for a moment what about trying to sleep for a short or larger moment ? ... */
    //usleep(200/MILLISEC);
    exit_prog++;
  }
 
  // Deactivate streaming
  if(ioctl(fd1, VIDIOC_STREAMOFF, &type) < 0){
    perror("VIDIOC_STREAMOFF");
    exit(1);
  }
  if(ioctl(fd2, VIDIOC_STREAMOFF, &type) < 0){
    perror("VIDIOC_STREAMOFF");
    exit(1);
  }
  

  // Free everything, and unload SDL.
  SDL_FreeYUVOverlay(frame1);
  SDL_FreeYUVOverlay(frame2);
  SDL_FreeYUVOverlay(frame3);
  SDL_FreeYUVOverlay(frame4);

  SDL_Quit();



}

//to save the frame to YUV format ...
void save_frame(void * buffer_start,  struct v4l2_buffer * bufferinfo){

  int YUVfile;
  if((YUVfile = open("./image.yuv", O_WRONLY | O_CREAT, 0660)) < 0){
    perror("open");
    exit(1);
  }
  
  write(YUVfile, buffer_start, bufferinfo->length);
  close(YUVfile);
}


int main(void){

  

  int fd1,fd2;
  /* open the devices */
  open_fd(&fd1);
  open_fd(&fd2);
  

  /* check for the device's capability */
  /* v4l_capability(fd1); */
  /* v4l_capability(fd2); */
  
  /* Bash commands to know the formats supported by the device */
  /* v4l2-ctl -d /dev/video0 --list-formats-ext */  

  /* the function should be adjusted for the type of device you're using */
  struct v4l2_format  format1,format2;
  set_video_format(fd1,&format1);
  set_video_format(fd2,&format2);

  /* inform the device about the buffers */
  struct v4l2_requestbuffers  bufrequest1,bufrequest2;
  buffers_initialization(fd1, &bufrequest1);
  buffers_initialization(fd2, &bufrequest2);
  
  /* ask to the device the size of the buffer it needs */
  struct v4l2_buffer  bufferinfo1,bufferinfo2;
  mem_needed_device(fd1,&bufferinfo1);
  mem_needed_device(fd2,&bufferinfo2);

  /*allocate memory to the buffer*/
  /*declare a pointer that should point to the buffer's memory area */
  void * buffer_start1;
  void * buffer_start2;
  allocate_mem(fd1,&bufferinfo1,&buffer_start1);
  allocate_mem(fd2,&bufferinfo2,&buffer_start2);

  /* get a frame from the device and display it ( most important part ! )  */
  stream(fd1,format1,buffer_start1,&bufferinfo1,fd2,format2,buffer_start2,&bufferinfo2);

  /* deallocate */
  deallocate_mem(&buffer_start1,&bufferinfo1);
  deallocate_mem(&buffer_start2,&bufferinfo2);
  /* close the file descriptor */
  close_fd(&fd1);
  close_fd(&fd2);

  
  
  return EXIT_SUCCESS;
}
