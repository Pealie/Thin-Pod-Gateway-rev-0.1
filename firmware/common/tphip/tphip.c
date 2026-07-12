#include "tphip.h"

#include <string.h>

#include <zephyr/sys/byteorder.h>

static int has_magic(const uint8_t *frame)
{
    return frame[0] == TPHIP_MAGIC_0 &&
           frame[1] == TPHIP_MAGIC_1 &&
           frame[2] == TPHIP_MAGIC_2 &&
           frame[3] == TPHIP_MAGIC_3;
}

static void write_common_header(
    uint8_t *frame,
    uint8_t opcode,
    uint8_t flags,
    uint16_t sequence,
    uint16_t status,
    uint16_t payload_len)
{
    frame[0] = TPHIP_MAGIC_0;
    frame[1] = TPHIP_MAGIC_1;
    frame[2] = TPHIP_MAGIC_2;
    frame[3] = TPHIP_MAGIC_3;
    frame[TPHIP_OFFSET_VERSION] = TPHIP_VERSION;
    frame[TPHIP_OFFSET_HEADER_LEN] = TPHIP_HEADER_LEN;
    frame[TPHIP_OFFSET_OPCODE] = opcode;
    frame[TPHIP_OFFSET_FLAGS] = flags;
    sys_put_le16(sequence, &frame[TPHIP_OFFSET_SEQUENCE]);
    sys_put_le16(status, &frame[TPHIP_OFFSET_STATUS]);
    sys_put_le16(payload_len, &frame[TPHIP_OFFSET_PAYLOAD_LEN]);
    sys_put_le16(0u, &frame[TPHIP_OFFSET_CRC16]);
}

uint16_t tphip_crc16_ccitt_false(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xffffu;
    size_t i;
    unsigned int bit;

    if (data == NULL && len != 0u) {
        return 0u;
    }

    for (i = 0u; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;

        for (bit = 0u; bit < 8u; bit++) {
            if ((crc & 0x8000u) != 0u) {
                crc = (uint16_t)((crc << 1) ^ 0x1021u);
            } else {
                crc = (uint16_t)(crc << 1);
            }
        }
    }

    return crc;
}

uint16_t tphip_crc16_frame(const uint8_t *frame, size_t logical_len)
{
    uint16_t crc = 0xffffu;
    size_t i;
    unsigned int bit;

    if (frame == NULL || logical_len < TPHIP_HEADER_LEN) {
        return 0u;
    }

    for (i = 0u; i < logical_len; i++) {
        uint8_t value = frame[i];

        if (i == TPHIP_OFFSET_CRC16 || i == (TPHIP_OFFSET_CRC16 + 1u)) {
            value = 0u;
        }

        crc ^= (uint16_t)value << 8;

        for (bit = 0u; bit < 8u; bit++) {
            if ((crc & 0x8000u) != 0u) {
                crc = (uint16_t)((crc << 1) ^ 0x1021u);
            } else {
                crc = (uint16_t)(crc << 1);
            }
        }
    }

    return crc;
}

int tphip_refresh_crc(uint8_t *frame, size_t logical_len)
{
    uint16_t crc;

    if (frame == NULL || logical_len < TPHIP_HEADER_LEN ||
        logical_len > TPHIP_RESPONSE_TRANSACTION_LEN) {
        return -1;
    }

    sys_put_le16(0u, &frame[TPHIP_OFFSET_CRC16]);
    crc = tphip_crc16_frame(frame, logical_len);
    sys_put_le16(crc, &frame[TPHIP_OFFSET_CRC16]);

    return 0;
}

void tphip_default_capabilities(struct tphip_capabilities *capabilities)
{
    if (capabilities == NULL) {
        return;
    }

    capabilities->protocol_major = 1u;
    capabilities->protocol_minor = 0u;
    capabilities->capability_flags =
        TPHIP_CAP_IRQ_SUPPORTED | TPHIP_CAP_CRC_SUPPORTED;
    capabilities->max_payload_bytes = TPHIP_MAX_PAYLOAD_BYTES;
    capabilities->max_record_bytes = 0u;
    capabilities->interface_build_id = TPHIP_INTERFACE_BUILD_ID;
}

int tphip_build_get_capabilities_request(
    uint8_t frame[TPHIP_REQUEST_TRANSACTION_LEN],
    uint16_t sequence)
{
    if (frame == NULL) {
        return -1;
    }

    memset(frame, 0, TPHIP_REQUEST_TRANSACTION_LEN);
    write_common_header(
        frame,
        TPHIP_OP_GET_CAPABILITIES,
        0u,
        sequence,
        0u,
        0u);

    return tphip_refresh_crc(frame, TPHIP_REQUEST_TRANSACTION_LEN);
}

