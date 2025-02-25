#include "pilousignature.h"
#include "zend_compile.h"
#include "ext/standard/base64.h"
#include <sodium.h>

struct _signedconfiguration
{

    zend_string *public_key;
    HashTable *signature;
    HashTable *eval_permissive;
};
 static  zend_string *public_key_property =NULL;
 static    zend_string *signatures_key_property  =NULL;
static     zend_string *file_key_property  =NULL;
 static    zend_string *signature_key_property  =NULL;
static     zend_string *eval_key_property  =NULL;
static     zend_string *unsafe  =NULL;
 static    zend_string *lax  =NULL;
 static     zval *lax_value;
static zval *unsafe_value;
signedconfiguration *init_signedconfiguration()
{
    signedconfiguration *result = (signedconfiguration *)emalloc(sizeof(signedconfiguration));
    HashTable *signatureHashTable;
    HashTable * evalPermissive;
    ALLOC_HASHTABLE(signatureHashTable);
    zend_hash_init(signatureHashTable, 0, NULL, ZVAL_PTR_DTOR, 0);
    ALLOC_HASHTABLE(evalPermissive);
    zend_hash_init(evalPermissive, 0, NULL, ZVAL_PTR_DTOR, 0);
    result->signature = signatureHashTable;
    result->eval_permissive=evalPermissive;
    return result;
}

signedconfiguration *create_signedconfiguration_json(zval *jsonPtr)
{
    signedconfiguration *result = init_signedconfiguration();
    public_key_property = zend_string_init("publicKey", strlen("publicKey"), 0);
    signatures_key_property = zend_string_init("signatures", strlen("signatures"), 0);
    file_key_property = zend_string_init("file", strlen("file"), 0);
    signature_key_property = zend_string_init("signature", strlen("signature"), 0);
    eval_key_property = zend_string_init("eval", strlen("eval"), 0);
    unsafe = zend_string_init("unsafe", strlen("unsafe"), 0);
    lax  = zend_string_init("lax", strlen("lax"), 0);
   lax_value  = (zval *) emalloc(sizeof(zval)); // Allouer mémoire
   unsafe_value  = (zval *) emalloc(sizeof(zval)); // Allouer mémoire
   
    ZVAL_LONG(lax_value, 1);
    ZVAL_LONG(unsafe_value, 2);

    zval *public_key_value;
    zval *signature_values;

    if (Z_TYPE_P(jsonPtr) == IS_OBJECT)
    {
        HashTable *obj_properties = Z_OBJPROP_P(jsonPtr);
        public_key_value = zend_hash_find(obj_properties, public_key_property);
         uint32_t size = zend_hash_num_elements(obj_properties);
                        
        if (public_key_value != NULL && Z_TYPE_P(public_key_value) == IS_STRING)
            {
                result->public_key = Z_STR_P(public_key_value);
            }
        else {
            php_error_docref(NULL, E_ERROR, " public key is not found or it is not a string");
        }
        
        signature_values = zend_hash_find(obj_properties, signatures_key_property);
        if (signature_values != NULL && Z_TYPE_P(signature_values) == IS_ARRAY)
        {

            zend_array *signatures = Z_ARR_P(signature_values);
            uint32_t size = zend_hash_num_elements(signatures);
            for (uint32_t i; i < size; i++)
            {
                zval *value = zend_hash_index_find(signatures, i);
                if (Z_TYPE_P(value) == IS_OBJECT)
                {
                    HashTable *signature = Z_OBJPROP_P(value);
                    zval *file_value = zend_hash_find(signature, file_key_property);
                    zval *signature_value = zend_hash_find(signature, signature_key_property);
                     zval *eval_value = zend_hash_find(signature, eval_key_property);
                   if (file_value!=NULL)   php_error_docref(NULL, E_NOTICE,"Debug: file_value est %i, %i ", file_value,Z_TYPE_P(file_value) );
              php_error_docref(NULL, E_NOTICE,"Debug: signature_value est %i, %i ", signature_value,Z_TYPE_P(signature_value) );
         
                    if (signature_value != NULL && Z_TYPE_P(signature_value) == IS_STRING && file_value != NULL && Z_TYPE_P(file_value) == IS_STRING)
                    {
                        zend_hash_update(result->signature, Z_STR_P(file_value), signature_value);
                        php_error_docref(NULL, E_NOTICE,"Debug: Le file est %s", ZSTR_VAL(Z_STR_P(file_value)) );
                        php_error_docref(NULL, E_NOTICE,"Debug: La valeur est %s", ZSTR_VAL(Z_STR_P(signature_value)) );
                        if (eval_value!=NULL &&  Z_TYPE_P(eval_value) == IS_STRING )
                        {
                            
  php_error_docref(NULL, E_NOTICE,"Debug: Le eval est %s %i", ZSTR_VAL(Z_STR_P(file_value)) ,zend_string_equals_ci(lax,Z_STR_P(eval_value)));
                        
                        if (   zend_string_equals_ci(unsafe,Z_STR_P(eval_value))) 
                        {
                           zend_hash_update(result->eval_permissive, Z_STR_P(file_value), unsafe_value);
                        }
                        if (zend_string_equals_ci(lax,Z_STR_P(eval_value))) zend_hash_update(result->eval_permissive, Z_STR_P(file_value), lax_value);
                        
                        }
       
                    }else
                    {
                          php_error_docref(NULL, E_ERROR, " signature is not a string");
       
                    }
                }
            }
        }else    php_error_docref(NULL, E_ERROR, " signature key is not found or it is not a array");
      
    };

    return result;
}

