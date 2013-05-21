#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
	
#include "xively.h"
#include "datastream_layer.h"

#define MY_UUID { 0x11, 0x91, 0xB6, 0x2B, 0xBA, 0x19, 0x99, 0x8F, 0xC1, 0x37, 0x14, 0x8F, 0x29, 0xA1, 0x44, 0xAC }
PBL_APP_INFO(MY_UUID,
             "CloudSensor", "Christian Brunner",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

#define DATASTREAM_PORTS 5

static Window window;
static DataStreamLayer datastream_layer[DATASTREAM_PORTS];

void request_datastream(int8_t port);
void handle_timer(AppContextRef app_ctx, AppTimerHandle handle, uint32_t cookie);


void success(int8_t port, int8_t icon, const char* data, DictionaryIterator* sent, void* context)
{
	if(port >= 0 && port < DATASTREAM_PORTS)
	{
		if (icon >= 0 && icon <= DATASTREAM_ICON_MEAT)
			datastream_layer_set_icon(&(datastream_layer[port]), icon);
		datastream_layer_set_data(&(datastream_layer[port]), data);
	}
}

void receiveFailed(void* context)
{
	datastream_layer_set_data(&(datastream_layer[4]), "rFailed");
	
	// request_datastream(0);
}

void sendFailed(AppMessageResult reason, void* context)
{
	// datastream_layer_set_data(&(datastream_layer[4]), "sFailed");
	
	switch (reason)
	{
		case APP_MSG_OK:
			datastream_layer_set_data(&(datastream_layer[4]), "OK");
			break;
		case APP_MSG_SEND_TIMEOUT:
			datastream_layer_set_data(&(datastream_layer[4]), "TOUT");
			break;
		case APP_MSG_SEND_REJECTED:
			datastream_layer_set_data(&(datastream_layer[4]), "REJ");
			break;
		case APP_MSG_NOT_CONNECTED:
			datastream_layer_set_data(&(datastream_layer[4]), "N_CON");
			break;
		case APP_MSG_APP_NOT_RUNNING:
			datastream_layer_set_data(&(datastream_layer[4]), "APP_NOT");
			break;
		case APP_MSG_INVALID_ARGS:
			datastream_layer_set_data(&(datastream_layer[4]), "I_ARGS");
			break;
		case APP_MSG_BUSY:
			datastream_layer_set_data(&(datastream_layer[4]), "BUSY");
			break;
		case APP_MSG_BUFFER_OVERFLOW:
			datastream_layer_set_data(&(datastream_layer[4]), "OFLOW");
			break;
		case APP_MSG_ALREADY_RELEASED:
			datastream_layer_set_data(&(datastream_layer[4]), "A_REL");
			break;
		case APP_MSG_CALLBACK_ALREADY_REGISTERED:
			datastream_layer_set_data(&(datastream_layer[4]), "A_REG");
			break;
		case APP_MSG_CALLBACK_NOT_REGISTERED:
			datastream_layer_set_data(&(datastream_layer[4]), "N_REG");
			break;
	}
	
	vibes_double_pulse();
}

void handle_tick(AppContextRef app_ctx, PebbleTickEvent *event)
{
}

void handle_init(AppContextRef ctx)
{
	resource_init_current_app(&APP_RESOURCES);
	window_init(&window, "Xively Watch");
	window_stack_push(&window, true /* Animated */);
	window_set_fullscreen(&window, true);
	window_set_background_color(&window, GColorBlack);
	
	char data[4] = "N/A\0";
	
	// Add datastream layer
	for (int idx = 0; idx < DATASTREAM_PORTS; idx++)
	{
		datastream_layer_init(&(datastream_layer[idx]), GPoint(0, 33*idx + 1));
		layer_add_child(&window.layer, &(datastream_layer[idx]).layer);
		datastream_layer_set_icon(&(datastream_layer[idx]), DATASTREAM_ICON_LOADING);
		datastream_layer_set_data(&(datastream_layer[idx]), data);
	}
	
	xively_register_callbacks((XIVELYCallbacks){.sendFailed=sendFailed,.receiveFailed=receiveFailed,.success=success}, (void*)ctx);
	
	// Request weather information in a second.
	app_timer_send_event(ctx, 250, 1);
	
}

void handle_deinit(AppContextRef ctx)
{
	for (int idx = 0; idx < DATASTREAM_PORTS; idx++)
	{
		datastream_layer_deinit(&(datastream_layer[idx]));
	}
}


void pbl_main(void *params)
{
	PebbleAppHandlers handlers =
	{
		.init_handler = &handle_init,
		.deinit_handler = &handle_deinit,
		.tick_info =
		{
			.tick_handler = &handle_tick,
			.tick_units = MINUTE_UNIT
		},
		.timer_handler = handle_timer,
		.messaging_info =
		{
			.buffer_sizes =
			{
				.inbound = 256,
				.outbound = 256,
			}
		}
	};
	
	app_event_loop(params, &handlers);
}

void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie)
{
	request_datastream(0);

	// Update again in fifteen minutes.
	if(cookie)
		app_timer_send_event(ctx, 60000, 1);
}

void request_datastream(int8_t port)
{
	if(port >= 0 && port < DATASTREAM_PORTS)
		xively_request_port();
}
