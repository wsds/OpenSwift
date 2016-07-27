#ifndef _LibSha1_h_
#define _LibSha1_h_

#define SHA1_HASH_SIZE           ( 160 / 8 )
#include "../baselib/DMLinux.h"
char * sha1(void * data, int length);

class Sha1 {
public:
	Sha1();

	static Sha1 *instance;
	static Sha1 * getInstance();

	bool is_initialized;
	bool initialize();

	char * buffer;

	int state[5];
	char result[SHA1_HASH_SIZE];
	char * result_str;

	void clearContext();

	void * encode(void * data, int length);

	char * encodeHex(void * data, int length);

};
#endif //_LibSha1_h_
