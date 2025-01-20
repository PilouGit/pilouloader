#include <sodium.h>
#include "php.h"
#include "zend_compile.h"

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
    fprintf(stderr, "ici custom_compile_file %s\n", ZSTR_VAL(file_handle->opened_path));
    // Lecture du fichier
    zend_op_array *op_array;
    if (file_handle->opened_path)
    {
        php_stream *stream = php_stream_open_wrapper(file_handle->opened_path, "rb", REPORT_ERRORS, NULL);
         char buffer[1024];
        size_t bytes_read;
         crypto_generichash_state state;
   
 while ((bytes_read = php_stream_read(stream, buffer, sizeof(buffer) - 1)) > 0) {
        crypto_generichash_update(&state,buffer,bytes_read);
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