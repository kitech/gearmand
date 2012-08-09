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



#include <config.h>
#include <libgearman/common.h>

#include <cstdarg>

#include <libgearman/log.hpp>

#include <cstdio>

void gearman_log(gearman_universal_st *state, gearman_verbose_t verbose,
                 const char *format, va_list args)
{
  char log_buffer[GEARMAN_MAX_ERROR_SIZE];

  if (not state->log_fn)
  {
    vsnprintf(log_buffer, GEARMAN_MAX_ERROR_SIZE, format, args);
    state->log_fn(log_buffer, verbose, state->log_context);
  }
}


void gearman_log_fatal(gearman_universal_st *gearman, const char *format,
                       ...)
{
  va_list args;

  if (gearman->verbose >= GEARMAN_VERBOSE_FATAL)
  {
    va_start(args, format);
    gearman_log(gearman, GEARMAN_VERBOSE_FATAL, format, args);
    va_end(args);
  }
}

void gearman_log_error(gearman_universal_st *gearman, const char *format,
                       ...)
{
  va_list args;

  if (gearman->verbose >= GEARMAN_VERBOSE_ERROR)
  {
    va_start(args, format);
    gearman_log(gearman, GEARMAN_VERBOSE_ERROR, format, args);
    va_end(args);
  }
}

void gearman_log_info(gearman_universal_st *gearman, const char *format,
                      ...)
{
  va_list args;

  if (gearman->verbose >= GEARMAN_VERBOSE_INFO)
  {
    va_start(args, format);
    gearman_log(gearman, GEARMAN_VERBOSE_INFO, format, args);
    va_end(args);
  }
}

void gearman_log_debug(gearman_universal_st *gearman, const char *format,
                       ...)
{
  va_list args;

  if (gearman->verbose >= GEARMAN_VERBOSE_DEBUG)
  {
    va_start(args, format);
    gearman_log(gearman, GEARMAN_VERBOSE_DEBUG, format, args);
    va_end(args);
  }
}

void gearman_log_crazy(gearman_universal_st *gearman, const char *format,
                       ...)
{
  va_list args;

  if (gearman->verbose >= GEARMAN_VERBOSE_CRAZY)
  {
    va_start(args, format);
    gearman_log(gearman, GEARMAN_VERBOSE_CRAZY, format, args);
    va_end(args);
  }
}