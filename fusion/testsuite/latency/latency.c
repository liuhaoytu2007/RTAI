#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <rtai/task.h>
#include <rtai/timer.h>
#include <rtai/sem.h>

RT_TASK latency_task, display_task;

RT_SEM display_sem;

long minjitter = 10000000,
     maxjitter = -10000000,
     avgjitter = 0,
     overrun = 0;

int sampling_period = 0;

#define SAMPLE_COUNT (1000000000 / sampling_period)

#define HISTOGRAM_CELLS 100

unsigned long histogram[HISTOGRAM_CELLS];

int do_histogram = 0, finished = 0;

static inline void add_histogram (long addval)

{
    long inabs = rt_timer_ticks2ns(addval >= 0 ? addval : -addval) / 1000; /* us steps */
    histogram[inabs < HISTOGRAM_CELLS ? inabs : HISTOGRAM_CELLS-1]++;
}

void latency (void *cookie)

{
    long minj, maxj = -10000000, dt, sumj;
    RTIME itime, expected, period;
    int err, count;

    err = rt_timer_start(RT_TIMER_ONESHOT);

    if (err)
	{
	printf("latency: cannot start timer, code %d\n",err);
	return;
	}

    period = rt_timer_ns2ticks(sampling_period);
    itime = rt_timer_read() + period * 5;
    expected = rt_timer_ns2ticks(itime);
    err = rt_task_set_periodic(NULL,itime,period);

    if (err)
	{
	printf("latency: failed to set periodic, code %d\n",err);
	return;
	}

    for (;;)
	{
	minj = 10000000;

	for (count = sumj = 0; count < SAMPLE_COUNT; count++)
	    {
	    expected += period;
	    err = rt_task_wait_period();

	    if (err)
		{
		if (err != -ETIMEDOUT)
		    rt_task_delete(NULL); /* Timer stopped. */

		overrun++;
		}
	    else
		{
		dt = (long)(rt_timer_tsc() - expected);
		if (dt > maxj) maxj = dt;
		if (dt < minj) minj = dt;
		sumj += dt;

		if (do_histogram && !finished)
		    add_histogram(dt);
		}
	    }

	minjitter = minj;
	maxjitter = maxj;
	avgjitter = sumj / SAMPLE_COUNT;
	rt_sem_v(&display_sem);
	}
}

void display (void *cookie)

{
    int err;

    err = rt_sem_create(&display_sem,"dispsem",0,S_FIFO);

    if (err)
	{
        printf("latency: cannot create semaphore: %s\n",strerror(-err));
	return;
	}

    for (;;)
	{
	err = rt_sem_p(&display_sem,RT_TIME_INFINITE);

	if (err)
	    {
	    if (err != -EIDRM)
		printf("latency: failed to pend on semaphore, code %d\n",err);

	    rt_task_delete(NULL);
	    }

	printf("min = %Ld ns, max = %Ld ns, avg = %Ld ns, overrun = %ld\n",
	       rt_timer_ticks2ns(minjitter),
	       rt_timer_ticks2ns(maxjitter),
	       rt_timer_ticks2ns(avgjitter),
	       overrun);
	}
}

void dump_histogram (void)

{
    int n;
  
    for (n = 0; n < HISTOGRAM_CELLS; n++)
	{
	long hits = histogram[n];

	if (hits)
	    fprintf(stderr,"%d - %d us: %ld\n",n,n + 1,hits);
	}
}

void cleanup_upon_sig(int sig __attribute__((unused)))

{
    rt_timer_stop();
    finished = 1;
    rt_sem_delete(&display_sem);

    if (do_histogram)
	dump_histogram();

    exit(0);
}

int main (int argc, char **argv)

{
    int err, c;

    while ((c = getopt(argc,argv,"hp:")) != EOF)
	switch (c)
	    {
	    case 'h':
		/* ./latency --h[istogram] */
		do_histogram = 1;
		break;

	    case 'p':

		sampling_period = atoi(optarg);
		break;

	    default:
		
		fprintf(stderr,"usage: latency [-h][-p <period_ns>]\n");
		exit(2);
	    }

    if (sampling_period == 0)
	sampling_period = XNARCH_CALIBRATION_PERIOD;

    signal(SIGINT, cleanup_upon_sig);
    signal(SIGTERM, cleanup_upon_sig);

    setvbuf(stdout, (char *)NULL, _IOLBF, 0);
    printf("== Sampling period: %d ns\n",sampling_period);

    err = rt_task_create(&display_task,"display",0,2,0);

    if (err)
	{
	printf("latency: failed to create display task, code %d\n",err);
	return 0;
	}

    err = rt_task_start(&display_task,&display,NULL);

    if (err)
	{
	printf("latency: failed to start display task, code %d\n",err);
	return 0;
	}

    err = rt_task_create(&latency_task,"sampling",0,1,T_FPU);

    if (err)
	{
	printf("latency: failed to create latency task, code %d\n",err);
	return 0;
	}

    err = rt_task_start(&latency_task,&latency,NULL);

    if (err)
	{
	printf("latency: failed to start latency task, code %d\n",err);
	return 0;
	}

    pause();

    return 0;
}
