/*
 *  Minimal configuration for TLS 1.1 (RFC 4346)
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
/*
 * Minimal configuration for TLS 1.1 (RFC 4346), implementing only the
 * required ciphersuite: AIUI_MBEDTLS_TLS_RSA_WITH_3DES_EDE_CBC_SHA
 *
 * See README.txt for usage instructions.
 */

#ifndef AIUI_MBEDTLS_CONFIG_H
#define AIUI_MBEDTLS_CONFIG_H

/* System support */
//#define AIUI_MBEDTLS_HAVE_ASM
#define AIUI_MBEDTLS_HAVE_TIME

/* mbed TLS feature support */
#define AIUI_MBEDTLS_CIPHER_MODE_CBC
#define AIUI_MBEDTLS_PKCS1_V15
#define AIUI_MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#define AIUI_MBEDTLS_SSL_PROTO_TLS1_1

/**
 * \def AIUI_MBEDTLS_ERROR_STRERROR_DUMMY
 *
 * Enable a dummy error function to make use of mbedtls_strerror() in
 * third party libraries easier when AIUI_MBEDTLS_ERROR_C is disabled
 * (no effect when AIUI_MBEDTLS_ERROR_C is enabled).
 *
 * You can safely disable this if AIUI_MBEDTLS_ERROR_C is enabled, or if you're
 * not using mbedtls_strerror() or error_strerror() in your application.
 *
 * Disable if you run into name conflicts and want to really remove the
 * mbedtls_strerror()
 */
#define AIUI_MBEDTLS_ERROR_STRERROR_DUMMY
/* mbed TLS modules */
#define AIUI_MBEDTLS_AES_C
#define AIUI_MBEDTLS_ASN1_PARSE_C
#define AIUI_MBEDTLS_ASN1_WRITE_C
#define AIUI_MBEDTLS_BIGNUM_C
#define AIUI_MBEDTLS_CIPHER_C
#define AIUI_MBEDTLS_CTR_DRBG_C
#define AIUI_MBEDTLS_DES_C
#define AIUI_MBEDTLS_ENTROPY_C
#define AIUI_MBEDTLS_MD_C
#define AIUI_MBEDTLS_MD5_C
#define AIUI_MBEDTLS_NET_C
#define AIUI_MBEDTLS_OID_C
#define AIUI_MBEDTLS_PK_C
#define AIUI_MBEDTLS_PK_PARSE_C
#define AIUI_MBEDTLS_RSA_C
#define AIUI_MBEDTLS_SHA1_C
#define AIUI_MBEDTLS_SHA256_C
#define AIUI_MBEDTLS_SSL_CLI_C
#define AIUI_MBEDTLS_SSL_SRV_C
#define AIUI_MBEDTLS_SSL_TLS_C
#define AIUI_MBEDTLS_X509_CRT_PARSE_C
#define AIUI_MBEDTLS_X509_USE_C

/* For test certificates */
#define AIUI_MBEDTLS_BASE64_C
#define AIUI_MBEDTLS_CERTS_C
#define AIUI_MBEDTLS_PEM_PARSE_C

/* For testing with compat.sh */
#define AIUI_MBEDTLS_FS_IO

/*
 * We assume CHAR_BIT is 8 in many places. In practice, this is true on our
 * target platforms, so not an issue, but let's just be extra sure.
 */
#include <limits.h>
#if CHAR_BIT != 8
#error "mbed TLS requires a platform with 8-bit chars"
#endif

#if defined(_WIN32)
#if !defined(AIUI_MBEDTLS_PLATFORM_C)
#error "AIUI_MBEDTLS_PLATFORM_C is required on Windows"
#endif

/* Fix the config here. Not convenient to put an #ifdef _WIN32 in config.h as
 * it would confuse config.pl. */
#if !defined(AIUI_MBEDTLS_PLATFORM_SNPRINTF_ALT) && \
    !defined(AIUI_MBEDTLS_PLATFORM_SNPRINTF_MACRO)
#define AIUI_MBEDTLS_PLATFORM_SNPRINTF_ALT
#endif
#endif /* _WIN32 */

#if defined(TARGET_LIKE_MBED) && \
    ( defined(AIUI_MBEDTLS_NET_C) || defined(AIUI_MBEDTLS_TIMING_C) )
#error "The NET and TIMING modules are not available for mbed OS - please use the network and timing functions provided by mbed OS"
#endif

#if defined(AIUI_MBEDTLS_DEPRECATED_WARNING) && \
    !defined(__GNUC__) && !defined(__clang__)
#error "AIUI_MBEDTLS_DEPRECATED_WARNING only works with GCC and Clang"
#endif

#if defined(AIUI_MBEDTLS_HAVE_TIME_DATE) && !defined(AIUI_MBEDTLS_HAVE_TIME)
#error "AIUI_MBEDTLS_HAVE_TIME_DATE without AIUI_MBEDTLS_HAVE_TIME does not make sense"
#endif

