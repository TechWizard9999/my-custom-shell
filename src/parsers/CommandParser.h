#pragma once

#include "Redirection.h"
#include <string>
#include <utility>
#include <vector>

class CommandParser {
private:
  enum State { NORMAL, SINGLE_QUOTE, DOUBLE_QUOTE };

public:
  static std::vector<std::string> parse(const std::string &input) {
    std::vector<std::string> tokens;
    std::string currentToken;
    State state = NORMAL;
    bool escapeNext = false;

    for (size_t i = 0; i < input.length(); i++) {
      char c = input[i];

      if (escapeNext) {
        currentToken += c;
        escapeNext = false;
        continue;
      }

      switch (state) {
      case NORMAL:
        if (c == '\\') {
          escapeNext = true;
        } else if (c == '\'') {
          state = SINGLE_QUOTE;
        } else if (c == '"') {
          state = DOUBLE_QUOTE;
        } else if (c == ' ' || c == '\t') {
          if (!currentToken.empty()) {
            tokens.push_back(currentToken);
            currentToken.clear();
          }
        } else {
          currentToken += c;
        }
        break;

      case SINGLE_QUOTE:
        if (c == '\'') {
          state = NORMAL;
        } else {
          currentToken += c;
        }
        break;

      case DOUBLE_QUOTE:
        if (c == '\\' && i + 1 < input.length()) {
          char next = input[i + 1];
          if (next == '"' || next == '\\') {
            currentToken += next;
            i++;
          } else {
            currentToken += c;
          }
        } else if (c == '"') {
          state = NORMAL;
        } else {
          currentToken += c;
        }
        break;
      }
    }

    if (!currentToken.empty()) {
      tokens.push_back(currentToken);
    }

    return tokens;
  }

  static std::pair<std::vector<std::string>, Redirection>
  parseWithRedirection(const std::string &input) {
    std::vector<std::string> tokens = parse(input);
    Redirection redir;
    std::vector<std::string> cmdTokens;

    for (size_t i = 0; i < tokens.size(); i++) {
      const std::string &token = tokens[i];

      if (token == ">" || token == "1>") {
        if (i + 1 < tokens.size()) {
          redir.hasOutputRedirect = true;
          redir.outputFile = tokens[i + 1];
          i++;
        }
      } else if (token == "2>") {
        if (i + 1 < tokens.size()) {
          redir.hasErrorRedirect = true;
          redir.errorFile = tokens[i + 1];
          i++;
        }
      } else {
        cmdTokens.push_back(token);
      }
    }

    return std::make_pair(cmdTokens, redir);
  }
};
