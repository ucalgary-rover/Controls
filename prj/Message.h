#ifndef MESSAGE_H
#define MESSAGE_H

#pragma once

#include <vector>
#include <iostream>
#include <stdint.h>

class Message
{
public:
  Message(int prty, int frmt, const std::vector<int> &instr);
  ~Message();

  bool isPriority() const;
  void printMessage() const;

private:
  bool m_priority;
  int m_format;
  std::vector<int> m_instructions;
};

#endif