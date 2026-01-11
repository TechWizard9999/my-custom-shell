#pragma once

#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>

class IORedirector {
private:
  int savedFds[2];
  bool isRedirecting[2];

  static const int OUT = 0;
  static const int ERR = 1;

  bool redirect(int fdIndex, int targetFd, const std::string &filename) {
    if (isRedirecting[fdIndex]) {
      return false;
    }

    int fileFd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fileFd < 0) {
      return false;
    }

    if (targetFd == STDOUT_FILENO) {
      std::cout.flush();
    } else {
      std::cerr.flush();
    }

    savedFds[fdIndex] = dup(targetFd);
    if (savedFds[fdIndex] < 0) {
      close(fileFd);
      return false;
    }

    if (dup2(fileFd, targetFd) < 0) {
      close(savedFds[fdIndex]);
      close(fileFd);
      savedFds[fdIndex] = -1;
      return false;
    }

    close(fileFd);
    isRedirecting[fdIndex] = true;
    return true;
  }

  void restoreFd(int fdIndex, int targetFd) {
    if (!isRedirecting[fdIndex] || savedFds[fdIndex] < 0) {
      return;
    }

    if (targetFd == STDOUT_FILENO) {
      std::cout.flush();
    } else {
      std::cerr.flush();
    }

    dup2(savedFds[fdIndex], targetFd);
    close(savedFds[fdIndex]);
    savedFds[fdIndex] = -1;
    isRedirecting[fdIndex] = false;
  }

public:
  IORedirector() {
    savedFds[OUT] = -1;
    savedFds[ERR] = -1;
    isRedirecting[OUT] = false;
    isRedirecting[ERR] = false;
  }

  bool redirectOutput(const std::string &filename) {
    return redirect(OUT, STDOUT_FILENO, filename);
  }

  bool redirectError(const std::string &filename) {
    return redirect(ERR, STDERR_FILENO, filename);
  }

  void restore() {
    restoreFd(OUT, STDOUT_FILENO);
    restoreFd(ERR, STDERR_FILENO);
  }

  ~IORedirector() { restore(); }
};
