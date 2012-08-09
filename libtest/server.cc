/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Data Differential YATL (i.e. libtest)  library
 *
 *  Copyright (C) 2012 Data Differential, http://datadifferential.com/
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
#include <libtest/common.h>

#include <cassert>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <iostream>

#include <algorithm> 
#include <functional> 
#include <locale>
#include <unistd.h>

// trim from end 
static inline std::string &rtrim(std::string &s)
{ 
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end()); 
  return s; 
}

#include <libtest/server.h>
#include <libtest/stream.h>
#include <libtest/killpid.h>

namespace libtest {

std::ostream& operator<<(std::ostream& output, const Server &arg)
{
  if (arg.is_socket())
  {
    output << arg.hostname();
  }
  else
  {
    output << arg.hostname() << ":" << arg.port();
  }

  if (arg.has_pid())
  {
    output << " Pid:" <<  arg.pid();
  }

  if (arg.has_socket())
  {
    output << " Socket:" <<  arg.socket();
  }

  if (arg.running().empty() == false)
  {
    output << " Exec:" <<  arg.running();
  }

  return output;  // for multiple << operators
}

#define MAGIC_MEMORY 123570

Server::Server(const std::string& host_arg, const in_port_t port_arg,
               const std::string& executable, const bool _is_libtool,
               bool is_socket_arg) :
  _magic(MAGIC_MEMORY),
  _is_socket(is_socket_arg),
  _port(port_arg),
  _hostname(host_arg),
  _app(executable, _is_libtool),
  out_of_ban_killed_(false)
{
}

Server::~Server()
{
}

bool Server::check()
{
  _app.slurp();
  _app.check();
  return true;
}

bool Server::validate()
{
  return _magic == MAGIC_MEMORY;
}

// If the server exists, kill it
bool Server::cycle()
{
  uint32_t limit= 3;

  // Try to ping, and kill the server #limit number of times
  while (--limit and 
         is_pid_valid(_app.pid()))
  {
    if (kill())
    {
      Log << "Killed existing server," << *this;
      dream(0, 50000);
      continue;
    }
  }

  // For whatever reason we could not kill it, and we reached limit
  if (limit == 0)
  {
    Error << "Reached limit, could not kill server";
    return false;
  }

  return true;
}

bool Server::wait_for_pidfile() const
{
  Wait wait(pid_file(), 4);

  return wait.successful();
}

bool Server::has_pid() const
{
  return (_app.pid() > 1);
}


bool Server::start()
{
  // If we find that we already have a pid then kill it.
  if (has_pid() == true)
  {
#if 0
    fatal_message("has_pid() failed, programer error");
#endif
  }

  // This needs more work.
#if 0
  if (gdb_is_caller())
  {
    _app.use_gdb();
  }
#endif

  if (port() == LIBTEST_FAIL_PORT)
  {
    throw libtest::disconnected(LIBYATL_DEFAULT_PARAM,
                                hostname(), port(), "Called failure");
  }

  if (getenv("YATL_PTRCHECK_SERVER"))
  {
    _app.use_ptrcheck();
  }
  else if (getenv("YATL_VALGRIND_SERVER"))
  {
    _app.use_valgrind();
  }

  if (args(_app) == false)
  {
    throw libtest::disconnected(LIBYATL_DEFAULT_PARAM,
                                hostname(), port(), "Could not build command()");
  }

  libtest::release_port(_port);
  Application::error_t ret;
  if (Application::SUCCESS !=  (ret= _app.run()))
  {
    throw libtest::disconnected(LIBYATL_DEFAULT_PARAM,
                                hostname(), port(), "Application::run() %s", libtest::Application::toString(ret));
    return false;
  }
  _running= _app.print();

  if (valgrind_is_caller())
  {
    dream(5, 50000);
  }

  size_t repeat= 5;
  _app.slurp();
  while (--repeat)
  {
    if (pid_file().empty() == false)
    {
      Wait wait(pid_file(), 8);

      if (wait.successful() == false)
      {
        if (_app.check())
        {
          _app.slurp();
          continue;
        }

        char buf[PATH_MAX];
        char *getcwd_buf= getcwd(buf, sizeof(buf));
        throw libtest::disconnected(LIBYATL_DEFAULT_PARAM,
                                    hostname(), port(),
                                    "Unable to open pidfile in %s for: %s stderr:%s",
                                    getcwd_buf ? getcwd_buf : "",
                                    _running.c_str(),
                                    _app.stderr_c_str());
      }
    }
  }

  uint32_t this_wait= 0;
  bool pinged= false;
  {
    uint32_t timeout= 20; // This number should be high enough for valgrind startup (which is slow)
    uint32_t waited;
    uint32_t retry;

    for (waited= 0, retry= 4; ; retry++, waited+= this_wait)
    {
      if ((pinged= ping()) == true)
      {
        break;
      }
      else if (waited >= timeout)
      {
        break;
      }

      this_wait= retry * retry / 3 + 1;
      libtest::dream(this_wait, 0);
    }
  }

  if (pinged == false)
  {
    // If we happen to have a pid file, lets try to kill it
    if ((pid_file().empty() == false) and (access(pid_file().c_str(), R_OK) == 0))
    {
      _app.slurp();
      if (kill_file(pid_file()) == false)
      {
        throw libtest::disconnected(LIBYATL_DEFAULT_PARAM,
                                    hostname(), port(),
                                    "Failed to kill off server, waited: %u after startup occurred, when pinging failed: %.*s stderr:%.*s",
                                    this_wait,
                                    int(_running.size()), _running.c_str(),
                                    int(_app.stderr_result_length()), _app.stderr_c_str());
      }
      else
      {
        throw libtest::disconnected(LIBYATL_DEFAULT_PARAM, 
                                    hostname(), port(),
                                    "Failed native ping(), pid: %d was alive: %s waited: %u server started, having pid_file. exec: %.*s stderr:%.*s",
                                    int(_app.pid()),
                                    _app.check() ? "true" : "false",
                                    this_wait,
                                    int(_running.size()), _running.c_str(),
                                    int(_app.stderr_result_length()), _app.stderr_c_str());
      }
    }
    else
    {
      throw libtest::disconnected(LIBYATL_DEFAULT_PARAM,
                                  hostname(), port(),
                                  "Failed native ping(), pid: %d is alive: %s waited: %u server started. exec: %.*s stderr:%.*s",
                                  int(_app.pid()),
                                  _app.check() ? "true" : "false",
                                  this_wait,
                                  int(_running.size()), _running.c_str(),
                                  int(_app.stderr_result_length()), _app.stderr_c_str());
    }
    _running.clear();

    return false;
  }

  return has_pid();
}

void Server::reset_pid()
{
  _running.clear();
  _pid_file.clear();
}

pid_t Server::pid() const
{
  return _app.pid();
}

void Server::add_option(const std::string& arg)
{
  _options.push_back(std::make_pair(arg, std::string()));
}

void Server::add_option(const std::string& name, const std::string& value)
{
  _options.push_back(std::make_pair(name, value));
}

bool Server::set_socket_file()
{
  char file_buffer[FILENAME_MAX];
  file_buffer[0]= 0;

  if (broken_pid_file())
  {
    snprintf(file_buffer, sizeof(file_buffer), "/tmp/%s.socketXXXXXX", name());
  }
  else
  {
    snprintf(file_buffer, sizeof(file_buffer), "var/run/%s.socketXXXXXX", name());
  }

  int fd;
  if ((fd= mkstemp(file_buffer)) == -1)
  {
    perror(file_buffer);
    return false;
  }
  close(fd);
  unlink(file_buffer);

  _socket= file_buffer;

  return true;
}

bool Server::set_pid_file()
{
  char file_buffer[FILENAME_MAX];
  file_buffer[0]= 0;

  if (broken_pid_file())
  {
    snprintf(file_buffer, sizeof(file_buffer), "/tmp/%s.pidXXXXXX", name());
  }
  else
  {
    snprintf(file_buffer, sizeof(file_buffer), "var/run/%s.pidXXXXXX", name());
  }

  int fd;
  if ((fd= mkstemp(file_buffer)) == -1)
  {
    throw libtest::fatal(LIBYATL_DEFAULT_PARAM, "mkstemp() failed on %s with %s", file_buffer, strerror(errno));
  }
  close(fd);
  unlink(file_buffer);

  _pid_file= file_buffer;

  return true;
}

bool Server::set_log_file()
{
  char file_buffer[FILENAME_MAX];
  file_buffer[0]= 0;

  snprintf(file_buffer, sizeof(file_buffer), "var/log/%s.logXXXXXX", name());
  int fd;
  if ((fd= mkstemp(file_buffer)) == -1)
  {
    throw libtest::fatal(LIBYATL_DEFAULT_PARAM, "mkstemp() failed on %s with %s", file_buffer, strerror(errno));
  }
  close(fd);

  _log_file= file_buffer;

  return true;
}

bool Server::args(Application& app)
{

  // Set a log file if it was requested (and we can)
  if (has_log_file_option())
  {
    set_log_file();
    log_file_option(app, _log_file);
  }

  if (getenv("LIBTEST_SYSLOG") and has_syslog())
  {
    app.add_option("--syslog");
  }

  // Update pid_file
  {
    if (_pid_file.empty() and set_pid_file() == false)
    {
      return false;
    }

    pid_file_option(app, pid_file());
  }

  if (has_socket_file_option())
  {
    if (set_socket_file() == false)
    {
      return false;
    }

    socket_file_option(app, _socket);
  }

  if (has_port_option())
  {
    port_option(app, _port);
  }

  for (Options::const_iterator iter= _options.begin(); iter != _options.end(); ++iter)
  {
    if ((*iter).second.empty() == false)
    {
      app.add_option((*iter).first, (*iter).second);
    }
    else
    {
      app.add_option((*iter).first);
    }
  }

  return true;
}

bool Server::kill()
{
  if (check_pid(_app.pid())) // If we kill it, reset
  {
    _app.murder();
    if (broken_pid_file() and pid_file().empty() == false)
    {
      unlink(pid_file().c_str());
    }

    if (broken_socket_cleanup() and has_socket() and not socket().empty())
    {
      unlink(socket().c_str());
    }

    reset_pid();

    return true;
  }

  return false;
}

} // namespace libtest
