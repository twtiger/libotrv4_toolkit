#include "../helper.c"
#include "test_helpers.h"

void otrng_toolkit_test_calculate_mac_key() {
  char *enc_key =
      "e90645447da92df66fbdf3b399078c2e01b28b30c60b2df726ce4fd82ba03971";

  const uint8_t mac_data[DATA_MSG_MAC_BYTES] = {
      0x50, 0x07, 0x3a, 0x41, 0xe8, 0x08, 0x3f, 0xd8, 0xb0, 0xc9, 0x77,
      0xdd, 0x99, 0xd4, 0x65, 0xe2, 0xe2, 0x8a, 0x70, 0xc2, 0xc5, 0x0d,
      0x4d, 0xd4, 0xfa, 0xd1, 0x02, 0x38, 0xd4, 0xe0, 0xdc, 0x6b, 0x33,
      0xa3, 0x1b, 0x08, 0x81, 0x57, 0x28, 0xe2, 0x10, 0x3c, 0x77, 0x62,
      0xdf, 0x3d, 0x47, 0x8c, 0x3f, 0xde, 0x27, 0xbb, 0x4b, 0x13, 0xa2,
      0xd7, 0xd0, 0xd7, 0x11, 0x7b, 0x28, 0xd9, 0xac, 0x90};

  unsigned char *argbuf;
  size_t argbuflen;
  msg_mac_key_p mac_key;

  argv_to_buf(&argbuf, &argbuflen, enc_key);

  calculate_mac(mac_key, argbuf);

  otrng_toolkit_assert_cmpmem(mac_key, mac_data, DATA_MSG_MAC_BYTES);
  free(argbuf);
}
