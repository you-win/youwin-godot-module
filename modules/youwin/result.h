#ifndef YOUWIN_RESULT_H
#define YOUWIN_RESULT_H

#include "modules/regex/regex.h"

#include "core/object.h"
#include "core/reference.h"

class SafeError : public Reference {
	GDCLASS(SafeError, Reference);

	String name;
	int code;
	String description;

public:
	String to_string();

	String get_name() const;
	void set_name(const String &p_name);

	int get_code() const;
	void set_code(const int p_code);

	String get_description() const;
	void set_description(const String &p_description);

	SafeError();
	~SafeError();

protected:
	static void _bind_methods();
};

class Result : public Reference {
	GDCLASS(Result, Reference);

	Variant value;

	Ref<SafeError> error;

public:
	String to_string();

	Variant get_value();
	void set_value(const Variant &p_value);

	Ref<SafeError> get_error();
	void set_error(Ref<SafeError> p_error);

	_FORCE_INLINE_ bool is_ok() const {
		return value.get_type() != Variant::NIL;
	}
	_FORCE_INLINE_ bool is_err() const {
		return value.get_type() == Variant::NIL;
	}

	void _set_error_code(const int p_code);
	void _set_error_description(const String &p_description);

	Variant unwrap();
	Ref<SafeError> unwrap_err();

	Variant expect(const String &p_text);
	Ref<SafeError> expect_err(const String &p_text);

	Result();
	~Result();

protected:
	static void _bind_methods();
};

class Safely : public Object {
	GDCLASS(Safely, Object);

	Dictionary error_codes;

public:
	Ref<Result> ok(const Variant &p_value = Variant(OK));
	Ref<Result> err(const int p_code, const String &p_description = "");

	bool failed(const Ref<Result> p_result);

	Ref<Result> maybe(const Variant &p_value);

	String describe(Ref<Result> p_result);

	Ref<Result> register_error_codes(const Dictionary p_error_codes);

	static Safely *create();
	static Safely *get_singleton();

	Safely();
	~Safely();

protected:
	static Safely *singleton;
	static void _bind_methods();
};

#endif YOUWIN_RESULT_H
