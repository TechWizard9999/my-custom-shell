#pragma once

#include <cstdlib>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

class PathSearcher {
public:
  static std::string find(const std::string &name) {
    const char *path_env = std::getenv("PATH");
    if (!path_env)
      return "";

    std::stringstream ss(path_env);
    std::string dir;
    while (std::getline(ss, dir, ':')) {
      std::string fullPath = dir + "/" + name;
      struct stat sb;
      if (stat(fullPath.c_str(), &sb) == 0 && S_ISREG(sb.st_mode) &&
          access(fullPath.c_str(), X_OK) == 0) {
        return fullPath;
      }
    }
    return "";
  }
};
