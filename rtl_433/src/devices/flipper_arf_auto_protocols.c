#include "decoder.h"
#include <string.h>

// -------------------------------------------------------------------------
// Subaru
// -------------------------------------------------------------------------
static int subaru_arf_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    if (bitbuffer->num_rows == 0) return DECODE_ABORT_EARLY;

    int r = 0;
    while (r < bitbuffer->num_rows && bitbuffer->bits_per_row[r] < 48) {
        r++;
    }
    if (r == bitbuffer->num_rows) return DECODE_ABORT_EARLY;

    uint8_t *b = bitbuffer->bb[r];

    uint64_t word = 0;
    for (int i = 0; i < 48; i++) {
        uint8_t bit = (b[i / 8] >> (7 - (i % 8))) & 1;
        word |= ((uint64_t)bit << i);
    }

    uint32_t id = word & 0xFFFFFFFF;
    uint8_t ctr = (word >> 32) & 0xFF;
    uint8_t btn_ck = (word >> 40) & 0xFF;
    uint8_t btn = btn_ck >> 4;
    uint8_t rx_ck = btn_ck & 0xF;

    uint8_t c = 0;
    for (int i = 0; i < 4; i++) c ^= (id >> (i * 8)) & 0xFFu;
    c ^= ctr ^ (btn & 0xFu);
    uint8_t calc_ck = c & 0xF;

    if (rx_ck != calc_ck) {
        return DECODE_FAIL_MIC;
    }

    data_t *data = data_make(
            "model",    "", DATA_STRING, "Subaru-ARF",
            "id",       "", DATA_FORMAT, "%08x", id,
            "counter",  "", DATA_INT,    ctr,
            "button",   "", DATA_INT,    btn,
            "mic",      "", DATA_STRING, "CHECKSUM",
            NULL);
    decoder_output_data(decoder, data);
    return 1;
}

static char const *const subaru_arf_fields[] = {
    "model", "id", "counter", "button", "mic", NULL,
};

r_device const flipper_arf_subaru = {
    .name        = "Subaru (Flipper-ARF)",
    .modulation  = OOK_PULSE_PWM,
    .short_width = 200,
    .long_width  = 600,
    .reset_limit = 8000,
    .decode_fn   = &subaru_arf_decode,
    .fields      = subaru_arf_fields,
};

// -------------------------------------------------------------------------
// Hyundai/Kia RIO
// -------------------------------------------------------------------------
static int hkr_arf_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    if (bitbuffer->num_rows == 0) return DECODE_ABORT_EARLY;

    int r = 0;
    while (r < bitbuffer->num_rows && bitbuffer->bits_per_row[r] < 64) {
        r++;
    }
    if (r == bitbuffer->num_rows) return DECODE_ABORT_EARLY;

    uint8_t *b = bitbuffer->bb[r];

    uint64_t word = 0;
    for (int i = 0; i < 64; i++) {
        uint8_t bit = (b[i / 8] >> (7 - (i % 8))) & 1;
        word |= ((uint64_t)bit << (63 - i));
    }

    uint32_t serial = (word >> 32) & 0xFFFFFFFF;
    uint16_t btn = (word >> 16) & 0xFFFF;
    uint16_t rx_ck = word & 0xFFFF;

    uint16_t calc_ck = 0;
    calc_ck += (serial >> 24) & 0xFF;
    calc_ck += (serial >> 16) & 0xFF;
    calc_ck += (serial >> 8) & 0xFF;
    calc_ck += serial & 0xFF;
    calc_ck += (btn >> 8) & 0xFF;
    calc_ck += btn & 0xFF;

    if (rx_ck != calc_ck) {
        return DECODE_FAIL_MIC;
    }

    data_t *data = data_make(
            "model",    "", DATA_STRING, "Hyundai-Kia-RIO-ARF",
            "id",       "", DATA_FORMAT, "%08x", serial,
            "button",   "", DATA_INT,    btn,
            "mic",      "", DATA_STRING, "CHECKSUM",
            NULL);
    decoder_output_data(decoder, data);
    return 1;
}

