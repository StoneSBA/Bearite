#include "bearite_api.h"
#include "bearite_il2cpp.h"

__attribute__((visibility("default")))
void bearite_mod_init(const BeariteApi* api, const char* mod_id) {
    if (!api || !mod_id) return;

    api->log(mod_id, "Silent Bear Adventure: mod loaded.");

    if (!bearite_il2cpp_init(api)) {
        api->log(mod_id, "IL2CPP initialization failed.");
        return;
    }

    api->log(mod_id, "IL2CPP ready.");
}
