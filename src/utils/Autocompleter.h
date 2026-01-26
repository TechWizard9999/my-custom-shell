#pragma once

#include <cstring>
#include <readline/readline.h>
#include <string>
#include <vector>

namespace Autocomplete {

inline std::vector<std::string> &getBuiltins() {
  static std::vector<std::string> builtins = {"echo", "exit", "type", "pwd",
                                              "cd"};
  return builtins;
}

inline char *commandGenerator(const char *text, int state) {
  static std::vector<std::string> matches;
  static size_t matchIndex = 0;

  if (state == 0) {
    matches.clear();
    matchIndex = 0;
    std::string textStr(text);
    for (const auto &word : getBuiltins()) {
      if (word.substr(0, textStr.size()) == textStr) {
        matches.push_back(word);
      }
    }
  }

  if (matchIndex < matches.size()) {
    return strdup(matches[matchIndex++].c_str());
  }

  return nullptr;
}

inline char **completionHandler(const char *text, int start, int end) {
  rl_attempted_completion_over = 1;
  return rl_completion_matches(text, commandGenerator);
}

inline void initialize() {
  rl_attempted_completion_function = completionHandler;
}

} // namespace Autocomplete
