//
//  rsa.c
//  c-libp2p
//
//  Created by John Jones on 11/3/16.
//  Copyright © 2016 JMJAtlanta. All rights reserved.
//

#include <stdio.h>
#include <string.h>

#include "libp2p/crypto/rsa.h"

// mbedtls stuff
#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/bignum.h"
#include "mbedtls/x509.h"
#include "mbedtls/rsa.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/oid.h"
#include "mbedtls/pk.h"

/**
 * Take an rsa context and turn it into a der formatted byte stream.
 * NOTE: the stream starts from the right. So there could be a lot of padding in front.
 * Pay attention to the returned size to cut the padding.
 * @param rsa the rsa key to encode
 * @param buf where to put the bytes
 * @param size the max size of the buffer. The actual size used is returned in this value
 * @returns true(1) on success, else 0
 */
int libp2p_crypto_rsa_write_private_key_der( mbedtls_rsa_context *rsa, unsigned char *buf, size_t* size )
{
    int ret;
    unsigned char *c = buf + *size;
    size_t len = 0;

    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_mpi( &c, buf, &rsa->QP ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_mpi( &c, buf, &rsa->DQ ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_mpi( &c, buf, &rsa->DP ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_mpi( &c, buf, &rsa->Q ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_mpi( &c, buf, &rsa->P ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_mpi( &c, buf, &rsa->D ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_mpi( &c, buf, &rsa->E ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_mpi( &c, buf, &rsa->N ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_int( &c, buf, 0 ) );

    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_len( &c, buf, len ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_tag( &c, buf, MBEDTLS_ASN1_CONSTRUCTED |
                                                    MBEDTLS_ASN1_SEQUENCE ) );

    *size = len;
    return 1;
}

/**
 * Take a context and turn it into a der formatted byte stream.
 * @param key the key
 * @param buf the buffer to be filled
 * @param size the max size of the buffer. The actual size used is returned in this value
 * @returns true(1) on success, else false(0)
 */
int libp2p_crypto_rsa_write_public_key_der( mbedtls_pk_context *key, unsigned char *buf, size_t* size )
{
    int ret;
    unsigned char *c;
    size_t len = 0, par_len = 0, oid_len;
    const char *oid;

    c = buf + *size;

    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_pk_write_pubkey( &c, buf, key ) );

    if( c - buf < 1 ) // buffer is too small
        return 0;

    /*
     *  SubjectPublicKeyInfo  ::=  SEQUENCE  {
     *       algorithm            AlgorithmIdentifier,
     *       subjectPublicKey     BIT STRING }
     */
    *--c = 0;
    len += 1;

    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_len( &c, buf, len ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_tag( &c, buf, MBEDTLS_ASN1_BIT_STRING ) );

    if( ( ret = mbedtls_oid_get_oid_by_pk_alg( mbedtls_pk_get_type( key ),
                                       &oid, &oid_len ) ) != 0 )
    {
        return 0;
    }

    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_algorithm_identifier( &c, buf, oid, oid_len,
                                                        par_len ) );

    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_len( &c, buf, len ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_tag( &c, buf, MBEDTLS_ASN1_CONSTRUCTED |
                                                MBEDTLS_ASN1_SEQUENCE ) );

    *size = len;
    return 1;
}

/***
 * Generate an RSA keypair of a certain size, and place the results in the struct
 * @param private_key where to put the results
 * @param num_bits_for_keypair the number of bits for the key, 1024 is the minimum
 * @returns true(1) on success
 */
int libp2p_crypto_rsa_generate_keypair(struct RsaPrivateKey* private_key, unsigned long num_bits_for_keypair) {
	
	mbedtls_rsa_context rsa;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	
	int exponent = 65537;
	int retVal = 1;
	
	unsigned char* buffer;

	const char *pers = "rsa_genkey";
	
	// initialize mbedtls structs
	mbedtls_ctr_drbg_init( &ctr_drbg );
	mbedtls_entropy_init( &entropy );
	
	// seed the routines
	if( ( retVal = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
									  (const unsigned char *) pers,
									  strlen( pers ) ) ) != 0 )
	{
		retVal = 0;
		goto exit;
	}
	
	// initialize the rsa struct
	mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );
	
	// finally, generate the key
	if( ( retVal = mbedtls_rsa_gen_key( &rsa, mbedtls_ctr_drbg_random, &ctr_drbg, (unsigned int)num_bits_for_keypair,
									   exponent ) ) != 0 )
	{
		retVal = 0;
		goto exit;
	}
	retVal = 1;
	
	// fill in values of structures
	private_key->D = *(rsa.D.p);
	private_key->DP = *(rsa.DP.p);
	private_key->DQ = *(rsa.DQ.p);
	private_key->E = *(rsa.E.p);
	private_key->N = *(rsa.N.p);
	private_key->P = *(rsa.P.p);
	private_key->Q = *(rsa.Q.p);
	private_key->QP = *(rsa.QP.p);

	size_t buffer_size = 1600;
	buffer = malloc(sizeof(char) * buffer_size);
	retVal = libp2p_crypto_rsa_write_private_key_der(&rsa, buffer, &buffer_size);
	if (retVal == 0)
		return 0;

	// allocate memory for the private key der
	private_key->der_length = buffer_size;
	private_key->der = malloc(sizeof(char) * buffer_size);
	// add in the der to the buffer
	memcpy(private_key->der, &buffer[1600-buffer_size], buffer_size);

exit:
	if (buffer != NULL)
		free(buffer);
	mbedtls_rsa_free( &rsa );
	mbedtls_ctr_drbg_free( &ctr_drbg );
	mbedtls_entropy_free( &entropy );
	
	if (retVal != 0) {
		// now do the public key.
		retVal = libp2p_crypto_rsa_private_key_fill_public_key(private_key);
	}

	return retVal;
}

/**
 * Use the private key DER to fill in the public key DER
 * @param private_key the private key to use
 * @reutrns true(1) on success
 */
int libp2p_crypto_rsa_private_key_fill_public_key(struct RsaPrivateKey* private_key) {
	// first build the rsa context
	mbedtls_pk_context ctx;
	mbedtls_pk_init(&ctx);
	mbedtls_pk_parse_key(&ctx, private_key->der, private_key->der_length, NULL, 0);

	// buffer
	size_t buffer_size = 1600;
	unsigned char buffer[buffer_size];
	memset(buffer, 0, buffer_size);

	// generate public key der
	int retVal = libp2p_crypto_rsa_write_public_key_der(&ctx, buffer, &buffer_size);
	mbedtls_pk_free(&ctx);
	if (retVal == 0) {
		return 0;
	}

	// allocate memory for the public key der
	private_key->public_key_length = buffer_size;
	private_key->public_key_der = malloc(sizeof(char) * buffer_size);
	if (private_key->public_key_der == NULL) {
		return 0;
	}

	//copy it into the struct
	memcpy(private_key->public_key_der, &buffer[1600-buffer_size], buffer_size);

	return 1;
}

/***
 * Free resources used by RsaPrivateKey
 * @param private_key the resources
 * @returns true(1)
 */
int libp2p_crypto_rsa_rsa_private_key_free(struct RsaPrivateKey* private_key) {
	if (private_key->der != NULL)
		free(private_key->der);
	if (private_key->public_key_der != NULL)
		free(private_key->public_key_der);
	return 1;
}
