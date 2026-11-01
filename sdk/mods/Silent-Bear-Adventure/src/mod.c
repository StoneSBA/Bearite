#include "bearite_api.h"
#include "bearite_il2cpp.h"

static float (*orig_get_PlayerSpeed)(void* self, const void* method);
static int (*orig_get_MaxHealth)(void* self, const void* method);

// Оставляем только 30% скорости
static float my_get_PlayerSpeed_hook(void* self, const void* method) {
    if (orig_get_PlayerSpeed) {
        return orig_get_PlayerSpeed(self, method) * 0.30f;
    }
    return 1.0f;
}

// Режим «Один удар — смерть» (1 HP)
static int my_get_MaxHealth_hook(void* self, const void* method) {
    return 1;
}

__attribute__((visibility("default"))) void bearite_mod_init(const BeariteApi* api, const char* mod_id) {
    int show_greeting = api->get_bool(mod_id, "greet", 1);
    if (show_greeting) {
        api->log(mod_id, "=========================================");
        api->log(mod_id, "     SILENT BEAR ADVENTURE: HARDCORE     ");
        api->log(mod_id, "=========================================");
    }

    if (!bearite_il2cpp_init(api)) {
        api->log(mod_id, "[ОШИБКА] Сбой инициализации IL2CPP!");
        return;
    }

    // Перехватываем скорость и здоровье по стандартным путям движка
    bearite_hook_method(api, mod_id, "Assembly-CSharp", "PlayerMovement", "get_Speed", 0, (void*)my_get_PlayerSpeed_hook, (void**)&orig_get_PlayerSpeed);
    bearite_hook_method(api, mod_id, "Assembly-CSharp", "PlayerHealth", "get_MaxHealth", 0, (void*)my_get_MaxHealth_hook, (void**)&orig_get_MaxHealth);

    api->log(mod_id, "[Успех] Базовые хардкорные функции активированы!");
}
