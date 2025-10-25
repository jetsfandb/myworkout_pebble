#include <malloc.h>
#include <string.h>
#include <pebble.h>
#include "workout.h"
#include "myworkout.h"

static Workout *workout;
static char s_set_actual_count_text[11];
static char s_exercise_settings_text[41];
static char s_set_weight_settings_text[12];
static char s_set_count_settings_text[12];
static char s_exercise_set_rest_text[41];

static Window *s_myworkout_window;
static Window* s_exercise_window;

static SimpleMenuLayer*   s_menu_layer;
static SimpleMenuSection* s_menu_section;
static SimpleMenuItem*    s_menu_items;
static GFont s_bold_font;
static GFont s_font;
static TextLayer* s_exercise_title_layer;
static TextLayer* s_set_actual_count_layer;
static TextLayer* s_settings_text_layer;
static TextLayer* s_set_weight_text_layer;
static TextLayer* s_set_count_text_layer;
static TextLayer* s_rest_text_layer;
static GBitmap* s_checkmark_image;
static AppTimer* s_set_rest_timer = 0;
static WakeupId s_wakeup_id = -1;
static time_t s_wakeup_timestamp = 0;

enum {
  PERSIST_WAKEUP // Persistent storage key for wakeup_id
};

int main(void) {
  myworkout_init();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_myworkout_window);
  app_event_loop();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done with event loop");
  myworkout_deinit();
}

static void myworkout_init(void) {
  s_myworkout_window = window_create();
  s_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  s_bold_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_checkmark_image = gbitmap_create_with_resource(RESOURCE_ID_CHECKMARK);
  workout = NULL;
  workout_init(&workout, "My Workout");
  workout_load(workout);
  build_exercise_list();
  window_set_window_handlers(s_myworkout_window, (WindowHandlers) {
    .load = myworkout_window_load,
    .appear = myworkout_window_appear,
    .unload = myworkout_window_unload,
  });
  s_exercise_window = window_create();
  window_set_window_handlers(s_exercise_window, (WindowHandlers) {
    .load = exercise_window_load,
    .unload = exercise_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_myworkout_window, animated);
}

static void myworkout_deinit(void) {  
  free(s_menu_items);
  free(s_menu_section);
  workout_free(&workout);
  window_destroy(s_myworkout_window);
}

static void myworkout_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  s_menu_layer = simple_menu_layer_create(bounds, window, s_menu_section, 1, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(s_menu_layer));
}

static void myworkout_window_appear(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "myworkout_window_appear");
}

static void myworkout_window_unload(Window *window) {
  simple_menu_layer_destroy(s_menu_layer);
  gbitmap_destroy(s_checkmark_image);
}

static void build_exercise_list() {
  s_menu_section = (SimpleMenuSection *) malloc(sizeof(SimpleMenuSection));
  s_menu_section->title = workout->title;
  s_menu_section->num_items = workout->exercises_size;
  s_menu_items = (SimpleMenuItem *) malloc(sizeof(SimpleMenuItem) * workout->exercises_size);
  for(Size e = 0; e < workout->exercises_size; e++) {
    s_menu_items[e].title = workout->exercises[e].title;
    s_menu_items[e].callback = exercise_selected;
  }
  s_menu_section->items = s_menu_items;
}

static void exercise_selected(int index, void* context) {
  if(workout->exercises[index].actual_set_size == workout->exercises[index].scheduled_set_size) {
    const uint32_t const segments[] = { 500, 100, 500, 100, 500};
    VibePattern pat = {
      .durations = segments,
      .num_segments = ARRAY_LENGTH(segments),
    };
    vibes_enqueue_custom_pattern(pat);  
  }
  else {
    workout->current_exercise = index;
    window_stack_push(s_exercise_window, true);
  }
}

static void exercise_window_load(Window *window) {
  s_wakeup_timestamp = 0;
  window_set_click_config_provider(window, (ClickConfigProvider) exercise_set_click_config_provider);  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  bounds.size.h = 30;
  
  s_exercise_title_layer = text_layer_create(bounds);
  text_layer_set_font(s_exercise_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_exercise_title_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_exercise_title_layer));

  bounds.origin.y += bounds.size.h;
  s_set_actual_count_layer = text_layer_create(bounds);
  text_layer_set_font(s_set_actual_count_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(window_layer, text_layer_get_layer(s_set_actual_count_layer));

  Size setting_size = workout->exercises[workout->current_exercise].setting_size;
  if(setting_size) {
    bounds.origin.y += bounds.size.h;
    s_settings_text_layer = text_layer_create(bounds);
    text_layer_set_font(s_settings_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
    layer_add_child(window_layer, text_layer_get_layer(s_settings_text_layer));   
  }

  bounds.origin.y += bounds.size.h;
  s_set_weight_text_layer = text_layer_create(bounds);
  text_layer_set_font(s_set_weight_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(window_layer, text_layer_get_layer(s_set_weight_text_layer));

  bounds.origin.y += bounds.size.h;
  s_set_count_text_layer = text_layer_create(bounds);
  text_layer_set_font(s_set_count_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(window_layer, text_layer_get_layer(s_set_count_text_layer));

  bounds.origin.y += bounds.size.h;
  s_rest_text_layer = text_layer_create(bounds);
  text_layer_set_font(s_rest_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer, text_layer_get_layer(s_rest_text_layer));
  exercise_display_current_set();
}

static void exercise_set_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler)exercise_set_select_click_handler);
}

