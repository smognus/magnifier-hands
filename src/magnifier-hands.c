#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "outlined_text.h"
#include "util.h"

#define MY_UUID { 0x2C, 0xFB, 0x3A, 0x88, 0xFF, 0x35, 0x4F, 0xE1, 0xBD, 0x05, 0x4D, 0x4D, 0x65, 0x55, 0xE1, 0xE4 }
PBL_APP_INFO(MY_UUID,
             "Magnifier Hands", "Smognus",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

#define PI 3.14159265359
const int trig_degree = (TRIG_MAX_ANGLE / 360);

Window window;
Layer clockface_layer;
Layer hand_layer;
Layer date_layer;

GPathInfo hour_hand_path_points = {
  .num_points = 6,
  .points = (GPoint []) {{6,0},{0,8},{-6,0},{-4,-40},{0,-45},{4,-40}}
};
GPathInfo minute_hand_path_points = {
  .num_points = 6,
  .points = (GPoint []) {{4,0},{0,8},{-4,0},{-3,-55},{0,-60},{3,-55}}
};
/* GPathInfo second_hand_path_points = { */
/*   .num_points = 6, */
/*   .points = (GPoint []) {{4,0},{0,8},{-4,0},{-3,-55},{0,-60},{3,-55}} */
/* }; */

/* GPathInfo date_diamond_points = { */
/*   .num_points = 4, */
/*   .points = (GPoint []) {{-8,8},{8,8},{8,-8},{-8,-8}} */
/* }; */

static void draw_dot(GContext* ctx, GPoint center, int radius) {
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx,GColorBlack);
  graphics_fill_circle(ctx, center, radius);
  graphics_draw_circle(ctx, center, radius);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_circle(ctx, center, radius+1);
}

static void clockface_layer_update_proc(Layer* layer, GContext* ctx) {
  int degree_step;
  int current_trig_degrees;
  GPoint current_point;
  char twelve[] = "12";
  char three[] = "3";
  char six[] = "6";
  char nine[] = "9";

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0,0,144,168), 0, GCornerNone);

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(72,84), 60);

  draw_dot(ctx, GPoint(10,10), 20);
  draw_dot(ctx, GPoint(134,10), 20);
  draw_dot(ctx, GPoint(10,158), 20);
  draw_dot(ctx, GPoint(134,158), 20);
  draw_dot(ctx, GPoint(10,10), 18);
  draw_dot(ctx, GPoint(134,10), 18);
  draw_dot(ctx, GPoint(10,158), 18);
  draw_dot(ctx, GPoint(134,158), 18);

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_circle(ctx, GPoint(72,84), 65);
  graphics_draw_circle(ctx, GPoint(72,84), 69);

  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_circle(ctx, GPoint(72,84), 58);
  graphics_draw_circle(ctx, GPoint(72,84), 44);
  graphics_draw_circle(ctx, GPoint(72,84), 50);

  draw_outlined_text(ctx,
  		     twelve,
  		     fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK),
  		     GRect(0,20,144,168-40),
  		     GTextOverflowModeWordWrap,
  		     GTextAlignmentCenter,
  		     1,
  		     false);
  draw_outlined_text(ctx,
  		     three,
  		     fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK),
  		     GRect(0,67,130,168-40),
  		     GTextOverflowModeWordWrap,
  		     GTextAlignmentRight,
  		     1,
  		     false);
  draw_outlined_text(ctx,
  		     six,
  		     fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK),
  		     GRect(0,111,144,168-40),
  		     GTextOverflowModeWordWrap,
  		     GTextAlignmentCenter,
  		     1,
  		     false);
  draw_outlined_text(ctx,
  		     nine,
  		     fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK),
  		     GRect(15,67,144,168-40),
  		     GTextOverflowModeWordWrap,
  		     GTextAlignmentLeft,
  		     1,
  		     false);

  degree_step = 6;
  for (int i=0;i<60;i++) {
    current_trig_degrees = (degree_step * i) * trig_degree;

    current_point.x = (sin_lookup(current_trig_degrees) * 62 / TRIG_MAX_RATIO) + 72;
    current_point.y = (-cos_lookup(current_trig_degrees) * 62 / TRIG_MAX_RATIO) + 84;
    draw_dot(ctx, current_point, 1);
  }

  // draw the outside hour markers
  degree_step = 30;
  for (int i=0;i<12;i++) {
    current_trig_degrees = (degree_step * i) * trig_degree;

    current_point.x = (sin_lookup(current_trig_degrees) * 64 / TRIG_MAX_RATIO) + 72;
    current_point.y = (-cos_lookup(current_trig_degrees) * 64 / TRIG_MAX_RATIO) + 84;
    draw_dot(ctx, current_point, 3);
  }

  // draw the inside hour markers
  degree_step = 30;
  for (int i=0;i<12;i++) {
    current_trig_degrees = (degree_step * i) * trig_degree;

    current_point.x = (sin_lookup(current_trig_degrees) * 48 / TRIG_MAX_RATIO) + 72;
    current_point.y = (-cos_lookup(current_trig_degrees) * 48 / TRIG_MAX_RATIO) + 84;
    if (i != 0 && i != 3 && i != 6 && i != 9) {
      draw_dot(ctx, current_point, 3);
    }
  }

  degree_step = 90;
  for (int i=0;i<4;i++) {
    current_trig_degrees = (degree_step * i) * trig_degree;

    current_point.x = (sin_lookup(current_trig_degrees) * 66 / TRIG_MAX_RATIO) + 72;
    current_point.y = (-cos_lookup(current_trig_degrees) * 66 / TRIG_MAX_RATIO) + 84;
    draw_dot(ctx, current_point, 5);
  }
}

