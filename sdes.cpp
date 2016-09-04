
#include "sdes.h"
#ifdef _DEBUG
#include <iostream>
#include <iomanip>
#define DEBUG(x) std::cout << "DEBUG: " << x << std::endl;
#else
#define DEBUG(X)
#endif

namespace sdes
{

	//patterns for different bit permuting functions
	const byte P10_PATTERN[10] = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6};
	const byte P8_PATTERN [8]  = {6, 3, 7, 4, 8, 5, 10, 9};
	const byte IP_PATTERN [8]  = {2, 6, 3, 1, 4, 8, 5, 7};
	const byte IIP_PATTERN[8]  = {4, 1, 3, 5, 7, 2, 8, 6};
	const byte EP_PATTERN [8]  = {4, 1, 2, 3, 2, 3, 4, 1};
	const byte RA_PATTERN [8]  = {1, 2, 5, 6, 3, 4, 7, 8};
	const byte P4_PATTERN [4]  = {2, 4, 3, 1};


	//forward P10 bit permutation
	Key P10(Key input)
	{
		return permutePattern<Key>(input, P10_PATTERN, 10, 10);
	}

	//forward P8 bit permutation
	byte P8(Key input)
	{
		return permutePattern<Key, byte>(input, P8_PATTERN, 10, 8);
	}

	//forward IP bit permutation
	byte IP(byte input)
	{
		return permutePattern<byte>(input, IP_PATTERN, 8, 8);
	}

	//reverse IP bit permutation
	byte IIP(byte input)
	{
		return permutePattern<byte>(input, IIP_PATTERN, 8, 8);
	}

	//E/P expand/permutation function (uses 4 LS bits of input)
	byte EP(byte input)
	{
		return permutePattern<byte>(input, EP_PATTERN, 4, 8);
	}

	//permutation to rearrange bits for convenience (in F_k)
	byte rearrange(byte input) 
	{
		return permutePattern<byte>(input, RA_PATTERN, 8, 8);
	}
	
	byte P4(byte input)
	{
		return permutePattern<byte>(input, P4_PATTERN, 4, 4);
	}

	//switch the left and right 4 bits
	byte SW(byte input)
	{
		return (input << 4) | (input >> 4);
	}

	//circular shift a <bits>-bit value 
	//shift can be positive (right shift) or negative
	byte circularShift(byte val, int shift, int bits)
	{
		//get the shift in the range [-4,4]
		shift = (-(-shift)%bits)%bits;

		byte result;
		if(shift > 0) //shift right
					//right part         left part
			result = ((val >> shift) | val << (bits - shift));
		else //shift left
					//left part          right part
			result = ((val << -shift) | val >> (bits + shift));
		
		//strip (zero) the bits that shouldn't be there
		result &= 0xFF >> (8 - bits);

		return result;
	}

	//combines all the P10, P8, and circularShift stuff into one function
	//because having to deal with 3 functions to do this is overcomplicated
	KeyPair slowGen8bitKeys(Key input)
	{

		Key permuted = P10(input);

		Key LS1_left  = circularShift(permuted >> 5,   -1);
		Key LS1_right = circularShift(permuted & 0x1F, -1);

		byte key1 = P8((LS1_left << 5) | LS1_right);
		
		Key LS2_left  = circularShift(LS1_left,  -2);
		Key LS2_right = circularShift(LS1_right, -2);

		byte key2 = P8((LS2_left << 5) | LS2_right);

		return KeyPair(key1, key2);
	}

	//NOTE: Because all the functions (circularShift, P8, P10) do nothing 
	//but rearrange bits, they can be logically reduced to one substitution function.
	KeyPair fastGen8bitKeys(Key input)
	{
		//bit orders for each key
		const byte key1order [8] = {0, 6, 8, 3, 7, 2, 9, 5}; //0x06837295;
		const byte key2order [8] = {7, 2, 5, 4, 9, 1, 8, 0}; //0x72549180;
		byte key1=0, key2=0;
		for(byte src=0,dest=0x80; dest; src++,dest>>=1)
		{
			key1 |= (input & (0x200 >> key1order[src])) ? dest : 0;
			key2 |= (input & (0x200 >> key2order[src])) ? dest : 0;
		}
		return KeyPair(key1, key2);

	}

	//if GOFASTER is defined, use the faster of the two keygen algorithms that
	//combines all of the bit permutation into one quick step instead of 
	//doing multiple permutations and circular shifts
	KeyPair gen8bitKeys(Key input)
	{
#ifdef GOFASTER
		return fastGen8bitKeys(input);
#else
		return slowGen8bitKeys(input);
#endif
	}
	
	//ABCD
	//EFGH
	//IJKL
	//MNOP  -> 0bABCDEFG...
	byte SX(byte input, uint32_t table)
	{
		//input [ABCD] -> row[AD] col[BC]
		byte rowcol = ((input & 0x8) | (input << 2 & 0x4) | (input >> 1 & 0x3)) & 0xF;
		return (table >> 30 - (rowcol << 1)) & 0x3;
	}

	byte S0(byte input)
	{
		return SX(input, 0x4EE427DE);
	}
	byte S1(byte input)
	{
		return SX(input, 0x1B87C493);
	}


	byte F_k(byte input, byte key)
	{
		byte left = input >> 4;
		byte right = input & 0x0F;
		byte p = rearrange(EP(right)) ^ key;
		byte s0out = S0(p >> 4);
		byte s1out = S1(p & 0xF);
		byte s = s0out << 2 | s1out; //making an assumption here
		//byte s = s1out << 2 | s0out; //making an assumption here
		return (left ^ P4(s)) << 4 | right;
	}

	byte encrypt(byte plaintext, Key key)
	{
		KeyPair keypair = gen8bitKeys(key);
		return IIP(
				F_k(
					SW(
						F_k(
							IP(plaintext), keypair.first
						   )), keypair.second));
	}

	byte decrypt(byte ciphertext, Key key)
	{
		KeyPair keypair = gen8bitKeys(key);
		return IIP(
				F_k(
					SW(
						F_k(
							IP(ciphertext), keypair.second
						   )), keypair.first));
	}


		
}