#if defined(AIUI_MBEDTLS_AESNI_C) && !defined(AIUI_MBEDTLS_HAVE_ASM)
#error "AIUI_MBEDTLS_AESNI_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_CTR_DRBG_C) && !defined(AIUI_MBEDTLS_AES_C)
#error "AIUI_MBEDTLS_CTR_DRBG_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_DHM_C) && !defined(AIUI_MBEDTLS_BIGNUM_C)
#error "AIUI_MBEDTLS_DHM_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_TRUNCATED_HMAC_COMPAT) && !defined(AIUI_MBEDTLS_SSL_TRUNCATED_HMAC)
#error "AIUI_MBEDTLS_SSL_TRUNCATED_HMAC_COMPAT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_CMAC_C) && \
    !defined(AIUI_MBEDTLS_AES_C) && !defined(AIUI_MBEDTLS_DES_C)
#error "AIUI_MBEDTLS_CMAC_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_NIST_KW_C) && \
    ( !defined(AIUI_MBEDTLS_AES_C) || !defined(AIUI_MBEDTLS_CIPHER_C) )
#error "AIUI_MBEDTLS_NIST_KW_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ECDH_C) && !defined(AIUI_MBEDTLS_ECP_C)
#error "AIUI_MBEDTLS_ECDH_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ECDSA_C) &&            \
    ( !defined(AIUI_MBEDTLS_ECP_C) ||           \
      !defined(AIUI_MBEDTLS_ASN1_PARSE_C) ||    \
      !defined(AIUI_MBEDTLS_ASN1_WRITE_C) )
#error "AIUI_MBEDTLS_ECDSA_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ECJPAKE_C) &&           \
    ( !defined(AIUI_MBEDTLS_ECP_C) || !defined(AIUI_MBEDTLS_MD_C) )
#error "AIUI_MBEDTLS_ECJPAKE_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ECDSA_DETERMINISTIC) && !defined(AIUI_MBEDTLS_HMAC_DRBG_C)
#error "AIUI_MBEDTLS_ECDSA_DETERMINISTIC defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ECP_C) && ( !defined(AIUI_MBEDTLS_BIGNUM_C) || (   \
    !defined(AIUI_MBEDTLS_ECP_DP_SECP192R1_ENABLED) &&                  \
    !defined(AIUI_MBEDTLS_ECP_DP_SECP224R1_ENABLED) &&                  \
    !defined(AIUI_MBEDTLS_ECP_DP_SECP256R1_ENABLED) &&                  \
    !defined(AIUI_MBEDTLS_ECP_DP_SECP384R1_ENABLED) &&                  \
    !defined(AIUI_MBEDTLS_ECP_DP_SECP521R1_ENABLED) &&                  \
    !defined(AIUI_MBEDTLS_ECP_DP_BP256R1_ENABLED)   &&                  \
    !defined(AIUI_MBEDTLS_ECP_DP_BP384R1_ENABLED)   &&                  \
    !defined(AIUI_MBEDTLS_ECP_DP_BP512R1_ENABLED)   &&                  \
    !defined(AIUI_MBEDTLS_ECP_DP_SECP192K1_ENABLED) &&                  \
    !defined(AIUI_MBEDTLS_ECP_DP_SECP224K1_ENABLED) &&                  \
    !defined(AIUI_MBEDTLS_ECP_DP_SECP256K1_ENABLED) ) )
#error "AIUI_MBEDTLS_ECP_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ENTROPY_C) && (!defined(AIUI_MBEDTLS_SHA512_C) &&      \
                                    !defined(AIUI_MBEDTLS_SHA256_C))
#error "AIUI_MBEDTLS_ENTROPY_C defined, but not all prerequisites"
#endif
#if defined(AIUI_MBEDTLS_ENTROPY_C) && defined(AIUI_MBEDTLS_SHA512_C) &&         \
    defined(AIUI_MBEDTLS_CTR_DRBG_ENTROPY_LEN) && (AIUI_MBEDTLS_CTR_DRBG_ENTROPY_LEN > 64)
#error "AIUI_MBEDTLS_CTR_DRBG_ENTROPY_LEN value too high"
#endif
#if defined(AIUI_MBEDTLS_ENTROPY_C) &&                                            \
    ( !defined(AIUI_MBEDTLS_SHA512_C) || defined(AIUI_MBEDTLS_ENTROPY_FORCE_SHA256) ) \
    && defined(AIUI_MBEDTLS_CTR_DRBG_ENTROPY_LEN) && (AIUI_MBEDTLS_CTR_DRBG_ENTROPY_LEN > 32)
