
#ifndef XTY_H
#define XTY_H

// debug output on-off
#define DEBUG 1

#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <assert.h>

#define XTY_OK		0
#define XTY_ERROR	-1

#include "xty_lock.h"
#include "xty_spinlock.h"
#include "xty_mutex.h"
#include "xty_log.h"
#include "xty_error.h"
#include "xty_exception.h"
#include "xty_cond.h"
#include "xty_sem.h"
#include "xty_shm.h"
#include "xty_alloc.h"
#include "xty_pool.h"
#include "xty_shared_pool.h"
#include "xty_array.h"
#include "xty_queue.h"
#include "xty_rbtree.h"
#include "xty_map.h"
#include "xty_event.h"
#include "xty_msg.h"
#include "xty_msg_serialization.h"
#include "xty_msg_callback.h"
#include "xty_msg_container.h"
#include "xty_msg_queue.h"
#include "xty_msg_loop.h"
#include "xty_conf_parse.h"
#include "xty_global_function.h"
#include "xty_job.h"
#include "xty_worker.h"
#include "xty_thread.h"
#include "xty_process.h"
#include "xty_session.h"
#include "xty_session_send_job.h"
#include "xty_session_recv_job.h"
#include "xty_facility.h"
#include "xty_msg_server.h"
#include "xty_msg_server_epoll_job.h"
#include "xty_msg_server_loop_job.h"
#include "xty_msg_server_send_job.h"

#endif

