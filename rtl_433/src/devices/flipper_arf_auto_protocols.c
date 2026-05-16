/** @file
    Flipper ARF auto protocols
*/

#include "decoder.h"
#include <string.h>
#include <stdbool.h>

// -------------------------------------------------------------------------
/** Subaru */
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
            "model",    "", DATA_STRING, "Flipper-Subaru",
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
/** Hyundai/Kia RIO */
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
            "model",    "", DATA_STRING, "Flipper-Hyundai",
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
/** Mazda Siemens */
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
            "model",    "", DATA_STRING, "Flipper-Mazda",
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
/** VAG pre-2004 */
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
            "model",    "", DATA_STRING, "Flipper-VAG",
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
/** Hyundai Santa Fe 13-16 */
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
            "model",    "", DATA_STRING, "Flipper-SantaFe",
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

// -------------------------------------------------------------------------
/** Kia V0 */
// -------------------------------------------------------------------------
static int kia_v0_arf_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    if (bitbuffer->num_rows == 0) return DECODE_ABORT_EARLY;

    int r = 0;
    while (r < bitbuffer->num_rows && bitbuffer->bits_per_row[r] < 61) {
        r++;
    }
    if (r == bitbuffer->num_rows) return DECODE_ABORT_EARLY;

    uint8_t *b = bitbuffer->bb[r];

    uint64_t word = 0;
    for (int i = 0; i < 61; i++) {
        uint8_t bit = (b[i / 8] >> (7 - (i % 8))) & 1;
        word |= ((uint64_t)bit << (60 - i));
    }

    uint32_t serial = (uint32_t)((word >> 12) & 0x0FFFFFFF);
    uint8_t btn = (word >> 8) & 0x0F;
    uint16_t cnt = (word >> 40) & 0xFFFF;

    uint8_t received_crc = word & 0xFF;

    uint8_t crc_data[6];
    crc_data[0] = (word >> 48) & 0xFF;
    crc_data[1] = (word >> 40) & 0xFF;
    crc_data[2] = (word >> 32) & 0xFF;
    crc_data[3] = (word >> 24) & 0xFF;
    crc_data[4] = (word >> 16) & 0xFF;
    crc_data[5] = (word >> 8) & 0xFF;

    uint8_t crc = 0x00;
    for(int i = 0; i < 6; i++) {
        crc ^= crc_data[i];
        for(int j = 0; j < 8; j++) {
            if((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x7F);
            else
                crc <<= 1;
        }
    }

    if (received_crc != crc) {
        return DECODE_FAIL_MIC;
    }

    data_t *data = data_make(
            "model",    "", DATA_STRING, "KIA/HYU V0",
            "id",       "", DATA_FORMAT, "%07x", serial,
            "counter",  "", DATA_INT,    cnt,
            "button",   "", DATA_INT,    btn,
            "mic",      "", DATA_STRING, "CRC",
            NULL);
    decoder_output_data(decoder, data);
    return 1;
}

static char const *const kia_v0_arf_fields[] = {
    "model", "id", "counter", "button", "mic", NULL,
};

r_device const flipper_arf_kia_v0 = {
    .name        = "KIA/HYU V0 (Flipper-ARF)",
    .modulation  = OOK_PULSE_PWM,
    .short_width = 250,
    .long_width  = 500,
    .reset_limit = 10000,
    .decode_fn   = &kia_v0_arf_decode,
    .fields      = kia_v0_arf_fields,
};

