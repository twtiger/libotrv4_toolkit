#include <stdio.h>
#include <string.h>

#include <libotr4/otrv4.h>
#include <libotr4/b64.h>
#include <libotr4/constants.h>
#include <libotr4/data_message.h>
#include <libotr4/dh.h>

typedef struct {
   uint8_t * b64_msg;
   size_t b64_msg_len;
   int type;
   int version;
   int sender_instance_tag;
   int receiver_instance_tag;
   ec_point_t our_ecdh;
   dh_public_key_t our_dh;
   char nonce[DATA_MSG_NONCE_BYTES];
   uint8_t * ciphertext;
   int ciphertext_len;
   char mac[DATA_MSG_MAC_BYTES];
} encoded_msg_t;

int
parse(encoded_msg_t *dst, const char * src, const int src_len);

int
parse(encoded_msg_t * dst, const char * src, const int src_len) {
    if (NULL == src) {
        return 1;
    }

    if (src_len <= 0) {
        return 1;
    }

    int otr_type = (int) get_message_type(src);
    if (otr_type != IN_MSG_OTR_ENCODED) {
        return 1;
    }

    int err = otrl_base64_otr_decode(src, &dst->b64_msg, &dst->b64_msg_len);
    if (err) {
       return 1;
    }

    otrv4_header_t header;
    if (!extract_header(&header, dst->b64_msg, dst->b64_msg_len)) {
	return 1;
    }

    if (header.type != OTR_DATA_MSG_TYPE) {
        return 1;
    }

    dst->type = header.type;
    dst->version = header.version;

    data_message_t * data = malloc(sizeof(data_message_t));
    if (!data) {
	return 1;
    }

    if (!data_message_deserialize(data, dst->b64_msg, dst->b64_msg_len)) {
       data_message_free(data);
       return 1;
    }

    dst->sender_instance_tag = data->sender_instance_tag;
    dst->receiver_instance_tag = data->receiver_instance_tag;
    *dst->our_ecdh = *data->our_ecdh;
    dst->our_dh = data->our_dh;
    memcpy(dst->nonce, data->nonce, DATA_MSG_NONCE_BYTES);

    dst->ciphertext = malloc(data->enc_msg_len);
    if (!dst->ciphertext) {
	free(data);
	return 1;
    }

    // XXX: check where old mackeys are deser
    memcpy(dst->ciphertext, data->enc_msg, data->enc_msg_len);
    dst->ciphertext_len = data->enc_msg_len;
    memcpy(dst->mac, data->mac, DATA_MSG_MAC_BYTES);

    data_message_free(data);

    return 0;
}

void encoded_message_destroy(encoded_msg_t * enc_msg)
{
	enc_msg->b64_msg_len = 0;
        free(enc_msg->b64_msg);
	enc_msg->b64_msg = NULL;

	enc_msg->ciphertext_len = 0;
	free(enc_msg->ciphertext);
	enc_msg->ciphertext = NULL;
}

void encoded_message_free(encoded_msg_t * enc_msg)
{
	if (!enc_msg)
		return;

	encoded_message_destroy(enc_msg);

	free(enc_msg);
}
