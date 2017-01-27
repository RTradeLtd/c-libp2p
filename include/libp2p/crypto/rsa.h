#ifndef rsa_h
#define rsa_h

#include <stddef.h>

struct RsaPrivateKey {
	// the basics of a key pair
	unsigned long long QP;
	unsigned long long DQ;
	unsigned long long DP;
	unsigned long long Q;
	unsigned long long P;
	unsigned long long D;
	unsigned long long E;
	unsigned long long N;
	// the keys in DER format
	// private
	char* der;
	size_t der_length;
	// public
	char* public_key_der;
	size_t public_key_length;
};

/**
 * generate a new private key
 * @param private_key the new private key
 * @param num_bits_for_keypair the size of the key (1024 minimum)
 * @returns true(1) on success
 */
int libp2p_crypto_rsa_generate_keypair(struct RsaPrivateKey* private_key, unsigned long num_bits_for_keypair);

/**
 * Use the private key DER to fill in the public key DER
 * @param private_key the private key to use
 * @reutrns true(1) on success
 */
int libp2p_crypto_rsa_private_key_fill_public_key(struct RsaPrivateKey* private_key);


/***
 * Free resources used by RsaPrivateKey
 * @param private_key the resources
 * @returns 0
 */
int libp2p_crypto_rsa_rsa_private_key_free(struct RsaPrivateKey* private_key);

/**
 * sign a message
 * @param private_key the private key
 * @param message the message to be signed
 * @param message_length the length of message
 * @param result the resultant signature. Note: should be pre-allocated and be the size of the private key (i.e. 2048)
 * @returns true(1) on successs, otherwise false(0)
 */
int libp2p_crypto_rsa_sign(struct RsaPrivateKey* private_key, unsigned char* message, size_t message_length, unsigned char* result);

#endif /* rsa_h */