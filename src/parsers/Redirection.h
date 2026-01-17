#pragma once

#include <string>

struct Redirection {
  std::string outputFile;
  std::string errorFile;
  bool hasOutputRedirect;
  bool hasErrorRedirect;
  bool appendOutput;

  Redirection()
      : outputFile(""), errorFile(""), hasOutputRedirect(false),
        hasErrorRedirect(false), appendOutput(false) {}
};