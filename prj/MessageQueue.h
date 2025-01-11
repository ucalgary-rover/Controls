#ifndef QUEUE_H
#define QUEUE_H

#include "Message.h"
#include <queue>
#include <iostream>

#pragma once

class MessageQueue
{
public:
  MessageQueue();
  ~MessageQueue();

  /* Add message into correct queue depending on priority
   *
   * args:
   * message (Message): the Message object being added to the queue
   *
   * returns:
   * none
   */
  void push(const Message message);

  /* Remove message into correct queue depending on priority
   *
   * args:
   * none
   *
   * returns:
   * none
   */
  void pop();

  //----------------//
  /* DATA RETRIEVAL */
  //----------------//

  /* Returns the message in the front of the queue
   *
   * args:
   * none
   *
   * returns:
   * (Message) the Message object in the front of the queue
   */
  Message front() const;

  /* Returns the message in the front of the regular (non-priority) queue
   *
   * args:
   * none
   *
   * returns:
   * (Message) the Message object in the front of the queue
   */
  Message frontRegular() const;

  /* Returns the message in the back of the queue
   *
   * args:
   * none
   *
   * returns:
   * (Message) the Message object in the back of the queue
   */
  Message back() const;

  /* Returns the message in the front of the priority queue
   *
   * args:
   * none
   *
   * returns:
   * (Message) the Message object in the back of the priority queue
   */
  Message backPriority() const;

  /* Returns how many elements are in the queue
   *
   * args:
   * none
   *
   * returns:
   * (size_t) the number of elements in the queue
   */
  size_t size() const;

  /* Returns how many elements are in the priority queue
   *
   * args:
   * none
   *
   * returns:
   * (size_t) the number of elements in the queue
   */
  size_t sizePriority() const;

  /* Returns how many elements are in the regular queue
   *
   * args:
   * none
   *
   * returns:
   * (size_t) the number of elements in the queue
   */
  size_t sizeRegular() const;

  /* Returns if the queue is empty (True) or not (False)
   *
   * args:
   * none
   *
   * returns:
   * (bool) if the queue is empty (True) or not (False)
   */
  bool empty() const;

private:
  std::queue<Message> priorityQueue;
  std::queue<Message> regularQueue;
};

#endif