enum tphip_validation_result tphip_validate_request(
    const uint8_t *frame,
    size_t received_len,
    struct tphip_request_view *view)
{
    uint16_t received_crc;
    uint16_t calculated_crc;

    if (view != NULL) {
        memset(view, 0, sizeof(*view));
    }

    if (frame == NULL || received_len != TPHIP_REQUEST_TRANSACTION_LEN) {
        return TPHIP_VALIDATE_BAD_PHYSICAL_LENGTH;
    }

    if (view != NULL) {
        view->opcode = frame[TPHIP_OFFSET_OPCODE];
        view->flags = frame[TPHIP_OFFSET_FLAGS];
        view->sequence = sys_get_le16(&frame[TPHIP_OFFSET_SEQUENCE]);
        view->status_or_reserved = sys_get_le16(&frame[TPHIP_OFFSET_STATUS]);
        view->payload_len = sys_get_le16(&frame[TPHIP_OFFSET_PAYLOAD_LEN]);
    }

    if (!has_magic(frame)) {
        return TPHIP_VALIDATE_BAD_MAGIC;
    }

    if (frame[TPHIP_OFFSET_VERSION] != TPHIP_VERSION) {
        return TPHIP_VALIDATE_BAD_VERSION;
    }

    if (frame[TPHIP_OFFSET_HEADER_LEN] != TPHIP_HEADER_LEN) {
        return TPHIP_VALIDATE_BAD_HEADER_LENGTH;
    }

    if (frame[TPHIP_OFFSET_FLAGS] != 0u) {
        return TPHIP_VALIDATE_BAD_FLAGS;
    }

    if (sys_get_le16(&frame[TPHIP_OFFSET_STATUS]) != 0u) {
        return TPHIP_VALIDATE_BAD_RESERVED;
    }

    if (sys_get_le16(&frame[TPHIP_OFFSET_PAYLOAD_LEN]) >
            TPHIP_MAX_PAYLOAD_BYTES ||
        sys_get_le16(&frame[TPHIP_OFFSET_PAYLOAD_LEN]) != 0u) {
        return TPHIP_VALIDATE_BAD_PAYLOAD_LENGTH;
    }

    received_crc = sys_get_le16(&frame[TPHIP_OFFSET_CRC16]);
    calculated_crc = tphip_crc16_frame(frame, TPHIP_REQUEST_TRANSACTION_LEN);

    if (received_crc != calculated_crc) {
        return TPHIP_VALIDATE_BAD_CRC;
    }

    if (frame[TPHIP_OFFSET_OPCODE] != TPHIP_OP_GET_CAPABILITIES) {
        return TPHIP_VALIDATE_BAD_OPCODE;
    }

    return TPHIP_VALIDATE_OK;
}

enum tphip_status tphip_status_from_validation(
    enum tphip_validation_result result)
{
    switch (result) {
    case TPHIP_VALIDATE_OK:
        return TPHIP_STATUS_OK;
    case TPHIP_VALIDATE_BAD_MAGIC:
        return TPHIP_STATUS_BAD_MAGIC;
    case TPHIP_VALIDATE_BAD_VERSION:
        return TPHIP_STATUS_BAD_VERSION;
    case TPHIP_VALIDATE_BAD_HEADER_LENGTH:
    case TPHIP_VALIDATE_BAD_PHYSICAL_LENGTH:
    case TPHIP_VALIDATE_BAD_PAYLOAD_LENGTH:
        return TPHIP_STATUS_BAD_LENGTH;
    case TPHIP_VALIDATE_BAD_FLAGS:
    case TPHIP_VALIDATE_BAD_RESERVED:
        return TPHIP_STATUS_BAD_FLAGS;
    case TPHIP_VALIDATE_BAD_CRC:
        return TPHIP_STATUS_BAD_CRC;
    case TPHIP_VALIDATE_BAD_OPCODE:
        return TPHIP_STATUS_UNKNOWN_OPCODE;
    case TPHIP_VALIDATE_BAD_SEQUENCE:
    case TPHIP_VALIDATE_BAD_STATUS:
    default:
        return TPHIP_STATUS_INTERNAL_ERROR;
    }
}

