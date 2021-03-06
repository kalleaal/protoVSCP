/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "TCAPMessages"
 * 	found in "tcap.asn"
 */

#ifndef	_Continue_H_
#define	_Continue_H_


#include <asn_application.h>

/* Including external dependencies */
#include "OrigTransactionID.h"
#include "DestTransactionID.h"
#include "DialoguePortion.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct ComponentPortion;

/* Continue */
typedef struct Continue {
	OrigTransactionID_t	 otid;
	DestTransactionID_t	 dtid;
	DialoguePortion_t	*dialoguePortion	/* OPTIONAL */;
	struct ComponentPortion	*components	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Continue_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Continue;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "ComponentPortion.h"

#endif	/* _Continue_H_ */
#include <asn_internal.h>