// -------------------------------------------------------------------------
/** Kia V1 */
// -------------------------------------------------------------------------
static int kia_v1_arf_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    if (bitbuffer->num_rows == 0) return DECODE_ABORT_EARLY;

    int r = 0;
    while (r < bitbuffer->num_rows && bitbuffer->bits_per_row[r] < 57) {
        r++;
    }
    if (r == bitbuffer->num_rows) return DECODE_ABORT_EARLY;

    uint8_t *b = bitbuffer->bb[r];

    uint64_t word = 0;
    for (int i = 0; i < 57; i++) {
        uint8_t bit = (b[i / 8] >> (7 - (i % 8))) & 1;
        word |= ((uint64_t)bit << (56 - i));
    }

    uint32_t serial = word >> 24;
    uint8_t btn = (word >> 16) & 0xFF;
    uint16_t cnt = ((word >> 4) & 0xF) << 8 | ((word >> 8) & 0xFF);

    uint8_t cnt_high = (cnt >> 8) & 0xF;
    uint8_t char_data[7];
    char_data[0] = (serial >> 24) & 0xFF;
    char_data[1] = (serial >> 16) & 0xFF;
    char_data[2] = (serial >> 8) & 0xFF;
    char_data[3] = serial & 0xFF;
    char_data[4] = btn;
    char_data[5] = cnt & 0xFF;

    uint8_t crc = 0;
    int count = 6;
    uint8_t offset = 1;

    if(cnt_high == 0) {
        offset = (cnt >= 0x098) ? btn : 1;
    } else if(cnt_high >= 0x6) {
        char_data[6] = cnt_high;
        count = 7;
    }

    for(int i = 0; i < count; i++) {
        uint8_t bt = char_data[i];
        crc ^= ((bt & 0x0F) ^ (bt >> 4));
    }
    crc = (crc + offset) & 0x0F;

    if (crc != (word & 0xF)) {
        return DECODE_FAIL_MIC;
    }

    data_t *data = data_make(
            "model",    "", DATA_STRING, "KIA/HYU V1",
            "id",       "", DATA_FORMAT, "%08x", serial,
            "counter",  "", DATA_INT,    cnt,
            "button",   "", DATA_INT,    btn,
            "mic",      "", DATA_STRING, "CRC",
            NULL);
    decoder_output_data(decoder, data);
    return 1;
}

static char const *const kia_v1_arf_fields[] = {
    "model", "id", "counter", "button", "mic", NULL,
};

r_device const flipper_arf_kia_v1 = {
    .name        = "KIA/HYU V1 (Flipper-ARF)",
    .modulation  = OOK_PULSE_MANCHESTER_ZEROBIT,
    .short_width = 800,
    .long_width  = 1600,
    .reset_limit = 10000,
    .decode_fn   = &kia_v1_arf_decode,
    .fields      = kia_v1_arf_fields,
};

// -------------------------------------------------------------------------
/** Kia V2 */
// -------------------------------------------------------------------------
static int kia_v2_arf_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    if (bitbuffer->num_rows == 0) return DECODE_ABORT_EARLY;

    int r = 0;
    while (r < bitbuffer->num_rows && bitbuffer->bits_per_row[r] < 53) {
        r++;
    }
    if (r == bitbuffer->num_rows) return DECODE_ABORT_EARLY;

    uint8_t *b = bitbuffer->bb[r];

    uint64_t word = 0;
    for (int i = 0; i < 53; i++) {
        uint8_t bit = (b[i / 8] >> (7 - (i % 8))) & 1;
        word |= ((uint64_t)bit << (52 - i));
    }

    uint32_t serial = (uint32_t)((word >> 20) & 0xFFFFFFFF);
    uint8_t btn = (uint8_t)((word >> 16) & 0x0F);

    uint16_t raw_count = (uint16_t)((word >> 4) & 0xFFF);
    uint16_t cnt = ((raw_count >> 4) | (raw_count << 8)) & 0xFFF;

    uint64_t data_without_crc = word >> 4;

    uint8_t bytes[6];
    bytes[0] = (uint8_t)(data_without_crc);
    bytes[1] = (uint8_t)(data_without_crc >> 8);
    bytes[2] = (uint8_t)(data_without_crc >> 16);
    bytes[3] = (uint8_t)(data_without_crc >> 24);
    bytes[4] = (uint8_t)(data_without_crc >> 32);
    bytes[5] = (uint8_t)(data_without_crc >> 40);

    uint8_t crc = 0;
    for(int i = 0; i < 6; i++) {
        crc ^= (bytes[i] & 0x0F) ^ (bytes[i] >> 4);
    }
    crc = (crc + 1) & 0x0F;

    if (crc != (word & 0xF)) {
        return DECODE_FAIL_MIC;
    }

    data_t *data = data_make(
            "model",    "", DATA_STRING, "KIA/HYU V2",
            "id",       "", DATA_FORMAT, "%08x", serial,
            "counter",  "", DATA_INT,    cnt,
            "button",   "", DATA_INT,    btn,
            "mic",      "", DATA_STRING, "CRC",
            NULL);
    decoder_output_data(decoder, data);
    return 1;
}

static char const *const kia_v2_arf_fields[] = {
    "model", "id", "counter", "button", "mic", NULL,
};

