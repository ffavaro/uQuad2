/**
 * uquad_time: lib with aux functions to help timevale handling.
 * Copyright (C) 2012  Rodrigo Rosa <rodrigorosa.lg gmail.com>, Matias Tailanian <matias tailanian.com>, Santiago Paternain <spaternain gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @file   uquad_aux_time.c
 * @author Rodrigo Rosa <rodrigorosa.lg gmail.com>, Matias Tailanian <matias tailanian.com>, Santiago Paternain <spaternain gmail.com>
 * @date   Sun May 27 11:20:03 2012
 *
 * @brief lib with aux functions to help timevale handling.
 *
 */
#include "uquad_aux_time.h"

int uquad_timeval_substract (struct timeval * result, struct timeval x, struct timeval y){
    /* Perform the carry for the later subtraction by updating y. */
    if (x.tv_usec < y.tv_usec) {
	int nsec = (y.tv_usec - x.tv_usec) / 1000000 + 1;
	y.tv_usec -= 1000000 * nsec;
	y.tv_sec += nsec;
    }
    if (x.tv_usec - y.tv_usec > 1000000) {
	int nsec = (y.tv_usec - x.tv_usec) / 1000000;
	y.tv_usec += 1000000 * nsec;
	y.tv_sec -= nsec;
    }
    
    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x.tv_sec - y.tv_sec;
    result->tv_usec = x.tv_usec - y.tv_usec;
    
    if(x.tv_sec < y.tv_sec)
	// -1 if diff is negative
	return -1;
    if(x.tv_sec > y.tv_sec)
	// 1 if diff is positive
	return 1;
    // second match, check usec
    if(x.tv_usec < y.tv_usec)
	// -1 if diff is negative
	return -1;
    if(x.tv_usec > y.tv_usec)
	// 1 if diff is positive
	return 1;

    // 0 if equal
    return 0;
}

int in_range_us(struct timeval tv_diff, long int min_us, long int max_us)
{
    int retval = 0;
    if(tv_diff.tv_sec > 0       ||
       tv_diff.tv_usec > max_us ||
       tv_diff.tv_usec < min_us)
    {
	if(tv_diff.tv_sec > 0 || tv_diff.tv_usec > max_us)
	    retval = 1;
	else
	    retval = -1;
    }
    return retval;
}

/**
 * calcula diferencia entre tiempo de entrada al loop ('tv_in')y tiempo
 * actual ('tv_end') y manda a dormir la cantidad de tiempo que falte 
 * para completar 'loop_duration_usec'.
 */
int wait_loop_T_US(unsigned long loop_duration_usec, struct timeval tv_in)
{
   struct timeval tv_end, tv_diff;
   gettimeofday(&tv_end,NULL);
   int retval = uquad_timeval_substract(&tv_diff, tv_end, tv_in);
   if(retval > 0)
   {
      if(tv_diff.tv_usec < loop_duration_usec)
         usleep(loop_duration_usec - (unsigned long)tv_diff.tv_usec); // Sobro tiempo, voy a dormir
   } else
         err_log("WARN: Main Absurd timing!");
   
#if DEBUG_TIMING_MAIN
      gettimeofday(&tv_end,NULL);
      retval = uquad_timeval_substract(&tv_diff, tv_end, tv_in);
      printf("duracion loop main: %lu\n",(unsigned long)tv_diff.tv_usec);
#endif

   return retval;
}


static struct timeval main_start_time;

void set_main_start_time(void)
{
   gettimeofday(&main_start_time,NULL);
   return;
}


struct timeval get_main_start_time(void)
{
   return main_start_time;
}

