/*
 * Written by Gilles Chanteperdrix <gilles.chanteperdrix@laposte.net>.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include "posix/thread.h"

/* Only CLOCK_MONOTONIC is supported for the moment. */

int clock_getres (clockid_t clock_id, struct timespec *res)

{
    if (clock_id != CLOCK_MONOTONIC || !res)
        return EINVAL;

    ticks2timespec(res, 1);

    return 0;
}

int clock_gettime (clockid_t clock_id, struct timespec *tp)

{
    if (clock_id != CLOCK_MONOTONIC || !tp)
        return EINVAL;

    ticks2timespec(tp, xnpod_get_time());

    return 0;    
}

int clock_settime(clockid_t clock_id, const struct timespec *tp)

{
    if (clock_id != CLOCK_MONOTONIC || !tp || tp->tv_nsec > 1000000000)
        return EINVAL;

    xnpod_set_time(timespec2ticks(tp));

    return 0;
}

int clock_nanosleep (clockid_t clock_id,
		     int flags,
		     const struct timespec *rqtp,
		     struct timespec *rmtp)
{
    xnticks_t start, timeout;
    pthread_t cur;
    spl_t s;

    if (clock_id != CLOCK_MONOTONIC || !rqtp || rqtp->tv_nsec > 1000000000)
        return EINVAL;

    cur = pse51_current_thread();
    timeout = timespec2ticks(rqtp);
    start = xnpod_get_time();

    switch (flags)
	{
	default:

	    return EINVAL;

	case TIMER_ABSTIME:

	    timeout -= start;
	    break;

	case 0:
	    break;
	}

    xnlock_get_irqsave(&nklock, s);
    
    xnpod_suspend_thread(&cur->threadbase, XNDELAY, timeout,NULL);

    thread_cancellation_point(cur);
        
    if (xnthread_test_flags(&cur->threadbase, XNBREAK))
	{
        xnlock_put_irqrestore(&nklock, s);

        if (flags == 0 && rmtp) {
            xnticks_t passed = xnpod_get_time() - start;

            ticks2timespec(rmtp, passed < timeout ? timeout-passed : 0);
        }

        return EINTR;
	}

    xnlock_put_irqrestore(&nklock, s);
    
    return 0;
}

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp) {

    return clock_nanosleep(CLOCK_REALTIME, 0, rqtp, rmtp);
}
