/*

Made by: Damon Mazurek
Date: 2025-01-24

This is a simple test that checks if MessageQueue works with multiple threads
concurrently pushing, popping, and viewing data into and from the MessageQueue

There are producer, consumer, and peeker threads. The producer pushes messages
into the MessageQueue, the consumer pops messages from MessageQueue, and the
peeker views the front element.

If everything works properly, there should be no errors (duh) and the
MessageQueue should be empty by the end of the test. Priority messages should
always be popped first.

Compile using:
    g++ -o MessageQueueTest MessageQueueTest.cpp MessageQueue.cpp Message.cpp
 */

#include "MessageQueue.h"
#include <iostream>
#include <mutex> // For std::mutex
#include <thread>
#include <vector>

// std::mutex coutMutex;

std::mutex coutMutex_push;
std::mutex coutMutex_pop;
std::mutex coutMutex_peek;

// Remove all
// std::lock_guard<std::mutex> lock(coutMutex);
// lines to get real output (it's unreadable)

void producer(MessageQueue& queue, int id, int numMessages) {
    for (int i = 0; i < numMessages; ++i) {
        // Alternate between priority and regular messages
        bool isPriority = (i % 2 == 0);
        Message msg(isPriority, id * 100 + i, { id, i });
        queue.push(msg);

        // Lock std::cout to ensure output is readable
        std::lock_guard<std::mutex> lock(coutMutex_push);
        // std::lock_guard<std::mutex> lock(coutMutex);

        std::cout << "Producer " << id << " pushed message " << i
                  << " (Priority: " << isPriority << ")\n";
    }
}

void consumer(MessageQueue& queue, int id, int numMessages) {
    for (int i = 0; i < numMessages; ++i) {
        try {
            Message msg = queue.pop(); // Remove the front message

            // Lock std::cout to ensure output is readable
            std::lock_guard<std::mutex> lock(
                coutMutex_pop); // remove this for true output
            // std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "Consumer " << id
                      << " popped message  (Priority: " << msg.isPriority()
                      << ")\n";
        } catch (const std::runtime_error& e) {

            // Lock std::cout to ensure output is readable
            std::lock_guard<std::mutex> lock(coutMutex_pop);
            // std::lock_guard<std::mutex> lock(coutMutex);

            std::cout << "Consumer " << id << " encountered error: " << e.what()
                      << "\n";
        }
    }
}

// New method to just peek the front message without removing it
void peekFront(MessageQueue& queue, int id) {
    try {
        Message msg = queue.front(); // Get the front message without popping it

        // Lock std::cout to ensure output is readable
        std::lock_guard<std::mutex> lock(coutMutex_peek);
        // std::lock_guard<std::mutex> lock(coutMutex);

        std::cout << "Consumer " << id
                  << " peeked message (Priority: " << msg.isPriority() << ")\n";
    } catch (const std::runtime_error& e) {

        // Lock std::cout to ensure output is readable
        std::lock_guard<std::mutex> lock(coutMutex_peek);
        // std::lock_guard<std::mutex> lock(coutMutex);

        std::cout << "Consumer " << id
                  << " encountered error while peeking: " << e.what() << "\n";
    }
}

#if 1
int main() {
    MessageQueue queue;

    // Number of producers, consumers, and messages per thread
    const int numProducers = 2;
    const int numConsumers = 2;
    const int messagesPerProducer = 10;

    // Launch producer threads
    std::vector<std::thread> producers;
    for (int i = 0; i < numProducers; ++i) {
        producers.emplace_back(producer, std::ref(queue), i + 1,
                               messagesPerProducer);
    }

    // Launch consumer threads
    std::vector<std::thread> consumers;
    for (int i = 0; i < numConsumers; ++i) {
        consumers.emplace_back(consumer, std::ref(queue), i + 1,
                               messagesPerProducer);
    }

    // Launch some peek-only threads
    std::vector<std::thread> peekers;
    for (int i = 0; i < numConsumers; ++i) {
        peekers.emplace_back(peekFront, std::ref(queue), i + 1);
    }

    // Join all threads
    for (auto& p : producers) {
        p.join();
    }
    for (auto& c : consumers) {
        c.join();
    }
    for (auto& peeker : peekers) {
        peeker.join();
    }

    // Check if the queue is empty
    std::cout << "Priority queue size: " << queue.sizePriority()
              << " (Expected 0 if all messages were processed)\n";
    std::cout << "Regular queue size: " << queue.sizeRegular()
              << " (Expected 0 if all messages were processed)\n";
    std::cout << "Final queue size: " << queue.size()
              << " (Expected 0 if all messages were processed)\n";
    return 0;
}

#endif