size_t tphip_build_response(
    uint8_t frame[TPHIP_RESPONSE_TRANSACTION_LEN],
    uint8_t request_opcode,
    uint16_t sequence,
    enum tphip_status status,
    const struct tphip_capabilities *capabilities)
{
    uint8_t flags = TPHIP_FLAG_RESPONSE;
    uint16_t payload_len = 0u;
    size_t logical_len;

    if (frame == NULL) {
        return 0u;
    }

    memset(frame, 0, TPHIP_RESPONSE_TRANSACTION_LEN);

    if (status != TPHIP_STATUS_OK) {
        flags |= TPHIP_FLAG_ERROR;
    } else if (request_opcode == TPHIP_OP_GET_CAPABILITIES &&
               capabilities != NULL) {
        payload_len = TPHIP_CAPABILITIES_PAYLOAD_LEN;
    }

    write_common_header(
        frame,
        (uint8_t)(request_opcode | 0x80u),
        flags,
        sequence,
        (uint16_t)status,
        payload_len);

    if (payload_len == TPHIP_CAPABILITIES_PAYLOAD_LEN) {
        frame[TPHIP_OFFSET_PAYLOAD + 0u] = capabilities->protocol_major;
        frame[TPHIP_OFFSET_PAYLOAD + 1u] = capabilities->protocol_minor;
        sys_put_le16(
            capabilities->capability_flags,
            &frame[TPHIP_OFFSET_PAYLOAD + 2u]);
        sys_put_le32(
            capabilities->max_payload_bytes,
            &frame[TPHIP_OFFSET_PAYLOAD + 4u]);
        sys_put_le32(
            capabilities->max_record_bytes,
            &frame[TPHIP_OFFSET_PAYLOAD + 8u]);
        sys_put_le32(
            capabilities->interface_build_id,
            &frame[TPHIP_OFFSET_PAYLOAD + 12u]);
    }

    logical_len = TPHIP_HEADER_LEN + payload_len;
    (void)tphip_refresh_crc(frame, logical_len);

    return logical_len;
}

enum tphip_validation_result tphip_validate_response(
    const uint8_t *frame,
    size_t received_len,
    uint8_t expected_opcode,
    uint16_t expected_sequence,
    struct tphip_response_view *view)
{
    uint8_t flags;
    uint16_t payload_len;
    uint16_t received_crc;
    uint16_t calculated_crc;
    uint16_t raw_status;
    size_t logical_len;

    if (view != NULL) {
        memset(view, 0, sizeof(*view));
    }

    if (frame == NULL || received_len != TPHIP_RESPONSE_TRANSACTION_LEN) {
        return TPHIP_VALIDATE_BAD_PHYSICAL_LENGTH;
    }

    if (!has_magic(frame)) {
        return TPHIP_VALIDATE_BAD_MAGIC;
    }

    if (frame[TPHIP_OFFSET_VERSION] != TPHIP_VERSION) {
        return TPHIP_VALIDATE_BAD_VERSION;
    }

    if (frame[TPHIP_OFFSET_HEADER_LEN] != TPHIP_HEADER_LEN) {
        return TPHIP_VALIDATE_BAD_HEADER_LENGTH;
    }

    flags = frame[TPHIP_OFFSET_FLAGS];
    if ((flags & TPHIP_FLAG_RESPONSE) == 0u ||
        (flags & TPHIP_RESERVED_FLAGS_MASK) != 0u) {
        return TPHIP_VALIDATE_BAD_FLAGS;
    }

    payload_len = sys_get_le16(&frame[TPHIP_OFFSET_PAYLOAD_LEN]);
    logical_len = TPHIP_HEADER_LEN + payload_len;

    if (payload_len > TPHIP_MAX_PAYLOAD_BYTES ||
        logical_len > TPHIP_RESPONSE_TRANSACTION_LEN) {
        return TPHIP_VALIDATE_BAD_PAYLOAD_LENGTH;
    }

    received_crc = sys_get_le16(&frame[TPHIP_OFFSET_CRC16]);
    calculated_crc = tphip_crc16_frame(frame, logical_len);
    if (received_crc != calculated_crc) {
        return TPHIP_VALIDATE_BAD_CRC;
    }

    if (sys_get_le16(&frame[TPHIP_OFFSET_SEQUENCE]) != expected_sequence) {
        return TPHIP_VALIDATE_BAD_SEQUENCE;
    }

    if (frame[TPHIP_OFFSET_OPCODE] != expected_opcode) {
        return TPHIP_VALIDATE_BAD_OPCODE;
    }

    raw_status = sys_get_le16(&frame[TPHIP_OFFSET_STATUS]);
    if (raw_status > TPHIP_STATUS_BAD_FLAGS) {
        return TPHIP_VALIDATE_BAD_STATUS;
    }

