#ifndef QUEUE_H
#define QUEUE_H

#include "Message.h"
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread> // For testing purposes only

#pragma once

class MessageQueue {
public:
  // Constructor
  MessageQueue();
  // Destructor
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
   * (Message) the Message object in the front of the queue
   */
  Message pop();

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
  Message front();

  /* Returns the message in the front of the regular (non-priority) queue
   *
   * args:
   * none
   *
   * returns:
   * (Message) the Message object in the front of the queue
   */
  Message frontRegular();

  /* Returns the message in the back of the queue
   *
   * args:
   * none
   *
   * returns:
   * (Message) the Message object in the back of the queue
   */
  Message back();

  /* Returns the message in the front of the priority queue
   *
   * args:
   * none
   *
   * returns:
   * (Message) the Message object in the back of the priority queue
   */
  Message backPriority();

  /* Returns how many elements are in the queue
   *
   * args:
   * none
   *
   * returns:
   * (size_t) the number of elements in the queue
   */
  size_t size();

  /* Returns how many elements are in the priority queue
   *
   * args:
   * none
   *
   * returns:
   * (size_t) the number of elements in the queue
   */
  size_t sizePriority();

  /* Returns how many elements are in the regular queue
   *
   * args:
   * none
   *
   * returns:
   * (size_t) the number of elements in the queue
   */
  size_t sizeRegular();

  /* Returns if the queue is empty (True) or not (False)
   *
   * args:
   * none
   *
   * returns:
   * (bool) if the queue is empty (True) or not (False)
   */
  bool empty();

private:
  std::queue<Message> priorityQueue;
  std::queue<Message> regularQueue;

  std::mutex m_mutex;
  std::condition_variable m_cond_push;
};

#endif