static char const *const hkr_arf_fields[] = {
    "model", "id", "button", "mic", NULL,
};

r_device const flipper_arf_hkr = {
    .name        = "Hyundai/Kia RIO (Flipper-ARF)",
    .modulation  = OOK_PULSE_PWM,
    .short_width = 312,
    .long_width  = 728,
    .reset_limit = 10000,
    .decode_fn   = &hkr_arf_decode,
    .fields      = hkr_arf_fields,
};

// -------------------------------------------------------------------------
// Mazda Siemens
// -------------------------------------------------------------------------
static int mazda_arf_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    if (bitbuffer->num_rows == 0) return DECODE_ABORT_EARLY;

    int r = 0;
    while (r < bitbuffer->num_rows && bitbuffer->bits_per_row[r] < 72) {
        r++;
    }
    if (r == bitbuffer->num_rows) return DECODE_ABORT_EARLY;

    uint8_t *b = bitbuffer->bb[r];

    uint8_t pkt[9] = {0};
    for (int i = 0; i < 72; i++) {
        uint8_t bit = (b[i / 8] >> (7 - (i % 8))) & 1;
        if (bit) {
            pkt[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    uint32_t hop = ((uint32_t)pkt[0]<<24)|((uint32_t)pkt[1]<<16)|((uint32_t)pkt[2]<<8)|pkt[3];
    uint32_t serial = ((uint32_t)pkt[4]<<16)|((uint32_t)pkt[5]<<8)|pkt[6];
    uint8_t ctr = pkt[7];
    uint8_t btn = pkt[8] >> 4;
    uint8_t rx_ck = pkt[8] & 0xF;

    uint8_t ck = 0;
    for (int i = 0; i < 4; i++) ck += (hop >> (i * 8)) & 0xFF;
    for (int i = 0; i < 3; i++) ck += (serial >> (i * 8)) & 0xFF;
    ck += ctr + btn;
    uint8_t calc_ck = (~ck) & 0xF;

    if (rx_ck != calc_ck) {
        return DECODE_FAIL_MIC;
    }

    data_t *data = data_make(
            "model",    "", DATA_STRING, "Mazda-Siemens-ARF",
            "id",       "", DATA_FORMAT, "%06x", serial,
            "hop",      "", DATA_FORMAT, "%08x", hop,
            "counter",  "", DATA_INT,    ctr,
            "button",   "", DATA_INT,    btn,
            "mic",      "", DATA_STRING, "CHECKSUM",
            NULL);
    decoder_output_data(decoder, data);
    return 1;
}

static char const *const mazda_arf_fields[] = {
    "model", "id", "hop", "counter", "button", "mic", NULL,
};

r_device const flipper_arf_mazda = {
    .name        = "Mazda Siemens (Flipper-ARF)",
    .modulation  = OOK_PULSE_PWM,
    .short_width = 450,
    .long_width  = 1350,
    .reset_limit = 14400,
    .decode_fn   = &mazda_arf_decode,
    .fields      = mazda_arf_fields,
};

// -------------------------------------------------------------------------
// VAG pre-2004
// -------------------------------------------------------------------------
static int vag_arf_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    if (bitbuffer->num_rows == 0) return DECODE_ABORT_EARLY;

    int r = 0;
    while (r < bitbuffer->num_rows && bitbuffer->bits_per_row[r] < 64) {
        r++;
    }
    if (r == bitbuffer->num_rows) return DECODE_ABORT_EARLY;

    uint8_t *b = bitbuffer->bb[r];

    uint64_t word = 0;
    for (int i = 0; i < 64; i++) {
        uint8_t bit = (b[i / 8] >> (7 - (i % 8))) & 1;
        word |= ((uint64_t)bit << (63 - i));
    }

    uint32_t tid = (uint32_t)(word >> 32);
    uint16_t ctr = (uint16_t)((word >> 16) & 0xFFFF);
    uint8_t btn = (uint8_t)((word >> 8) & 0xFF);
    uint8_t rx_ck = (uint8_t)(word & 0xFF);

    uint8_t s = 0;
    s += (tid >> 24) & 0xFF;
    s += (tid >> 16) & 0xFF;
    s += (tid >> 8) & 0xFF;
    s += tid & 0xFF;
    s += (ctr >> 8) & 0xFF;
    s += ctr & 0xFF;
    s += btn;
    uint8_t calc_ck = (uint8_t)(~s);

    if (rx_ck != calc_ck) {
        return DECODE_FAIL_MIC;
    }

    data_t *data = data_make(
            "model",    "", DATA_STRING, "VAG-2004-ARF",
            "id",       "", DATA_FORMAT, "%08x", tid,
            "counter",  "", DATA_INT,    ctr,
            "button",   "", DATA_INT,    btn,
            "mic",      "", DATA_STRING, "CHECKSUM",
            NULL);
    decoder_output_data(decoder, data);
    return 1;
}

static char const *const vag_arf_fields[] = {
    "model", "id", "counter", "button", "mic", NULL,
};

r_device const flipper_arf_vag = {
    .name        = "VAG pre-2004 (Flipper-ARF)",
    .modulation  = OOK_PULSE_PWM,
    .short_width = 250,
    .long_width  = 550,
    .reset_limit = 11000,
    .decode_fn   = &vag_arf_decode,
    .fields      = vag_arf_fields,
};

// -------------------------------------------------------------------------
// Hyundai Santa Fe 13-16
// -------------------------------------------------------------------------
static int santafe_arf_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    if (bitbuffer->num_rows == 0) return DECODE_ABORT_EARLY;

    int r = 0;
    while (r < bitbuffer->num_rows && bitbuffer->bits_per_row[r] < 80) {
        r++;
    }
    if (r == bitbuffer->num_rows) return DECODE_ABORT_EARLY;

    uint8_t *b = bitbuffer->bb[r];
    uint8_t pkt[10] = {0};

    for (int i = 0; i < 80; i++) {
        uint8_t bit = (b[i / 8] >> (7 - (i % 8))) & 1;
        if (bit) {
            pkt[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    uint32_t rolling = ((uint32_t)pkt[0]<<24)|((uint32_t)pkt[1]<<16)|((uint32_t)pkt[2]<<8)|pkt[3];
    uint32_t serial = ((uint32_t)pkt[4]<<16)|((uint32_t)pkt[5]<<8)|pkt[6];
    uint8_t ctr = pkt[7];
    uint8_t btn = pkt[8];
    uint8_t rx_crc = pkt[9];

    uint8_t crc = 0xFF;
    for (int i = 0; i < 9; i++) {
        crc ^= pkt[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc = (crc << 1);
            }
        }
    }

    if (rx_crc != crc) {
        return DECODE_FAIL_MIC;
    }

    data_t *data = data_make(
            "model",    "", DATA_STRING, "SantaFe-ARF",
            "id",       "", DATA_FORMAT, "%06x", serial,
            "rolling",  "", DATA_FORMAT, "%08x", rolling,
            "counter",  "", DATA_INT,    ctr,
            "button",   "", DATA_INT,    btn,
            "mic",      "", DATA_STRING, "CRC",
            NULL);
    decoder_output_data(decoder, data);
    return 1;
}

static char const *const santafe_arf_fields[] = {
    "model", "id", "rolling", "counter", "button", "mic", NULL,
};

r_device const flipper_arf_santafe = {
    .name        = "Hyundai Santa Fe 13-16 (Flipper-ARF)",
    .modulation  = OOK_PULSE_PWM,
    .short_width = 125,
    .long_width  = 375,
    .reset_limit = 12000,
    .decode_fn   = &santafe_arf_decode,
    .fields      = santafe_arf_fields,
};
