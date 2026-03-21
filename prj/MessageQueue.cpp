#include "MessageQueue.h"

static const char* file = "MessageQueue";

//-------------------------------//
/* DATA CREATION AND DESTRUCTION */
//-------------------------------//

/*
 * Add message into correct queue depending on priority
 */
void MessageQueue::push(const Message message) {
    // Thread acquires lock
    std::unique_lock<std::mutex> lock(m_mutex);

    // Check for Queue limit
    if (m_regularQueue.size() >= QUEUE_LIMIT) {
        m_regularQueue
            .pop(); // Remove the oldest message from the regular queue
        // Logging::logW(file, "Queue limit reached, discarded oldest
        // message.");
    }

    m_regularQueue.push(message);

    // If there is a thread waiting to pop an element with nothing in the
    // queues, this sends a signal to let that thread know that something has
    // been added to the queue and it is now safe to pop
    m_cond_push.notify_one();
}

/*
 * Remove message from correct queue depending on priority
 */
Message MessageQueue::pop() {
    Message returnMessage;

    // Thread acquires lock
    std::unique_lock<std::mutex> lock(m_mutex);

    // Waits for a signal from push() method if the queue is empty.
    // Makes sure that there is something to pop
    m_cond_push.wait(lock, [this]() {
        return !m_priorityQueue.empty() || !m_regularQueue.empty();
    });

    // get front() and pop
    if (!m_priorityQueue.empty()) {
        returnMessage = m_priorityQueue.front();
        m_priorityQueue.pop();
    } else if (!m_regularQueue.empty()) {
        returnMessage = m_regularQueue.front();
        m_regularQueue.pop();
    }

    return returnMessage;
}

//----------------//
/* DATA RETRIEVAL */
//----------------//

/*
 * Returns the message in the front of the queue
 */
Message MessageQueue::front() {
    // Thread acquires lock
    std::unique_lock<std::mutex> lock(m_mutex);

    // Waits for a signal from push() method if the queue is empty.
    // Makes sure that there is a message to view
    m_cond_push.wait(lock, [this]() {
        return !m_priorityQueue.empty() || !m_regularQueue.empty();
    });

    if (!m_priorityQueue.empty()) {
        return m_priorityQueue.front();
    } else if (!m_regularQueue.empty()) {
        return m_regularQueue.front();
    }

    // Throw runtime error if queue is empty
    throw std::runtime_error("Queue is empty. Cannot retrieve front element.");
}

/*
 * Returns the message in the front of the regular (non-priority) queue
 */
Message MessageQueue::frontRegular() {

    // Thread acquires lock
    std::unique_lock<std::mutex> lock(m_mutex);

    // Waits for a signal from push() method if the queue is empty.
    // Makes sure that there is a message to view
    m_cond_push.wait(lock, [this]() { return !m_regularQueue.empty(); });

    if (!m_regularQueue.empty()) {
        return m_regularQueue.front();
    }

    // Throw runtime error if queue is empty
    throw std::runtime_error(
        "Queue is empty. Cannot retrieve frontRegular element.");
}

/*
 * Returns the message in the back of the queue
 */
Message MessageQueue::back() {

    // Thread acquires lock
    std::unique_lock<std::mutex> lock(m_mutex);

    // Waits for a signal from push() method if the queue is empty.
    // Makes sure that there is a message to view
    m_cond_push.wait(lock, [this]() {
        return !m_priorityQueue.empty() || !m_regularQueue.empty();
    });

    if (!m_regularQueue.empty()) {
        return m_regularQueue.back();
    } else if (!m_priorityQueue.empty()) {
        return m_priorityQueue.back();
    }

    // Throw runtime error if queue is empty
    throw std::runtime_error("Queue is empty. Cannot retrieve back element.");
}

/*
 * Returns the message in the front of the priority queue
 */
Message MessageQueue::backPriority() {

    // Thread acquires lock
    std::unique_lock<std::mutex> lock(m_mutex);

    // Waits for a signal from push() method if the queue is empty.
    // Makes sure that there is a message to view
    m_cond_push.wait(lock, [this]() { return !m_priorityQueue.empty(); });

    if (!m_priorityQueue.empty()) {
        return m_priorityQueue.back();
    }

    // Throw runtime error if queue is empty
    throw std::runtime_error(
        "Queue is empty. Cannot retrieve backPriority element.");
}

