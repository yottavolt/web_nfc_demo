#ifndef NEOPIXEL_RING_H
#define NEOPIXEL_RING_H

#include <Adafruit_NeoPixel.h>

enum RingPattern {
    PATTERN_SOLID,
    PATTERN_SPIN_FADE,  // Windows-like loading ring with fading tail
    PATTERN_FILL        // Starts at 0 and fills up to 100%
};

class NeoPixelRing {
private:
    Adafruit_NeoPixel _strip;
    uint16_t _numPixels;
    
    RingPattern _currentPattern = PATTERN_SOLID;
    uint32_t _baseColor = 0;
    
    uint32_t _lastUpdate = 0;
    uint16_t _updateInterval = 50; // ms between animation frames
    
    // Animation state variables
    int16_t _spinIndex = 0;
    uint8_t _fillPercent = 0;

    // Helper to extract RGB channels for manual dimming calculations
    void getRGB(uint32_t color, uint8_t &r, uint8_t &g, uint8_t &b) {
        r = (uint8_t)(color >> 16);
        g = (uint8_t)(color >> 8);
        b = (uint8_t)color;
    }

    void updateSpinFade() {
        uint8_t baseR, baseG, baseB;
        getRGB(_baseColor, baseR, baseG, baseB);

        for (uint16_t i = 0; i < _numPixels; i++) {
            // Calculate distance behind the leading pixel index
            int16_t distance = (_spinIndex - i + _numPixels) % _numPixels;
            
            // Only illuminate a fading tail (e.g., across 5 pixels or scaling to ring size)
            // Pixels further back get dimmer.
            if (distance < _numPixels / 2 || _numPixels <= 4) {
                float fadeFactor = 1.0f - ((float)distance / (_numPixels / 2.0f));
                if (fadeFactor < 0.0f) fadeFactor = 0.0f;
                
                _strip.setPixelColor(i, _strip.Color(baseR * fadeFactor, baseG * fadeFactor, baseB * fadeFactor));
            } else {
                _strip.setPixelColor(i, 0); // Off
            }
        }
        _strip.show();

        // Advance head index
        _spinIndex = (_spinIndex + 1) % _numPixels;
    }

    void updateFill() {
        uint8_t baseR, baseG, baseB;
        getRGB(_baseColor, baseR, baseG, baseB);
        
        // Determine exactly how many pixels should be illuminated
        uint16_t targetPixels = ((uint32_t)_fillPercent * _numPixels) / 100;

        for (uint16_t i = 0; i < _numPixels; i++) {
            if (i < targetPixels) {
                _strip.setPixelColor(i, _strip.Color(baseR, baseG, baseB));
            } else {
                _strip.setPixelColor(i, 0);
            }
        }
        _strip.show();
    }

public:
    // Constructor initializes the under-the-hood NeoPixel object
    NeoPixelRing(uint16_t numPixels, uint8_t pin, neoPixelType type = NEO_GRB + NEO_KHZ800) 
        : _strip(numPixels, pin, type), _numPixels(numPixels) {}

    void init() {
        _strip.begin();
        _strip.show(); // Turn all off immediately
    }

    /**
     * Set static color or animation baseline
     */
    void setColor(uint32_t color) {
        _baseColor = color;
        if (_currentPattern == PATTERN_SOLID) {
            for (uint16_t i = 0; i < _numPixels; i++) {
                _strip.setPixelColor(i, _baseColor);
            }
            _strip.show();
        }
    }

    /**
     * Switch patterns and speeds
     * @param pattern PATTERN_SOLID, PATTERN_SPIN_FADE, or PATTERN_FILL
     * @param speedMs Animation update interval in ms (lower is faster)
     */
    void setPattern(RingPattern pattern, uint16_t speedMs = 50) {
        _currentPattern = pattern;
        _updateInterval = speedMs;
    }

    /**
     * Updates the target fill percentage (0 to 100) for PATTERN_FILL
     */
    void setFillPercentage(uint8_t percent) {
        if (percent > 100) percent = 100;
        _fillPercent = percent;
    }

    /**
     * Non-blocking execution loop. Call this continuously inside loop().
     */
    void handle() {
        if (_currentPattern == PATTERN_SOLID) return;

        if (millis() - _lastUpdate >= _updateInterval) {
            _lastUpdate = millis();

            switch (_currentPattern) {
                case PATTERN_SPIN_FADE:
                    updateSpinFade();
                    break;
                case PATTERN_FILL:
                    updateFill();
                    break;
                default:
                    break;
            }
        }
    }
    
    // Quick helper to generate color values identical to strip.Color()
    uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
        return _strip.Color(r, g, b);
    }
};

#endif // NEOPIXEL_RING_H