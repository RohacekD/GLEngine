#pragma once

#include <GLRenderer/GUI/GUIComponent.h>

namespace GLEngine {
namespace GLRenderer {
namespace GUI {
// This should look like bool value for user and has size of bool value
// @todo unit testing
class C_CheckBoxValue : public I_GUIComponent {
public:
	C_CheckBoxValue(bool value, std::string&& name = "");
	virtual void Draw() const override;

	void SetName(std::string&& name);
	bool GetValue() const;

	operator bool() const;
	bool operator=(bool val);
	bool operator==(bool val) const;
	bool operator!=(bool val) const;
	C_CheckBoxValue operator!();
	bool* operator&() const;
private:
	std::string m_Name;
	// it is mutable anyway as imgui is free to change its value
	mutable bool		m_Value;
};

static_assert(std::is_constructible<C_CheckBoxValue, bool>::value, "It is not convertible");
}}}