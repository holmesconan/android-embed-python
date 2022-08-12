#ifndef __PYTHONTHREAD_H__
#define __PYTHONTHREAD_H__
#include <thread>
#include <filesystem>

class PythonThread
{
  std::thread _thread;
  std::filesystem::path _apkDir;
  std::filesystem::path _filesDir;
  std::filesystem::path _externalFilesDir;

  enum class RedirectType {
    StdOut, StdErr
  };

public:
  PythonThread(const char *apkDir, const char *filesDir, const char *externalFilesDir);
  PythonThread(PythonThread &&) = delete;
  PythonThread(const PythonThread &) = delete;
  PythonThread &operator=(PythonThread &&) = delete;
  PythonThread &operator=(const PythonThread &) = delete;
  ~PythonThread();

  inline void join(void) { _thread.join(); }
  inline void detach(void) { _thread.detach(); }

  void start(void);

private:
  void _redirect_output(RedirectType type);
  void _thread_func(void);
  void _set_python_env(void);
  void _execute_python_script(void);
};

#endif // __PYTHONTHREAD_H__