#include <pebble.h>

enum {
	KEY_ERROR,
	KEY_ROUTE_SHORT_NAME,
	KEY_ROUTE_LONG_NAME,
	KEY_DISPLAY_NAME,
	KEY_DEPARTURE_TIMESTAMP
};

typedef struct {
	char route_short_name[8];
	char route_long_name[32];
	char display_name[32];
	time_t timestamp;
	int departure_delta_min;
} Departure;

static Departure departures[10];
static int n = 0;

static Window *window;
static TextLayer *text_layer;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
	n = 0;

	Tuple *t = dict_read_first(iterator);

	APP_LOG(APP_LOG_LEVEL_DEBUG, "n = %u", n);

	while(t != NULL) {

		switch(t->key) {
			case KEY_ERROR:
				if (strlen(t->value->cstring) > 0){
					text_layer_set_text(text_layer, t->value->cstring);
				}
				break;
			case KEY_ROUTE_SHORT_NAME:
				strncpy(t->value->cstring, departures[n].route_short_name, sizeof(departures[n].route_short_name));
				break;
			case KEY_ROUTE_LONG_NAME:
				strncpy(t->value->cstring, departures[n].route_long_name, sizeof(departures[n].route_long_name));
				break;
			case KEY_DISPLAY_NAME:
				strncpy(t->value->cstring, departures[n].display_name, sizeof(departures[n].display_name));
				break;
			case KEY_DEPARTURE_TIMESTAMP:
				departures[n].timestamp = t->value->int32;
				break;
			default:
				APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
				break;
		}

		t = dict_read_next(iterator);
	}

	n++;
}
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	text_layer = text_layer_create((GRect) { .origin = { 0, 10 }, .size = { bounds.size.w, 120 } });
	text_layer_set_text(text_layer, "Loading...");
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
	text_layer_destroy(text_layer);
}

static void init(void) {
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	const bool animated = true;
	window_stack_push(window, animated);

	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);

	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit(void) {
	window_destroy(window);
}

int main(void) {
	init();

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

	app_event_loop();
	deinit();
}
