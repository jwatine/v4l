#include "camera_constantes.h"
#include "camera_c/camera.h"


/*** First task : queue and dequeue the buffer ***/
char *  input_queue_buff( char buffer[BUFF_SIZE]){
  // Queue the buffer.
  if(ioctl(fd, VIDIOC_QBUF, &bufferinfo) < 0){
    perror("VIDIOC_QBUF");
    exit(1);
  }


  // Dequeue the buffer.
  if(ioctl(fd, VIDIOC_DQBUF, &bufferinfo) < 0){
    perror("VIDIOC_QBUF");
    exit(1);
  }
  

  memcpy(buffer,buffer_start,BUFF_SIZE);


  return buffer;
    
}
/*** second task : operation on buffers ***/
char *  operation_buffer_data(char  buffer[BUFF_SIZE]){

  int coefficient = 5;
  int i;
  for (i=0; i<bufferinfo.length;i++){
    buffer[i]*=coefficient;
  }
  
  return buffer;
}



/*** third task : display the video  ***/
//lock the frame so that we can copy new values from the buffer data
//in the pixels buffer.
void output_frame(char  buffer[BUFF_SIZE]){

  SDL_LockYUVOverlay(frame);
  memcpy((frame)->pixels[0],buffer, bufferinfo.length);
  (frame)->pitches[0] = 320;
 

  //display effectively the frame to the screen
  if(!SDL_DisplayYUVOverlay(frame,&position)){
    perror("SDL_Display:");
    // exit(1);
  }
  //unlock
  SDL_UnlockYUVOverlay(frame);
 


}
