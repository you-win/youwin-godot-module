#include "register_types.h"

#include "result.h"

void register_youwin_types() {
	ClassDB::register_class<SafeError>();
	ClassDB::register_class<Result>();
	ClassDB::register_class<Safely>();
}

void unregister_youwin_types() {
}
