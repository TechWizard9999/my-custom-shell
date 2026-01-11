#pragma once

#include <string>
#include <vector>

class Command {
public:
  virtual ~Command() = default;

  virtual std::string getName() const = 0;

  virtual int execute(const std::vector<std::string> &args) = 0;
};