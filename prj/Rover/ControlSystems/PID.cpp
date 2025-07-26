#include "phidget22.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// PID Controller Structure
typedef struct {
    double kp;             // Proportional gain
    double ki;             // Integral gain
    double kd;             // Derivative gain
    double setpoint;       // Desired angle (degrees)
    double integral;       // Integral accumulator
    double previous_error; // Previous error for derivative
    double max_output;     // Maximum output limit
    double min_output;     // Minimum output limit
    double max_integral;   // Integral windup limit
} PIDController;

// Global variables
PhidgetStepperHandle stepper = NULL;
PIDController pid;
double current_angle = 0.0;
double target_angle = 0.0;
int running = 1;
double steps_per_degree = 200.0 / 360.0; // Assuming 200 steps per revolution

// Function prototypes
void initPID(PIDController* pid, double kp, double ki, double kd);
double calculatePID(PIDController* pid, double current_value, double dt);
void onPositionChange(PhidgetStepperHandle ch, void* ctx, double position);
int setupStepper();
void cleanup();
double stepsToAngle(double steps);
double angleToSteps(double angle);

// Initialize PID controller
void initPID(PIDController* pid, double kp, double ki, double kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->setpoint = 0.0;
    pid->integral = 0.0;
    pid->previous_error = 0.0;
    pid->max_output = 5000.0;   // Maximum velocity (steps/sec)
    pid->min_output = -5000.0;  // Minimum velocity (steps/sec)
    pid->max_integral = 1000.0; // Prevent integral windup
}

// Calculate PID output
double calculatePID(PIDController* pid, double current_value, double dt) {
    double error = pid->setpoint - current_value;

    // Proportional term
    double proportional = pid->kp * error;

    // Integral term
    pid->integral += error * dt;

    // Prevent integral windup
    if (pid->integral > pid->max_integral) {
        pid->integral = pid->max_integral;
    } else if (pid->integral < -pid->max_integral) {
        pid->integral = -pid->max_integral;
    }

    double integral_term = pid->ki * pid->integral;

    // Derivative term
    double derivative = 0.0;
    if (dt > 0) {
        derivative = pid->kd * (error - pid->previous_error) / dt;
    }

    // Calculate total output
    double output = proportional + integral_term + derivative;

    // Limit output
    if (output > pid->max_output) {
        output = pid->max_output;
    } else if (output < pid->min_output) {
        output = pid->min_output;
    }

    // Store error for next iteration
    pid->previous_error = error;

    return output;
}

// Convert steps to angle in degrees
double stepsToAngle(double steps) { return steps / steps_per_degree; }

// Convert angle in degrees to steps
double angleToSteps(double angle) { return angle * steps_per_degree; }

// Position change event handler
void onPositionChange(PhidgetStepperHandle ch, void* ctx, double position) {
    current_angle = stepsToAngle(position);
    printf("Current position: %.2f steps, Current angle: %.2f degrees\n",
           position, current_angle);
}

// Setup stepper motor
int setupStepper() {
    PhidgetReturnCode res;

    // Create stepper handle
    res = PhidgetStepper_create(&stepper);
    if (res != EPHIDGET_OK) {
        printf("Failed to create stepper handle\n");
        return -1;
    }

    // Set position change event handler
    res = PhidgetStepper_setOnPositionChangeHandler(stepper, onPositionChange,
                                                    NULL);
    if (res != EPHIDGET_OK) {
        printf("Failed to set position change handler\n");
        return -1;
    }

    // Open and wait for attachment
    res = Phidget_openWaitForAttachment((PhidgetHandle)stepper, 5000);
    if (res != EPHIDGET_OK) {
        printf("Failed to open stepper (timeout or device not found)\n");
        return -1;
    }

    // Configure stepper parameters
    res = PhidgetStepper_setCurrentLimit(stepper,
                                         2.0); // Set current limit to 2A
    if (res != EPHIDGET_OK) {
        printf("Failed to set current limit\n");
        return -1;
    }

    res = PhidgetStepper_setAcceleration(stepper, 5000.0); // Set acceleration
    if (res != EPHIDGET_OK) {
        printf("Failed to set acceleration\n");
        return -1;
    }

    res = PhidgetStepper_setVelocityLimit(stepper, 5000.0); // Set max velocity
    if (res != EPHIDGET_OK) {
        printf("Failed to set velocity limit\n");
        return -1;
    }

    // Set control mode to run mode (velocity control)
    res = PhidgetStepper_setControlMode(stepper, CONTROL_MODE_RUN);
    if (res != EPHIDGET_OK) {
        printf("Failed to set control mode\n");
        return -1;
    }

    // Engage the motor
    res = PhidgetStepper_setEngaged(stepper, 1);
    if (res != EPHIDGET_OK) {
        printf("Failed to engage stepper\n");
        return -1;
    }

    printf("Stepper motor setup complete\n");
    return 0;
}

