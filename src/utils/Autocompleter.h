#pragma once

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <readline/readline.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

class Autocompleter {
private:
  std::vector<std::string> builtins;
  std::vector<std::string> matches;
  size_t matchIndex;

  void findBuiltinMatches(const std::string &prefix) {
    for (const auto &builtin : builtins) {
      if (builtin.substr(0, prefix.size()) == prefix) {
        matches.push_back(builtin);
      }
    }
  }

  void findExecutableMatches(const std::string &prefix) {
    const char *pathEnv = std::getenv("PATH");
    if (!pathEnv)
      return;

    std::stringstream ss(pathEnv);
    std::string dir;

    while (std::getline(ss, dir, ':')) {
      DIR *dp = opendir(dir.c_str());
      if (!dp)
        continue;

      struct dirent *entry;
      while ((entry = readdir(dp)) != nullptr) {
        std::string name = entry->d_name;

        if (name.substr(0, prefix.size()) == prefix) {
          std::string fullPath = dir + "/" + name;
          if (access(fullPath.c_str(), X_OK) == 0) {
            if (std::find(matches.begin(), matches.end(), name) ==
                matches.end()) {
              matches.push_back(name);
            }
          }
        }
      }
      closedir(dp);
    }
  }

public:
  Autocompleter()
      : builtins({"echo", "exit", "type", "pwd", "cd"}), matchIndex(0) {}

  void generateMatches(const std::string &prefix) {
    matches.clear();
    matchIndex = 0;
    findBuiltinMatches(prefix);
    findExecutableMatches(prefix);
  }

  char *getNextMatch() {
    if (matchIndex < matches.size()) {
      return strdup(matches[matchIndex++].c_str());
    }
    return nullptr;
  }

  static Autocompleter &getInstance() {
    static Autocompleter instance;
    return instance;
  }
};

namespace Autocomplete {

inline char *commandGenerator(const char *text, int state) {
  if (state == 0) {
    Autocompleter::getInstance().generateMatches(text);
  }
  return Autocompleter::getInstance().getNextMatch();
}

inline char **completionHandler(const char *text, int start, int end) {
  rl_attempted_completion_over = 1;
  return rl_completion_matches(text, commandGenerator);
}

inline void initialize() {
  rl_attempted_completion_function = completionHandler;
}

} 
