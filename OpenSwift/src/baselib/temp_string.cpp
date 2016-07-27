#include "temp_string.h"

int strcmp(char *src, char *dst) {

	int ret = 0;

	while (!(ret = *src - *dst) && *dst)

		++src, ++dst;

	if (ret < 0)

		ret = -1;

	else if (ret > 0)

		ret = 1;

	return (ret);
}

int strcmp(char *src, char *dst, char * keyEnd) {

	if (keyEnd == NULL) {
		return strcmp(src, dst);
	}

	int src_length = strlen(src, keyEnd);
	int dst_length = strlen(dst, keyEnd);

	if (src_length != dst_length) {
		return -2;
	}

	int ret = 0;

	for (int i = 0; i < src_length; i++) {
		if (*src != *dst) {
			ret = -1;
			break;
		}
		src++;
		dst++;
	}

	return (ret);
}

int strlen(char *str) {
	char *eos = str;

	while (*eos++) {
	}

	return (eos - str - 1);
}

int strlen(char *str, char * keyEnd) {
	if (keyEnd == NULL) {
		return strlen(str);
	}

	char *i = str;

	while (*i) {
		char *j = keyEnd;
		while (*j) {
			if (*i == *j) {
				j = NULL;
				break;
			}
			j++;
		}
		if (j == NULL) {
			break;
		}
		i++;
	}

	return (i - str);
}

int strcopy(char *source, char *target) {
	char *eos = source;

	while (*eos) {
		*target = *eos;
		eos++;
		target++;
	}

	*target = '\0';

	return (eos - source - 1);
}

int strcopy(char *source, char *target, int length) {
	*(target + length) = '\0';

	while (length >= 0) {
		length--;
		*(target + length) = *(source + length);
	}

	return length;
}
int strappend(char *target, char *source) {

	int offset = strlen(target);
	target = target + offset;
	return strcopy(source, target);
}

void strclear(char *str) {
	*str = '\0';
}

int parseNubmerToString(int number, char * target) {
	char buf[15] = ""; //need memory optimize
	int len = 0;
	if (number == 0) {
		target[0] = 48;
		target[1] = '\0';
		return len;
	} else if (number < 0) {
		target[0] = 61;
		len = 1;
		number = -number;
	}

	while (number != 0) {
		buf[len++] = number % 10 + NUMBERCHARSTART;
		number /= 10;
	}
	for (int j = len - 1; j >= 0; j--) {
		target[len - j - 1] = buf[j];
	}
	target[len] = '\0';
	return len;
}

int parseNubmerToString(long number, char * target) {
	char buf[32] = ""; //need memory optimize
	int len = 0;
	if (number == 0) {
		target[0] = 48;
		target[1] = '\0';
		return len;
	} else if (number < 0) {
		target[0] = 61;
		len = 1;
		number = -number;
	}

	while (number != 0) {
		buf[len++] = number % 10 + NUMBERCHARSTART;
		number /= 10;
	}
	for (int j = len - 1; j >= 0; j--) {
		target[len - j - 1] = buf[j];
	}
	target[len] = '\0';
	return len;
}

int parseStringToNubmer(char* string, int length) {
	char number_char;
	int result = 0;
	int index = 0;
	bool isNegative = false;
	if (string[index] == CHAR_minus) {
		isNegative = true;
		index++;
	}

	for (index = index + 0; index < length; index++) {
		number_char = string[index];
		if (number_char >= NUMBERCHARSTART && number_char <= NUMBERCHAREND) {
			result = number_char - NUMBERCHARSTART + result * 10;
		} else {
			//parse error
		}
	}

	if (isNegative == true) {
		result = -result;
	}
	return result;
}

bool isNumber(char *string) {
	char *eos = string;

	while (*eos) {
		if (*eos >= NUMBERCHARSTART && NUMBERCHAREND >= *eos) {
		} else {
			return false;
		}
		eos++;
	}

	return true;
}