/*
 * Returns how many elements are in the queue
 */
size_t MessageQueue::size() {

    // Thread acquires lock
    std::unique_lock<std::mutex> lock(m_mutex);

    return m_priorityQueue.size() + m_regularQueue.size();
}

/*
 * Returns how many elements are in the priority queue
 */
size_t MessageQueue::sizePriority() {

    // Thread acquires lock
    std::unique_lock<std::mutex> lock(m_mutex);

    return m_priorityQueue.size();
}

/*
 * Returns how many elements are in the regular queue
 */
size_t MessageQueue::sizeRegular() {

    // Thread acquires lock
    std::unique_lock<std::mutex> lock(m_mutex);

    return m_regularQueue.size();
}

/*
 * Returns if the queue is empty (True) or not (False)
 */
bool MessageQueue::empty() {

    // Thread acquires lock
    std::unique_lock<std::mutex> lock(m_mutex);

    return (m_priorityQueue.empty() && m_regularQueue.empty());
}

/*
 * Returns if the queue is empty (True) or not (False)
 */
bool MessageQueue::isQueueLimit() {

    // Compare size of this MessageQueue Object to the QUEUE_LIMIT
    if (this->size() >= QUEUE_LIMIT)
        return true;

    else
        return false;
}

// Testing for custom MessageQueue
#if 0
int main()
{


    /* REGULAR TESTING */

    // Create Messages
    Message msg1(1, 10, {1, 2, 3});    // Priority message
    Message msg2(0, 20, {4, 5, 6});    // Regular message
    Message msg3(1, 30, {7, 8, 9});    // Priority message
    Message msg4(0, 40, {10, 11, 12}); // Regular message

    // Create the MessageQueue
    MessageQueue queue;

    // Test 1: Test push() and size()
    queue.push(msg1);
    queue.push(msg2);
    queue.push(msg3);
    queue.push(msg4);
    Logging::logD(file, "Test 1: Queue size after 4 pushes: %d (Expected: 4)", queue.size());

    // Test 2: Test front() and printMessage()
    Logging::logD(file, "Test 2: Front message details (Expected: msg1):");
    queue.front().printMessage();

    // Test 3: Test back() and printMessage()
    Logging::logD(file, "Test 3: Back message details (Expected: msg4):");
    queue.back().printMessage();

    // Test 4: Test frontRegular()
    Logging::logD(file, "Test 4: Front regular message details (Expected: msg2):");
    queue.frontRegular().printMessage();

    // Test 5: Test backPriority()
    Logging::logD(file, "Test 5: Back priority message details (Expected: msg3):");
    queue.backPriority().printMessage();

    // Test 6: Test pop() and size()
    queue.pop(); // Removes msg1 (priority)
    Logging::logD(file, "Test 6: Queue size after 1 pop: %d (Expected: 3)", queue.size());
    Logging::logD(file, "Front message details after 1 pop (Expected: msg3):");
    queue.front().printMessage();

    // Test 7: Test empty() on non-empty queue
    Logging::logD(file, "Test 7: Queue empty status (Expected: 0): %d", queue.empty());

    // Test 8: Clear the queue and test empty()
    queue.pop(); // Removes msg3
    queue.pop(); // Removes msg2
    queue.pop(); // Removes msg4
    Logging::logD(file, "Test 8: Queue empty status after clearing (Expected: 1): %d", queue.empty());

    // Test 9: Test edge case - front() and back() on empty queue
    Logging::logD(file, "Test 9: Attempting front() and back() on empty queue (Expected: No output, undefined behavior):");
    try
    {
        queue.front().printMessage();
    }
    catch (...)
    {
        Logging::logD(file, "Handled empty queue for front()");
    }
    try
    {
        queue.back().printMessage();
    }
    catch (...)
    {
        Logging::logD(file, "Handled empty queue for back()");
    }





    /* REGULAR TESTING */


    return 0;
}

#endif