static void exercise_display_current_set() {
  text_layer_set_text(s_exercise_title_layer, workout->exercises[workout->current_exercise].title);
  snprintf(s_set_actual_count_text, sizeof(s_set_actual_count_text), "Set: %d", (int) workout->exercises[workout->current_exercise].actual_set_size + 1);
  text_layer_set_text(s_set_actual_count_layer, s_set_actual_count_text);

  memset(s_exercise_settings_text, 0, sizeof(s_exercise_settings_text));
  Size setting_size = workout->exercises[workout->current_exercise].setting_size;
  for(Size s = 0; s < setting_size; s++) {
    if(s) {
      snprintf(s_exercise_settings_text, sizeof(s_exercise_settings_text), "%s, %s: %d", s_exercise_settings_text, workout->exercises[workout->current_exercise].settings[s].title, workout->exercises[workout->current_exercise].settings[s].value);
    }
    else {
      snprintf(s_exercise_settings_text, sizeof(s_exercise_settings_text), "%s: %d", workout->exercises[workout->current_exercise].settings[s].title, workout->exercises[workout->current_exercise].settings[s].value);
    }
  }

  if(setting_size) {
    text_layer_set_text(s_settings_text_layer, s_exercise_settings_text);
  }

  snprintf(s_set_weight_settings_text, 
          sizeof(s_set_weight_settings_text), 
          "Weight: %d", 
          (int) workout->exercises[workout->current_exercise].scheduled_sets[workout->exercises[workout->current_exercise].actual_set_size].weight);
  text_layer_set_text(s_set_weight_text_layer, s_set_weight_settings_text);

  snprintf(s_set_count_settings_text, 
          sizeof(s_set_count_settings_text), 
          "Count: %d", 
          (int) workout->exercises[workout->current_exercise].scheduled_sets[workout->exercises[workout->current_exercise].actual_set_size].count);
  text_layer_set_text(s_set_count_text_layer, s_set_count_settings_text);
  layer_set_hidden(text_layer_get_layer(s_rest_text_layer), true);
}

static void exercise_display_set_rest() {
  int countdown = s_wakeup_timestamp - time(NULL);
  if(countdown > 0) {
    snprintf(s_exercise_set_rest_text, sizeof(s_exercise_set_rest_text), "Resting for %d seconds...", countdown);
    text_layer_set_text(s_rest_text_layer, s_exercise_set_rest_text);
  }
  else {
    s_wakeup_timestamp = 0;
  }
}

static void exercise_set_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  time_t wakeup_time = time(NULL) + 30;
  s_wakeup_id = wakeup_schedule(wakeup_time, workout->current_set, false);
  if(wakeup_query(s_wakeup_id, &s_wakeup_timestamp)) {
    persist_write_int(PERSIST_WAKEUP, s_wakeup_id);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "wakeup scheduled");
  }
  layer_set_hidden(text_layer_get_layer(s_rest_text_layer), false);
  s_set_rest_timer = app_timer_register(0, rest_timer_handler, NULL);
}

static void exercise_window_unload(Window *window) {
  layer_remove_child_layers(window_get_root_layer(window));
  text_layer_destroy(s_exercise_title_layer);
  text_layer_destroy(s_settings_text_layer);
  text_layer_destroy(s_set_weight_text_layer);
  text_layer_destroy(s_set_count_text_layer);
  text_layer_destroy(s_rest_text_layer);
}

static void rest_timer_handler(void *data) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "rest_timer_handler");
  int countdown = s_wakeup_timestamp - time(NULL);
  if(countdown > 0) {
    layer_mark_dirty(text_layer_get_layer(s_rest_text_layer));
    exercise_display_set_rest();
    s_set_rest_timer = app_timer_register(1000, rest_timer_handler, data);
  }
  else {
    s_wakeup_timestamp = 0;
    s_set_rest_timer = 0;
    vibes_long_pulse();
    wakeup_cancel(s_wakeup_id);
    s_wakeup_id = -1;
    persist_delete(PERSIST_WAKEUP);
    next_set();
  }
}

static void next_set() {
  Exercise* exercise = &workout->exercises[workout->current_exercise];
  Set* scheduled = &exercise->scheduled_sets[exercise->actual_set_size];
  add_actual_set(exercise, scheduled->weight, scheduled->count);
  if(exercise->actual_set_size == exercise->scheduled_set_size) {
    s_menu_items[workout->current_exercise].icon = s_checkmark_image;
    window_stack_pop(true);
  }
  else {
    exercise_display_current_set();
  }
}
