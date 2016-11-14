
#pragma once

#include "config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

enum _err_codes {
	ERR_SUCCESS = 0,
};

enum _sched_types {
	RR   = 0x01,
	SRJF = 0x02,
	FIFO = 0x04,
	RAND = 0x08,
};

#define test_and_out(a) if (a) goto out;
#define test_and_exit(a) if (a < 0) goto err_exit;