#error "AIUI_MBEDTLS_CTR_DRBG_ENTROPY_LEN value too high"
#endif
#if defined(AIUI_MBEDTLS_ENTROPY_C) && \
    defined(AIUI_MBEDTLS_ENTROPY_FORCE_SHA256) && !defined(AIUI_MBEDTLS_SHA256_C)
#error "AIUI_MBEDTLS_ENTROPY_FORCE_SHA256 defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_TEST_NULL_ENTROPY) && \
    ( !defined(AIUI_MBEDTLS_ENTROPY_C) || !defined(AIUI_MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES) )
#error "AIUI_MBEDTLS_TEST_NULL_ENTROPY defined, but not all prerequisites"
#endif
#if defined(AIUI_MBEDTLS_TEST_NULL_ENTROPY) && \
     ( defined(AIUI_MBEDTLS_ENTROPY_NV_SEED) || defined(AIUI_MBEDTLS_ENTROPY_HARDWARE_ALT) || \
    defined(AIUI_MBEDTLS_HAVEGE_C) )
#error "AIUI_MBEDTLS_TEST_NULL_ENTROPY defined, but entropy sources too"
#endif

#if defined(AIUI_MBEDTLS_GCM_C) && (                                        \
        !defined(AIUI_MBEDTLS_AES_C) && !defined(AIUI_MBEDTLS_CAMELLIA_C) )
#error "AIUI_MBEDTLS_GCM_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ECP_RANDOMIZE_JAC_ALT) && !defined(AIUI_MBEDTLS_ECP_INTERNAL_ALT)
#error "AIUI_MBEDTLS_ECP_RANDOMIZE_JAC_ALT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ECP_ADD_MIXED_ALT) && !defined(AIUI_MBEDTLS_ECP_INTERNAL_ALT)
#error "AIUI_MBEDTLS_ECP_ADD_MIXED_ALT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ECP_DOUBLE_JAC_ALT) && !defined(AIUI_MBEDTLS_ECP_INTERNAL_ALT)
#error "AIUI_MBEDTLS_ECP_DOUBLE_JAC_ALT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT) && !defined(AIUI_MBEDTLS_ECP_INTERNAL_ALT)
#error "AIUI_MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ECP_NORMALIZE_JAC_ALT) && !defined(AIUI_MBEDTLS_ECP_INTERNAL_ALT)
#error "AIUI_MBEDTLS_ECP_NORMALIZE_JAC_ALT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ECP_DOUBLE_ADD_MXZ_ALT) && !defined(AIUI_MBEDTLS_ECP_INTERNAL_ALT)
#error "AIUI_MBEDTLS_ECP_DOUBLE_ADD_MXZ_ALT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ECP_RANDOMIZE_MXZ_ALT) && !defined(AIUI_MBEDTLS_ECP_INTERNAL_ALT)
#error "AIUI_MBEDTLS_ECP_RANDOMIZE_MXZ_ALT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ECP_NORMALIZE_MXZ_ALT) && !defined(AIUI_MBEDTLS_ECP_INTERNAL_ALT)
#error "AIUI_MBEDTLS_ECP_NORMALIZE_MXZ_ALT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_HAVEGE_C) && !defined(AIUI_MBEDTLS_TIMING_C)
#error "AIUI_MBEDTLS_HAVEGE_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_HKDF_C) && !defined(AIUI_MBEDTLS_MD_C)
#error "AIUI_MBEDTLS_HKDF_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_HMAC_DRBG_C) && !defined(AIUI_MBEDTLS_MD_C)
#error "AIUI_MBEDTLS_HMAC_DRBG_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED) &&                 \
    ( !defined(AIUI_MBEDTLS_ECDH_C) || !defined(AIUI_MBEDTLS_X509_CRT_PARSE_C) )
#error "AIUI_MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED) &&                 \
    ( !defined(AIUI_MBEDTLS_ECDH_C) || !defined(AIUI_MBEDTLS_X509_CRT_PARSE_C) )
#error "AIUI_MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED) && !defined(AIUI_MBEDTLS_DHM_C)
#error "AIUI_MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED) &&                     \
    !defined(AIUI_MBEDTLS_ECDH_C)
#error "AIUI_MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED) &&                   \
    ( !defined(AIUI_MBEDTLS_DHM_C) || !defined(AIUI_MBEDTLS_RSA_C) ||           \
      !defined(AIUI_MBEDTLS_X509_CRT_PARSE_C) || !defined(AIUI_MBEDTLS_PKCS1_V15) )
#error "AIUI_MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED) &&                 \
    ( !defined(AIUI_MBEDTLS_ECDH_C) || !defined(AIUI_MBEDTLS_RSA_C) ||          \
      !defined(AIUI_MBEDTLS_X509_CRT_PARSE_C) || !defined(AIUI_MBEDTLS_PKCS1_V15) )
