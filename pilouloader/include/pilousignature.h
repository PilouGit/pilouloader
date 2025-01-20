#ifndef __PILOUSIGNATURE
#define __PILOUSIGNATURE
#include "php.h"

typedef struct _signedconfiguration signedconfiguration;

signedconfiguration * create_signedconfiguration_json(zval * );
zend_string * get_publickey(signedconfiguration *);

void delete_signedconfiguration(signedconfiguration *);
zend_string * get_signature(signedconfiguration * , zend_string * filename);

unsigned char * compute_hash(signedconfiguration * configuration,php_stream * stream);

bool check_integrity(signedconfiguration * configuration,zend_string * filename, unsigned char * signature);
#endif