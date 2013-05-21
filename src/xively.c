#include "pebble_os.h"
#include "xively.h"

#define XIVELY_COMMAND_KEY		0xFFFF
#define XIVELY_APP_ID_KEY		0xFFFE

#define XIVELY_PORT1_DATA_KEY	0xFF1F
#define XIVELY_PORT1_ICON_KEY	0xFF1E
	
#define XIVELY_PORT2_DATA_KEY	0xFF2F
#define XIVELY_PORT2_ICON_KEY	0xFF2E

#define XIVELY_PORT3_DATA_KEY	0xFF3F
#define XIVELY_PORT3_ICON_KEY	0xFF3E

#define XIVELY_PORT4_DATA_KEY	0xFF4F
#define XIVELY_PORT4_ICON_KEY	0xFF4E

#define XIVELY_PORT5_DATA_KEY	0xFF5F
#define XIVELY_PORT5_ICON_KEY	0xFF5E

#define XIVELY_CMD_GET_PORTS		0x0001
#define XIVELY_CMD_GET_PORTS_DATA	0x0002

static bool callbacks_registered;
static AppMessageCallbacksNode app_callbacks;
static XIVELYCallbacks xively_callbacks;
static int32_t our_app_id;

static void app_send_failed(DictionaryIterator* failed, AppMessageResult reason, void* context);
static void app_received(DictionaryIterator* received, void* context);
static void app_dropped(void* context, AppMessageResult reason);

XIVELYResult xively_request_port()
{
	DictionaryIterator* dictionary;
	AppMessageResult app_result = app_message_out_get(&dictionary);
	if(app_result != APP_MSG_OK)
		return app_result;	
	
	DictionaryResult dict_result = dict_write_int8(dictionary, XIVELY_COMMAND_KEY, 0);
	if(dict_result != DICT_OK)
		return dict_result << 12;
	
	AppMessageResult result = app_message_out_send();
	app_message_out_release(); // We don't care if it's already released.
	
	return result;
}

XIVELYResult xively_get_ports()
{
	DictionaryIterator* dictionary;
	AppMessageResult app_result = app_message_out_get(&dictionary);
	if(app_result != APP_MSG_OK)
		return app_result;	
	
	DictionaryResult dict_result = dict_write_int8(dictionary, XIVELY_COMMAND_KEY, XIVELY_CMD_GET_PORTS);
	if(dict_result != DICT_OK)
		return dict_result << 12;
	
	AppMessageResult result = app_message_out_send();
	app_message_out_release();
	
	return result;
}

bool xively_register_callbacks(XIVELYCallbacks callbacks, void* context)
{
	xively_callbacks = callbacks;
	if(callbacks_registered)
	{
		if(app_message_deregister_callbacks(&app_callbacks) == APP_MSG_OK)
			callbacks_registered = false;
	}
	
	if(!callbacks_registered)
	{
		app_callbacks = (AppMessageCallbacksNode)
		{
			.callbacks =
			{
				.out_failed = app_send_failed,
				.in_received = app_received,
				.in_dropped = app_dropped,
			},
			.context = context
		};
		
		if(app_message_register_callbacks(&app_callbacks) == APP_MSG_OK)
			callbacks_registered = true;
	}
	return callbacks_registered;
}

static void app_send_failed(DictionaryIterator* failed, AppMessageResult reason, void* context)
{
	if(!xively_callbacks.sendFailed)
		return;
	
	xively_callbacks.sendFailed(reason, context);
}

static void app_dropped(void* context, AppMessageResult reason)
{
	if(!xively_callbacks.receiveFailed)
		return;
	
	xively_callbacks.receiveFailed(context);
}


static void app_received(DictionaryIterator* received, void* context)
{
	// XBUG vibes_short_pulse();

	// Reconnect message (special: no app id)
	int8_t idxPort = 0;
	for (uint32_t idxCurrentPort = XIVELY_PORT1_DATA_KEY; idxCurrentPort <= XIVELY_PORT5_DATA_KEY; idxCurrentPort += 16, idxPort++)
	{
		Tuple* data_tuple = dict_find(received, idxCurrentPort);
		if (data_tuple)
		{
			Tuple* icon_tuple = dict_find(received, idxCurrentPort - 1);
			if(icon_tuple)
			{
				if(xively_callbacks.success)
					xively_callbacks.success(idxPort, icon_tuple->value->int8, data_tuple->value->cstring, received, context);
			}
		}
	}
}

void xively_set_app_id(int8_t new_app_id)
{
	our_app_id = new_app_id;
}