bool isNumber(char *string, int length) {

	for (int index = 0; index < length; index++) {
		char number_char = string[index];
		if (number_char >= '0' && '9' >= number_char) {
		} else if (number_char == '-' || number_char == '.') {
		} else {
			return false;
		}
	}

	return true;
}

bool isNumberHex(char *string, int length) {
	if(string[0]=='0'&&(string[1]=='x'||string[1]=='X')){
		for (int index = 2; index < length; index++) {
			char number_char = string[index];
			if (number_char >= '0' && '9' >= number_char) {
			} else if (number_char >= 'A' && 'F' >= number_char) {
			} else if (number_char >= 'a' && 'F' >= number_char) {
			} else {
				return false;
			}
		}
		return true;
	}
	return false;
}

bool isJSON(char *string, int length) {
	char first_char = string[0];
	char last_char = string[length - 1];
	if (first_char == '[' && last_char == ']') {
		return true;
	}

	if (first_char == '{' && last_char == '}') {
		return true;
	}
	return false;
}

int numberToString(int number, char * char_string) {

	int len = 0;
	bool isNegative = false;
	if (number >= 0) {
		isNegative = false;
	} else {
		isNegative = true;
		number = -number;
	}

	do {
		buf[len++] = number % radix + NUMBER_CHAR_START;
		number /= 10;
	} while (number != 0);

	if (isNegative == true) {
		buf[len++] = '-';
	}

	if (char_string != NULL) {
		for (int j = len - 1; j >= 0; j--) {
			char_string[len - j - 1] = buf[j];
		}
		char_string[len] = '\0';
	}

	return len;
}

int getLength(char *char_string) {
	char *i = char_string;

	while (*i++) {
	}
	int length = (i - char_string - 1);

	return length;
}

int getLength(char *char_string, char * keyEnd) {
	if (keyEnd == NULL) {
		return getLength(char_string);
	}

	char *i = char_string;

	while (*i) {
		char *j = keyEnd;
		while (*j) {
			if (*i == *j) {
				j = NULL;
				break;
			}
			j++;
		}
		if (j == NULL) {
			break;
		}
		i++;
	}
	int length = (i - char_string);

	return length;
}

bool stringCompare(char *left, char *right) {
	bool isSame = false;
	int diff = 0;

	while (!(diff = *left - *right) && *right) {
		++left, ++right;
	}

	if (diff == 0) {
		isSame = true;
	}

	return isSame;
}

bool stringCompare(char *left, char *right, char * keyEnd) {

	if (keyEnd == NULL) {
		return stringCompare(left, right);
	}

	int left_length = getLength(left, keyEnd);
	int right_length = getLength(right, keyEnd);

	if (left_length != right_length) {
		return false;
	}
	bool isSame = stringCompare(left, right, left_length);

	return isSame;
}
bool stringCompare(void *left, void *right, int length) {
	return stringCompare((char*) left, (char*) right, length);
}
bool stringCompare(char *left, char *right, int length) {

	bool isSame = true;

	for (int i = 0; i < length; i++) {
		if (*left != *right) {
			isSame = false;
			break;
		}
		left++;
		right++;
	}

	return isSame;
}

void reverse_memcpy(void * destination, void * source, int count) {
	if (destination == NULL || source == NULL || count <= 0) {
		return;
	}

	char * destination_char = (char *) destination;
	char * source_char = (char *) source;
	for (int i = count - 1; i >= 0; i--) {
		*(destination_char + i) = *(source_char + i);
	}
}

void obverse_memcpy(void * destination, void * source, int count) {
	memcpy(destination, source, (unsigned int) count);
}

void obverse_memcpy_slow(void * destination, void * source, int count) {
	if (destination == NULL || source == NULL || count <= 0) {
		return;
	}

	char * destination_char = (char *) destination;
	char * source_char = (char *) source;
	for (int i = 0; i < count; i++) {
		*(destination_char + i) = *(source_char + i);
	}
}