#error "AIUI_MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED) &&                 \
    ( !defined(AIUI_MBEDTLS_ECDH_C) || !defined(AIUI_MBEDTLS_ECDSA_C) ||          \
      !defined(AIUI_MBEDTLS_X509_CRT_PARSE_C) )
#error "AIUI_MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED) &&                   \
    ( !defined(AIUI_MBEDTLS_RSA_C) || !defined(AIUI_MBEDTLS_X509_CRT_PARSE_C) || \
      !defined(AIUI_MBEDTLS_PKCS1_V15) )
#error "AIUI_MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_KEY_EXCHANGE_RSA_ENABLED) &&                       \
    ( !defined(AIUI_MBEDTLS_RSA_C) || !defined(AIUI_MBEDTLS_X509_CRT_PARSE_C) || \
      !defined(AIUI_MBEDTLS_PKCS1_V15) )
#error "AIUI_MBEDTLS_KEY_EXCHANGE_RSA_ENABLED defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED) &&                    \
    ( !defined(AIUI_MBEDTLS_ECJPAKE_C) || !defined(AIUI_MBEDTLS_SHA256_C) ||      \
      !defined(AIUI_MBEDTLS_ECP_DP_SECP256R1_ENABLED) )
#error "AIUI_MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_MEMORY_BUFFER_ALLOC_C) &&                          \
    ( !defined(AIUI_MBEDTLS_PLATFORM_C) || !defined(AIUI_MBEDTLS_PLATFORM_MEMORY) )
#error "AIUI_MBEDTLS_MEMORY_BUFFER_ALLOC_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PADLOCK_C) && !defined(AIUI_MBEDTLS_HAVE_ASM)
#error "AIUI_MBEDTLS_PADLOCK_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PEM_PARSE_C) && !defined(AIUI_MBEDTLS_BASE64_C)
#error "AIUI_MBEDTLS_PEM_PARSE_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PEM_WRITE_C) && !defined(AIUI_MBEDTLS_BASE64_C)
#error "AIUI_MBEDTLS_PEM_WRITE_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PK_C) && \
    ( !defined(AIUI_MBEDTLS_RSA_C) && !defined(AIUI_MBEDTLS_ECP_C) )
#error "AIUI_MBEDTLS_PK_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PK_PARSE_C) && !defined(AIUI_MBEDTLS_PK_C)
#error "AIUI_MBEDTLS_PK_PARSE_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PK_WRITE_C) && !defined(AIUI_MBEDTLS_PK_C)
#error "AIUI_MBEDTLS_PK_WRITE_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PKCS11_C) && !defined(AIUI_MBEDTLS_PK_C)
#error "AIUI_MBEDTLS_PKCS11_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_EXIT_ALT) && !defined(AIUI_MBEDTLS_PLATFORM_C)
#error "AIUI_MBEDTLS_PLATFORM_EXIT_ALT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_EXIT_MACRO) && !defined(AIUI_MBEDTLS_PLATFORM_C)
#error "AIUI_MBEDTLS_PLATFORM_EXIT_MACRO defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_EXIT_MACRO) &&\
    ( defined(AIUI_MBEDTLS_PLATFORM_STD_EXIT) ||\
        defined(AIUI_MBEDTLS_PLATFORM_EXIT_ALT) )
#error "AIUI_MBEDTLS_PLATFORM_EXIT_MACRO and AIUI_MBEDTLS_PLATFORM_STD_EXIT/AIUI_MBEDTLS_PLATFORM_EXIT_ALT cannot be defined simultaneously"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_TIME_ALT) &&\
    ( !defined(AIUI_MBEDTLS_PLATFORM_C) ||\
        !defined(AIUI_MBEDTLS_HAVE_TIME) )
#error "AIUI_MBEDTLS_PLATFORM_TIME_ALT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_TIME_MACRO) &&\
    ( !defined(AIUI_MBEDTLS_PLATFORM_C) ||\
        !defined(AIUI_MBEDTLS_HAVE_TIME) )
#error "AIUI_MBEDTLS_PLATFORM_TIME_MACRO defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_TIME_TYPE_MACRO) &&\
    ( !defined(AIUI_MBEDTLS_PLATFORM_C) ||\
        !defined(AIUI_MBEDTLS_HAVE_TIME) )
#error "AIUI_MBEDTLS_PLATFORM_TIME_TYPE_MACRO defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_TIME_MACRO) &&\
    ( defined(AIUI_MBEDTLS_PLATFORM_STD_TIME) ||\
        defined(AIUI_MBEDTLS_PLATFORM_TIME_ALT) )
#error "AIUI_MBEDTLS_PLATFORM_TIME_MACRO and AIUI_MBEDTLS_PLATFORM_STD_TIME/AIUI_MBEDTLS_PLATFORM_TIME_ALT cannot be defined simultaneously"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_TIME_TYPE_MACRO) &&\
    ( defined(AIUI_MBEDTLS_PLATFORM_STD_TIME) ||\
        defined(AIUI_MBEDTLS_PLATFORM_TIME_ALT) )
