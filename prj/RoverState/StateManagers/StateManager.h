#pragma once

#include <mutex>

template <typename State> class StateManager {
public:
    StateManager() { state = State(); }

    StateManager(State initialState) { state = initialState; }

    StateManager& operator=(const StateManager& other) {
        if (this != &other) {
            state = other.state;
        }
        return *this;
    }

    void updateState(State newState) {
        std::lock_guard<std::mutex> lock(mtx);
        state = newState;
    }

    State getState() {
        std::lock_guard<std::mutex> lock(mtx);
        State currentState = state;
        return currentState;
    }

private:
    State state;
    std::mutex mtx;
};