    if (raw_status == TPHIP_STATUS_OK) {
        if ((flags & TPHIP_FLAG_ERROR) != 0u) {
            return TPHIP_VALIDATE_BAD_FLAGS;
        }

        if (expected_opcode == TPHIP_RSP_GET_CAPABILITIES &&
            payload_len != TPHIP_CAPABILITIES_PAYLOAD_LEN) {
            return TPHIP_VALIDATE_BAD_PAYLOAD_LENGTH;
        }
    } else {
        if ((flags & TPHIP_FLAG_ERROR) == 0u) {
            return TPHIP_VALIDATE_BAD_FLAGS;
        }

        if (payload_len != 0u) {
            return TPHIP_VALIDATE_BAD_PAYLOAD_LENGTH;
        }
    }

    if (view != NULL) {
        view->opcode = frame[TPHIP_OFFSET_OPCODE];
        view->flags = flags;
        view->sequence = sys_get_le16(&frame[TPHIP_OFFSET_SEQUENCE]);
        view->status = (enum tphip_status)raw_status;
        view->payload_len = payload_len;
        view->logical_len = logical_len;
    }

    return TPHIP_VALIDATE_OK;
}

int tphip_decode_capabilities(
    const uint8_t *frame,
    const struct tphip_response_view *view,
    struct tphip_capabilities *capabilities)
{
    if (frame == NULL || view == NULL || capabilities == NULL ||
        view->status != TPHIP_STATUS_OK ||
        view->opcode != TPHIP_RSP_GET_CAPABILITIES ||
        view->payload_len != TPHIP_CAPABILITIES_PAYLOAD_LEN) {
        return -1;
    }

    capabilities->protocol_major = frame[TPHIP_OFFSET_PAYLOAD + 0u];
    capabilities->protocol_minor = frame[TPHIP_OFFSET_PAYLOAD + 1u];
    capabilities->capability_flags =
        sys_get_le16(&frame[TPHIP_OFFSET_PAYLOAD + 2u]);
    capabilities->max_payload_bytes =
        sys_get_le32(&frame[TPHIP_OFFSET_PAYLOAD + 4u]);
    capabilities->max_record_bytes =
        sys_get_le32(&frame[TPHIP_OFFSET_PAYLOAD + 8u]);
    capabilities->interface_build_id =
        sys_get_le32(&frame[TPHIP_OFFSET_PAYLOAD + 12u]);

    return 0;
}

const char *tphip_status_name(enum tphip_status status)
{
    switch (status) {
    case TPHIP_STATUS_OK:
        return "OK";
    case TPHIP_STATUS_BUSY:
        return "BUSY";
    case TPHIP_STATUS_BAD_MAGIC:
        return "BAD_MAGIC";
    case TPHIP_STATUS_BAD_VERSION:
        return "BAD_VERSION";
    case TPHIP_STATUS_BAD_LENGTH:
        return "BAD_LENGTH";
    case TPHIP_STATUS_BAD_CRC:
        return "BAD_CRC";
    case TPHIP_STATUS_UNKNOWN_OPCODE:
        return "UNKNOWN_OPCODE";
    case TPHIP_STATUS_NOT_READY:
        return "NOT_READY";
    case TPHIP_STATUS_INTERNAL_ERROR:
        return "INTERNAL_ERROR";
    case TPHIP_STATUS_BAD_FLAGS:
        return "BAD_FLAGS";
    default:
        return "UNKNOWN_STATUS";
    }
}

const char *tphip_validation_name(enum tphip_validation_result result)
{
    switch (result) {
    case TPHIP_VALIDATE_OK:
        return "OK";
    case TPHIP_VALIDATE_BAD_PHYSICAL_LENGTH:
        return "BAD_PHYSICAL_LENGTH";
    case TPHIP_VALIDATE_BAD_MAGIC:
        return "BAD_MAGIC";
    case TPHIP_VALIDATE_BAD_VERSION:
        return "BAD_VERSION";
    case TPHIP_VALIDATE_BAD_HEADER_LENGTH:
        return "BAD_HEADER_LENGTH";
    case TPHIP_VALIDATE_BAD_FLAGS:
        return "BAD_FLAGS";
    case TPHIP_VALIDATE_BAD_RESERVED:
        return "BAD_RESERVED";
    case TPHIP_VALIDATE_BAD_PAYLOAD_LENGTH:
        return "BAD_PAYLOAD_LENGTH";
    case TPHIP_VALIDATE_BAD_CRC:
        return "BAD_CRC";
    case TPHIP_VALIDATE_BAD_OPCODE:
        return "BAD_OPCODE";
    case TPHIP_VALIDATE_BAD_SEQUENCE:
        return "BAD_SEQUENCE";
    case TPHIP_VALIDATE_BAD_STATUS:
        return "BAD_STATUS";
    default:
        return "UNKNOWN_VALIDATION_RESULT";
    }
}