#error "AIUI_MBEDTLS_PLATFORM_TIME_TYPE_MACRO and AIUI_MBEDTLS_PLATFORM_STD_TIME/AIUI_MBEDTLS_PLATFORM_TIME_ALT cannot be defined simultaneously"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_FPRINTF_ALT) && !defined(AIUI_MBEDTLS_PLATFORM_C)
#error "AIUI_MBEDTLS_PLATFORM_FPRINTF_ALT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_FPRINTF_MACRO) && !defined(AIUI_MBEDTLS_PLATFORM_C)
#error "AIUI_MBEDTLS_PLATFORM_FPRINTF_MACRO defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_FPRINTF_MACRO) &&\
    ( defined(AIUI_MBEDTLS_PLATFORM_STD_FPRINTF) ||\
        defined(AIUI_MBEDTLS_PLATFORM_FPRINTF_ALT) )
#error "AIUI_MBEDTLS_PLATFORM_FPRINTF_MACRO and AIUI_MBEDTLS_PLATFORM_STD_FPRINTF/AIUI_MBEDTLS_PLATFORM_FPRINTF_ALT cannot be defined simultaneously"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_FREE_MACRO) &&\
    ( !defined(AIUI_MBEDTLS_PLATFORM_C) || !defined(AIUI_MBEDTLS_PLATFORM_MEMORY) )
#error "AIUI_MBEDTLS_PLATFORM_FREE_MACRO defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_FREE_MACRO) &&\
    defined(AIUI_MBEDTLS_PLATFORM_STD_FREE)
#error "AIUI_MBEDTLS_PLATFORM_FREE_MACRO and AIUI_MBEDTLS_PLATFORM_STD_FREE cannot be defined simultaneously"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_FREE_MACRO) && !defined(AIUI_MBEDTLS_PLATFORM_CALLOC_MACRO)
#error "AIUI_MBEDTLS_PLATFORM_CALLOC_MACRO must be defined if AIUI_MBEDTLS_PLATFORM_FREE_MACRO is"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_CALLOC_MACRO) &&\
    ( !defined(AIUI_MBEDTLS_PLATFORM_C) || !defined(AIUI_MBEDTLS_PLATFORM_MEMORY) )
#error "AIUI_MBEDTLS_PLATFORM_CALLOC_MACRO defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_CALLOC_MACRO) &&\
    defined(AIUI_MBEDTLS_PLATFORM_STD_CALLOC)
#error "AIUI_MBEDTLS_PLATFORM_CALLOC_MACRO and AIUI_MBEDTLS_PLATFORM_STD_CALLOC cannot be defined simultaneously"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_CALLOC_MACRO) && !defined(AIUI_MBEDTLS_PLATFORM_FREE_MACRO)
#error "AIUI_MBEDTLS_PLATFORM_FREE_MACRO must be defined if AIUI_MBEDTLS_PLATFORM_CALLOC_MACRO is"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_MEMORY) && !defined(AIUI_MBEDTLS_PLATFORM_C)
#error "AIUI_MBEDTLS_PLATFORM_MEMORY defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_PRINTF_ALT) && !defined(AIUI_MBEDTLS_PLATFORM_C)
#error "AIUI_MBEDTLS_PLATFORM_PRINTF_ALT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_PRINTF_MACRO) && !defined(AIUI_MBEDTLS_PLATFORM_C)
#error "AIUI_MBEDTLS_PLATFORM_PRINTF_MACRO defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_PRINTF_MACRO) &&\
    ( defined(AIUI_MBEDTLS_PLATFORM_STD_PRINTF) ||\
        defined(AIUI_MBEDTLS_PLATFORM_PRINTF_ALT) )
#error "AIUI_MBEDTLS_PLATFORM_PRINTF_MACRO and AIUI_MBEDTLS_PLATFORM_STD_PRINTF/AIUI_MBEDTLS_PLATFORM_PRINTF_ALT cannot be defined simultaneously"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_SNPRINTF_ALT) && !defined(AIUI_MBEDTLS_PLATFORM_C)
#error "AIUI_MBEDTLS_PLATFORM_SNPRINTF_ALT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_SNPRINTF_MACRO) && !defined(AIUI_MBEDTLS_PLATFORM_C)
#error "AIUI_MBEDTLS_PLATFORM_SNPRINTF_MACRO defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_SNPRINTF_MACRO) &&\
    ( defined(AIUI_MBEDTLS_PLATFORM_STD_SNPRINTF) ||\
        defined(AIUI_MBEDTLS_PLATFORM_SNPRINTF_ALT) )