static void hand_layer_update_proc(Layer* layer, GContext* ctx) {
  PblTm time;
  get_time(&time);
  /* int second_angle = time.tm_sec * 6; */
  int minute_angle = time.tm_min * 6;
  int hour_angle = (time.tm_hour * 30) + (time.tm_min / 2);
  GPath minute_hand_path;
  GPath hour_hand_path;
  /* GPath second_hand_path; */
  GPoint center = GPoint(72,84);
  /* GPoint minute_magnifier_center; */
  /* GPoint minute_magnifier_hand_edge_one; */
  /* GPoint minute_magnifier_hand_edge_two; */
  /* GPoint magnified_minute_marker_center; */
  /* int magnified_minute_marker_radius = 10; */

  /* int magnifier_offset; */
  /* int magnifier_radius = 20; */

  gpath_init(&minute_hand_path, &minute_hand_path_points);
  gpath_move_to(&minute_hand_path, center);
  gpath_rotate_to(&minute_hand_path, trig_degree * minute_angle);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  gpath_draw_filled(ctx, &minute_hand_path);
  gpath_draw_outline(ctx, &minute_hand_path);

  gpath_init(&hour_hand_path, &hour_hand_path_points);
  gpath_move_to(&hour_hand_path, center);
  gpath_rotate_to(&hour_hand_path, trig_degree * hour_angle);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  gpath_draw_filled(ctx, &hour_hand_path);
  gpath_draw_outline(ctx, &hour_hand_path);

  /* gpath_init(&second_hand_path, &second_hand_path_points); */
  /* gpath_move_to(&second_hand_path, center); */
  /* gpath_rotate_to(&second_hand_path, trig_degree * second_angle); */
  /* graphics_context_set_fill_color(ctx, GColorBlack); */
  /* graphics_context_set_stroke_color(ctx, GColorWhite); */
  /* gpath_draw_filled(ctx, &second_hand_path); */
  /* gpath_draw_outline(ctx, &second_hand_path); */

  /* // draw the minute hand magnification */
  /* magnifier_offset = abs(minute_hand_path_points.points[4].y) - 4; */

  /* minute_magnifier_center.x = (sin_lookup(minute_angle * trig_degree) * magnifier_offset / TRIG_MAX_RATIO) + 72; */
  /* minute_magnifier_center.y = (-cos_lookup(minute_angle * trig_degree) * magnifier_offset / TRIG_MAX_RATIO) + 84; */
  /* graphics_context_set_stroke_color(ctx, GColorBlack); */
  /* graphics_context_set_fill_color(ctx, GColorWhite); */
  /* graphics_fill_circle(ctx, minute_magnifier_center, magnifier_radius); */

  /* for (int i=0;i<5;i++) { */
  /*   graphics_draw_circle(ctx, minute_magnifier_center, magnifier_radius+i); */
  /* } */

  /* minute_magnifier_hand_edge_one.x = (sin_lookup(((minute_angle+180)-30) * trig_degree) * magnifier_radius / TRIG_MAX_RATIO) +  */
  /*   minute_magnifier_center.x; */
  /* minute_magnifier_hand_edge_one.y = (-cos_lookup(((minute_angle+180)-30) * trig_degree) * magnifier_radius / TRIG_MAX_RATIO) +  */
  /*   minute_magnifier_center.y; */
  /* minute_magnifier_hand_edge_two.x = (sin_lookup(((minute_angle+180)+30) * trig_degree) * magnifier_radius / TRIG_MAX_RATIO) + */
  /*   minute_magnifier_center.x; */
  /* minute_magnifier_hand_edge_two.y = (-cos_lookup(((minute_angle+180)+30) * trig_degree) * magnifier_radius / TRIG_MAX_RATIO) + */
  /*   minute_magnifier_center.y; */

  /* graphics_draw_line(ctx, minute_magnifier_center, minute_magnifier_hand_edge_one); */
  /* graphics_draw_line(ctx, minute_magnifier_center, minute_magnifier_hand_edge_two); */

  /* // draw the minute marker magnification */
  /* magnified_minute_marker_center.x = (sin_lookup(minute_angle * trig_degree) */
  /* 				      * (magnifier_offset + magnified_minute_marker_radius) */
  /* 				      / TRIG_MAX_RATIO) + 72; */
  /* magnified_minute_marker_center.y = (-cos_lookup(minute_angle * trig_degree) */
  /* 				      * (magnifier_offset + magnified_minute_marker_radius) */
  /* 				      / TRIG_MAX_RATIO) + 84; */

  /* for (int i=3;i>0;i--) { */
  /*   graphics_draw_circle(ctx, magnified_minute_marker_center, magnified_minute_marker_radius-i); */
  /* } */
}

