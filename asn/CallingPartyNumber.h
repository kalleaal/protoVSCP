/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "INAP"
 * 	found in "INAP_DEF.asn"
 */

#ifndef	_CallingPartyNumber_H_
#define	_CallingPartyNumber_H_


#include <asn_application.h>

/* Including external dependencies */
#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CallingPartyNumber */
typedef OCTET_STRING_t	 CallingPartyNumber_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CallingPartyNumber;
asn_struct_free_f CallingPartyNumber_free;
asn_struct_print_f CallingPartyNumber_print;
asn_constr_check_f CallingPartyNumber_constraint;
ber_type_decoder_f CallingPartyNumber_decode_ber;
der_type_encoder_f CallingPartyNumber_encode_der;
xer_type_decoder_f CallingPartyNumber_decode_xer;
xer_type_encoder_f CallingPartyNumber_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _CallingPartyNumber_H_ */
#include <asn_internal.h>
