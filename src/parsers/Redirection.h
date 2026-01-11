#pragma once

#include <string>

struct Redirection {
  std::string outputFile;
  std::string errorFile;
  bool hasOutputRedirect;
  bool hasErrorRedirect;

  Redirection()
      : outputFile(""), errorFile(""), hasOutputRedirect(false),
        hasErrorRedirect(false) {}
};
    