#error "AIUI_MBEDTLS_PLATFORM_SNPRINTF_MACRO and AIUI_MBEDTLS_PLATFORM_STD_SNPRINTF/AIUI_MBEDTLS_PLATFORM_SNPRINTF_ALT cannot be defined simultaneously"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_STD_MEM_HDR) &&\
    !defined(AIUI_MBEDTLS_PLATFORM_NO_STD_FUNCTIONS)
#error "AIUI_MBEDTLS_PLATFORM_STD_MEM_HDR defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_STD_CALLOC) && !defined(AIUI_MBEDTLS_PLATFORM_MEMORY)
#error "AIUI_MBEDTLS_PLATFORM_STD_CALLOC defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_STD_CALLOC) && !defined(AIUI_MBEDTLS_PLATFORM_MEMORY)
#error "AIUI_MBEDTLS_PLATFORM_STD_CALLOC defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_STD_FREE) && !defined(AIUI_MBEDTLS_PLATFORM_MEMORY)
#error "AIUI_MBEDTLS_PLATFORM_STD_FREE defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_STD_EXIT) &&\
    !defined(AIUI_MBEDTLS_PLATFORM_EXIT_ALT)
#error "AIUI_MBEDTLS_PLATFORM_STD_EXIT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_STD_TIME) &&\
    ( !defined(AIUI_MBEDTLS_PLATFORM_TIME_ALT) ||\
        !defined(AIUI_MBEDTLS_HAVE_TIME) )
#error "AIUI_MBEDTLS_PLATFORM_STD_TIME defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_STD_FPRINTF) &&\
    !defined(AIUI_MBEDTLS_PLATFORM_FPRINTF_ALT)
#error "AIUI_MBEDTLS_PLATFORM_STD_FPRINTF defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_STD_PRINTF) &&\
    !defined(AIUI_MBEDTLS_PLATFORM_PRINTF_ALT)
#error "AIUI_MBEDTLS_PLATFORM_STD_PRINTF defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_STD_SNPRINTF) &&\
    !defined(AIUI_MBEDTLS_PLATFORM_SNPRINTF_ALT)
#error "AIUI_MBEDTLS_PLATFORM_STD_SNPRINTF defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_ENTROPY_NV_SEED) &&\
    ( !defined(AIUI_MBEDTLS_PLATFORM_C) || !defined(AIUI_MBEDTLS_ENTROPY_C) )
#error "AIUI_MBEDTLS_ENTROPY_NV_SEED defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_NV_SEED_ALT) &&\
    !defined(AIUI_MBEDTLS_ENTROPY_NV_SEED)
#error "AIUI_MBEDTLS_PLATFORM_NV_SEED_ALT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_STD_NV_SEED_READ) &&\
    !defined(AIUI_MBEDTLS_PLATFORM_NV_SEED_ALT)
#error "AIUI_MBEDTLS_PLATFORM_STD_NV_SEED_READ defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_STD_NV_SEED_WRITE) &&\
    !defined(AIUI_MBEDTLS_PLATFORM_NV_SEED_ALT)
#error "AIUI_MBEDTLS_PLATFORM_STD_NV_SEED_WRITE defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_NV_SEED_READ_MACRO) &&\
    ( defined(AIUI_MBEDTLS_PLATFORM_STD_NV_SEED_READ) ||\
      defined(AIUI_MBEDTLS_PLATFORM_NV_SEED_ALT) )
#error "AIUI_MBEDTLS_PLATFORM_NV_SEED_READ_MACRO and AIUI_MBEDTLS_PLATFORM_STD_NV_SEED_READ cannot be defined simultaneously"
#endif

#if defined(AIUI_MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO) &&\
    ( defined(AIUI_MBEDTLS_PLATFORM_STD_NV_SEED_WRITE) ||\
      defined(AIUI_MBEDTLS_PLATFORM_NV_SEED_ALT) )
#error "AIUI_MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO and AIUI_MBEDTLS_PLATFORM_STD_NV_SEED_WRITE cannot be defined simultaneously"
#endif

#if defined(AIUI_MBEDTLS_RSA_C) && ( !defined(AIUI_MBEDTLS_BIGNUM_C) ||         \
    !defined(AIUI_MBEDTLS_OID_C) )
#error "AIUI_MBEDTLS_RSA_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_RSA_C) && ( !defined(AIUI_MBEDTLS_PKCS1_V21) &&         \
    !defined(AIUI_MBEDTLS_PKCS1_V15) )
#error "AIUI_MBEDTLS_RSA_C defined, but none of the PKCS1 versions enabled"
#endif

#if defined(AIUI_MBEDTLS_X509_RSASSA_PSS_SUPPORT) &&                        \
    ( !defined(AIUI_MBEDTLS_RSA_C) || !defined(AIUI_MBEDTLS_PKCS1_V21) )
