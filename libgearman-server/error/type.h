/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Gearmand client and server library.
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2008 Brian Aker, Eric Day
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

/**
 * Error codes.
 */
typedef enum
{
  GEARMAN_SUCCESS,
  GEARMAN_IO_WAIT,
  GEARMAN_SHUTDOWN,
  GEARMAN_SHUTDOWN_GRACEFUL,
  GEARMAN_ERRNO,
  GEARMAN_EVENT,
  GEARMAN_TOO_MANY_ARGS,
  GEARMAN_NO_ACTIVE_FDS,
  GEARMAN_INVALID_MAGIC,
  GEARMAN_INVALID_COMMAND,
  GEARMAN_INVALID_PACKET,
  GEARMAN_UNEXPECTED_PACKET,
  GEARMAN_GETADDRINFO,
  GEARMAN_NO_SERVERS,
  GEARMAN_LOST_CONNECTION, // Valid for worker return
  GEARMAN_MEMORY_ALLOCATION_FAILURE,
  GEARMAN_JOB_EXISTS,
  GEARMAN_JOB_QUEUE_FULL,
  GEARMAN_SERVER_ERROR,
  GEARMAN_WORK_ERROR,
  GEARMAN_WORK_DATA,
  GEARMAN_WORK_WARNING,
  GEARMAN_WORK_STATUS,
  GEARMAN_WORK_EXCEPTION,
  GEARMAN_WORK_FAIL,
  GEARMAN_NOT_CONNECTED,
  GEARMAN_COULD_NOT_CONNECT,
  GEARMAN_SEND_IN_PROGRESS,
  GEARMAN_RECV_IN_PROGRESS,
  GEARMAN_NOT_FLUSHING,
  GEARMAN_DATA_TOO_LARGE,
  GEARMAN_INVALID_FUNCTION_NAME,
  GEARMAN_INVALID_WORKER_FUNCTION,
  GEARMAN_NO_REGISTERED_FUNCTION,
  GEARMAN_NO_REGISTERED_FUNCTIONS,
  GEARMAN_NO_JOBS,
  GEARMAN_ECHO_DATA_CORRUPTION,
  GEARMAN_NEED_WORKLOAD_FN,
  GEARMAN_PAUSE,
  GEARMAN_UNKNOWN_STATE,
  GEARMAN_PTHREAD,
  GEARMAN_PIPE_EOF,
  GEARMAN_QUEUE_ERROR,
  GEARMAN_FLUSH_DATA,
  GEARMAN_SEND_BUFFER_TOO_SMALL,
  GEARMAN_IGNORE_PACKET,
  GEARMAN_UNKNOWN_OPTION,
  GEARMAN_TIMEOUT,
  GEARMAN_ARGUMENT_TOO_LARGE,
  GEARMAN_MAX_RETURN /* Always add new error code before */
} gearmand_error_t;

#define gearmand_success(X) ((X) == GEARMAN_SUCCESS)
#define gearmand_failed(X) ((X) != GEARMAN_SUCCESS)
