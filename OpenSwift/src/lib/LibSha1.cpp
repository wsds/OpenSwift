///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  LibSha1
//  Implementation of SHA1 hash function.
//  Original author:  wsds <wsds888@163.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LibSha1.h"

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

// blk0() and blk() perform the initial expand.
#define blk0(i) (block[i] = (rol(block[i],24)&0xFF00FF00) \
    |(rol(block[i],8)&0x00FF00FF))

#define blk(i) (block[i&15] = rol(block[(i+13)&15]^block[(i+8)&15] \
    ^block[(i+2)&15]^block[i&15],1))

// (R0+R1), R2, R3, R4 are the different operations used in SHA1
#define R0(v,w,x,y,z,i)  z += ((w&(x^y))^y)     + blk0(i)+ 0x5A827999 + rol(v,5); w=rol(w,30);
#define R1(v,w,x,y,z,i)  z += ((w&(x^y))^y)     + blk(i) + 0x5A827999 + rol(v,5); w=rol(w,30);
#define R2(v,w,x,y,z,i)  z += (w^x^y)           + blk(i) + 0x6ED9EBA1 + rol(v,5); w=rol(w,30);
#define R3(v,w,x,y,z,i)  z += (((w|x)&y)|(w&x)) + blk(i) + 0x8F1BBCDC + rol(v,5); w=rol(w,30);
#define R4(v,w,x,y,z,i)  z += (w^x^y)           + blk(i) + 0xCA62C1D6 + rol(v,5); w=rol(w,30);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TransformFunction
//
//  Hash a single 512-bit block. This is the core of the algorithm
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
void TransformFunction(int state[5], const char buffer[64]) {
	unsigned int a;
	unsigned int b;
	unsigned int c;
	unsigned int d;
	unsigned int e;
	unsigned char workspace[64];
	unsigned int * block = (unsigned int *) workspace;

	memcpy(block, buffer, 64);

	// Copy context->state[] to working vars
	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];

	// 4 rounds of 20 operations each. Loop unrolled.
	R0(a, b, c, d, e, 0);
	R0(e, a, b, c, d, 1);
	R0(d, e, a, b, c, 2);
	R0(c, d, e, a, b, 3);
	R0(b, c, d, e, a, 4);
	R0(a, b, c, d, e, 5);
	R0(e, a, b, c, d, 6);
	R0(d, e, a, b, c, 7);
	R0(c, d, e, a, b, 8);
	R0(b, c, d, e, a, 9);
	R0(a, b, c, d, e, 10);
	R0(e, a, b, c, d, 11);
	R0(d, e, a, b, c, 12);
	R0(c, d, e, a, b, 13);
	R0(b, c, d, e, a, 14);
	R0(a, b, c, d, e, 15);
	R1(e, a, b, c, d, 16);
	R1(d, e, a, b, c, 17);
	R1(c, d, e, a, b, 18);
	R1(b, c, d, e, a, 19);
	R2(a, b, c, d, e, 20);
	R2(e, a, b, c, d, 21);
	R2(d, e, a, b, c, 22);
	R2(c, d, e, a, b, 23);
	R2(b, c, d, e, a, 24);
	R2(a, b, c, d, e, 25);
	R2(e, a, b, c, d, 26);
	R2(d, e, a, b, c, 27);
	R2(c, d, e, a, b, 28);
	R2(b, c, d, e, a, 29);
	R2(a, b, c, d, e, 30);
	R2(e, a, b, c, d, 31);
	R2(d, e, a, b, c, 32);
	R2(c, d, e, a, b, 33);
	R2(b, c, d, e, a, 34);
	R2(a, b, c, d, e, 35);
	R2(e, a, b, c, d, 36);
	R2(d, e, a, b, c, 37);
	R2(c, d, e, a, b, 38);
	R2(b, c, d, e, a, 39);
	R3(a, b, c, d, e, 40);
	R3(e, a, b, c, d, 41);
	R3(d, e, a, b, c, 42);
	R3(c, d, e, a, b, 43);
	R3(b, c, d, e, a, 44);
	R3(a, b, c, d, e, 45);
	R3(e, a, b, c, d, 46);
	R3(d, e, a, b, c, 47);
	R3(c, d, e, a, b, 48);
	R3(b, c, d, e, a, 49);
	R3(a, b, c, d, e, 50);
	R3(e, a, b, c, d, 51);
	R3(d, e, a, b, c, 52);
	R3(c, d, e, a, b, 53);
	R3(b, c, d, e, a, 54);
	R3(a, b, c, d, e, 55);
	R3(e, a, b, c, d, 56);
	R3(d, e, a, b, c, 57);
	R3(c, d, e, a, b, 58);
	R3(b, c, d, e, a, 59);
	R4(a, b, c, d, e, 60);
	R4(e, a, b, c, d, 61);
	R4(d, e, a, b, c, 62);
	R4(c, d, e, a, b, 63);
	R4(b, c, d, e, a, 64);
	R4(a, b, c, d, e, 65);
	R4(e, a, b, c, d, 66);
	R4(d, e, a, b, c, 67);
	R4(c, d, e, a, b, 68);
	R4(b, c, d, e, a, 69);
	R4(a, b, c, d, e, 70);
	R4(e, a, b, c, d, 71);
	R4(d, e, a, b, c, 72);
	R4(c, d, e, a, b, 73);
	R4(b, c, d, e, a, 74);
	R4(a, b, c, d, e, 75);
	R4(e, a, b, c, d, 76);
	R4(d, e, a, b, c, 77);
	R4(c, d, e, a, b, 78);
	R4(b, c, d, e, a, 79);

	// Add the working vars back into context.state[]
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
}

