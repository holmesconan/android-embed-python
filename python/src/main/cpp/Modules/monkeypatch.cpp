#include <string>
#include <android/log.h>
#include <pybind11/embed.h>

#define TAG "python"

namespace py = pybind11;

struct RedirectOutput {
  int level;
  std::string line;

  RedirectOutput(std::string type) {
      level = (type == "stdout") ? ANDROID_LOG_INFO : ANDROID_LOG_ERROR;
  }
  RedirectOutput(RedirectOutput&&) = default;
  RedirectOutput(const RedirectOutput&) = default;
  RedirectOutput& operator=(RedirectOutput&&) = default;
  RedirectOutput& operator=(const RedirectOutput&) = default;

  void write(const py::object &buffer) {
    line += buffer.cast<std::string>();
    if (line.back() == '\n') {
      __android_log_write(level, TAG, line.data());
      line = std::string();
    }
  }

  void flush() {
    if (line.size() > 0) {
      __android_log_write(level, TAG, line.data());
      line = std::string();
    }
  }
};

PYBIND11_EMBEDDED_MODULE(monkeypatch, m) {
  py::class_<RedirectOutput> ro(m, "RedirectOutput");
  ro.def(py::init<std::string>());
  ro.def("write", &RedirectOutput::write);
  ro.def("flush", &RedirectOutput::flush);

  m.attr("stdout") = ro("stdout");
  m.attr("stderr") = ro("stderr");
}