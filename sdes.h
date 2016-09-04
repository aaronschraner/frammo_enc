#ifndef SDES_H
#define SDES_H
#include "common.h"
#include <utility>

//functions for SDES encryption

namespace sdes
{
	typedef uint16_t Key; //pretend this is 10 bits
	typedef std::pair<byte, byte> KeyPair;

	//Bit permutation functions
	byte IP(byte input);  //IP permutation
	byte IIP(byte input); //inverse IP permutation
	Key P10(Key input);   //P10 permutation
	byte P8(Key input);   //P8 permutation
	byte EP(byte input);  //E/P expansion/permutation
	byte rearrange(byte input); //permutation to rearrange bits for convenience (in F_k)
	byte P4(byte input);
	byte SW(byte input);
	template <typename I, typename O = I> //generic bit permutation function
		O permutePattern(I input, const byte* bitOrder, int bitsIn, int bitsOut);

	byte circularShift(byte val, int shift, int bits = 5); //bidirectional circular shift
	byte SX(byte input, uint32_t table); 
	byte S0(byte input);
	byte S1(byte input);

	//Key pair generation functions
	KeyPair fastGen8bitKeys(Key input);
	KeyPair slowGen8bitKeys(Key input);
	KeyPair gen8bitKeys(Key input);

	//Intermediate SDES functions
	byte F_k(byte input, byte key);

	//High level SDES functions
	byte encrypt(byte plaintext, Key key);
	byte decrypt(byte ciphertext, Key key);
}

//////////////////////////////////////////////////////////////////////////////////////////
//permutePattern<I,O>
// Takes:
//  - input:	I			(Input value to be bit-permuted)
//  - bitOrder: const byte* (Order that bits of input should appear in output (1..bitsIn))
//  - bitsIn:	int			(Number of bits to be used in the input)
//  - bitsOut:	int			(Number of bits to put in the output)
// Returns:
//	The result of the bit permutation (type <O>, <bitsOut> bits long)
// 
// Description:
//	Takes the input value and rearranges its bits in the order specified by 
//	  <bitOrder>. Each element in bitOrder corresponds to the input bit that
//	  should go in that position of the output. For example if you were to 
//	  permute the input {A, B, C, D} with the bitOrder {2, 4, 3, 1}, the output
//	  would be a 4 bit number with bit values {B, D, C, A}.
//
// Note: bit permutation is used so often in this cipher it just made 
//	more sense to write a generic templated function and reuse it with 
//	different bit patterns.
//////////////////////////////////////////////////////////////////////////////////////////
template <typename I, typename O>
	O sdes::permutePattern(I input, const byte* bitOrder, int bitsIn, int bitsOut)
{
	//initialize the result to 0
	O result = 0;

	//for each output bit position, find the corresponding input bit and 
	// if it is true, set that bit of result to true
	for(int i=0; i < bitsOut; i++)
		result |= input & (1 << bitsIn - bitOrder[i]) ? 
			1 << bitsOut - 1 - i : 0;

	//return the permuted value
	return result;

}

#endif
