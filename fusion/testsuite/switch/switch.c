/*
   Task switch latency test.
   Max Krasnyansky <maxk@qualcomm.com

   Based on latency.c by Philippe Gerum <rpm@xenomai.org>
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <rtai/task.h>
#include <rtai/timer.h>
#include <rtai/sem.h>

RT_TASK event_task, worker_task;

RT_SEM switch_sem;
RTIME  switch_tsc;
unsigned long long switch_count;

long long minjitter = 10000000;
long long maxjitter = -10000000;
long long avgjitter = 0;
long long lost = 0;
long long nsamples = 10000;
long long sampling_period = 1000000;

#define HISTOGRAM_CELLS 100

unsigned long histogram[HISTOGRAM_CELLS];

int do_histogram = 0;
int ignore = 5;

static inline void add_histogram(long addval)
{
       long inabs = rt_timer_ticks2ns(addval >= 0 ? addval : -addval) / 1000;  /* usec steps */
       histogram[inabs < HISTOGRAM_CELLS ? inabs : HISTOGRAM_CELLS - 1]++;
}

void dump_histogram(void)
{
       int n;

       for (n = 0; n < HISTOGRAM_CELLS; n++) {
               long hits = histogram[n];
               if (hits)
                       fprintf(stderr, "%d - %d us: %ld\n", n, n + 1, hits);
       }
}

void event(void *cookie)
{
       int err;

       err = rt_timer_start(TM_ONESHOT);
       if (err) {
               printf("switch: cannot start timer, code %d\n", err);
               return;
       }

       err = rt_task_set_periodic(NULL, TM_NOW, sampling_period);
       if (err) {
               printf("switch: failed to set periodic, code %d\n", err);
               return;
       }

       for (;;) {
               err = rt_task_wait_period();
               if (err) {
                       if (err != -ETIMEDOUT) {
                               /* Timer stopped. */
                               rt_task_delete(NULL);
                       }
               }

               switch_count++;
               switch_tsc = rt_timer_tsc();

               rt_sem_broadcast(&switch_sem);
       }
}

void worker(void *cookie)
{
       long long minj = 10000000, maxj = -10000000, dt, sumj = 0;
       unsigned long long count = 0;
       int err, n;

       err = rt_sem_create(&switch_sem, "dispsem", 0, S_FIFO);
       if (err) {
               printf("switch: cannot create semaphore: %s\n",
                      strerror(-err));
               return;
       }

       for (n=0; n<nsamples; n++) {
               err = rt_sem_p(&switch_sem, TM_INFINITE);
               if (err) {
                       if (err != -EIDRM)
                               printf("switch: failed to pend on semaphore, code %d\n", err);

                       rt_task_delete(NULL);
               }

               if (++count != switch_count) {
                       count = switch_count;
                       lost++;
                       continue;
               }

               // First few switches are slow.
               // Probably due to the Linux <-> RT context migration at task startup.
               if (count < ignore)
                       continue;

               dt = (long) (rt_timer_tsc() - switch_tsc);
               if (dt > maxj)
                       maxj = dt;
               if (dt < minj)
                       minj = dt;
               sumj += dt;

               if (do_histogram)
                       add_histogram(dt);
       }

       rt_timer_stop();
       rt_sem_delete(&switch_sem);

       minjitter = minj;
       maxjitter = maxj;
       avgjitter = sumj / n;

       printf("RTH|%12s|%12s|%12s|%12s\n",
                      "lat min", "lat avg", "lat max", "lost");

       printf("RTD|%12Ld|%12Ld|%12Ld|%12lld\n",
                      rt_timer_ticks2ns(minjitter),
                      rt_timer_ticks2ns(avgjitter),
                      rt_timer_ticks2ns(maxjitter), lost);

       if (do_histogram)
               dump_histogram();

       exit(0);
}

int main(int argc, char **argv)
{
       int err, c;

       while ((c = getopt(argc, argv, "hp:n:i:")) != EOF)
               switch (c) {
               case 'h':
                       /* ./switch --h[istogram] */
                       do_histogram = 1;
                       break;

               case 'p':
                       sampling_period = atoi(optarg) * 1000;
                       break;

               case 'n':
                       nsamples = atoi(optarg);
                       break;

               case 'i':
                       ignore = atoi(optarg);
                       break;

               default:

                       fprintf(stderr, "usage: switch [options]\n"
                               "\t-h             - enable histogram\n" 
                               "\t-p <period_us> - timer period\n"
                               "\t-n <samples>   - number of samples to collect\n"
                               "\t-i <samples>   - number of _first_ samples to ignore\n");
                       exit(2);
               }

       if (sampling_period == 0)
               sampling_period = 100000;       /* ns */

       signal(SIGINT, SIG_IGN);
       signal(SIGTERM, SIG_IGN);

       setlinebuf(stdout);

       printf("== Sampling period: %llu us\n", sampling_period / 1000);
       printf("== Do not interrupt this program\n");

       err = rt_task_create(&worker_task, "worker", 0, 2, T_FPU);
       if (err) {
               printf("switch: failed to create worker task, code %d\n", err);
               return 0;
       }

       err = rt_task_start(&worker_task, &worker, NULL);
       if (err) {
               printf("switch: failed to start worker task, code %d\n", err);
               return 0;
       }

       err = rt_task_create(&event_task, "event", 0, 1, 0);
       if (err) {
               printf("switch: failed to create event task, code %d\n", err);
               return 0;
       }

       err = rt_task_start(&event_task, &event, NULL);
       if (err) {
               printf("switch: failed to start event task, code %d\n", err);
               return 0;
       }

       pause();

       return 0;
}