#error "AIUI_MBEDTLS_X509_RSASSA_PSS_SUPPORT defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_PROTO_SSL3) && ( !defined(AIUI_MBEDTLS_MD5_C) ||     \
    !defined(AIUI_MBEDTLS_SHA1_C) )
#error "AIUI_MBEDTLS_SSL_PROTO_SSL3 defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_PROTO_TLS1) && ( !defined(AIUI_MBEDTLS_MD5_C) ||     \
    !defined(AIUI_MBEDTLS_SHA1_C) )
#error "AIUI_MBEDTLS_SSL_PROTO_TLS1 defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_1) && ( !defined(AIUI_MBEDTLS_MD5_C) ||     \
    !defined(AIUI_MBEDTLS_SHA1_C) )
#error "AIUI_MBEDTLS_SSL_PROTO_TLS1_1 defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_2) && ( !defined(AIUI_MBEDTLS_SHA1_C) &&     \
    !defined(AIUI_MBEDTLS_SHA256_C) && !defined(AIUI_MBEDTLS_SHA512_C) )
#error "AIUI_MBEDTLS_SSL_PROTO_TLS1_2 defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_PROTO_DTLS)     && \
    !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_1)  && \
    !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_2)
#error "AIUI_MBEDTLS_SSL_PROTO_DTLS defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_CLI_C) && !defined(AIUI_MBEDTLS_SSL_TLS_C)
#error "AIUI_MBEDTLS_SSL_CLI_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_TLS_C) && ( !defined(AIUI_MBEDTLS_CIPHER_C) ||     \
    !defined(AIUI_MBEDTLS_MD_C) )
#error "AIUI_MBEDTLS_SSL_TLS_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_SRV_C) && !defined(AIUI_MBEDTLS_SSL_TLS_C)
#error "AIUI_MBEDTLS_SSL_SRV_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_TLS_C) && (!defined(AIUI_MBEDTLS_SSL_PROTO_SSL3) && \
    !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1) && !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_1) && \
    !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_2))
#error "AIUI_MBEDTLS_SSL_TLS_C defined, but no protocols are active"
#endif

#if defined(AIUI_MBEDTLS_SSL_TLS_C) && (defined(AIUI_MBEDTLS_SSL_PROTO_SSL3) && \
    defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_1) && !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1))
#error "Illegal protocol selection"
#endif

#if defined(AIUI_MBEDTLS_SSL_TLS_C) && (defined(AIUI_MBEDTLS_SSL_PROTO_TLS1) && \
    defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_2) && !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_1))
#error "Illegal protocol selection"
#endif

#if defined(AIUI_MBEDTLS_SSL_TLS_C) && (defined(AIUI_MBEDTLS_SSL_PROTO_SSL3) && \
    defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_2) && (!defined(AIUI_MBEDTLS_SSL_PROTO_TLS1) || \
    !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_1)))
#error "Illegal protocol selection"
#endif

#if defined(AIUI_MBEDTLS_SSL_DTLS_HELLO_VERIFY) && !defined(AIUI_MBEDTLS_SSL_PROTO_DTLS)
#error "AIUI_MBEDTLS_SSL_DTLS_HELLO_VERIFY  defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE) && \
    !defined(AIUI_MBEDTLS_SSL_DTLS_HELLO_VERIFY)
#error "AIUI_MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE  defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_DTLS_ANTI_REPLAY) &&                              \
    ( !defined(AIUI_MBEDTLS_SSL_TLS_C) || !defined(AIUI_MBEDTLS_SSL_PROTO_DTLS) )
#error "AIUI_MBEDTLS_SSL_DTLS_ANTI_REPLAY  defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_DTLS_BADMAC_LIMIT) &&                              \
    ( !defined(AIUI_MBEDTLS_SSL_TLS_C) || !defined(AIUI_MBEDTLS_SSL_PROTO_DTLS) )
#error "AIUI_MBEDTLS_SSL_DTLS_BADMAC_LIMIT  defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_ENCRYPT_THEN_MAC) &&   \
    !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1)   &&      \
    !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_1) &&      \
    !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_2)
#error "AIUI_MBEDTLS_SSL_ENCRYPT_THEN_MAC defined, but not all prerequsites"
#endif

#if defined(AIUI_MBEDTLS_SSL_EXTENDED_MASTER_SECRET) && \
    !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1)   &&          \
    !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_1) &&          \
    !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1_2)
#error "AIUI_MBEDTLS_SSL_EXTENDED_MASTER_SECRET defined, but not all prerequsites"
#endif

#if defined(AIUI_MBEDTLS_SSL_TICKET_C) && !defined(AIUI_MBEDTLS_CIPHER_C)
#error "AIUI_MBEDTLS_SSL_TICKET_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_CBC_RECORD_SPLITTING) && \
    !defined(AIUI_MBEDTLS_SSL_PROTO_SSL3) && !defined(AIUI_MBEDTLS_SSL_PROTO_TLS1)
