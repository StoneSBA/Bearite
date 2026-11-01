#include "bearite_api.h"
#include "bearite_il2cpp.h" 

// =============================================================================
// 1. ОРИГИНАЛЬНЫЕ УКАЗАТЕЛИ (СЮДА СОХРАНЯТСЯ ФУНКЦИИ ИГРЫ)
// =============================================================================
static float (*orig_get_PlayerSpeed)(void* self, const void* method);
static float (*orig_get_HitboxRadius)(void* self, const void* method);
static int (*orig_get_MaxHealth)(void* self, const void* method);
static float (*orig_get_MonsterSpeed)(void* self, const void* method);
static float (*orig_get_DetectionRadius)(void* self, const void* method);
static int (*orig_get_BossMaxHealth)(void* self, const void* method);
static float (*orig_get_TrapSpeed)(void* self, const void* method); 

// =============================================================================
// 2. ХУКИ-МОДИФИКАТОРЫ (НАШ ХАРДКОРНЫЙ КОД)
// ============================================================================= 

// Урезаем скорость игрока до 30%
static float my_get_PlayerSpeed_hook(void* self, const void* method) {
float normal_speed = orig_get_PlayerSpeed(self, method);
return normal_speed * 0.30f;
} 

// Увеличиваем хитбокс игрока в 1.25 раза (сложнее уворачиваться)
static float my_get_HitboxRadius_hook(void* self, const void* method) {
float normal_radius = orig_get_HitboxRadius(self, method);
return normal_radius * 1.25f;
} 

// Насильно ставим максимальное здоровье в 1 HP (один удар — смерть)
static int my_get_MaxHealth_hook(void* self, const void* method) {
return 1;
} 

// Ускоряем перемещение монстров в 2 раза
static float my_get_MonsterSpeed_hook(void* self, const void* method) {
float normal_monster_speed = orig_get_MonsterSpeed(self, method);
return normal_monster_speed * 2.0f;
} 

// Увеличиваем зоркость/обзор врагов в 3 раза
static float my_get_DetectionRadius_hook(void* self, const void* method) {
float normal_distance = orig_get_DetectionRadius(self, method);
return normal_distance * 3.0f;
} 

// Увеличиваем здоровье боссов в 3 раза
static int my_get_BossMaxHealth_hook(void* self, const void* method) {
int normal_boss_hp = orig_get_BossMaxHealth(self, method);
return normal_boss_hp * 3;
} 

// Ускоряем движение ловушек в 2.5 раза
static float my_get_TrapSpeed_hook(void* self, const void* method) {
float normal_trap_speed = orig_get_TrapSpeed(self, method);
return normal_trap_speed * 2.5f;
} 

// =============================================================================
// 3. ТОЧКА ВХОДА МОДА (ИНИЦИАЛИЗАЦИЯ И ПОДКЛЮЧЕНИЕ К ИГРЕ)
// =============================================================================
**attribute**((visibility("default"))) void bearite_mod_init(const BeariteApi* api, const char* mod_id) { 

// Считываем булеву настройку из вашего mod.json
int show_greeting = api->get_bool(mod_id, "greet", 1);
if (show_greeting) {
api->log(mod_id, "=========================================");
api->log(mod_id, "     SILENT BEAR ADVENTURE: HARDCORE     ");
api->log(mod_id, "   Мод активирован! Удачи, StoneSBA!   ");
api->log(mod_id, "=========================================");
}

// Инициализируем систему IL2CPP
if (!bearite_il2cpp_init(api)) {
api->log(mod_id, "[КРИТИЧЕСКАЯ ОШИБКА] Сбой инициализации IL2CPP!");
return;
}

// Внедряем все 7 усложнений в код игры через движок Bearite
bearite_hook_method(api, mod_id, "Assembly-CSharp", "PlayerMovement", "get_Speed", 0, (void*)my_get_PlayerSpeed_hook, (void**)&orig_get_PlayerSpeed);
bearite_hook_method(api, mod_id, "Assembly-CSharp", "PlayerMovement", "get_Radius", 0, (void*)my_get_HitboxRadius_hook, (void**)&orig_get_HitboxRadius);
bearite_hook_method(api, mod_id, "Assembly-CSharp", "PlayerHealth", "get_MaxHealth", 0, (void*)my_get_MaxHealth_hook, (void**)&orig_get_MaxHealth);
bearite_hook_method(api, mod_id, "Assembly-CSharp", "EnemyAI", "get_MoveSpeed", 0, (void*)my_get_MonsterSpeed_hook, (void**)&orig_get_MonsterSpeed);
bearite_hook_method(api, mod_id, "Assembly-CSharp", "EnemyAI", "get_ViewDistance", 0, (void*)my_get_DetectionRadius_hook, (void**)&orig_get_DetectionRadius);
bearite_hook_method(api, mod_id, "Assembly-CSharp", "BossHealth", "get_MaxHealth", 0, (void*)my_get_BossMaxHealth_hook, (void**)&orig_get_BossMaxHealth);
bearite_hook_method(api, mod_id, "Assembly-CSharp", "TrapController", "get_Speed", 0, (void*)my_get_TrapSpeed_hook, (void**)&orig_get_TrapSpeed);

api->log(mod_id, "[Успех] Все усложнения добавлены в очередь перехвата!");

}
