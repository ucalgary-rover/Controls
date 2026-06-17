/**
 * @file StateManager.h
 * @brief The StateManager template provides a model for safe concurrent access to data structures shared amongst multiple threads.
 *
 * @details This class provides an interface for getting and setting shared data structures in a thread safe manner.
 * For more complex updates where a simple overwrite is not sufficient the getStateAndLock -> updateAndUnlock methods can be used.
 */

#pragma once

#include <mutex>

template <typename State>
class StateManager {
public:
    StateManager() { state = State(); }

    StateManager(State initialState) { state = initialState; }

    StateManager& operator=(const StateManager& other) {
        if (this != &other) {
            state = other.state;
        }
        return *this;
    }

    /**
     * @brief Locks and reads the current state, unlocking on return.
     */
    State getState() {
        std::lock_guard<std::mutex> lock(mtx);
        State currentState = state;
        return currentState;
    }

    /**
     * @brief Locks and updates the state, unlocking on return.
     */
    void updateState(State newState) {
        std::lock_guard<std::mutex> lock(mtx);
        state = newState;
    }

    /**
     * @brief Locks and returns the current state. 
     * The user MUST call updateAndUnlock after processing is complete to unblock other threads.
     */
    State getAndLock() {
        mtx.lock();
        return state;
    }

    /**
     * @brief Updates the current state and unlocks.
     * This method should ONLY be invoked after getAndLock has been called.
     */
    void updateAndUnlock(State newState) {
        state = newState;
        mtx.unlock();
    }

private:
    State state;
    std::mutex mtx;
};
