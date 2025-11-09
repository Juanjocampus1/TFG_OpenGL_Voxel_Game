#include "Time.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Time::Time() : timeOfDay(0.2f), speed(1.0f), elapsedGameTime(0.0f) {
    // Start at morning (0.2 = ~sunrise)
}

Time& Time::Get() {
    static Time instance;
    return instance;
}

void Time::setTime(float t) {
    timeOfDay = t;
    while (timeOfDay >= 1.0f) timeOfDay -= 1.0f;
    while (timeOfDay < 0.0f) timeOfDay += 1.0f;
}

float Time::getTime() const { 
    return timeOfDay; 
}

void Time::setSpeed(float s) { 
    speed = s; 
}

float Time::getSpeed() const { 
    return speed; 
}

void Time::update(float deltaSeconds) {
    // Acumular tiempo real de juego
    elapsedGameTime += deltaSeconds;
    
    // Un ciclo completo dura 600 segundos (10 minutos)
    // speed = 1.0 significa tiempo real
    const float CYCLE_DURATION = 600.0f; // 10 minutos
    
    // Incrementar timeOfDay basado en tiempo real, no en frames
    float increment = (deltaSeconds * speed) / CYCLE_DURATION;
    timeOfDay += increment;
    
    // Wrap around [0, 1)
    while (timeOfDay >= 1.0f) timeOfDay -= 1.0f;
    while (timeOfDay < 0.0f) timeOfDay += 1.0f;
}

float Time::getElapsedTime() const {
    return elapsedGameTime;
}

bool Time::isDay() const {
    // Día es de 0.25 (amanecer) a 0.75 (atardecer)
    return timeOfDay >= 0.25f && timeOfDay < 0.75f;
}

bool Time::isNight() const {
    return !isDay();
}

float Time::getSunAngle() const {
    // El sol va de este (sunrise) a oeste (sunset)
    // 0.0 = medianoche (sol abajo)
    // 0.25 = amanecer (sol en horizonte este)
    // 0.5 = mediodía (sol arriba)
    // 0.75 = atardecer (sol en horizonte oeste)
    // 1.0 = medianoche (sol abajo)
    
    // Convertir timeOfDay a ángulo (0 a 2?)
    // Offset para que 0.5 = mediodía esté arriba
    float angle = (timeOfDay - 0.25f) * 2.0f * static_cast<float>(M_PI);
    return angle;
}

float Time::getAmbientStrength() const {
    // Calcular intensidad de luz ambiente basada en timeOfDay
    // Más oscuro durante la noche, más brillante durante el día
    
    float t = timeOfDay;
    float ambient;
    
    if (t < 0.25f) {
        // Noche profunda (medianoche a amanecer)
        // 0.0 = 0.15 (muy oscuro)
        // 0.25 = 0.4 (empieza a aclarar)
        float u = t / 0.25f;
        ambient = 0.15f + u * 0.25f; // 0.15 -> 0.4
    } 
    else if (t < 0.5f) {
        // Amanecer a mediodía
        // 0.25 = 0.4
        // 0.5 = 1.0 (máxima luz)
        float u = (t - 0.25f) / 0.25f;
        ambient = 0.4f + u * 0.6f; // 0.4 -> 1.0
    } 
    else if (t < 0.75f) {
        // Mediodía a atardecer
        // 0.5 = 1.0
        // 0.75 = 0.4
        float u = (t - 0.5f) / 0.25f;
        ambient = 1.0f - u * 0.6f; // 1.0 -> 0.4
    } 
    else {
        // Atardecer a medianoche
        // 0.75 = 0.4
        // 1.0 = 0.15 (muy oscuro)
        float u = (t - 0.75f) / 0.25f;
        ambient = 0.4f - u * 0.25f; // 0.4 -> 0.15
    }
    
    return ambient;
}
