#include "register_types.h"

#include "core/engine.h"

#include "result.h"

static Safely *safely = nullptr;

void register_youwin_types() {
	ClassDB::register_class<SafeError>();
	ClassDB::register_class<Result>();
	ClassDB::register_class<Safely>();

	safely = Safely::create();

	Engine::get_singleton()->add_singleton(Engine::Singleton("Safely", Safely::get_singleton()));
}

void unregister_youwin_types() {
	if (safely) {
		memdelete(safely);
	}
}