static void date_layer_update_proc(Layer* layer, GContext* ctx) {
  /* GPath date_diamond_path; */
  PblTm time;
  char day_text[] = "  ";
  
  get_time(&time);

  /* gpath_init(&date_diamond_path, &date_diamond_points); */
  /* gpath_move_to(&date_diamond_path, GPoint(72, 84)); */
  /* gpath_rotate_to(&date_diamond_path, TRIG_MAX_ANGLE / 8); // 45 degrees */
  /* graphics_context_set_fill_color(ctx, GColorBlack); */
  /* graphics_context_set_stroke_color(ctx, GColorWhite); */
  /* gpath_draw_filled(ctx, &date_diamond_path); */
  /* gpath_draw_outline(ctx, &date_diamond_path); */

  draw_dot(ctx, GPoint(72, 84), 10);

  strcpy(day_text, itoa(time.tm_mday));
  draw_outlined_text(ctx,
  		     day_text,
  		     fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
  		     GRect(0,75,144,168-80),
  		     GTextOverflowModeWordWrap,
  		     GTextAlignmentCenter,
  		     0,
  		     true);
}

static void handle_tick(AppContextRef ctx, PebbleTickEvent* tick) {
    (void) tick;
    layer_mark_dirty(&window.layer);
}

void handle_init(AppContextRef ctx) {
  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);

  layer_init(&clockface_layer, window.layer.frame);
  clockface_layer.update_proc = &clockface_layer_update_proc;
  layer_add_child(&window.layer, &clockface_layer);

  layer_init(&hand_layer, window.layer.frame);
  hand_layer.update_proc = &hand_layer_update_proc;
  layer_add_child(&window.layer, &hand_layer);

  layer_init(&date_layer, window.layer.frame);
  date_layer.update_proc = &date_layer_update_proc;
  layer_add_child(&window.layer, &date_layer);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .tick_info = {
      .tick_handler = &handle_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
