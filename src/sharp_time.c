#include <pebble.h>

#define KEY_BACKGROUND_COLOR 0
#define KEY_TIME_COLOR 1
#define KEY_DATE_COLOR 2
#define KEY_INFO_COLOR 3
#define KEY_STEP_COLOR 4
#define KEY_CONDITIONS 5

static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer, *s_battery_layer, *s_steps_layer;


static void update_time(){
  // Get a tm structure 
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? 
                                          "%H:%M" : "%I:%M", tick_time);

  text_layer_set_text(s_time_layer, s_buffer);

  static char date_buffer[24];
  strftime(date_buffer, sizeof(date_buffer), "%a %e %b", tick_time);

  text_layer_set_text(s_date_layer, date_buffer);
}

static void set_background_color(int color){
  GColor background_color = GColorFromHEX(color);

  persist_write_int(KEY_BACKGROUND_COLOR, color);
  window_set_background_color(s_main_window, background_color);
}

static void set_time_color(int color) {
  GColor time_color = GColorFromHEX(color);

  persist_write_int(KEY_TIME_COLOR, color);
  text_layer_set_text_color(s_time_layer, time_color);
}

static void set_date_color(int color){
  GColor date_color = GColorFromHEX(color);

  persist_write_int(KEY_DATE_COLOR, color);
  text_layer_set_text_color(s_date_layer, date_color);
}

static void set_info_color(int color){
  GColor info_color = GColorFromHEX(color);

  persist_write_int(KEY_INFO_COLOR, color);
  text_layer_set_text_color(s_battery_layer, info_color);
}

static void set_steps_color(int color){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting steps color");
  GColor steps_color = GColorFromHEX(color);

  persist_write_int(KEY_STEP_COLOR, color);
  text_layer_set_text_color(s_steps_layer, steps_color);
}

static void battery_callback(BatteryChargeState state) {

  static char buffer[5];
  snprintf (buffer, sizeof (buffer), "%d%%", (int) state.charge_percent);
  text_layer_set_text (s_battery_layer, buffer);
}

static void health_handler(HealthEventType event, void *context) {
  static char s_value_buffer[8];
  if (event == HealthEventMovementUpdate) {
    // display the step count
    snprintf(s_value_buffer, sizeof(s_value_buffer), "%ds", (int)health_service_sum_today(HealthMetricStepCount));
    text_layer_set_text(s_steps_layer, s_value_buffer);
  }
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Inbox received handler");

  Tuple *background_color_t = dict_find(iter, KEY_BACKGROUND_COLOR);
  Tuple *time_color_t = dict_find(iter, KEY_TIME_COLOR);
  Tuple *date_color_t = dict_find(iter, KEY_DATE_COLOR);
  Tuple *info_color_t = dict_find(iter, KEY_INFO_COLOR);
  Tuple *steps_color_t = dict_find(iter, KEY_STEP_COLOR);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Steps");

  // static char temperature_buffer[4];

  // Tuple *temp_tuple = dict_find(iter, KEY_TEMPERATURE);


  if (background_color_t) {
    int background_color = background_color_t->value->int32;
    set_background_color(background_color);
  }

  if(time_color_t) {
    int time_color = time_color_t->value->int32;
    set_time_color(time_color);
  }

  if(date_color_t) {
    int date_color = date_color_t->value->int32;
    set_date_color(date_color);
  }

  if(info_color_t) {
    int info_color = info_color_t->value->int32;
    set_info_color(info_color);
  }

  if(steps_color_t) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Steps color pass");

    int steps_color = steps_color_t->value->int32;
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting steps color: %d", steps_color);
    set_steps_color(steps_color);
    // snprintf(temperature_buffer, sizeof(temperature_buffer), "%dF", (int)temp_tuple->value->int32);
    // text_layer_set_text(s_steps_layer, temperature_buffer);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  update_time();
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  if(persist_exists(KEY_BACKGROUND_COLOR)){
    int background_color = persist_read_int(KEY_BACKGROUND_COLOR);
    set_background_color(background_color);
  }

  if(persist_exists(KEY_TIME_COLOR)){
    int time_color = persist_read_int(KEY_TIME_COLOR);
    set_time_color(time_color);
  }

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  #if defined(PBL_ROUND)
    s_date_layer = text_layer_create(GRect(0,100,180,30));
  #else 
    //s_date_layer = text_layer_create(GRect(0,100,144,30));
    s_date_layer = text_layer_create(GRect(0,100,144,30));
  #endif
    
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  if(persist_exists(KEY_DATE_COLOR)) {
    int date_color = persist_read_int(KEY_DATE_COLOR);
    set_date_color(date_color);
  }

  // Add to Window
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));

  
  s_battery_layer = text_layer_create(GRect(0, 130, PBL_IF_ROUND_ELSE(125, 100), 30));
  
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);

  // Update the battery display on load
  BatteryChargeState charge_state = battery_state_service_peek();
  static char buffer[5];
  snprintf (buffer, sizeof (buffer), "%d%%", (int) charge_state.charge_percent);

  text_layer_set_text(s_battery_layer, buffer);
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));

  if(persist_exists(KEY_INFO_COLOR)){
    int info_color = persist_read_int(KEY_INFO_COLOR);
    set_info_color(info_color);
  }


  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));

  //GRect(0, 130, PBL_IF_ROUND_ELSE(125, 110), 30)
  // Create temperature Layer
  s_steps_layer = text_layer_create(
    GRect(0,130, PBL_IF_ROUND_ELSE(180, 195), 50));

  // Style the text
  text_layer_set_background_color(s_steps_layer, GColorClear);
  text_layer_set_text_color(s_steps_layer, GColorWhite);
  text_layer_set_text_alignment(s_steps_layer, GTextAlignmentCenter);
  text_layer_set_text(s_steps_layer, "...");
  text_layer_set_font(s_steps_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_steps_layer));

  if(persist_exists(KEY_STEP_COLOR)){
    int steps_color = persist_read_int(KEY_STEP_COLOR);
    set_steps_color(steps_color);
  }

  // subscribe to health events
  if(health_service_events_subscribe(health_handler, NULL)) {
    // force initial steps display
    health_handler(HealthEventMovementUpdate, NULL);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }
} 

static void main_window_unload(Window *s_main_window) {
  health_service_events_unsubscribe();
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_battery_layer);
  // Destroy weather elements
  text_layer_destroy(s_steps_layer);

}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_set_background_color(s_main_window, GColorIslamicGreen);

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  update_time();


  // Register with TickTimeerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Register for battery level updates
  battery_state_service_subscribe(battery_callback);

  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  // Destroy Window 
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}