#error "AIUI_MBEDTLS_SSL_CBC_RECORD_SPLITTING defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_SSL_SERVER_NAME_INDICATION) && \
        !defined(AIUI_MBEDTLS_X509_CRT_PARSE_C)
#error "AIUI_MBEDTLS_SSL_SERVER_NAME_INDICATION defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_THREADING_PTHREAD)
#if !defined(AIUI_MBEDTLS_THREADING_C) || defined(AIUI_MBEDTLS_THREADING_IMPL)
#error "AIUI_MBEDTLS_THREADING_PTHREAD defined, but not all prerequisites"
#endif
#define AIUI_MBEDTLS_THREADING_IMPL
#endif

#if defined(AIUI_MBEDTLS_THREADING_ALT)
#if !defined(AIUI_MBEDTLS_THREADING_C) || defined(AIUI_MBEDTLS_THREADING_IMPL)
#error "AIUI_MBEDTLS_THREADING_ALT defined, but not all prerequisites"
#endif
#define AIUI_MBEDTLS_THREADING_IMPL
#endif

#if defined(AIUI_MBEDTLS_THREADING_C) && !defined(AIUI_MBEDTLS_THREADING_IMPL)
#error "AIUI_MBEDTLS_THREADING_C defined, single threading implementation required"
#endif
#undef AIUI_MBEDTLS_THREADING_IMPL

#if defined(AIUI_MBEDTLS_VERSION_FEATURES) && !defined(AIUI_MBEDTLS_VERSION_C)
#error "AIUI_MBEDTLS_VERSION_FEATURES defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_X509_USE_C) && ( !defined(AIUI_MBEDTLS_BIGNUM_C) ||  \
    !defined(AIUI_MBEDTLS_OID_C) || !defined(AIUI_MBEDTLS_ASN1_PARSE_C) ||      \
    !defined(AIUI_MBEDTLS_PK_PARSE_C) )
#error "AIUI_MBEDTLS_X509_USE_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_X509_CREATE_C) && ( !defined(AIUI_MBEDTLS_BIGNUM_C) ||  \
    !defined(AIUI_MBEDTLS_OID_C) || !defined(AIUI_MBEDTLS_ASN1_WRITE_C) ||       \
    !defined(AIUI_MBEDTLS_PK_WRITE_C) )
#error "AIUI_MBEDTLS_X509_CREATE_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_X509_CRT_PARSE_C) && ( !defined(AIUI_MBEDTLS_X509_USE_C) )
#error "AIUI_MBEDTLS_X509_CRT_PARSE_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_X509_CRL_PARSE_C) && ( !defined(AIUI_MBEDTLS_X509_USE_C) )
#error "AIUI_MBEDTLS_X509_CRL_PARSE_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_X509_CSR_PARSE_C) && ( !defined(AIUI_MBEDTLS_X509_USE_C) )
#error "AIUI_MBEDTLS_X509_CSR_PARSE_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_X509_CRT_WRITE_C) && ( !defined(AIUI_MBEDTLS_X509_CREATE_C) )
#error "AIUI_MBEDTLS_X509_CRT_WRITE_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_X509_CSR_WRITE_C) && ( !defined(AIUI_MBEDTLS_X509_CREATE_C) )
#error "AIUI_MBEDTLS_X509_CSR_WRITE_C defined, but not all prerequisites"
#endif

#if defined(AIUI_MBEDTLS_HAVE_INT32) && defined(AIUI_MBEDTLS_HAVE_INT64)
#error "AIUI_MBEDTLS_HAVE_INT32 and AIUI_MBEDTLS_HAVE_INT64 cannot be defined simultaneously"
#endif /* AIUI_MBEDTLS_HAVE_INT32 && AIUI_MBEDTLS_HAVE_INT64 */

#if ( defined(AIUI_MBEDTLS_HAVE_INT32) || defined(AIUI_MBEDTLS_HAVE_INT64) ) && \
    defined(AIUI_MBEDTLS_HAVE_ASM)
#error "AIUI_MBEDTLS_HAVE_INT32/AIUI_MBEDTLS_HAVE_INT64 and AIUI_MBEDTLS_HAVE_ASM cannot be defined simultaneously"
#endif /* (AIUI_MBEDTLS_HAVE_INT32 || AIUI_MBEDTLS_HAVE_INT64) && AIUI_MBEDTLS_HAVE_ASM */

/*
 * Avoid warning from -pedantic. This is a convenient place for this
 * workaround since this is included by every single file before the
 * #if defined(AIUI_MBEDTLS_xxx_C) that results in emtpy translation units.
 */
typedef int aiui_mbedtls_iso_c_forbids_empty_translation_units;

#endif /* AIUI_MBEDTLS_CONFIG_H */

