#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "datastream_layer.h"

static uint8_t DATASTREAM_ICONS[] =
{
	RESOURCE_ID_ICON_LOADING,
	RESOURCE_ID_ICON_LOADING,
	RESOURCE_ID_ICON_TEMPERATURE,
	RESOURCE_ID_ICON_HUMIDITY,
	RESOURCE_ID_ICON_BBQ,
	RESOURCE_ID_ICON_MEAT
};

void datastream_layer_init(DataStreamLayer* datastream_layer, GPoint pos)
{
	layer_init(&datastream_layer->layer, GRect(pos.x, pos.y, 144, 33));
	
	text_layer_init(&datastream_layer->text_layer, GRect(34, -5, 110, 33));
	text_layer_set_text_alignment(&datastream_layer->text_layer, GTextAlignmentCenter);
	text_layer_set_font(&datastream_layer->text_layer, fonts_get_system_font (FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_background_color(&datastream_layer->text_layer, GColorBlack);
	text_layer_set_text_color(&datastream_layer->text_layer, GColorWhite);
	layer_add_child(&datastream_layer->layer, &datastream_layer->text_layer.layer);
	
	datastream_layer->icon = DATASTREAM_ICON_NOICON;
}

void datastream_layer_set_icon(DataStreamLayer* datastream_layer, DataStreamIcon icon)
{
	if (datastream_layer->icon != icon)
	{
		if(datastream_layer->icon != DATASTREAM_ICON_NOICON)
		{
			layer_remove_from_parent(&datastream_layer->icon_layer.layer.layer);
			bmp_deinit_container(&datastream_layer->icon_layer);
		}
	
		datastream_layer->icon = icon;
		bmp_init_container(DATASTREAM_ICONS[icon], &datastream_layer->icon_layer);
		layer_add_child(&datastream_layer->layer, &datastream_layer->icon_layer.layer.layer);
		layer_set_frame(&datastream_layer->icon_layer.layer.layer, GRect(0, 0, 32, 32));	
	}
}

void datastream_layer_set_data(DataStreamLayer* datastream_layer, const char* data)
{
	int count = strlen(data);
	if (count >= 16)
		count = 15;
	
	memcpy(datastream_layer->data_str, data, count);
	datastream_layer->data_str[count] = '\0';
	text_layer_set_text(&datastream_layer->text_layer, datastream_layer->data_str);
}

void datastream_layer_deinit(DataStreamLayer* datastream_layer)
{
	if(datastream_layer->icon != DATASTREAM_ICON_NOICON)
		bmp_deinit_container(&datastream_layer->icon_layer);
}
