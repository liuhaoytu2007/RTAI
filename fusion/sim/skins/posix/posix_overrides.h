#ifndef PSE51_OVERRIDE_LIBC_DEFINES_H
#define PSE51_OVERRIDE_LIBC_DEFINES_H

#undef errno
struct sched_param {
    int sched_priority;
};
#define pthread_attr_t pse51_attr_t
#define pthread_t pse51_thread_t
#define pthread_mutexattr_t pse51_mutexattr_t
#define pthread_mutex_t pse51_mutex_t
#define clockid_t pse51_clockid_t
#define pthread_condattr_t pse51_condattr_t
#define pthread_cond_t pse51_cond_t
struct sigaction {
    sighandler_t *sa_handler;
    sigset_t sa_mask;
    int sa_flags;
};
#define pthread_key_t pse51_key_t
#define pthread_once_t pse51_once_t


#define pthread_attr_init pse51_threadattr_init
#define pthread_attr_destroy pse51_threadattr_destroy
#define pthread_attr_getdetachstate pse51_threadattr_getdetachstate
#define pthread_attr_setdetachstate pse51_threadattr_setdetachstate
#define pthread_attr_getstackaddr pse51_threadattr_getstackaddr
#define pthread_attr_setstackaddr pse51_threadattr_setstackaddr
#define pthread_attr_getstacksize pse51_threadattr_getstacksize
#define pthread_attr_setstacksize pse51_threadattr_setstacksize
#define pthread_attr_getinheritsched pse51_threadattr_getinheritsched
#define pthread_attr_setinheritsched pse51_threadattr_setinheritsched
#define pthread_attr_getschedpolicy pse51_threadattr_getschedpolicy
#define pthread_attr_setschedpolicy pse51_threadattr_setschedpolicy
#define pthread_attr_getschedparam pse51_threadattr_getschedparam
#define pthread_attr_setschedparam pse51_threadattr_setschedparam
#define pthread_attr_getscope pse51_threadattr_getscope
#define pthread_attr_setscope pse51_threadattr_setscope
#define pthread_attr_getname_np pse51_threadattr_getname_np
#define pthread_attr_setname_np pse51_threadattr_setname_np
#define pthread_attr_getfp_np pse51_threadattr_getfp_np
#define pthread_attr_setfp_np pse51_threadattr_setfp_np
#define pthread_detach pse51_thread_detach
#define pthread_equal pse51_thread_equal
#define pthread_exit pse51_thread_exit
#define pthread_join pse51_thread_join
#define pthread_self pse51_thread_self
#define pthread_getschedparam pse51_thread_getschedparam
#define pthread_setschedparam pse51_thread_setschedparam
#define pthread_mutexattr_init pse51_mutexattr_init
#define pthread_mutexattr_destroy pse51_mutexattr_destroy
#define pthread_mutexattr_gettype pse51_mutexattr_gettype
#define pthread_mutexattr_settype pse51_mutexattr_settype
#define pthread_mutexattr_getprotocol pse51_mutexattr_getprotocol
#define pthread_mutexattr_setprotocol pse51_mutexattr_setprotocol
#define pthread_mutex_init pse51_mutex_init
#define pthread_mutex_destroy pse51_mutex_destroy
#define pthread_mutex_trylock pse51_mutex_trylock
#define pthread_mutex_lock pse51_mutex_lock
#define pthread_mutex_timedlock pse51_mutex_timedlock
#define pthread_mutex_unlock pse51_mutex_unlock
#define pthread_mutex_getprioceiling pse51_mutex_getprioceiling
#define pthread_mutex_setprioceiling pse51_mutex_setprioceiling
#define pthread_condattr_init pse51_condattr_init
#define pthread_condattr_destroy pse51_condattr_destroy
#define pthread_condattr_getclock pse51_condattr_getclock
#define pthread_condattr_setclock pse51_condattr_setclock
#define pthread_cond_init pse51_cond_init
#define pthread_cond_destroy pse51_cond_destroy
#define pthread_cond_wait pse51_cond_wait
#define signal pse51_signal
#define pthread_cond_broadcast pse51_cond_broadcast
#define sem_init pse51_sem_init
#define sem_destroy pse51_sem_destroy
#define sem_post pse51_sem_post
#define sem_trywait pse51_sem_trywait
#define sem_wait pse51_sem_wait
#define sem_timedwait pse51_sem_timedwait
#define sem_getvalue pse51_sem_getvalue
#define pthread_cancel pse51_cancel
#define pthread_cleanup_pop pse51_cleanup_pop
#define pthread_setcancelstate pse51_setcancelstate
#define pthread_setcanceltype pse51_setcanceltype
#define pthread_testcancel pse51_testcancel
#define sigemptyset pse51_sigemptyset
#define sigfillset pse51_sigfillset
#define sigaddset pse51_sigaddset
#define sigdelset pse51_sigdelset
#define sigismember pse51_sigismember
#define pthread_kill pse51_kill
#define pthread_sigmask pse51_thread_sigmask
#define sigpending pse51_sigpending
#define sigwait pse51_sigwait
#define pthread_key_delete pse51_key_delete
#define pthread_getspecific pse51_getspecific
#define pthread_setspecific pse51_setspecific
#define clock_getres pse51_clock_getres
#define clock_gettime pse51_clock_gettime
#define clock_settime pse51_clock_settime
#endif /*PSE51_OVERRIDE_LIBC_DEFINES_H*/
