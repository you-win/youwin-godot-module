#include "result.h"

#pragma region SafeError

String SafeError::to_string() {
	Array vals;
	vals.push_back(name.empty() ? String("") : (String("Name: ") + name + String(" ")));
	vals.push_back(code);
	vals.push_back(description);

	return String("Error - {0}Code: {1}, Description: {2}").format(vals);
}

String SafeError::get_name() const {
	return name;
}

void SafeError::set_name(const String &p_name) {
	name = p_name;
}

int SafeError::get_code() const {
	return code;
}

void SafeError::set_code(const int p_code) {
	code = p_code;
}

String SafeError::get_description() const {
	return description;
}

void SafeError::set_description(const String &p_description) {
	description = p_description;
}

SafeError::SafeError() {
	code = -1;
	description = "";
}

SafeError::~SafeError() {}

void SafeError::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_name"), &SafeError::get_name);
	ClassDB::bind_method(D_METHOD("set_name", "name"), &SafeError::set_name);

	ClassDB::bind_method(D_METHOD("get_code"), &SafeError::get_code);
	ClassDB::bind_method(D_METHOD("set_code", "code"), &SafeError::set_code);

	ClassDB::bind_method(D_METHOD("get_description"), &SafeError::get_description);
	ClassDB::bind_method(D_METHOD("set_description", "description"), &SafeError::set_description);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "code"), "set_code", "get_code");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description", "get_description");
}

#pragma endregion

#pragma region Result

String Result::to_string() {
	if (is_ok()) {
		Array vals;
		vals.push_back(String(value));

		return String("Ok - {0}").format(vals);
	} else {
		return error->to_string();
	}
}

Variant Result::get_value() {
	return value;
}

void Result::set_value(const Variant &p_value) {
	value = p_value;
}

Ref<SafeError> Result::get_error() {
	return error;
}

void Result::set_error(Ref<SafeError> p_error) {
	error = p_error;
}

Variant Result::unwrap() {
	ERR_FAIL_COND_V(is_err(), Variant());

	return value;
}

Ref<SafeError> Result::unwrap_err() {
	ERR_FAIL_COND_V(is_ok(), Variant());

	return error;
}

Variant Result::expect(const String &p_text) {
	if (is_err()) {
		print_error(p_text);
		ERR_FAIL_V_MSG(Variant(), p_text);
	}

	return value;
}

Ref<SafeError> Result::expect_err(const String &p_text) {
	if (is_ok()) {
		print_error(p_text);
		ERR_FAIL_V_MSG(Variant(), p_text);
	}

	return error;
}

Result::Result() :
		value(Variant()) {}

Result::~Result() {}

void Result::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_value"), &Result::get_value);
	ClassDB::bind_method(D_METHOD("set_value", "value"), &Result::set_value);

	ClassDB::bind_method(D_METHOD("get_error"), &Result::get_error);
	ClassDB::bind_method(D_METHOD("set_error", "error"), &Result::set_error);

	ClassDB::bind_method(D_METHOD("is_ok"), &Result::is_ok);
	ClassDB::bind_method(D_METHOD("is_err"), &Result::is_err);

	ClassDB::bind_method(D_METHOD("unwrap"), &Result::unwrap);
	ClassDB::bind_method(D_METHOD("unwrap_err"), &Result::unwrap_err);
	ClassDB::bind_method(D_METHOD("expect", "text"), &Result::expect);
	ClassDB::bind_method(D_METHOD("expect_err", "text"), &Result::expect_err);

	ADD_PROPERTY(PropertyInfo(Variant::NIL, "value"), "set_value", "get_value");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "error"), "set_error", "get_error");
}

#pragma endregion

#pragma region Safely

Ref<Result> Safely::ok(const Variant &p_value) {
	ERR_FAIL_COND_V_MSG(
			p_value.get_type() == Variant::NIL,
			err(ERR_INVALID_PARAMETER, "ok(...) must not contain null"),
			"ok(...) must not contain null");

	Ref<Result> r;
	r.instance();

	r->set_value(p_value);

	return r;
}

Ref<Result> Safely::err(const int p_code, const String &p_description) {
	Ref<Result> r;
	r.instance();

	Ref<SafeError> e;
	e.instance();

	e->set_name(error_codes.get(p_code, ""));
	e->set_code(p_code);
	e->set_description(p_description);

	r->set_error(e);

	return r;
}

bool Safely::failed(const Ref<Result> p_result) {
	return p_result.is_null() || p_result->is_err();
}

Ref<Result> Safely::maybe(const Variant &p_value) {
	ERR_FAIL_COND_V(p_value.get_type() == Variant::NIL, err(INT_MAX, "Result is null, this is likely a function failure"));

	if (p_value.get_type() == Variant::OBJECT &&
			p_value.is_ref() &&
			static_cast<Ref<Reference>>(p_value)->is_class("Result")) {
		return static_cast<Ref<Result>>(p_value);
	}

	Ref<Result> r;
	r.instance();

	r->set_value(p_value);

	return r;
}

String Safely::describe(Ref<Result> p_result) {
	return p_result.is_null() ? err(INT_MAX, "Result is null, this is likely a function failure")->to_string() : p_result->to_string();
}

Ref<Result> Safely::register_error_codes(const Dictionary p_error_codes) {
	Array keys = p_error_codes.keys();
	Array vals = p_error_codes.values();

	for (int i = 0; i < keys.size(); i++) {
		error_codes[vals[i]] = keys[i];
	}

	return ok(OK);
}

Safely *Safely::singleton = nullptr;

Safely *Safely::get_singleton() {
	return singleton;
}

Safely *Safely::create() {
	ERR_FAIL_COND_V_MSG(singleton, nullptr, "Result singleton already exists.");
	return memnew(Safely);
}

void Safely::_bind_methods() {
	ClassDB::bind_method(D_METHOD("ok", "value"), &Safely::ok, DEFVAL(Variant(OK)));
	ClassDB::bind_method(D_METHOD("err", "code", "description"), &Safely::err, DEFVAL(""));

	ClassDB::bind_method(D_METHOD("failed", "result"), &Safely::failed);

	ClassDB::bind_method(D_METHOD("maybe", "value"), &Safely::maybe);

	ClassDB::bind_method(D_METHOD("describe", "result"), &Safely::describe);

	ClassDB::bind_method(D_METHOD("register_error_codes", "error_codes"), &Safely::register_error_codes);
}

Safely::Safely() {
	singleton = this;
	error_codes[INT_MAX] = "UNHANDLED_ERROR";
}

Safely::~Safely() {}

#pragma endregion
