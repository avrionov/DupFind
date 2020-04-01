// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
//
//  Copyright © 2020 Nikolay Avrionov
//  This file is part of project: DupFind
//

//MD5.cpp
#include "stdafx.h"


#include <stdio.h>
#include <time.h>
#include <string.h>
#include "md5.h"

extern LONG gStopFlag;

#define MD 5

/* Constants for MD5Transform routine.
*/

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21


static void __fastcall MD5Transform (UINT4 [4], unsigned char [64]);
static void Encode (unsigned char *, UINT4 *, unsigned int);
static void Decode (UINT4 *, unsigned char *, unsigned int);
//static void MD5_memcpy (POINTER, POINTER, unsigned int);
//static void MD5_memset (POINTER, int, unsigned int);


#define MD5_memcpy memcpy
#define MD5_memset memset

static unsigned char PADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
*/
//#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define F(x, y, z) ((((z) ^ (y)) & (x)) ^ (z))

//#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define G(x, y, z) ((((x) ^ (y)) & (z)) ^ (y))

#define H(x, y, z) ((x) ^ (y) ^ (z))
//#define H(x, y, z) ((x) ^ (y) ^ (z))

#define I(x, y, z) ((y) ^ ((x) | (~z)))


/* ROTATE_LEFT rotates x left n bits.
*/
//#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

// but our compiler has an intrinsic!

