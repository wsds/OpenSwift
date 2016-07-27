#include "DMString.h"

DMString * newDMString(int length) {
	DMString * dm_string1 = (DMString *) swift_malloc(length);
	dm_string1->used_length = 0;
	dm_string1->type = TYPE_STRING;
	return dm_string1;
}

void freeDMString(DMString * dm_string) {
	swift_free((MemorySpace *) dm_string);
}

bool compareDMString(DMString * left, DMString * right) {
	return compareMemeorySpace((MemorySpace *) left, (MemorySpace *) right);
}

void DMString::resize(int new_length) {

}

DMString& DMString::operator+(int number) {

	int targetLength = numberToString(number, number_string_buffer);

	if (this->max_length < this->used_length + targetLength) {
		this->resize(this->used_length + targetLength); //synchronous
	}

	reverse_memcpy(this->char_string + this->used_length, number_string_buffer, targetLength);
	this->used_length = this->used_length + targetLength;

	return *this;
}

DMString& DMString::operator*(const char * char_string) {

	if (char_string == NULL) {
		return *this;
	}

	int targetLength = getLength((char *) char_string);

	if (this->max_length < this->used_length + targetLength) {
		this->resize(this->used_length + targetLength); //synchronous
	}

	memcpy(this->char_string + this->used_length, char_string, targetLength);
	this->used_length = this->used_length + targetLength;

	return *this;
}

DMString& DMString::operator*(DMString * dm_string) {

	int targetLength = dm_string->used_length;

	if (this->max_length < this->used_length + targetLength) {
		this->resize(this->used_length + targetLength); //synchronous
	}

	memcpy(this->char_string + this->used_length, dm_string->char_string, targetLength);
	this->used_length = this->used_length + targetLength;

	return *this;
}

DMString& DMString::operator%(int number) {
	int targetLength = numberToString(number, number_string_buffer);
	this->format(number_string_buffer, targetLength);
	return *this;
}

DMString& DMString::operator%(DMString * dm_string) {
	int targetLength = dm_string->used_length;
	if (dm_string->max_length < targetLength) {
		targetLength = dm_string->max_length;
	}
	this->format(dm_string->char_string, targetLength);
	return *this;
}

DMString & DMString::operator%(const char * char_string) {
	int targetLength = getLength((char *) char_string);
	this->format((char *) char_string, targetLength);
	return *this;
}

bool DMString::operator==(const char * char_string) {
	int targetLength = getLength((char *) char_string);
	if (this->max_length != targetLength) {
		return false;
	}

	bool isSame = stringCompare(this->char_string, (char *) char_string, this->max_length);
	return isSame;
}

bool DMString::operator ==(DMString& dm_string) {
	if (this->max_length != dm_string.max_length) {
		return false;
	}

	bool isSame = stringCompare(this->char_string, dm_string.char_string, this->max_length);
	return isSame;
}

void DMString::format(DMString * dm_string) {
	int targetLength = dm_string->used_length;
	if (dm_string->max_length < targetLength) {
		targetLength = dm_string->max_length;
	}
	this->format(dm_string->char_string, targetLength);
}
void DMString::format(char * char_string, int targetLength) {

	if (char_string == NULL) {
		return;
	}

	int first_CHAR_AT = -1;
	int second_CHAR_AT = -1;
	for (int i = 0; i < this->used_length; i++) {
		if (*(this->char_string + i) == CHAR_AT) {
			if (first_CHAR_AT == -1) {
				first_CHAR_AT = i;
			} else {
				second_CHAR_AT = i;
				break;
			}
		}
	}

	if (first_CHAR_AT == -1 || second_CHAR_AT == -1) {
		*this * char_string;
		return;
	}

	int formatLength = second_CHAR_AT - first_CHAR_AT + 1;

	if (this->max_length < this->used_length + targetLength - formatLength) {
		this->resize(this->used_length + targetLength - formatLength); //synchronous
	}

	if (formatLength > targetLength) {
		obverse_memcpy_slow(this->char_string + first_CHAR_AT + targetLength, this->char_string + second_CHAR_AT + 1, this->used_length - second_CHAR_AT - 1);

	} else if (formatLength < targetLength) {
		reverse_memcpy(this->char_string + first_CHAR_AT + targetLength, this->char_string + second_CHAR_AT + 1, this->used_length - second_CHAR_AT - 1);
	}

	memcpy(this->char_string + first_CHAR_AT, char_string, targetLength);
	this->used_length = this->used_length + targetLength - formatLength;

	return;
}

void DMString::copy(char * char_string, int targetLength) {
	if (char_string == NULL) {
		return;
	}

	if (targetLength > this->max_length) {
		return;
	}

	memcpy(this->char_string, char_string, targetLength);
	this->used_length = targetLength;
}

void DMString::copy(const char * char_string) {
	int targetLength = getLength((char *) char_string);
	this->copy((char *) char_string, targetLength);
}

void DMString::set(const char * char_string) {
	if (this->isZero == IS_ZERO) {
		int targetLength = getLength((char *) char_string);
		this->char_string = (char *) char_string;
		this->used_length = targetLength;
		this->max_length = targetLength;
	}
}

void DMString::clear() {
	this->used_length = 0;
}

