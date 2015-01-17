#include "ConfigVar.h"

ConfigVar::ConfigVar (const std::string& name, const std::string& value, unsigned int flags) :
		_name(name), _flags(flags), _value(value), _dirty(false)
{
	_intValue = atoi(_value.c_str());
	_floatValue = atof(_value.c_str());
}

ConfigVar::~ConfigVar ()
{
}

void ConfigVar::setValue (const std::string& value)
{
	if (_flags & CV_READONLY) {
		return;
	}
	_dirty = _value != value;
	_value = value;
	_intValue = atoi(_value.c_str());
	_floatValue = atof(_value.c_str());
}
