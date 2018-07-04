#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <string.h>
#include <errno.h>
#include <termios.h>

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


int open_fd(){
  int fd;
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
  if((fd = open("/dev/video0", O_RDWR)) < 0){
    perror("open");
    exit(1);
  }
  else
    return fd;
}

void close_fd(int fd){
  if(!close(fd)){
    perror("close");
    exit(1);
      
  }
}


int main(void){

  int fd=open_fd();
  
  struct v4l2_capability cap;
  if(ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0){
    perror("VIDIOC_QUERYCAP");
    exit(1);
  }

  close_fd(fd);
  
  //Bash commands to know the formats supported by the device
  //v4l2-ctl -d /dev/video0 --list-formats-ext
  
  return EXIT_SUCCESS;
}
