#ifndef THINPOD_TPHIP_H_
#define THINPOD_TPHIP_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TPHIP_MAGIC_0 0x54u
#define TPHIP_MAGIC_1 0x50u
#define TPHIP_MAGIC_2 0x47u
#define TPHIP_MAGIC_3 0x57u

#define TPHIP_VERSION 1u
#define TPHIP_HEADER_LEN 16u
#define TPHIP_REQUEST_TRANSACTION_LEN 16u
#define TPHIP_CAPABILITIES_PAYLOAD_LEN 16u
#define TPHIP_RESPONSE_TRANSACTION_LEN 32u
#define TPHIP_MAX_PAYLOAD_BYTES 64u

#define TPHIP_OFFSET_MAGIC 0u
#define TPHIP_OFFSET_VERSION 4u
#define TPHIP_OFFSET_HEADER_LEN 5u
#define TPHIP_OFFSET_OPCODE 6u
#define TPHIP_OFFSET_FLAGS 7u
#define TPHIP_OFFSET_SEQUENCE 8u
#define TPHIP_OFFSET_STATUS 10u
#define TPHIP_OFFSET_PAYLOAD_LEN 12u
#define TPHIP_OFFSET_CRC16 14u
#define TPHIP_OFFSET_PAYLOAD 16u

#define TPHIP_OP_GET_CAPABILITIES 0x01u
#define TPHIP_RSP_GET_CAPABILITIES 0x81u

#define TPHIP_FLAG_RESPONSE 0x01u
#define TPHIP_FLAG_ERROR 0x02u
#define TPHIP_RESERVED_FLAGS_MASK 0xfcu

#define TPHIP_CAP_STATUS_SUPPORTED 0x0001u
#define TPHIP_CAP_COUNTERS_SUPPORTED 0x0002u
#define TPHIP_CAP_IRQ_SUPPORTED 0x0004u
#define TPHIP_CAP_CRC_SUPPORTED 0x0008u

#define TPHIP_INTERFACE_BUILD_ID 0x00010001u

#if TPHIP_HEADER_LEN != 16u
#error "TPHIP header length must remain 16 bytes"
#endif

#if TPHIP_REQUEST_TRANSACTION_LEN != 16u
#error "TPHIP request transaction length must remain 16 bytes"
#endif

#if TPHIP_RESPONSE_TRANSACTION_LEN != 32u
#error "TPHIP response transaction length must remain 32 bytes"
#endif

enum tphip_status {
    TPHIP_STATUS_OK = 0x0000,
    TPHIP_STATUS_BUSY = 0x0001,
    TPHIP_STATUS_BAD_MAGIC = 0x0002,
    TPHIP_STATUS_BAD_VERSION = 0x0003,
    TPHIP_STATUS_BAD_LENGTH = 0x0004,
    TPHIP_STATUS_BAD_CRC = 0x0005,
    TPHIP_STATUS_UNKNOWN_OPCODE = 0x0006,
    TPHIP_STATUS_NOT_READY = 0x0007,
    TPHIP_STATUS_INTERNAL_ERROR = 0x0008,
    TPHIP_STATUS_BAD_FLAGS = 0x0009,
};

enum tphip_validation_result {
    TPHIP_VALIDATE_OK = 0,
    TPHIP_VALIDATE_BAD_PHYSICAL_LENGTH,
    TPHIP_VALIDATE_BAD_MAGIC,
    TPHIP_VALIDATE_BAD_VERSION,
    TPHIP_VALIDATE_BAD_HEADER_LENGTH,
    TPHIP_VALIDATE_BAD_FLAGS,
    TPHIP_VALIDATE_BAD_RESERVED,
    TPHIP_VALIDATE_BAD_PAYLOAD_LENGTH,
    TPHIP_VALIDATE_BAD_CRC,
    TPHIP_VALIDATE_BAD_OPCODE,
    TPHIP_VALIDATE_BAD_SEQUENCE,
    TPHIP_VALIDATE_BAD_STATUS,
};

struct tphip_request_view {
    uint8_t opcode;
    uint8_t flags;
    uint16_t sequence;
    uint16_t status_or_reserved;
    uint16_t payload_len;
};

struct tphip_response_view {
    uint8_t opcode;
    uint8_t flags;
    uint16_t sequence;
    enum tphip_status status;
    uint16_t payload_len;
    size_t logical_len;
};

struct tphip_capabilities {
    uint8_t protocol_major;
    uint8_t protocol_minor;
    uint16_t capability_flags;
    uint32_t max_payload_bytes;
    uint32_t max_record_bytes;
    uint32_t interface_build_id;
};

uint16_t tphip_crc16_ccitt_false(const uint8_t *data, size_t len);
uint16_t tphip_crc16_frame(const uint8_t *frame, size_t logical_len);
int tphip_refresh_crc(uint8_t *frame, size_t logical_len);

void tphip_default_capabilities(struct tphip_capabilities *capabilities);

int tphip_build_get_capabilities_request(
    uint8_t frame[TPHIP_REQUEST_TRANSACTION_LEN],
    uint16_t sequence);

enum tphip_validation_result tphip_validate_request(
    const uint8_t *frame,
    size_t received_len,
    struct tphip_request_view *view);

enum tphip_status tphip_status_from_validation(
    enum tphip_validation_result result);

size_t tphip_build_response(
    uint8_t frame[TPHIP_RESPONSE_TRANSACTION_LEN],
    uint8_t request_opcode,
    uint16_t sequence,
    enum tphip_status status,
    const struct tphip_capabilities *capabilities);

enum tphip_validation_result tphip_validate_response(
    const uint8_t *frame,
    size_t received_len,
    uint8_t expected_opcode,
    uint16_t expected_sequence,
    struct tphip_response_view *view);

int tphip_decode_capabilities(
    const uint8_t *frame,
    const struct tphip_response_view *view,
    struct tphip_capabilities *capabilities);

const char *tphip_status_name(enum tphip_status status);
const char *tphip_validation_name(enum tphip_validation_result result);

#ifdef __cplusplus
}
#endif

#endif