#ifndef HOTKEY_H
#define HOTKEY_H

#include <Qt>

class Hotkey
{
public:
	inline Hotkey() = default;
	inline Hotkey(const Qt::Key key, const Qt::KeyboardModifiers modifiers = Qt::NoModifier);
	inline Hotkey(const Qt::MouseButton mouseButton, const Qt::KeyboardModifiers modifiers = Qt::NoModifier);
	~Hotkey() = default;

	inline bool isMouse() const;
	inline bool isValid() const;

	inline Qt::Key key() const;
	inline Qt::MouseButton mouseButton() const;
	inline Qt::KeyboardModifiers modifiers() const;
	inline bool hasModifier(const Qt::KeyboardModifier modifier) const;

	inline void reset();

	inline void setKey(const Qt::Key key);
	inline void setMouseButton(const Qt::MouseButton mouseButton);
	inline void setModifier(const Qt::KeyboardModifier modifier, const bool on = true);
	inline void setModifiers(const Qt::KeyboardModifiers modifiers);

	inline bool operator==(const Hotkey &other) const;

private:
	Qt::Key m_key = Qt::Key_unknown;
	Qt::MouseButton m_mouseButton = Qt::NoButton;
	Qt::KeyboardModifiers m_modifiers = Qt::NoModifier;
};



inline Hotkey::Hotkey(const Qt::Key key, const Qt::KeyboardModifiers modifiers)
	: m_key(key)
	, m_modifiers(modifiers)
{}

inline Hotkey::Hotkey(const Qt::MouseButton mouseButton, const Qt::KeyboardModifiers modifiers)
	: m_mouseButton(mouseButton)
	, m_modifiers(modifiers)
{}

inline bool Hotkey::isMouse() const
{
	return (m_mouseButton != Qt::NoButton);
}

inline bool Hotkey::isValid() const
{
	return (m_key != Qt::Key_unknown || m_mouseButton != Qt::NoButton);
}

inline Qt::Key Hotkey::key() const
{
	return m_key;
}

inline Qt::MouseButton Hotkey::mouseButton() const
{
	return m_mouseButton;
}

inline Qt::KeyboardModifiers Hotkey::modifiers() const
{
	return m_modifiers;
}

inline bool Hotkey::hasModifier(const Qt::KeyboardModifier modifier) const
{
	return m_modifiers.testFlag(modifier);
}

inline void Hotkey::reset()
{
	m_key = Qt::Key_unknown;
	m_mouseButton = Qt::NoButton;
}

inline void Hotkey::setKey(const Qt::Key key)
{
	m_key = key;
	m_mouseButton = Qt::NoButton;
}

inline void Hotkey::setMouseButton(const Qt::MouseButton mouseButton)
{
	m_key = Qt::Key_unknown;
	m_mouseButton = mouseButton;
}

inline void Hotkey::setModifier(const Qt::KeyboardModifier modifier, const bool on)
{
	m_modifiers.setFlag(modifier, on);
}

inline void Hotkey::setModifiers(const Qt::KeyboardModifiers modifiers)
{
	m_modifiers = modifiers;
}

inline bool Hotkey::operator==(const Hotkey &other) const
{
	return m_key == other.m_key &&
		   m_mouseButton == other.m_mouseButton &&
		   m_modifiers == other.m_modifiers;
}

#endif // HOTKEY_H
