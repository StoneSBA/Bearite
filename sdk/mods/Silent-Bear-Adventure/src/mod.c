#include "bearite_api.h"
#include "bearite_il2cpp.h" 

// Оригинальные указатели на функции игры
static void (*orig_LoadLevel)(void* self, void* levelName, int targetSpawnerIndex, bool useTransition, bool saveCoins, int gameMode, const void* method);
static bool (*orig_IsPlayerInvulnerableInLevel)(void* self, const void* method);
static void (*orig_OnLocalPlayerHealthUpdated)(void* self, int newHealth, const void* method); 

// 1. ХУК НА ЗАГРУЗКУ УРОВНЯ: Насильно включаем встроенный Хардкор (режим 1)
static void my_LoadLevel_hook(void* self, void* levelName, int targetSpawnerIndex, bool useTransition, bool saveCoins, int gameMode, const void* method) {
int hardcoreMode = 1; // 1 — это режим Хардкора в коде игры
orig_LoadLevel(self, levelName, targetSpawnerIndex, useTransition, saveCoins, hardcoreMode, method);
} 

// 2. ХУК НА УЯЗВИМОСТЬ: Никакого бессмертия для медведя, всегда возвращаем false!
static bool my_IsPlayerInvulnerableInLevel_hook(void* self, const void* method) {
return false;
} 

// 3. ХУК НА ЗДОРОВЬЕ: Режим «Один удар — смерть». Любое изменение здоровья ниже максимума убивает игрока
static void my_OnLocalPlayerHealthUpdated_hook(void* self, int newHealth, const void* method) {
// Если игроку нанесли урон (здоровье уменьшилось), принудительно ставим 0 HP — мгновенная смерть
if (newHealth < 100 && newHealth > 0) {
newHealth = 0;
}
orig_OnLocalPlayerHealthUpdated(self, newHealth, method);
} 

**attribute**((visibility("default"))) void bearite_mod_init(const BeariteApi* api, const char* mod_id) { 

int show_greeting = api->get_bool(mod_id, "greet", 1);
if (show_greeting) {
api->log(mod_id, "=========================================");
api->log(mod_id, "   SILENT BEAR ADVENTURE: REAL ONE-HIT   ");
api->log(mod_id, "   1 удар = смерть + встроенный Хардкор! ");
api->log(mod_id, "=========================================");
}

// Запуск движка IL2CPP
if (!bearite_il2cpp_init(api)) {
api->log(mod_id, "[ОШИБКА] Не удалось инициализировать IL2CPP!");
return;
}

// Подключаем хук к LevelLoader (метод LoadLevel с 5 аргументами)
bearite_hook_method(api, mod_id, "", "LevelLoader", "LoadLevel", 5, (void*)my_LoadLevel_hook, (void**)&orig_LoadLevel);

// Подключаем хук к LevelManager (метод IsPlayerInvulnerableInLevel с 0 аргументов)
bearite_hook_method(api, mod_id, "", "LevelManager", "IsPlayerInvulnerableInLevel", 0, (void*)my_IsPlayerInvulnerableInLevel_hook, (void**)&orig_IsPlayerInvulnerableInLevel);

// Подключаем хук к NetworkPlayerController (метод OnLocalPlayerHealthUpdated с 1 аргументом)
bearite_hook_method(api, mod_id, "", "NetworkPlayerController", "OnLocalPlayerHealthUpdated", 1, (void*)my_OnLocalPlayerHealthUpdated_hook, (void**)&orig_OnLocalPlayerHealthUpdated);

api->log(mod_id, "[Успех] Все хардкорные правила успешно добавлены в очередь!");

}