#define ROTATE_LEFT(x,n) (x) = _rotl(x,n)

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
*/
#define FF(a, b, c, d, x, s, ac) { \
	(a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
	(a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
	(a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
	(a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}

/* MD5 initialization. Begins an MD5 operation, writing a new context.
*/
void MD5Init (MD5_CTX *context)
/* context */
{
	context->count[0] = context->count[1] = 0;
	/* Load magic initialization constants.
	*/
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}

/* MD5 block update operation. Continues an MD5 message-digest
operation, processing another message block, and updating the
context.
*/
void MD5Update (
				MD5_CTX *context,                                        /* context */
				unsigned char *input,                                /* input block */
				unsigned int inputLen)                     /* length of input block */
{
	unsigned int i, index, partLen;

	/* Compute number of bytes mod 64 */
	index = (unsigned int)((context->count[0] >> 3) & 0x3F);

	/* Update number of bits */
	if ((context->count[0] += ((UINT4)inputLen << 3)) < ((UINT4)inputLen << 3))
		context->count[1]++;

	context->count[1] += ((UINT4)inputLen >> 29);

	partLen = 64 - index;

	/* Transform as many times as possible.
	*/
	if (inputLen >= partLen) {
		MD5_memcpy ((POINTER)&context->buffer[index], (POINTER)input, partLen);
		MD5Transform (context->state, context->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64)
		{
			if (i % (128*128*4)== 0)
			 if (InterlockedCompareExchange (&gStopFlag, 1,1) ==1)
				return;
			MD5Transform (context->state, &input[i]);
		}

		index = 0;
	}
	else
		i = 0;

	/* Buffer remaining input */
	MD5_memcpy ((POINTER)&context->buffer[index], (POINTER)&input[i], inputLen-i);
}

/* MD5 finalization. Ends an MD5 message-digest operation, writing the
the message digest and zeroizing the context.
*/
void MD5Final (
			   unsigned char digest[16],                         /* message digest */
			   MD5_CTX *context)                                       /* context */
{
	unsigned char bits[8];
	unsigned int index, padLen;

	/* Save number of bits */
	Encode (bits, context->count, 8);

	/* Pad out to 56 mod 64.
	*/
	index = (unsigned int)((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	MD5Update (context, PADDING, padLen);

	/* Append length (before padding) */
	MD5Update (context, bits, 8);

	/* Store state in digest */
	Encode (digest, context->state, 16);

	/* Zeroize sensitive information.
	*/
	MD5_memset ((POINTER)context, 0, sizeof (*context));
}

/* MD5 basic transformation. Transforms state based on block.
*/
static void __fastcall MD5Transform (UINT4 state[4], unsigned char block[64])
{
	UINT4 a = state[0], b = state[1], c = state[2], d = state[3], *x;

//	Decode (x, block, 64);
	x = (UINT4*)block;

	/* Round 1 */
	FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
	GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	/* Zeroize sensitive information.
	*/
	//MD5_memset ((POINTER)x, 0, sizeof (x));
}

/* Encodes input (UINT4) into output (unsigned char). Assumes len is
a multiple of 4.
*/
static void Encode (
					unsigned char *output,
					UINT4 *input,
					unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (unsigned char)(input[i] & 0xff);
		output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);
		output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
		output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
	}
}

/* Decodes input (unsigned char) into output (UINT4). Assumes len is
a multiple of 4.
*/
static void Decode (UINT4 *output, unsigned char *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((UINT4)input[j]) | (((UINT4)input[j+1]) << 8) |
		(((UINT4)input[j+2]) << 16) | (((UINT4)input[j+3]) << 24);
}

/* Note: Replace "for loop" with standard memcpy if possible.
*/

static void MD5_memcpy (
						POINTER output,
						POINTER input,
						unsigned int len)
{
	unsigned int i;

	for (i = 0; i < len; i++)
		output[i] = input[i];
}

/* Note: Replace "for loop" with standard memset if possible.
*/

static void MD5_memset (
						POINTER output,
						int value,
						unsigned int len)
{
	unsigned int i;

	for (i = 0; i < len; i++)
		((char *)output)[i] = (char)value;
}

/* Length of test block, number of test blocks.
*/
#define TEST_BLOCK_LEN 1000
#define TEST_BLOCK_COUNT 1000

static void MDString (char *);
static void MDTimeTrial (void);
static void MDTestSuite (void);
static void MDFile (char *);
static void MDFilter (void);
static void MDPrint (unsigned char [16]);

#define MD5_CTX MD5_CTX
#define MDInit MD5Init
#define MDUpdate MD5Update
#define MDFinal MD5Final


static void MDString (char *string)

{
	MD5_CTX context;
	unsigned char digest[16];
	unsigned int len = strlen (string);

	MDInit (&context);
	MDUpdate (&context, (unsigned char*)string, len);
	MDFinal (digest, &context);

	printf ("MD%d (\"%s\") = ", MD, string);
	MDPrint (digest);
	printf ("\n");
}

/* Measures the time to digest TEST_BLOCK_COUNT TEST_BLOCK_LEN-byte
blocks.
*/
static void MDTimeTrial ()
{
	MD5_CTX context;
	time_t endTime, startTime;
	unsigned char block[TEST_BLOCK_LEN], digest[16];
	unsigned int i;


	printf
		("MD%d time trial. Digesting %d %d-byte blocks ...", MD,
		TEST_BLOCK_LEN, TEST_BLOCK_COUNT);

	/* Initialize block */
	for (i = 0; i < TEST_BLOCK_LEN; i++)
		block[i] = (unsigned char)(i & 0xff);

	/* Start timer */
	time (&startTime);

	/* Digest blocks */
	MDInit (&context);
	for (i = 0; i < TEST_BLOCK_COUNT; i++)
		MDUpdate (&context, block, TEST_BLOCK_LEN);
	MDFinal (digest, &context);

	/* Stop timer */
	time (&endTime);

	printf (" done\n");
	printf ("Digest = ");
	MDPrint (digest);
	printf ("\nTime = %ld seconds\n", (long)(endTime-startTime));
	printf
		("Speed = %ld bytes/second\n",
		(long)TEST_BLOCK_LEN * (long)TEST_BLOCK_COUNT/(endTime-startTime));
}

/* Digests a reference suite of strings and prints the results.
*/
static void MDTestSuite ()
{
	printf ("MD%d test suite:\n", MD);

	MDString ("");
	MDString ("a");
	MDString ("abc");
	MDString ("message digest");
	MDString ("abcdefghijklmnopqrstuvwxyz");
	MDString
		("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	MDString
		("1234567890123456789012345678901234567890\
		 1234567890123456789012345678901234567890");
}

/* Digests a file and prints the result.
*/

bool MDFileMM (const TCHAR *filename, unsigned char *digest, DWORD sampleSize)
{
	HANDLE fd = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == fd)
		false;
		
	DWORD fileSize =  ::GetFileSize (fd, NULL);;

	DWORD size;

	if (sampleSize == 0)
		size = fileSize;
	else 
		size = min (sampleSize, fileSize);

	HANDLE hMap = CreateFileMapping (fd, NULL, PAGE_READONLY, 0, 0, NULL); 

	if (hMap == NULL) 
	{
		CloseHandle (fd);		
		false;
	}

	BYTE* pByte = (BYTE *) MapViewOfFile (hMap, FILE_MAP_READ, 0, 0, 0);

	if (NULL == pByte)
	{
		CloseHandle (hMap);
		CloseHandle(fd);
		return false;
	}

	MD5_CTX context;

	MDInit (&context);
	//while (len = fread (buffer, 1, 1024, file))
	MDUpdate (&context, pByte, size);
	MDFinal (digest, &context);

	UnmapViewOfFile(pByte);
	CloseHandle(hMap);
	CloseHandle(fd);
	return true;
}

void MDFile (const char *filename, unsigned char *digest)
{
	FILE *file;
	MD5_CTX context;
	int len;
	unsigned char buffer[1024];

	if ((file = fopen (filename, "rb")) == NULL)
		printf ("%s can't be opened\n", filename);

	else {
		MDInit (&context);
		while (len = fread (buffer, 1, 1024, file))
			MDUpdate (&context, buffer, len);
		MDFinal (digest, &context);

		fclose (file);

		/*printf ("MD%d (%s) = ", MD, filename);
		MDPrint (digest);
		printf ("\n");*/
	}
}

/* Digests the standard input and prints the result.
*/
static void MDFilter ()
{
	MD5_CTX context;
	int len;
	unsigned char buffer[16], digest[16];

	MDInit (&context);
	while (len = fread (buffer, 1, 16, stdin))
		MDUpdate (&context, buffer, len);
	MDFinal (digest, &context);

	MDPrint (digest);
	printf ("\n");
}

/* Prints a message digest in hexadecimal.
*/
static void MDPrint (unsigned char digest[16])
{

	unsigned int i;

	for (i = 0; i < 16; i++)
		printf ("%02x", digest[i]);
}

/*
static void
md5_asm3(DWORD *state, DWORD *src)
{
	struct {
		void *state;
		void *src;
		void *ebp;
		void *esp;
		void *ebx;
	}tmp;

	tmp.state = state;
	tmp.src = src;

	__asm {
		 mov ebp, [eax+0x8];
		 mov esp, [eax+0xc];
		 mov ebx, [eax+0x10];
		 mov eax, ebp;
		 mov [ebp], esi;
		 mov [esi], eax;
		 mov [esi+0x4], ebx;
		 mov [esi+0x8], ecx;
		 mov [esi+0xc], edx;
		 mov [ebp+0x4], esi;

#define STEP1(r1,r2,r3,r4,offset,constant,rotate) \
	"  mov %%e" #r2 "x, %%edi\n" \
	"   mov %%e" #r2 "x, %%esp\n" \
	"   not %%esp\n" \
	"  and %%e" #r3 "x, %%edi\n" \
	"   and %%e" #r4 "x, %%esp\n" \
	"  or %%esp, %%edi\n" \
	"  add %%edi, %%e" #r1 "x\n" \
	"  mov (" #offset "* 4)(%%esi), %%edi\n" \
	"  add $" #constant ", %%edi\n" \
	"  add %%edi, %%e" #r1 "x\n" \
	"  rol $" #rotate ", %%e" #r1 "x\n" \
	"  add %%e" #r2 "x, %%e" #r1 "x\n"

		STEP1 (a, b, c, d, 0, 0xd76aa478, 7)
		STEP1 (d, a, b, c, 1, 0xe8c7b756, 12)
		STEP1 (c, d, a, b, 2, 0x242070db, 17)
		STEP1 (b, c, d, a, 3, 0xc1bdceee, 22)
		STEP1 (a, b, c, d, 4, 0xf57c0faf, 7)
		STEP1 (d, a, b, c, 5, 0x4787c62a, 12)
		STEP1 (c, d, a, b, 6, 0xa8304613, 17)
		STEP1 (b, c, d, a, 7, 0xfd469501, 22)
		STEP1 (a, b, c, d, 8, 0x698098d8, 7)
		STEP1 (d, a, b, c, 9, 0x8b44f7af, 12)
		STEP1 (c, d, a, b, 10, 0xffff5bb1, 17)
		STEP1 (b, c, d, a, 11, 0x895cd7be, 22)
		STEP1 (a, b, c, d, 12, 0x6b901122, 7)
		STEP1 (d, a, b, c, 13, 0xfd987193, 12)
		STEP1 (c, d, a, b, 14, 0xa679438e, 17)
		STEP1 (b, c, d, a, 15, 0x49b40821, 22)

#define STEP2(r1,r2,r3,r4,offset,constant,rotate) \
	"  mov %%e" #r4 "x, %%edi\n" \
	"   mov %%e" #r4 "x, %%esp\n" \
	"   not %%esp\n" \
	"  and %%e" #r2 "x, %%edi\n" \
	"   and %%e" #r3 "x, %%esp\n" \
	"  or %%esp, %%edi\n" \
	"  add %%edi, %%e" #r1 "x\n" \
	"  mov (" #offset "* 4)(%%esi), %%edi\n" \
	"  add $" #constant ", %%edi\n" \
	"  add %%edi, %%e" #r1 "x\n" \
	"  rol $" #rotate ", %%e" #r1 "x\n" \
	"  add %%e" #r2 "x, %%e" #r1 "x\n"

		STEP2(a, b, c, d, 1, 0xf61e2562, 5)
		STEP2(d, a, b, c, 6, 0xc040b340, 9)
		STEP2(c, d, a, b, 11, 0x265e5a51, 14)
		STEP2(b, c, d, a, 0, 0xe9b6c7aa, 20)
		STEP2(a, b, c, d, 5, 0xd62f105d, 5)
		STEP2(d, a, b, c, 10, 0x02441453, 9)
		STEP2(c, d, a, b, 15, 0xd8a1e681, 14)
		STEP2(b, c, d, a, 4, 0xe7d3fbc8, 20)
		STEP2(a, b, c, d, 9, 0x21e1cde6, 5)
		STEP2(d, a, b, c, 14, 0xc33707d6, 9)
		STEP2(c, d, a, b, 3, 0xf4d50d87, 14)
		STEP2(b, c, d, a, 8, 0x455a14ed, 20)
		STEP2(a, b, c, d, 13, 0xa9e3e905, 5)
		STEP2(d, a, b, c, 2, 0xfcefa3f8, 9)
		STEP2(c, d, a, b, 7, 0x676f02d9, 14)
		STEP2(b, c, d, a, 12, 0x8d2a4c8a, 20)

#define STEP3(r1,r2,r3,r4,offset,constant,rotate) \
	"  mov %%e" #r4 "x, %%edi\n" \
	"  xor %%e" #r3 "x, %%edi\n" \
	"  xor %%e" #r2 "x, %%edi\n" \
	"  add %%edi, %%e" #r1 "x\n" \
	"  mov (" #offset "* 4)(%%esi), %%edi\n" \
	"  add $" #constant ", %%edi\n" \
	"  add %%edi, %%e" #r1 "x\n" \
	"  rol $" #rotate ", %%e" #r1 "x\n" \
	"  add %%e" #r2 "x, %%e" #r1 "x\n"

		STEP3 (a, b, c, d, 5, 0xfffa3942, 4)
		STEP3 (d, a, b, c, 8, 0x8771f681, 11)
		STEP3 (c, d, a, b, 11, 0x6d9d6122, 16)
		STEP3 (b, c, d, a, 14, 0xfde5380c, 23)
		STEP3 (a, b, c, d, 1, 0xa4beea44, 4)
		STEP3 (d, a, b, c, 4, 0x4bdecfa9, 11)
		STEP3 (c, d, a, b, 7, 0xf6bb4b60, 16)
		STEP3 (b, c, d, a, 10, 0xbebfbc70, 23)
		STEP3 (a, b, c, d, 13, 0x289b7ec6, 4)
		STEP3 (d, a, b, c, 0, 0xeaa127fa, 11)
		STEP3 (c, d, a, b, 3, 0xd4ef3085, 16)
		STEP3 (b, c, d, a, 6, 0x04881d05, 23)
		STEP3 (a, b, c, d, 9, 0xd9d4d039, 4)
		STEP3 (d, a, b, c, 12, 0xe6db99e5, 11)
		STEP3 (c, d, a, b, 15, 0x1fa27cf8, 16)
		STEP3 (b, c, d, a, 2, 0xc4ac5665, 23)

#define STEP4(r1,r2,r3,r4,offset,constant,rotate) \
	"  mov %%e" #r4 "x, %%edi\n" \
	"  not %%edi\n" \
	"  or %%e" #r2 "x, %%edi\n" \
	"  xor %%e" #r3 "x, %%edi\n" \
	"  add %%edi, %%e" #r1 "x\n" \
	"  mov (" #offset "* 4)(%%esi), %%edi\n" \
	"  add $" #constant ", %%edi\n" \
	"  add %%edi, %%e" #r1 "x\n" \
	"  rol $" #rotate ", %%e" #r1 "x\n" \
	"  add %%e" #r2 "x, %%e" #r1 "x\n"

		STEP4 (a, b, c, d, 0, 0xf4292244, 6)
		STEP4 (d, a, b, c, 7, 0x432aff97, 10)
		STEP4 (c, d, a, b, 14, 0xab9423a7, 15)
		STEP4 (b, c, d, a, 5, 0xfc93a039, 21)
		STEP4 (a, b, c, d, 12, 0x655b59c3, 6)
		STEP4 (d, a, b, c, 3, 0x8f0ccc92, 10)
		STEP4 (c, d, a, b, 10, 0xffeff47d, 15)
		STEP4 (b, c, d, a, 1, 0x85845dd1, 21)
		STEP4 (a, b, c, d, 8, 0x6fa87e4f, 6)
		STEP4 (d, a, b, c, 15, 0xfe2ce6e0, 10)
		STEP4 (c, d, a, b, 6, 0xa3014314, 15)
		STEP4 (b, c, d, a, 13, 0x4e0811a1, 21)
		STEP4 (a, b, c, d, 4, 0xf7537e82, 6)
		STEP4 (d, a, b, c, 11, 0xbd3af235, 10)
		STEP4 (c, d, a, b, 2, 0x2ad7d2bb, 15)
		STEP4 (b, c, d, a, 9, 0xeb86d391, 21)

		"  mov (%%ebp), %%edi\n"
		"  add %%eax, 0x0(%%edi)\n"
		"  add %%ebx, 0x4(%%edi)\n"
		"  add %%ecx, 0x8(%%edi)\n"
		"  add %%edx, 0xc(%%edi)\n"
		"  mov 0x10(%%ebp), %%ebx\n"
		"  mov 0xc(%%ebp), %%esp\n"
		"  mov 0x8(%%ebp), %%ebp\n"
		:
	: "a" (&tmp)
		: "esi", "ecx", "edx", "edi");
#undef STEP1
#undef STEP2
#undef STEP3
#undef STEP4
}*/