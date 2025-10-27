#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <functional>
#include "KeyCodes.h"
#include "MouseCodes.h"

enum class EventType {
    None = 0,
    KeyPressed, KeyReleased,
    MouseButtonPressed, MouseButtonReleased,
    MouseMoved, MouseScrolled
};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; } \
                               virtual EventType GetEventType() const override { return GetStaticType(); } \
                               virtual const char* GetName() const override { return #type; }

class Event {
public:
    virtual EventType GetEventType() const = 0;
    virtual const char* GetName() const = 0;
    virtual std::string ToString() const { return GetName(); }

    bool Handled = false;
};

class KeyEvent : public Event {
public:
    KeyCode GetKeyCode() const { return m_KeyCode; }

protected:
    KeyEvent(KeyCode keycode) : m_KeyCode(keycode) {}

    KeyCode m_KeyCode;
};

class KeyPressedEvent : public KeyEvent {
public:
    KeyPressedEvent(KeyCode keycode, int repeatCount) : KeyEvent(keycode), m_RepeatCount(repeatCount) {}

    int GetRepeatCount() const { return m_RepeatCount; }

    std::string ToString() const override {
        return std::string("KeyPressedEvent: ") + std::to_string((int)m_KeyCode) + " (" + std::to_string(m_RepeatCount) + " repeats)";
    }

    EVENT_CLASS_TYPE(KeyPressed)
private:
    int m_RepeatCount;
};

class KeyReleasedEvent : public KeyEvent {
public:
    KeyReleasedEvent(KeyCode keycode) : KeyEvent(keycode) {}

    std::string ToString() const override {
        return std::string("KeyReleasedEvent: ") + std::to_string((int)m_KeyCode);
    }

    EVENT_CLASS_TYPE(KeyReleased)
};

class MouseMovedEvent : public Event {
public:
    MouseMovedEvent(float x, float y) : m_MouseX(x), m_MouseY(y) {}

    float GetX() const { return m_MouseX; }
    float GetY() const { return m_MouseY; }

    std::string ToString() const override {
        return std::string("MouseMovedEvent: ") + std::to_string(m_MouseX) + ", " + std::to_string(m_MouseY);
    }

    EVENT_CLASS_TYPE(MouseMoved)
private:
    float m_MouseX, m_MouseY;
};

class MouseButtonPressedEvent : public Event {
public:
    MouseButtonPressedEvent(int button) : m_Button(button) {}

    int GetButton() const { return m_Button; }

    std::string ToString() const override {
        return std::string("MouseButtonPressedEvent: ") + std::to_string(m_Button);
    }

    EVENT_CLASS_TYPE(MouseButtonPressed)
private:
    int m_Button;
};

class EventDispatcher {
public:
    EventDispatcher(Event& event) : m_Event(event) {}

    template<typename T, typename F>
    bool Dispatch(const F& func) {
        if (m_Event.GetEventType() == T::GetStaticType()) {
            m_Event.Handled = func(static_cast<T&>(m_Event));
            return true;
        }
        return false;
    }

private:
    Event& m_Event;
};

#endif