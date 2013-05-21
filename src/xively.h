#ifndef XIVELX_H
#define XIVELY_H

#define XIVELY_UUID { 0x11, 0x91, 0xB6, 0x2B, 0xBA, 0x19, 0x99, 0x8F, 0xC1, 0x37, 0x14, 0x8F, 0x29, 0xA1, 0x44, 0xAC }

// Shared values.
typedef enum {
	XIVELY_OK 							= 0,
	XIVELY_SEND_TIMEOUT 				= APP_MSG_SEND_TIMEOUT,
	XIVELY_SEND_REJECTED				= APP_MSG_SEND_REJECTED,
	XIVELY_NOT_CONNECTED				= APP_MSG_NOT_CONNECTED,
	XIVELY_BRIDGE_NOT_RUNNING			= APP_MSG_APP_NOT_RUNNING,
	XIVELY_INVALID_ARGS					= APP_MSG_INVALID_ARGS,
	XIVELY_BUSY							= APP_MSG_BUSY,
	XIVELY_BUFFER_OVERFLOW				= APP_MSG_BUFFER_OVERFLOW,
	XIVELY_ALREADY_RELEASED				= APP_MSG_ALREADY_RELEASED,
	XIVELY_CALLBACK_ALREADY_REGISTERED	= APP_MSG_CALLBACK_ALREADY_REGISTERED,
	XIVELY_CALLBACK_NOT_REGISTERED		= APP_MSG_CALLBACK_NOT_REGISTERED,
	XIVELY_NOT_ENOUGH_STORAGE			= DICT_NOT_ENOUGH_STORAGE << 12,
	XIVELY_INVALID_DICT_ARGS			= DICT_INVALID_ARGS << 12,
	XIVELY_INTERNAL_INCONSISTENCY		= DICT_INTERNAL_INCONSISTENCY << 12,
	XIVELY_INVALID_BRIDGE_RESPONSE		= 1 << 17
} XIVELYResult;

// XIVELY Request callbacks
typedef void(*XIVELYSendFailedHandler)(AppMessageResult reason, void* context);
typedef void(*XIVELYReceiveFailedHandler)(void* context);
typedef void(*XIVELYSucceededHandler)(int8_t port, int8_t icon, const char* data, DictionaryIterator* sent, void* context);

// XIVELY stuff
typedef struct
{
	XIVELYSendFailedHandler sendFailed;
	XIVELYReceiveFailedHandler receiveFailed;
	XIVELYSucceededHandler success;
} XIVELYCallbacks;

// XIVELY requests
XIVELYResult xively_request_port();
bool xively_register_callbacks(XIVELYCallbacks callbacks, void* context);

// Local cookies
// Basic API
void xively_set_app_id(int8_t id);


#endif