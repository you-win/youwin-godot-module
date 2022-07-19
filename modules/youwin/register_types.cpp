#include "register_types.h"

#include "core/config/engine.h"
#include "result.h"

static Safely *safely = nullptr;

void initialize_youwin_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_CLASS(SafeError);
	GDREGISTER_CLASS(Result);
	GDREGISTER_CLASS(Safely);

	safely = Safely::create();

	Engine::get_singleton()->add_singleton(Engine::Singleton("Safely", Safely::get_singleton()));
}

void uninitialize_youwin_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	if (safely) {
		memdelete(safely);
	}
}
