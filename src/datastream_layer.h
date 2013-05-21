#ifndef DATASTREAM_LAYER_H
#define DATASTREAM_LAYER_H

typedef enum {
	DATASTREAM_ICON_NOICON = 0,
	DATASTREAM_ICON_LOADING,
	DATASTREAM_ICON_TEMPERATURE,
	DATASTREAM_ICON_HUMIDITY,
	DATASTREAM_ICON_BBQ,
	DATASTREAM_ICON_MEAN,
} DataStreamIcon;

typedef struct {
	Layer layer;
	BmpContainer icon_layer;
	TextLayer text_layer;
	DataStreamIcon icon;
	char data_str[16];
} DataStreamLayer;


void datastream_layer_init(DataStreamLayer* datastream_layer, GPoint pos);
void datastream_layer_deinit(DataStreamLayer* datastream_layer);
void datastream_layer_set_icon(DataStreamLayer* datastream_layer, DataStreamIcon icon);
void datastream_layer_set_data(DataStreamLayer* datastream_layer, const char* data);

#endif
