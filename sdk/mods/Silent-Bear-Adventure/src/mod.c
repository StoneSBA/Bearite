#include <stdbool.h>
#include "bearite_api.h"
#include "bearite_il2cpp.h"

// Оригинальные указатели на функции игры
static void (*orig_LoadLevel)(void* self, void* levelName, int targetSpawnerIndex, bool useTransition, bool saveCoins, int gameMode, const void* method);
static bool (*orig_IsPlayerInvulnerableInLevel)(void* self, const void* method);
static void (*orig_OnLocalPlayerHealthUpdated)(void* self, int newHealth, const void* method);

// 1. Включаем встроенный Хардкор (режим 1)
static void my_LoadLevel_hook(void* self, void* levelName, int targetSpawnerIndex, bool useTransition, bool saveCoins, int gameMode, const void* method) {
    orig_LoadLevel(self, levelName, targetSpawnerIndex, useTransition, saveCoins, 1, method);
}

// 2. Отключаем бессмертие, всегда возвращаем false
static bool my_IsPlayerInvulnerableInLevel_hook(void* self, const void* method) {
    return false; 
}

// 3. Режим «Один удар — смерть». Если здоровье изменилось и оно не нулевое — сразу убиваем
static void my_OnLocalPlayerHealthUpdated_hook(void* self, int newHealth, const void* method) {
    if (newHealth > 0) {
        newHealth = 0; 
    }
    orig_OnLocalPlayerHealthUpdated(self, newHealth, method);
}

__attribute__((visibility("default"))) void bearite_mod_init(const BeariteApi* api, const char* mod_id) {
    
    int show_greeting = api->get_bool(mod_id, "greet", 1);
    if (show_greeting) {
        api->log(mod_id, "=========================================");
        api->log(mod_id, "   SILENT BEAR ADVENTURE: REAL ONE-HIT   ");
        api->log(mod_id, "=========================================");
    }

    if (!bearite_il2cpp_init(api)) {
        return;
    }

    // Подключаем проверенные хуки по классам из логов
    bearite_hook_method(api, mod_id, "", "LevelLoader", "LoadLevel", 5, (void*)my_LoadLevel_hook, (void**)&orig_LoadLevel);
    bearite_hook_method(api, mod_id, "", "LevelManager", "IsPlayerInvulnerableInLevel", 0, (void*)my_IsPlayerInvulnerableInLevel_hook, (void**)&orig_IsPlayerInvulnerableInLevel);
    bearite_hook_method(api, mod_id, "", "NetworkPlayerController", "OnLocalPlayerHealthUpdated", 1, (void*)my_OnLocalPlayerHealthUpdated_hook, (void**)&orig_OnLocalPlayerHealthUpdated);
}