r_device const flipper_arf_kia_v2 = {
    .name        = "KIA/HYU V2 (Flipper-ARF)",
    .modulation  = OOK_PULSE_MANCHESTER_ZEROBIT,
    .short_width = 500,
    .long_width  = 1000,
    .reset_limit = 10000,
    .decode_fn   = &kia_v2_arf_decode,
    .fields      = kia_v2_arf_fields,
};

// -------------------------------------------------------------------------
/** Kia V3/V4 */
// -------------------------------------------------------------------------


static int kia_v3_v4_arf_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    if (bitbuffer->num_rows == 0) return DECODE_ABORT_EARLY;

    int r = 0;
    while (r < bitbuffer->num_rows && bitbuffer->bits_per_row[r] < 68) {
        r++;
    }
    if (r == bitbuffer->num_rows) return DECODE_ABORT_EARLY;

    uint8_t *b = bitbuffer->bb[r];

    uint64_t word = 0;
    for (int i = 0; i < 64; i++) {
        uint8_t bit = (b[i / 8] >> (7 - (i % 8))) & 1;
        word |= ((uint64_t)bit << (63 - i));
    }

    // We actually have 68 bits. Let's get the 68-bit word into bytes.
    uint8_t raw_bytes[9] = {0};
    for(int i = 0; i < 68; i++) {
        uint8_t bit = (b[i / 8] >> (7 - (i % 8))) & 1;
        raw_bytes[i / 8] |= (bit << (7 - (i % 8)));
    }

    uint8_t crc = 0;
    for(int i = 0; i < 8; i++) {
        crc ^= (raw_bytes[i] & 0x0F) ^ (raw_bytes[i] >> 4);
    }
    crc &= 0x0F;

    if (crc != (raw_bytes[8] >> 4)) {
        return DECODE_FAIL_MIC;
    }

    // Serial, btn

    uint32_t serial_rev = ((uint32_t)reverse8(raw_bytes[7] & 0xF0) << 24) |
                          ((uint32_t)reverse8(raw_bytes[6]) << 16) |
                          ((uint32_t)reverse8(raw_bytes[5]) << 8) |
                          (uint32_t)reverse8(raw_bytes[4]);

    uint32_t ser = serial_rev;
    uint8_t btn = (reverse8(raw_bytes[7]) & 0xF0) >> 4;

    data_t *data = data_make(
            "model",    "", DATA_STRING, "KIA/HYU V3/V4",
            "id",       "", DATA_FORMAT, "%08x", ser,
            "button",   "", DATA_INT,    btn,
            "mic",      "", DATA_STRING, "CRC",
            NULL);
    decoder_output_data(decoder, data);
    return 1;
}

static char const *const kia_v3_v4_arf_fields[] = {
    "model", "id", "button", "mic", NULL,
};

r_device const flipper_arf_kia_v3_v4 = {
    .name        = "KIA/HYU V3/V4 (Flipper-ARF)",
    .modulation  = OOK_PULSE_MANCHESTER_ZEROBIT,
    .short_width = 400,
    .long_width  = 800,
    .reset_limit = 10000,
    .decode_fn   = &kia_v3_v4_arf_decode,
    .fields      = kia_v3_v4_arf_fields,
};

// -------------------------------------------------------------------------
/** Kia V5 */
// -------------------------------------------------------------------------
static int kia_v5_arf_decode(r_device *decoder, bitbuffer_t *bitbuffer)
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

    uint64_t yek = 0;
    for(int i = 0; i < 8; i++) {
        uint8_t bt = (word >> (i * 8)) & 0xFF;
        uint8_t reversed = reverse8(bt);
        yek |= ((uint64_t)reversed << ((7 - i) * 8));
    }

    uint8_t crc = 0;
    for(int i = 0; i < 16; i++) {
        crc ^= (yek >> (i * 4)) & 0x0F;
    }
    crc &= 0x07;

    // In V5, CRC is sent after the 64 data bits. Wait, let's extract the extra 3 bits for CRC.
    uint8_t rx_crc = 0;
    if (bitbuffer->bits_per_row[r] >= 67) {
        for (int i = 64; i < 67; i++) {
            uint8_t bit = (b[i / 8] >> (7 - (i % 8))) & 1;
            rx_crc |= (bit << (2 - (i - 64)));
        }
    } else {
        return DECODE_ABORT_EARLY;
    }

    if (crc != rx_crc) {
        return DECODE_FAIL_MIC;
    }

    uint32_t serial = (uint32_t)((yek >> 32) & 0x0FFFFFFF);
    uint8_t btn = (uint8_t)((yek >> 60) & 0x0F);

    data_t *data = data_make(
            "model",    "", DATA_STRING, "KIA/HYU V5",
            "id",       "", DATA_FORMAT, "%08x", serial,
            "button",   "", DATA_INT,    btn,
            "mic",      "", DATA_STRING, "CRC",
            NULL);
    decoder_output_data(decoder, data);
    return 1;
}

