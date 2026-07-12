#ifndef QUEUE_H
#define QUEUE_H

#define DEFAULT_QUEUE_LIMIT 5 // The maximum size of the queue

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread> // For testing purposes only

#pragma once

template <typename MessageType>
class MessageQueue {

public:
    // Constructor
    MessageQueue(size_t max_size = DEFAULT_QUEUE_LIMIT) : MAX_SIZE(max_size) { }
    // Destructor
    ~MessageQueue() = default;

    /** Add message into correct queue depending on priority
     *
     * @param
     * message (Message): the Message object being added to the queue
     *
     * @return
     * none
     */
    void push(const MessageType message) {
        // Thread acquires lock
        std::unique_lock<std::mutex> lock(m_mutex);

        // Check for Queue limit
        if (m_queue.size() >= MAX_SIZE) {
            m_queue.pop(); // Remove the oldest message from the regular queue
        }

        m_queue.push(message);

        // If there is a thread waiting to pop an element with nothing in the
        // queues, this sends a signal to let that thread know that something has
        // been added to the queue and it is now safe to pop
        m_cond_push.notify_one();
    }

    /** Remove message into correct queue depending on priority
     *
     * @param
     * none
     *
     * @return
     * (Message) the Message object in the front of the queue
     */
    MessageType pop() {
        MessageType returnMessage;

        // Thread acquires lock
        std::unique_lock<std::mutex> lock(m_mutex);

        // Waits for a signal from push() method if the queue is empty.
        // Makes sure that there is something to pop
        m_cond_push.wait(lock, [this]() { return !m_queue.empty(); });

        if (m_queue.empty()) {
            // Throw runtime error if queue is empty
            throw std::runtime_error(
                "Queue is empty. Cannot retrieve front element.");
        }

        // get front() and pop
        returnMessage = m_queue.front();
        m_queue.pop();
        return returnMessage;
    }
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
    MessageType front() {
        // Thread acquires lock
        std::unique_lock<std::mutex> lock(m_mutex);

        // Waits for a signal from push() method if the queue is empty.
        // Makes sure that there is a message to view
        m_cond_push.wait(lock, [this]() { return !m_queue.empty(); });

        if (m_queue.empty()) {
            // Throw runtime error if queue is empty
            throw std::runtime_error(
                "Queue is empty. Cannot retrieve front element.");
        }

        return m_queue.front();
    }

    /** Returns the message in the back of the queue
     *
     * @param
     * none
     *
     * @return
     * (Message) the Message object in the back of the queue
     */
    MessageType back() {
        // Thread acquires lock
        std::unique_lock<std::mutex> lock(m_mutex);

        // Waits for a signal from push() method if the queue is empty.
        // Makes sure that there is a message to view
        m_cond_push.wait(lock, [this]() { return !m_queue.empty(); });

        if (m_queue.empty()) {
            // Throw runtime error if queue is empty
            throw std::runtime_error(
                "Queue is empty. Cannot retrieve back element.");
        }

        return m_queue.back();
    }

    /** Returns how many elements are in the queue
     *
     * @param
     * none
     *
     * @return
     * (size_t) the number of elements in the queue
     */
    size_t size() {
        // Thread acquires lock
        std::unique_lock<std::mutex> lock(m_mutex);

        return m_queue.size();
    }

    /** Returns if the queue is empty (True) or not (False)
     *
     * @param
     * none
     *
     * @return
     * (bool) if the queue is empty (True) or not (False)
     */
    bool empty() {
        // Thread acquires lock
        std::unique_lock<std::mutex> lock(m_mutex);

        return m_queue.empty();
    }

private:
    const size_t MAX_SIZE;
    std::queue<MessageType> m_queue; // The regular queue

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
    bool isQueueLimit() {
        // Compare size of this MessageQueue Object to the MAX_SIZE
        if (this->size() >= MAX_SIZE)
            return true;

        else
            return false;
    }
};

#endif