// Set target angle
void setTargetAngle(double angle) {
    target_angle = angle;
    pid.setpoint = angle;
    printf("Target angle set to: %.2f degrees\n", angle);
}

// Main PID control loop
void pidControlLoop() {
    struct timespec last_time, current_time;
    clock_gettime(CLOCK_MONOTONIC, &last_time);

    double tolerance = 1.0; // Angle tolerance in degrees

    while (running) {
        clock_gettime(CLOCK_MONOTONIC, &current_time);

        // Calculate time difference
        double dt = (current_time.tv_sec - last_time.tv_sec)
                    + (current_time.tv_nsec - last_time.tv_nsec) / 1e9;

        if (dt >= 0.01) { // Run PID at 100Hz
            // Calculate PID output (velocity in steps/sec)
            double velocity_output = calculatePID(&pid, current_angle, dt);

            // Apply velocity to stepper
            PhidgetReturnCode res = PhidgetStepper_setVelocityLimit(
                stepper, fabs(velocity_output));
            if (res == EPHIDGET_OK) {
                // Set direction based on sign of velocity
                if (velocity_output > 0) {
                    PhidgetStepper_setTargetPosition(stepper,
                                                     1e15); // Move forward
                } else if (velocity_output < 0) {
                    PhidgetStepper_setTargetPosition(stepper,
                                                     -1e15); // Move backward
                } else {
                    // Stop motor when velocity is zero or close to target
                    double error = fabs(target_angle - current_angle);
                    if (error < tolerance) {
                        PhidgetStepper_setVelocityLimit(stepper, 0);
                        printf("Target reached! Current angle: %.2f degrees\n",
                               current_angle);
                    }
                }
            }

            // Debug output
            double error = target_angle - current_angle;
            printf("Error: %.2f°, PID Output: %.2f steps/sec, Current: %.2f°, "
                   "Target: %.2f°\n",
                   error, velocity_output, current_angle, target_angle);

            last_time = current_time;
        }

        usleep(10000); // Sleep for 10ms
    }
}

// Cleanup function
void cleanup() {
    if (stepper != NULL) {
        PhidgetStepper_setEngaged(stepper, 0); // Disengage motor
        Phidget_close((PhidgetHandle)stepper);
        PhidgetStepper_delete(&stepper);
    }
}

// Main function
int main() {
    printf("Phidget Stepper PID Angle Controller\n");
    printf("====================================\n");

    // Initialize PID controller
    // Tune these values based on your system's response
    initPID(&pid, 50.0, 5.0, 2.0); // kp, ki, kd

    // Setup stepper motor
    if (setupStepper() != 0) {
        printf("Failed to setup stepper motor\n");
        return -1;
    }

    // Get initial position
    double initial_position;
    PhidgetStepper_getPosition(stepper, &initial_position);
    current_angle = stepsToAngle(initial_position);
    printf("Initial angle: %.2f degrees\n", current_angle);

    // Interactive loop for setting target angles
    printf("\nEnter target angles (or 'q' to quit):\n");

    char input[100];
    while (1) {
        printf("Target angle (degrees): ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        if (input[0] == 'q' || input[0] == 'Q') {
            running = 0;
            break;
        }

        double angle = atof(input);
        if (angle != 0.0 || input[0] == '0') {
            setTargetAngle(angle);

            // Run PID control for this target
            pidControlLoop();
        } else {
            printf("Invalid input. Please enter a number or 'q' to quit.\n");
        }
    }

    // Cleanup
    cleanup();
    printf("Program terminated.\n");

    return 0;
}