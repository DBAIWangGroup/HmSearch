
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#include "usage.h"


//static struct rusage Save_r;
static struct timeval Save_t;

//long __user_time_sec;
//long __user_time_usec;

//long __sys_time_sec;
//long __sys_time_usec;
double __elapse_time;
double __elapse_time_sec;
double __elapse_time_usec;

char __usage_information[1024] = "";
char _user_time[256] = "";

void ResetUsage(void) {
  //getrusage(RUSAGE_SELF, &Save_r);
  __elapse_time = __elapse_time_sec = __elapse_time_usec = 0;
  gettimeofday(&Save_t, NULL);
}

double ShowUsage(void) {
  struct timeval res;
  struct timeval elapse_t;
  //struct rusage r;

  //getrusage(RUSAGE_SELF, &r);
  gettimeofday(&elapse_t, NULL);

/*
  if (elapse_t.tv_usec < Save_t.tv_usec) {
    elapse_t.tv_sec--;
    elapse_t.tv_usec += 1000000;
  }
  if (r.ru_utime.tv_usec < Save_r.ru_utime.tv_usec) {
    r.ru_utime.tv_sec--;
    r.ru_utime.tv_usec += 1000000;
  }
  if (r.ru_stime.tv_usec < Save_r.ru_stime.tv_usec) {
    r.ru_stime.tv_sec--;
    r.ru_stime.tv_usec += 1000000;
  }
*/
  timersub(&elapse_t, &Save_t, &res);
  __elapse_time += (res.tv_sec) + res.tv_usec/1000000.0;

  /* sprintf(__usage_information, */
  /*         "Searching usage stats %.6f elapsed", __elapse_time); */
  return __elapse_time;
}


void StartTimer(struct mytimer *timer){
  gettimeofday(&(timer->save_t), NULL);
}

void PauseTImer(struct mytimer *timer){
  struct timeval res;
  struct timeval elapse_t;
  gettimeofday(&elapse_t, NULL); 
  timersub(&elapse_t, &(timer->save_t), &res);
  timer->timesum += (res.tv_sec) + res.tv_usec/1000000.0;  
}







