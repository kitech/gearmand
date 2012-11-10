/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Gearmand client and server library.
 *
 *  Copyright (C) 2012 Data Differential, http://datadifferential.com/
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

#include <libgearman-server/constants.h>

#ifdef __cplusplus
extern "C" {
#endif

    /*
      会分另在什么时候调用这几个队列函数呢？
      add，加入队列，应该是在接收到任务，发出去之前调用吧？
      flush, 清空队列，什么时候？
      done，从队列删掉，1是在任务分配到worker服务器之后worker完成之前调用，2还是在worker完成之后调用？
           如果是2,那么如果worker执行过程崩溃，这个done还会不会调用呢？
                  这个task会分配给其他的worker处理吗？
                  如果这个时候再启动worker，这个task还会再分配吗？
                  如果重启job server这个task是否还会再次执行呢？

      replay，重新执行队列中的task，这个是在什么时候调用呢？
     */
gearmand_error_t gearman_queue_add(gearman_server_st *server,
                                   const char *unique,
                                   size_t unique_size,
                                   const char *function_name,
                                   size_t function_name_size,
                                   const void *data,
                                   size_t data_size,
                                   gearman_job_priority_t priority,
                                   int64_t when);

gearmand_error_t gearman_queue_flush(gearman_server_st *server);

gearmand_error_t gearman_queue_done(gearman_server_st *server,
                                    const char *unique,
                                    size_t unique_size,
                                    const char *function_name,
                                    size_t function_name_size);

gearmand_error_t gearman_queue_replay(gearman_server_st *server,
                                      gearman_queue_add_fn *add_fn,
                                      void *add_context);

    // 这个不是重复声明了吗？
gearmand_error_t gearman_queue_done(gearman_server_st *server,
                                    const char *unique,
                                    size_t unique_size,
                                    const char *function_name,
                                    size_t function_name_size);

#ifdef __cplusplus
}
#endif
