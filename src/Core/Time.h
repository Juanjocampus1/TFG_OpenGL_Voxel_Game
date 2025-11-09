#pragma once

class Time {
private:
 float timeOfDay; // 0.0 a 1.0 (ciclo completo)
 float speed; // Multiplier de velocidad
 float elapsedGameTime; // Tiempo total del juego en segundos
 
 Time();
public:
 static Time& Get();
 
 void setTime(float t);
 float getTime() const;
 
 void setSpeed(float s);
 float getSpeed() const;
 
 void update(float deltaSeconds);
 
 // Obtener tiempo de juego total
 float getElapsedTime() const;
 
 // Obtener info del ciclo
 bool isDay() const; // true si es de día (0.25 - 0.75)
 bool isNight() const; // true si es de noche
 float getSunAngle() const; // Ángulo del sol en radianes
 
 // Obtener factor de luz ambiente (0.0 - 1.0)
 float getAmbientStrength() const;
};