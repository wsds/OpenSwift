#include "DMLog.h"

void DMLog(MemorySpace * object) {
	if (object == NULL) {
		DMLog("NULL", 4);
		return;
	}
	if (object->type == TYPE_NUMBER) {
		DMLog((DMInt32 *) object);
	} else if (object->type == TYPE_STRING) {
		DMLog((DMString *) object);
	} else if (object->type == TYPE_JSON) {
		DMJSON * dm_json = (DMJSON*) object->pointer;
		DMLog(dm_json);
	}
}

#define COLOR_Black 0
#define COLOR_Red 1
#define COLOR_Green 2
#define COLOR_Yellow 3
#define COLOR_Blue 4
#define COLOR_Magenta 5
#define COLOR_Cyan 6
#define COLOR_White 7

int current_color = COLOR_White;
bool show_color = false;

void setColor(int color) {
	if (show_color == false || current_color == color) {
		return;
	}
	current_color = color;
	char * color_script = NULL;
	if (color == COLOR_Black) {
		color_script = "\033[1;30m";
	} else if (color == COLOR_Red) {
		color_script = "\033[1;31m";
	} else if (color == COLOR_Green) {
		color_script = "\033[1;32m";
	} else if (color == COLOR_Yellow) {
		color_script = "\033[1;33m";
	} else if (color == COLOR_Blue) {
		color_script = "\033[1;34m";
	} else if (color == COLOR_Magenta) {
		color_script = "\033[1;35m";
	} else if (color == COLOR_Cyan) {
		color_script = "\033[1;36m";
	} else if (color == COLOR_White) {
		color_script = "\033[1;37m";
	}

	DMLog(color_script, getLength(color_script));
}

void DMLog(DMString * dm_string) {
	setColor(COLOR_Blue);
	DMLog(dm_string->char_string, dm_string->used_length);
}
void DMLog(DMJSON * dm_json) {
	DMString * dm_string = stringifyJSON2DMString(dm_json);
	setColor(COLOR_White);
	DMLog(dm_string->char_string, dm_string->used_length);
}

void DMLog(DMInt32 * dm_int) {
	int targetLength = numberToString(dm_int->number, number_string_buffer);
	setColor(COLOR_Yellow);
	DMLog(number_string_buffer, targetLength);
}

void DMLog(const char * message) {
	DMLog((char*) message, getLength((char*) message));
}

char dm_log_buffer[1024];
void DMLog(char * message, int length) {
	memcpy(dm_log_buffer, message, length);
	dm_log_buffer[length] = '\n';
	dm_log_buffer[length + 1] = '\0';
	std::cout << dm_log_buffer;
}

void clearCurrentColor() {
	current_color = -1;
}
