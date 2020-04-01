
//MD5.h

#ifndef __MD5_H__
#define __MD5_H__

#include "MessageDigest.h"

//Typical DISCLAIMER:
//The code in this project is Copyright (C) 2003 by George Anescu. You have the right to
//use and distribute the code in any way you see fit as long as this paragraph is included
//with the distribution. No warranties or claims are made as to the validity of the
//information and code contained herein, so use it at your own risk.

//MD5 Message Digest algorithm 
//When you send data over a network, there are three issues most organizations have, security, authenticity,
//and integrity. The security of your data ensures that no one can read your data. This is important for the
//military, where secrets have to be kept from enemy hands. Authenticity guarantees the originator of the
//data, you know for certain who sent the data. This is important for the legal world, such as digital signatures.
//Integrity guarantees that the data has not been altered in transit, that the data you received is the data that
//was sent. This is important for many industries, such as the financial world. MD5 is such a tool, it guarantees
//the integrity of your data. 
//Developed in 1994, MD5 is a one-way hash algorithm that takes any length of data and produces a 128 bit "fingerprint"
//or "message digest". This fingerprint is  "non-reversible", it is computationally infeasible to determine the file
//based on the fingerprint. This means someone cannot figure out your data based on its MD5 fingerprint.
//When you download a new file or patch, one of the first things you can do is a MD5 hash of the file. Compare the
//fingerprint to a known good fingerpint (usually posted on remote site). If the fingerprints match, you can be assured
//of the file?s integrity. This is how the tool Tripwire works. It builds a database of fingerprints for all your binaries,
//then later on compares the binaries to that database.
//Since MD5 does not encrypt data, it is not restricted by any exportation rules. You can freely use and distribute this
//tool anywhere in the world.
//
//MD5 TEST VALUES:
//1)""
//"d41d8cd98f00b204e9800998ecf8427e"
//
//2)"a"
//"0cc175b9c0f1b6a831c399e269772661"
//
//3)"abc"
//"900150983cd24fb0d6963f7d28e17f72"
//
//4)"message digest"
//"f96b697d7cb7938d525a2f31aaf161d0"
//
//5)"abcdefghijklmnopqrstuvwxyz"
//"c3fcd3d76192e4007dfb496cca67e13b"
//
//6)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
//"d174ab98d277d9f5a5611c2c9f419d9f"
//
//7)"12345678901234567890123456789012345678901234567890123456789012345678901234567890"
//"57edf4a22be3c955ac49da2e2107b67a"
class CMD5 : public IMessageDigest
{
public:
	//CONSTRUCTOR
	CMD5();
	//Update context to reflect the concatenation of another buffer of bytes.
	void AddData(char const* pcData, int iDataLength);
	//Final wrapup - pad to 64-byte boundary with the bit pattern 
	//1 0*(64-bit count of bits processed, MSB-first)
	void FinalDigest(char* pcDigest);
	//Reset current operation in order to prepare a new one
	void Reset();

private:
	enum { MD128LENGTH=4 };;
	//Context Variables
	unsigned int m_auiBuf[4];
	unsigned int m_auiBits[2];
	unsigned char m_aucIn[64];
	//
	static unsigned int F1(unsigned int x, unsigned int y, unsigned int z);
	static unsigned int F2(unsigned int x, unsigned int y, unsigned int z);
	static unsigned int F3(unsigned int x, unsigned int y, unsigned int z);
	static unsigned int F4(unsigned int x, unsigned int y, unsigned int z);
	//This is the central step in the MD5 algorithm.
	static void MD5STEP(unsigned int (*f)(unsigned int x, unsigned int y, unsigned int z),
		unsigned int& w, unsigned int x, unsigned int y, unsigned int z, unsigned int data, unsigned int s);
	//
	//The core of the MD5 algorithm, this alters an existing MD5 hash to
	//reflect the addition of 16 longwords of new data. MD5Update blocks
	//the data and converts bytes into longwords for this routine.
	void Transform();
};

inline unsigned int CMD5::F1(unsigned int x, unsigned int y, unsigned int z)
{
	return (z ^ (x & (y ^ z))); //OR (x & y | ~x & z)
}

inline unsigned int CMD5::F2(unsigned int x, unsigned int y, unsigned int z)
{
	return F1(z, x, y);
}

inline unsigned int CMD5::F3(unsigned int x, unsigned int y, unsigned int z)
{
	return x ^ y ^ z;
}

inline unsigned int CMD5::F4(unsigned int x, unsigned int y, unsigned int z)
{
	return (y ^ (x | ~z));
}

//This is the central step in the MD5 algorithm.
inline void CMD5::MD5STEP(unsigned int (*f)(unsigned int x, unsigned int y, unsigned int z),
	unsigned int& w, unsigned int x, unsigned int y, unsigned int z, unsigned int data, unsigned int s)
{
	w += f(x, y, z) + data;
	w = w << s | w >> (32-s);
	w += x;
}

#endif // __MD5_H__

