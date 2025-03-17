#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_LIMIT 100 // The maximum size of the queue

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

    /** Add message into correct queue depending on priority
     *
     * @param
     * message (Message): the Message object being added to the queue
     *
     * @return
     * none
     */
    void push(const Message message);

    /** Remove message into correct queue depending on priority
     *
     * @param
     * none
     *
     * @return
     * (Message) the Message object in the front of the queue
     */
    Message pop();

    //----------------//
    /** DATA RETRIEVAL */
    //----------------//

    /** Returns the message in the front of the queue
     *
     * @param
     * none
     *
     * @return
     * (Message) the Message object in the front of the queue
     */
    Message front();

    /** Returns the message in the front of the regular (non-priority) queue
     *
     * @param
     * none
     *
     * @return
     * (Message) the Message object in the front of the queue
     */
    Message frontRegular();

    /** Returns the message in the back of the queue
     *
     * @param
     * none
     *
     * @return
     * (Message) the Message object in the back of the queue
     */
    Message back();

    /** Returns the message in the front of the priority queue
     *
     * @param
     * none
     *
     * @return
     * (Message) the Message object in the back of the priority queue
     */
    Message backPriority();

    /** Returns how many elements are in the queue
     *
     * @param
     * none
     *
     * @return
     * (size_t) the number of elements in the queue
     */
    size_t size();

    /** Returns how many elements are in the priority queue
     *
     * @param
     * none
     *
     * @return
     * (size_t) the number of elements in the queue
     */
    size_t sizePriority();

    /** Returns how many elements are in the regular queue
     *
     * @param
     * none
     *
     * @return
     * (size_t) the number of elements in the queue
     */
    size_t sizeRegular();

    /** Returns if the queue is empty (True) or not (False)
     *
     * @param
     * none
     *
     * @return
     * (bool) if the queue is empty (True) or not (False)
     */
    bool empty();

private:
    std::queue<Message> m_priorityQueue; // The priority queue
    std::queue<Message> m_regularQueue;  // The regular queue

    std::mutex m_mutex; // Lock to prevent accesses by multiples threads
    std::condition_variable
        m_cond_push; // Used to signal when a push has been done on a queue (For
                     // threading purposes)

    /** Returns if queue has reached a maximum capacity
     *
     * @param
     * none
     *
     * @return
     * (bool) if the queue is at its limit (True) or not (False)
     */
    bool isQueueLimit();
};

#endif