#ifndef MESSAGE_H
#define MESSAGE_H

#pragma once

#include <iostream>
#include <stdint.h>
#include <vector>

class Message {
public:
  Message(int prty, int frmt, const std::vector<int> &instr);
  Message();
  Message(Message const &src);
  ~Message();
  Message &operator=(const Message &src);

  bool isPriority() const;
  void printMessage() const;

private:
  bool m_priority;
  int m_format;
  std::vector<int> m_instructions;
};

#endif