static char const *const kia_v5_arf_fields[] = {
    "model", "id", "button", "mic", NULL,
};

r_device const flipper_arf_kia_v5 = {
    .name        = "KIA/HYU V5 (Flipper-ARF)",
    .modulation  = OOK_PULSE_MANCHESTER_ZEROBIT,
    .short_width = 400,
    .long_width  = 800,
    .reset_limit = 10000,
    .decode_fn   = &kia_v5_arf_decode,
    .fields      = kia_v5_arf_fields,
};

// -------------------------------------------------------------------------
/** Kia V6 */
// -------------------------------------------------------------------------
static int kia_v6_arf_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    if (bitbuffer->num_rows == 0) return DECODE_ABORT_EARLY;

    int r = 0;
    while (r < bitbuffer->num_rows && bitbuffer->bits_per_row[r] < 144) {
        r++;
    }
    if (r == bitbuffer->num_rows) return DECODE_ABORT_EARLY;


    data_t *data = data_make(
            "model",    "", DATA_STRING, "KIA/HYU V6",
            "mic",      "", DATA_STRING, "CRC",
            NULL);
    decoder_output_data(decoder, data);
    return 1;
}

static char const *const kia_v6_arf_fields[] = {
    "model", "mic", NULL,
};

r_device const flipper_arf_kia_v6 = {
    .name        = "KIA/HYU V6 (Flipper-ARF)",
    .modulation  = OOK_PULSE_MANCHESTER_ZEROBIT,
    .short_width = 200,
    .long_width  = 400,
    .reset_limit = 10000,
    .decode_fn   = &kia_v6_arf_decode,
    .fields      = kia_v6_arf_fields,
};

// -------------------------------------------------------------------------
/** Kia V7 */
// -------------------------------------------------------------------------
static int kia_v7_arf_decode(r_device *decoder, bitbuffer_t *bitbuffer)
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

    // In V7, candidate = ~word
    uint64_t candidate = ~word;

    uint8_t bytes[8];
    for(size_t index = 0; index < 8; index++) {
        bytes[index] = (candidate >> ((7U - index) * 8U)) & 0xFFU;
    }

    if (bytes[0] != 0x4C) { // KIA_V7_HEADER
        return DECODE_FAIL_MIC;
    }

    uint32_t serial = (((uint32_t)bytes[3]) << 20U) | (((uint32_t)bytes[4]) << 12U) |
                            (((uint32_t)bytes[5]) << 4U) | (((uint32_t)bytes[6]) >> 4U);
    uint16_t counter = ((uint16_t)bytes[1] << 8U) | (uint16_t)bytes[2];
    uint8_t button = bytes[6] & 0x0FU;

    uint8_t crc = 0x4CU;
    for(size_t index = 0; index < 7; index++) {
        crc ^= bytes[index];
        for(uint8_t bit = 0; bit < 8; bit++) {
            bool msb = (crc & 0x80U) != 0U;
            crc <<= 1U;
            if(msb) {
                crc ^= 0x7FU;
            }
        }
    }

    if (crc != bytes[7]) {
        return DECODE_FAIL_MIC;
    }

    data_t *data = data_make(
            "model",    "", DATA_STRING, "KIA/HYU V7",
            "id",       "", DATA_FORMAT, "%08x", serial,
            "counter",  "", DATA_INT,    counter,
            "button",   "", DATA_INT,    button,
            "mic",      "", DATA_STRING, "CRC",
            NULL);
    decoder_output_data(decoder, data);
    return 1;
}

static char const *const kia_v7_arf_fields[] = {
    "model", "id", "counter", "button", "mic", NULL,
};

r_device const flipper_arf_kia_v7 = {
    .name        = "KIA/HYU V7 (Flipper-ARF)",
    .modulation  = OOK_PULSE_MANCHESTER_ZEROBIT,
    .short_width = 250,
    .long_width  = 500,
    .reset_limit = 10000,
    .decode_fn   = &kia_v7_arf_decode,
    .fields      = kia_v7_arf_fields,
};