Sha1::Sha1() {
	this->is_initialized = false;
}

Sha1 * Sha1::instance = NULL;
Sha1 * Sha1::getInstance() {
	if (instance == NULL) {
		instance = new Sha1();
	}
	return instance;
}

bool Sha1::initialize() {
	if (this->is_initialized == true) {
		return true;
	}
	this->is_initialized = true;

	this->buffer = (char*) JSMalloc(64);
	this->result_str = (char *) JSMalloc(40);

	this->clearContext();

	return true;
}

void Sha1::clearContext() {
	this->state[0] = 0x67452301;
	this->state[1] = 0xEFCDAB89;
	this->state[2] = 0x98BADCFE;
	this->state[3] = 0x10325476;
	this->state[4] = 0xC3D2E1F0;
}

void * Sha1::encode(void * data, int length) {

	char * dataStr = (char *) data;

	this->clearContext();

	int partCount = length / 64;

	for (int p = 0; p < partCount; p++) {
		memcpy(this->buffer, dataStr + 64 * p, 64);
		TransformFunction(this->state, this->buffer);

	}

	int tail_length = length % 64;
	if (tail_length >= 0) {
		memcpy(this->buffer, dataStr + 64 * partCount, tail_length);
		int complement_length = 56 - tail_length;

		*(this->buffer + tail_length) = 0x80;
		if (complement_length > 1) {
			memset(this->buffer + tail_length + 1, 0, complement_length - 1);

		} else {
			memset(this->buffer + tail_length + 1, 0, 64 - tail_length - 1);
			TransformFunction(this->state, this->buffer);

			memset(this->buffer, 0, 64);
		}

		long long length64 = (long long) (length * 8);

		for (int i = 0; i < 8; i++) {
			*(this->buffer + 56 + i) = length64 >> ((7 - i) * 8);
		}

		TransformFunction(this->state, this->buffer);
	}

	for (int i = 0; i < SHA1_HASH_SIZE; i++) {
		this->result[i] = (char) ((this->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
	}

	return this->result;
}

void byteToHexString(char ib, char * str, int index) {
	char Digit[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	str[index] = Digit[((ib >> 4) & 0X0F)];
	str[index + 1] = Digit[ib & 0X0F];
}

char * Sha1::encodeHex(void * data, int length) {
	this->encode(data, length);

	for (int i = 0; i < SHA1_HASH_SIZE; i++) {
		byteToHexString(this->result[i], this->result_str, i * 2);
	}
	return this->result_str;
}

char * sha1(void * data, int length) {
	Sha1 * sha1 = Sha1::getInstance();
	sha1->initialize();
	char* result_str = (char*) sha1->encodeHex(data, length);
	return result_str;
}
