
#include "php.h"
#include "zend_compile.h"
#include "ext/json/php_json.h"
#include "pilousignature.h"
#include "piloucustomhandler.h"
#include <sodium.h>
// Sauvegarde de l'ancien handler
static const char * config_file_path=NULL;
signedconfiguration * signed_configuration;
 zend_op_array *(*old_compile_file)(zend_file_handle *file_handle, int type);
zend_op_array *(*old_compile_string)(zend_string *source_string, const char *filename, zend_compile_position position);



static PHP_INI_MH(OnUpdateConfigurationFile)
{
     if (stage == PHP_INI_STAGE_RUNTIME) {
        php_error_docref(NULL, E_WARNING, "config_file_path cannot be changed at runtime.");
        return FAILURE;
    }

    config_file_path = ZSTR_VAL(new_value);
    return SUCCESS;
}
ZEND_INI_BEGIN()
//name, default_value, modifiable, on_modify, property_name, struct_type, struct_ptr)
    ZEND_INI_ENTRY("pilouloader.config_file", "", PHP_INI_SYSTEM, OnUpdateConfigurationFile)
    
ZEND_INI_END()
// Initialisation de l'extension
PHP_MINIT_FUNCTION(pilouloader)
{
     REGISTER_INI_ENTRIES();
    fprintf(stderr, "startup here\n ");
    if (sodium_init() < 0)
    {
        printf("Erreur d'initialisation de libsodium\n");
        return FAILURE;
    }
    fprintf(stderr, "end of sodium here\n ");

   
    old_compile_file = zend_compile_file;
    zend_compile_file = custom_compile_file;
    old_compile_string=zend_compile_string;
    zend_compile_string=custom_compile_string;
    return SUCCESS;
}
PHP_MINFO_FUNCTION(pilouloader)
{
    DISPLAY_INI_ENTRIES();
}
// Restauration à la désinstallation
PHP_MSHUTDOWN_FUNCTION(pilouloader)
{
    UNREGISTER_INI_ENTRIES();

    fprintf(stderr, "shutdown\n ");
    zend_compile_file = old_compile_file;
    zend_compile_string=old_compile_string;
    return SUCCESS;
}
PHP_RINIT_FUNCTION(pilouloader) {
      php_printf("PHP_RINIT_FUNCTION\n");

    if (config_file_path == NULL || config_file_path[0] == '\0') {
        php_error_docref(NULL, E_ERROR, "No configuration file specified in 'pilouloader.config_file'.");
        return FAILURE;
    }

    php_printf("Using configuration file: %s\n", config_file_path);

   
 php_stream * stream = php_stream_open_wrapper(config_file_path, "r", REPORT_ERRORS | IGNORE_URL, NULL); 
    fprintf(stderr, "stream not null\n ");
    if (!stream) {
        php_error_docref(NULL, E_WARNING, "Impossible d'ouvrir le fichier : %s", config_file_path);
        return FAILURE;
    }
    
    // Lire tout le contenu du fichier
    zend_string * file_content = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL , 1);

    // Fermer le flux
    php_stream_close(stream);
    zval result;
     if (php_json_decode_ex(&result, file_content->val, file_content->len, 0, PHP_JSON_PARSER_DEFAULT_DEPTH) == FAILURE) {
         return FAILURE;
    }
    signed_configuration=create_signedconfiguration_json(&result);
    if (signed_configuration==NULL) php_error_docref(NULL, E_ERROR, "Error in reading Configuration File: %s", config_file_path);
    return SUCCESS;
}




// Nouveau handler pour intercepter les fichiers

// Déclaration du module
zend_module_entry pilouloader_module_entry = {
    STANDARD_MODULE_HEADER,
    "pilouloader",
    NULL,
     PHP_MINIT(pilouloader),
    PHP_MSHUTDOWN(pilouloader),
    PHP_RINIT(pilouloader),
    NULL,
    NULL,
    "0.1",
    STANDARD_MODULE_PROPERTIES};

ZEND_GET_MODULE(pilouloader)