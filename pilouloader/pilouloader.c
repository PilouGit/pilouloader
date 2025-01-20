
#include "php.h"
#include "zend_compile.h"
#include "ext/json/php_json.h"
#include "pilousignature.h"
// Sauvegarde de l'ancien handler
static zend_op_array *(*old_compile_file)(zend_file_handle *file_handle, int type);

// Fonction personnalisée de déchiffrement
char *decrypt_file_content(const char *encrypted_content, size_t length)
{
    char *decrypted = emalloc(length + 1);
    for (size_t i = 0; i < length; i++)
    {
        decrypted[i] = encrypted_content[i] ^ 0x55; // Exemple de XOR simple
    }
    decrypted[length] = '\0';
    return decrypted;
}

// Nouveau handler pour intercepter les fichiers
zend_op_array *custom_compile_file(zend_file_handle *file_handle, int type)
{
   

    php_stream * stream = php_stream_open_wrapper("/home/pilou/securephp/pilousigner/command/test.json", "r", REPORT_ERRORS | IGNORE_URL, NULL); 
    fprintf(stderr, "stream not null\n ");
    if (!stream) {
        php_error_docref(NULL, E_WARNING, "Impossible d'ouvrir le fichier : %s", "/home/pilou/securephp/pilousigner/command/test.json");
        return FAILURE;
    }
fprintf(stderr, "read json\n ");
    
    // Lire tout le contenu du fichier
    zend_string * file_content = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL , 1);

    // Fermer le flux
    php_stream_close(stream);
    zval result;
     if (php_json_decode_ex(&result, file_content->val, file_content->len, 0, PHP_JSON_PARSER_DEFAULT_DEPTH) == FAILURE) {
         return FAILURE;
    }
signedconfiguration * signedconfigurat=create_signedconfiguration_json(&result);

    fprintf(stderr, "ici custom_compile_file %s\n", ZSTR_VAL(file_handle->opened_path));
    // Lecture du fichier
    zend_op_array *op_array;
    if (file_handle->opened_path)
    {
        php_stream *stream = php_stream_open_wrapper(ZSTR_VAL(file_handle->opened_path), "rb", REPORT_ERRORS, NULL);
        unsigned char * signature=compute_hash(signedconfigurat,stream);
         
        // Afficher les résultats
        if (! check_integrity(signedconfigurat,ZSTR_VAL(file_handle->opened_path), signature)){
            php_error_docref(NULL, E_NOTICE,"Debug: signature_value de %s est coromppue ", ZSTR_VAL(file_handle->opened_path));
            zend_throw_error(NULL, "Cannot signature_value de %s est coromppue ", ZSTR_VAL(file_handle->opened_path));
 return FAILURE;
        }


    
    }

    // Si le fichier n'est pas chiffré, utilisez l'ancien handler
    return old_compile_file(file_handle, type);
}

// Initialisation de l'extension
PHP_MINIT_FUNCTION(custom_loader)
{
    fprintf(stderr, "startup here\n ");
    if (sodium_init() < 0)
    {
        printf("Erreur d'initialisation de libsodium\n");
        return FAILURE;
    }
    fprintf(stderr, "end of sodium here\n ");

   
    old_compile_file = zend_compile_file;
    zend_compile_file = custom_compile_file;
    return SUCCESS;
}

// Restauration à la désinstallation
PHP_MSHUTDOWN_FUNCTION(custom_loader)
{
    fprintf(stderr, "shutdown\n ");
    zend_compile_file = old_compile_file;
    return SUCCESS;
}

// Déclaration du module
zend_module_entry custom_loader_module_entry = {
    STANDARD_MODULE_HEADER,
    "custom_loader",
    NULL,
    PHP_MINIT(custom_loader),
    PHP_MSHUTDOWN(custom_loader),
    NULL,
    NULL,
    NULL,
    "0.1",
    STANDARD_MODULE_PROPERTIES};

ZEND_GET_MODULE(custom_loader)