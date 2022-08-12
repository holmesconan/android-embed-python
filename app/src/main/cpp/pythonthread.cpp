#include <cstdio>
#include <cstring>
#include <string>
#include <dlfcn.h>
#include <android/log.h>
#include <android/dlext.h>
#include <pybind11/embed.h>

#include "pythonthread.h"

#define TAG "python"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

namespace py = pybind11;
namespace fs = std::filesystem;
using namespace pybind11::literals;

PythonThread::PythonThread(const char *apkDir, const char *filesDir, const char *externalFilesDir)
: _apkDir(apkDir)
, _filesDir(filesDir)
, _externalFilesDir(externalFilesDir)
{
  // Initialize the class.
}

PythonThread::~PythonThread()
{
  // Shutdown the class.
}

void PythonThread::start()
{
  // android_dlextinfo extinfo = {0};
  // extinfo.flags = ANDROID_DLEXT_USE_NAMESPACE;
  // extinfo.library_namespace = NULL;
  // auto lib = _apkDir / "lib-dynload._csv.cpython-310.so";
  // void *handle = android_dlopen_ext(lib.c_str(), RTLD_NOW, &extinfo);
  // LOGD("load %s handle = %p", lib.c_str(), handle);

  auto lib = _externalFilesDir / "lib-dynload" / "_csv.cpython-310.so";
  void *handle = dlopen(lib.c_str(), RTLD_NOW);
  LOGD("load %s handle = %p", lib.c_str(), handle);

  _redirect_output(RedirectType::StdOut);
  _redirect_output(RedirectType::StdErr);
 _thread = std::thread(&PythonThread::_thread_func, this);
}

void PythonThread::_redirect_output(RedirectType type)
{
  FILE *fp;
  auto logsDir = _externalFilesDir / "logs";

  if ( !fs::exists(logsDir) ) {
    fs::create_directories(logsDir);
  }

  std::string logFile;
  switch (type) {
  case RedirectType::StdOut:
    fp = stdout;
    logFile = logsDir / "stdout.log";
    break;
  case RedirectType::StdErr:
    fp = stderr;
    logFile = logsDir / "stderr.log";
    break;
  default:
    LOGE("unknown RedirectType: %d", int(type));
    return;
  }

  // TODO: Truncate the log file by size.

  fflush(fp);
  fp = freopen(logFile.c_str(), "a", fp);
  if (fp == nullptr) {
    LOGE("freopen %s failed.", logFile.c_str());
    return;
  }

  fprintf(fp, "redirect %s ok.\n", (type == RedirectType::StdOut ? "stdout" : "stderr") );
  fflush(fp);
}

void PythonThread::_thread_func(void)
{
  _set_python_env();
  try {
      _execute_python_script();
  } catch (py::error_already_set &ex) {
      __android_log_print(ANDROID_LOG_ERROR, TAG, "Exception: error_already_set");
  } catch (std::exception &ex) {
    __android_log_print(ANDROID_LOG_ERROR, TAG, "Exception: %s", ex.what());
  } catch (...) {
    __android_log_write(ANDROID_LOG_FATAL, TAG, "Unhandled exception");
  }

  LOGI("python thread exit.");
}

void PythonThread::_set_python_env(void)
{
  static wchar_t s_wPythonHome[256];
  size_t size = 0;

  auto homeDirs = std::string(_filesDir / "python3.10");
  wchar_t *t_wPythonHome = Py_DecodeLocale(homeDirs.data(), &size);
  memcpy(s_wPythonHome, t_wPythonHome, size);
  Py_SetPythonHome(s_wPythonHome);
  PyMem_RawFree(t_wPythonHome);
  LOGD("PYTHONHOME=%s", homeDirs.c_str());

  auto pythonPaths = std::string(_filesDir / "python3.10");
  pythonPaths += ":" + std::string(_externalFilesDir / "lib-dynload");
  wchar_t *wPythonPaths = Py_DecodeLocale(pythonPaths.data(), &size);
  Py_SetPath(wPythonPaths);
  PyMem_RawFree(wPythonPaths);
  LOGD("PYTHONPATH=%s", pythonPaths.c_str());
}

void PythonThread::_execute_python_script()
{
  py::scoped_interpreter guard{};

  /* patch the stdout and stderr */
  auto py_sys = py::module_::import("sys");
  auto patch = py::module_::import("monkeypatch");
  py_sys.attr("stdout") = patch.attr("stdout");
  py_sys.attr("stderr") = patch.attr("stderr");

  /* try to import so package */
  auto locals = py::dict("a"_a=1);
  py::exec(R"(
    from time import time, sleep

    while True:
      sleep(60.0)
      print(f"{time():.2f}", 'hello', a+1)
      a += 1
  )", py::globals(), locals);
}
