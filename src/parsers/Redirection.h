#pragma once

#include <string>

struct Redirection {
  std::string outputFile;
  std::string errorFile;
  bool hasOutputRedirect;
  bool hasErrorRedirect;
  bool appendOutput;
  bool appendError;

  Redirection()
      : outputFile(""), errorFile(""), hasOutputRedirect(false),
        hasErrorRedirect(false), appendOutput(false), appendError(false) {}
};