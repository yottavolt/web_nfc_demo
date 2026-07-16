#ifndef SOLENOID_DRIVER_H
#define SOLENOID_DRIVER_H

#include <Arduino.h>

/**
 * SolenoidDriver Class
 * Handles sequential testing of solenoids using Software PWM via Timer Interrupt.
 * Works on all pins (including PA8, PA9) by manually toggling them in an ISR.
 */
class SolenoidDriver {
private:
    // "inline" tells the compiler to allocate memory for this variable right here in the header.
    // This fixes the "undefined reference" error.
    inline static SolenoidDriver* _instance = nullptr;

    const uint8_t _pins[6] = {PB12, PB13, PB14, PB15, PA8, PA9};
    const uint8_t _numPins = 6;
    
    // 80% of 25 is roughly 20
    const uint8_t _duty80 = 20; 
    
    uint8_t _dutyCycles[6] = {0, 0, 0, 0, 0, 0};
    uint8_t _pwmCounter = 0; // Counts 0 to 2le

    // The Timer Interrupt Service Routine
    static void timerISR() {
        if (_instance == nullptr) return;

        _instance->_pwmCounter++;
        if (_instance->_pwmCounter >= 25) {
            _instance->_pwmCounter = 0;
        }

        for (uint8_t i = 0; i < _instance->_numPins; i++) {
            if (_instance->_pwmCounter < _instance->_dutyCycles[i]) {
                digitalWrite(_instance->_pins[i], HIGH);
            } else {
                digitalWrite(_instance->_pins[i], LOW);
            }
        }
    }

    void setupTimer() {
        // Using HardwareTimer(TIM2) for STM32duino compatibility
        HardwareTimer *MyTim = new HardwareTimer(TIM2);
        
        // Set frequency to 10kHz. 
        // 10,000Hz / 25 steps = 400Hz actual PWM frequency.
        MyTim->setOverflow(10000, HERTZ_FORMAT); 
        MyTim->attachInterrupt(timerISR);
        MyTim->resume();
    }

public:
    SolenoidDriver() {
        _instance = this;
    }

    void init() {
        for (uint8_t i = 0; i < _numPins; i++) {
            pinMode(_pins[i], OUTPUT);
            digitalWrite(_pins[i], LOW);
            _dutyCycles[i] = 0;
        }
        setupTimer();
    }

    /**
     * Executes the test sequence:
     * For each pin: Set to 80% -> Wait 500ms -> Set to 0% -> Wait 1s
     */
    void test() {
        for (uint8_t i = 0; i < _numPins; i++) {
            setPin(i, true);  // 80% Duty
            delay(500);
            setPin(i, false); // 0% Duty
            delay(1000);
        }
    }

    /**
     * @param pin_index (0 to 5)
     * @param state (true for 80% duty, false for 0%)
     */
    void setPin(uint8_t pin_index, bool state) {
        if (pin_index < _numPins) {
            _dutyCycles[pin_index] = state ? _duty80 : 0;
        }
    }
};

#endif // SOLENOID_DRIVER_H