zend_string * get_publickey(signedconfiguration * configuration)
{
    return configuration->public_key;
}
zend_string * get_signature( signedconfiguration * configuration,zend_string * filename)
{
 zval *signature_value = zend_hash_find(configuration->signature, filename);
 if (signature_value!=NULL) return Z_STR_P(signature_value);
 else return NULL;
 }
int get_eval( signedconfiguration * configuration,zend_string * filename)
{
    php_error_docref(NULL, E_NOTICE,"Debug: Le eval est %s", filename->val);
 zval *eval_value = zend_hash_find(configuration->eval_permissive, filename);
 if (eval_value!=NULL && zval_get_long(eval_value)==zval_get_long(unsafe_value)) return 2 ;
 if (eval_value!=NULL && zval_get_long(eval_value)==zval_get_long(lax_value)) return 1 ;
 
 else return 0;
 }

 void delete_signedconfiguration(signedconfiguration * configuration)
 {
    efree(configuration);
 }


unsigned char * compute_hash(php_stream * stream)
 {
  char buffer[1024];
        size_t bytes_read;
        unsigned char * hash=(unsigned char *) malloc(sizeof(unsigned char)*32);
        crypto_hash_sha256_state state;

    // Initialiser le contexte pour SHA-256 (32 octets de sortie, pas de clé)
    if (crypto_hash_sha256_init(&state) != 0) {
        printf("Erreur lors de l'initialisation du contexte de hachage\n");
        return hash;
    }
 while ((bytes_read = php_stream_read(stream, buffer, sizeof(buffer) - 1)) > 0) {
        crypto_hash_sha256_update(&state,buffer,bytes_read);
 }
    if (crypto_hash_sha256_final(&state, hash) != 0) {
        printf("Erreur lors de la finalisation du hachage\n");
        return hash;
    }

    // Convertir le hachage en Base64
    /*char base64[sodium_base64_ENCODED_LEN(sizeof(hash), sodium_base64_VARIANT_ORIGINAL)];
    sodium_bin2base64(base64, sizeof(base64), hash, sizeof(hash), sodium_base64_VARIANT_ORIGINAL);
*/
    return  hash;
 }

 bool check_integrity(signedconfiguration * configuration,zend_string * filename, unsigned char * hashvalue) {

if (configuration==NULL) php_error_docref(NULL, E_ERROR,"Debug: Configuration  is NULL   ");
  zval *signature_value = zend_hash_find(configuration->signature, filename);
    bool result =false;
  if (signature_value!=NULL )
             {
            zend_string * signature= Z_STR_P(signature_value);
            zend_string * decoded_signature = php_base64_decode_ex((const unsigned char *)signature->val, signature->len, 1); // Strict mode
            zend_string * decoded_publickey = php_base64_decode_ex((const unsigned char *)configuration->public_key->val, configuration->public_key->len, 1); // Strict mode
             unsigned long long signed_message_len=32;
            result= crypto_sign_verify_detached(decoded_signature->val, hashvalue, 32, decoded_publickey->val)==0;

             }    
             return result; 
 }