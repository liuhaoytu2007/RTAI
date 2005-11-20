/**
 * @file
 * Real-Time Driver Model for RTAI
 *
 * @note Copyright (C) 2005 Jan Kiszka <jan.kiszka@web.de>
 * @note Copyright (C) 2005 Joerg Langenberg <joerg.langenberg@gmx.net>
 * @note Copyright (C) 2005 Paolo Mantegazza <mantegazza@aero.polimi.it>
 *
 * RTAI is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * RTAI is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RTAI; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*!
 * @defgroup rtdm Real-Time Driver Model
 *
 * The Real-Time Driver Model (RTDM) provides a unified interface to
 * both users and developers of real-time device
 * drivers. Specifically, it addresses the constraints of mixed
 * RT/non-RT systems like RTAI. RTDM conforms to POSIX
 * semantics (IEEE Std 1003.1) where available and applicable.
 */

/*!
 * @ingroup rtdm
 * @defgroup profiles Device Profiles
 *
 * Device profiles define which operation handlers a driver of a certain class
 * has to implement, which name or protocol it has to register, which IOCTLs
 * it has to provide, and further details. Sub-classes can be defined in order
 * to extend a device profile with more hardware-specific functions.
 */

#include <rtdm/rtdm.h>
#include <rtdm/core.h>
#include <rtdm/device.h>
#include <rtdm/proc.h>
#include <rtdm/rtdm_driver.h>

MODULE_DESCRIPTION("Real-Time Driver Model");
MODULE_LICENSE("GPL");

static int rtdm_fdcount(void)
{
	return fd_count;
}

#ifdef TRUE_LXRT_WAY

static struct rt_fun_entry rtdm[] = {
	[__rtdm_fdcount] = { 1, rtdm_fdcount },
	[__rtdm_open]    = { 1, _rtdm_open },
	[__rtdm_socket]  = { 1, _rtdm_socket },
	[__rtdm_close]   = { 1, _rtdm_close },
	[__rtdm_ioctl]   = { 1, _rtdm_ioctl },
	[__rtdm_read]    = { 1, _rtdm_read },
	[__rtdm_write]   = { 1, _rtdm_write },
	[__rtdm_recvmsg] = { 1, _rtdm_recvmsg },
	[__rtdm_sendmsg] = { 1, _rtdm_sendmsg },
};

#else

static int sys_rtdm_open(const char *path, int oflag)
{
	char krnl_path[RTDM_MAX_DEVNAME_LEN + 1];
	struct task_struct *curr = current;

	if (unlikely(!__xn_access_ok(curr, VERIFY_READ, path, sizeof(krnl_path)))) {
	        return -EFAULT;
	}
	__xn_copy_from_user(curr, krnl_path, path, sizeof(krnl_path)-1);
	krnl_path[sizeof(krnl_path) - 1] = '\0';
	return _rtdm_open(curr, (const char *)krnl_path, oflag);
}

static int sys_rtdm_socket(int protocol_family, int socket_type, int protocol)
{
	return _rtdm_socket(current, protocol_family, socket_type, protocol);
}

static int sys_rtdm_close(int fd)
{
	return _rtdm_close(current, fd, 0);
}

static int sys_rtdm_ioctl(int fd, int request, void *arg)
{
	return _rtdm_ioctl(current, fd, request, arg);
}

static int sys_rtdm_read(int fd, void *buf, size_t nbytes)
{
	return _rtdm_read(current, fd, buf, nbytes);
}

static int sys_rtdm_write(int fd, void *buf, size_t nbytes)
{
	return _rtdm_write(current, fd, buf, nbytes);
}

static int sys_rtdm_recvmsg(int fd, struct msghdr *msg, int flags)
{
	struct msghdr krnl_msg;
	struct task_struct *curr = current;
	int ret;

	if (unlikely(!__xn_access_ok(curr, VERIFY_WRITE, msg, sizeof(krnl_msg)))) {
		return -EFAULT;
	}
	__xn_copy_from_user(curr, &krnl_msg, msg, sizeof(krnl_msg));
	if ((ret = _rtdm_recvmsg(curr, fd, &krnl_msg, flags)) >= 0) {
		__xn_copy_to_user(curr, msg, &krnl_msg, sizeof(krnl_msg));
	}
	return ret;
}

static int sys_rtdm_sendmsg(int fd, const struct msghdr *msg, int flags)
{
	struct msghdr krnl_msg;
	struct task_struct *curr = current;

	if (unlikely(!__xn_access_ok(curr, VERIFY_READ, msg, sizeof(krnl_msg)))) {
		return -EFAULT;
	}
	__xn_copy_from_user(curr, &krnl_msg, msg, sizeof(krnl_msg));
	return _rtdm_sendmsg(curr, fd, &krnl_msg, flags);
}

static struct rt_fun_entry rtdm[] = {
	[__rtdm_fdcount] = { 1, rtdm_fdcount },
	[__rtdm_open]    = { 1, sys_rtdm_open },
	[__rtdm_socket]  = { 1, sys_rtdm_socket },
	[__rtdm_close]   = { 1, sys_rtdm_close },
	[__rtdm_ioctl]   = { 1, sys_rtdm_ioctl },
	[__rtdm_read]    = { 1, sys_rtdm_read },
	[__rtdm_write]   = { 1, sys_rtdm_write },
	[__rtdm_recvmsg] = { 1, sys_rtdm_recvmsg },
	[__rtdm_sendmsg] = { 1, sys_rtdm_sendmsg },
};

#endif

xnlock_t nklock = XNARCH_LOCK_UNLOCKED;

// this is difficult to inline; needed mostly because RTDM isr does not care of
// the PIC; WARNING: the RTAI dispatcher might have cared of the ack already
int xnintr_irq_handler(unsigned long irq, xnintr_t *intr)
{
        int retval = ((int (*)(void *))intr->isr)(intr);
        ++intr->hits;
        if (retval & XN_ISR_ENABLE) {
                xnintr_enable(intr);
        }
        if (retval & XN_ISR_CHAINED) {
                rt_pend_linux_irq(intr->irq);
        }
        return 0;
}

EXPORT_SYMBOL(xnintr_irq_handler);

int __init rtdm_skin_init(void)
{
	int err;

        if(set_rt_fun_ext_index(rtdm, RTDM_INDX)) {
                printk("Recompile your module with a different index\n");
                return -EACCES;
        }

	if ((err = rtdm_dev_init())) {
	        goto fail;
	}
	if ((err = rtdm_core_init())) {
	        goto cleanup_dev;
	}
#ifdef CONFIG_PROC_FS
	if ((err = rtdm_proc_init())) {
	        goto cleanup_core;
	}
#endif /* CONFIG_PROC_FS */
	xnprintf("starting RTDM services.\n");

	return 0;

#ifdef CONFIG_PROC_FS
	rtdm_proc_cleanup();
#endif /* CONFIG_PROC_FS */
cleanup_core:
	rtdm_core_cleanup();
cleanup_dev:
	rtdm_dev_cleanup();
fail:
	return err;
}

void rtdm_skin_exit(void)
{
	xnprintf("stopping RTDM services.\n");
	rtdm_core_cleanup();
	rtdm_dev_cleanup();
        reset_rt_fun_ext_index(rtdm, RTDM_INDX);
#ifdef CONFIG_PROC_FS
	rtdm_proc_cleanup();
#endif /* CONFIG_PROC_FS */
}

module_init(rtdm_skin_init);
module_exit(rtdm_skin_exit);
