#pragma once

#include "Command.h"
#include <cstdlib>
#include <iostream>
#include <unistd.h>

class CdCommand : public Command {
public:
  std::string getName() const override { return "cd"; }

  int execute(const std::vector<std::string> &args) override {
    if (args.size() < 2) {
      std::cerr << "cd: missing argument" << std::endl;
      return 1;
    }

    std::string path = args[1];

    if (path[0] == '~') {
      const char *homedir = getenv("HOME");
      if (homedir != NULL) {
        if (path == "~") {
          path = homedir;
        } else {
          path = std::string(homedir) + path.substr(1);
        }
      }
    }

    if (chdir(path.c_str()) == 0) {
      return 0;
    } else {
      std::cerr << "cd: " << path << ": No such file or directory" << std::endl;
      return 1;
    }
  }
};
