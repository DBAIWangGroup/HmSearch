/*
 * usage.h
 *
 *  Created on: Oct 27, 2011
 *      Author: jianbin
 */
#ifndef __usage_h__
#define __usage_h__



struct mytimer{
  struct timeval save_t;
  double timesum;
};

void StartTimer(struct mytimer *timer);

/* { */
/*   gettimeofday(&(timer->save_t), NULL); */
/* } */

void PauseTImer(struct mytimer *timer);
/* { */
/*   struct timeval res; */
/*   struct timeval elapse_t; */
 
/*   timersub(&elapse_t, &(timer->save_t), &res); */
/*   timer->timesum += (res.tv_sec) + res.tv_usec/1000000.0;   */
/* } */

void ResetUsage(void);

double ShowUsage(void);

#endif


