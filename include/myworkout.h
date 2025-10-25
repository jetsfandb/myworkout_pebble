#include <pebble.h>

int main(void);
static void myworkout_window_load(Window *window);
static void myworkout_window_appear(Window *window);
static void myworkout_window_unload(Window *window);
static void myworkout_init(void);
static void myworkout_deinit(void);
static void exercise_selected(int index, void* context);
static void exercise_window_load(Window *window);
static void exercise_set_click_config_provider(void *context);
static void exercise_display_current_set();
static void exercise_set_select_click_handler(ClickRecognizerRef recognizer, void *context);
static void exercise_window_unload(Window *window);
static void build_exercise_list();
static void rest_timer_handler(void *data);
static void next_set();
