#include "piloucustomhandler.h"
#include "pilousignature.h"
extern 
signedconfiguration * signed_configuration;
extern zend_op_array *(*old_compile_file)(zend_file_handle *file_handle, int type);
extern  zend_op_array *(*old_compile_string)(zend_string *source_string, const char *filename, zend_compile_position position);

 zend_op_array * custom_compile_string(zend_string *source_string, const char *filename, zend_compile_position position)
 {
      php_error_docref(NULL, E_NOTICE,"Debug: Eval de %s (%s) est ok ", source_string->val, filename);
     return old_compile_string(source_string, filename,position);
 }
zend_op_array *custom_compile_file(zend_file_handle *file_handle, int type)
{
   

   
    fprintf(stderr, "ici custom_compile_file %s\n", ZSTR_VAL(file_handle->opened_path));
    // Lecture du fichier
    zend_op_array *op_array;
    if (file_handle->opened_path)
    {
        php_stream *stream = php_stream_open_wrapper(ZSTR_VAL(file_handle->opened_path), "rb", REPORT_ERRORS, NULL);
        unsigned char * hashvalue=compute_hash(signed_configuration,stream);
         
        // Afficher les résultats
        if (! check_integrity(signed_configuration,file_handle->opened_path, hashvalue)){
            php_error_docref(NULL, E_NOTICE,"Debug: signature_value de %s est coromppue ", ZSTR_VAL(file_handle->opened_path));
            zend_throw_error(NULL, "Cannot signature_value de %s est coromppue ", ZSTR_VAL(file_handle->opened_path));
 return FAILURE;
        }else
        {
            php_error_docref(NULL, E_NOTICE,"Debug: signature_value de %s est ok ", ZSTR_VAL(file_handle->opened_path));
            
        }



    
    }else
    {
         php_error_docref(NULL, E_ERROR,"could not open stream of  %s  ", ZSTR_VAL(file_handle->opened_path));
           
    }

    // Si le fichier n'est pas chiffré, utilisez l'ancien handler
    return old_compile_file(file_handle, type);
}


