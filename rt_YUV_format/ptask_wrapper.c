
/* ----------------------------------------------------------------------------
 * Copyright (C) 2009-2011, ONERA, Toulouse, FRANCE - LIFL, Lille, FRANCE
 *
 * This file is part of Prelude
 *
 * Prelude is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation ; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Prelude is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY ; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program ; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *---------------------------------------------------------------------------- */


// Authors: Titouan ROOS, Julien FORGET

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "ptask.h"
#include "pbarrier.h"
#include "encoded_task_params.h"

#include "open_device.h"
#include "camera_constantes.h"

#ifndef DEBUG
#define DEBUG
#endif

#if _POSIX_C_SOURCE <= 199309L
#error Version trop ancienne
#endif 

void task_body()
{
  int task_index = ptask_get_index();
  struct encoded_task_params *task_struct = ptask_get_argument();
  struct dword_t task_dword = task_struct->e_t_dword;
  int job_dd_idx=0;

#ifdef DEBUG
  struct timespec tp;
  double date_before_exec_ns,date_before_exec_s,date_after_exec_ns,date_after_exec_s;
  double total_time,total_sec,total_nsec;
#endif
  while(1)
    {
      //printf("Executing task %s... \n",task_struct->e_t_name);

#ifdef DEBUG
      if(!strcmp(task_struct->e_t_name,"queue_buff0")){
	if(!clock_gettime(0,&tp)){
	  perror("clock_gettime before ");
	}
	date_before_exec_ns=tp.tv_nsec;
	date_before_exec_s=tp.tv_sec;
      }
#endif
      ptime next_deadline;
      // call step function
      task_struct->e_t_body(NULL);
      
      // compute next deadline
      job_dd_idx++; // remember that first deadline was set at creation
      if (job_dd_idx==task_dword.pat_size) job_dd_idx=task_dword.pref_size;
      if (job_dd_idx < task_dword.pref_size)
        {
          next_deadline= (ptime)(task_dword.pref[job_dd_idx]);
        }
      else
        {
          next_deadline=(ptime)(task_dword.pat[job_dd_idx]);
        }
      ptask_set_deadline(task_index, next_deadline, MILLI);
      //printf("...task %s done, next deadline = %d \n", task_struct->e_t_name, ptask_get_deadline(task_index, MILLI));
      
#ifdef DEBUG
      if(!strcmp(task_struct->e_t_name,"queue_buff0")){
	if(!clock_gettime(0,&tp)){
	  perror("clock_gettime after ");
	}
	date_after_exec_ns=tp.tv_nsec;
	date_after_exec_s=tp.tv_sec;
	
	total_sec=date_after_exec_s-date_before_exec_s;
	total_nsec=(date_after_exec_ns-date_before_exec_ns);
	
	total_time=total_sec*1000+total_nsec/1000000;
      
	if (ptask_deadline_miss()){
	  printf("deadline miss : task %s , real time : %lf \n ",task_struct->e_t_name,total_time);
	}
	printf("...task %s done, next deadline = %d \n", task_struct->e_t_name, ptask_get_deadline(task_index, MILLI));
	      
      }

#endif
      
      ptask_wait_for_period();
    }
}

void initialisation(){

  /* open the device */
  open_fd();

  /* the function should be adjusted for the type of device you're using */
  set_video_format();

  /* inform the device about the buffer */
  struct v4l2_requestbuffers  bufrequest;
  buffers_initialization(&bufrequest);

  /*allocate memory to the buffer*/
  /*declare a pointer that should point to the buffer's memory area */
  mem_needed_device(fd,&bufferinfo);

  /* allocate memory */
  allocate_mem(fd,&bufferinfo,&buffer_start);

  /* initialiaze streaming */
  init_stream(&frame,&position, &screen);



}


int main()
{
  struct encoded_task_params *task_set;
  tpars param;
  int task_number, i;

  struct sigaction action;
  action.sa_handler=handler_signal;
  sigaction(SIGINT,&action,NULL);

  
  initialisation();
  ptask_init(SCHED_DEADLINE, PARTITIONED, PRIO_INHERITANCE);

  // get Prelude task set
  get_task_set(&task_number, &task_set);

  // create tasks
  for (i=0; i<task_number; i++)
    {
      ptime deadline;
      struct dword_t dw=task_set[i].e_t_dword;
      if(dw.pref_size>0)
        deadline=(ptime)(dw.pref[0]);
      else
        deadline=(ptime)(dw.pat[0]);
      ptask_param_init(param);
      ptask_param_argument(param, &task_set[i]);
      ptask_param_period(param, (ptime)(task_set[i].e_t_period), MILLI);
      ptask_param_deadline(param, deadline, MILLI);
      ptask_param_processor(param, 0); // mono-core scheduling
      ptask_param_runtime(param, (ptime)(task_set[i].e_t_wcet), MILLI);
      ptask_param_activation(param, DEFERRED);

      int ret = ptask_create_param(task_body, &param);

      if (ret<0) {
        fprintf(stderr,"Could not create task %s",task_set[i].e_t_name);
        exit(-1);
      }
      // printf("Created task %s with dd %d\n", task_set[i].e_t_name, (int)deadline);
    }
  printf("All tasks created\n");

  ptime now = ptask_gettime(MILLI);
  for (i=0; i<task_number; i++) {
    ptime offt = 10 + now; 
    int r = ptask_activate_at(i, offt, MILLI);
    if (r < 0) {
      printf("Could not activate task %s\n", task_set[i].e_t_name);
    }
  }
  printf("All tasks activated\n");
  
  
  // Don't exit otherwise tasks terminate
  while(1);

  // This should never return
  return 1;
}
