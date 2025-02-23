#ifndef __PILOUCUSTOMHANDLER
#define __PILOUCUSTOMHANDLER
#include "php.h"


zend_op_array *custom_compile_file(zend_file_handle *file_handle, int type);
 zend_op_array * custom_compile_string(zend_string *source_string, const char *filename, zend_compile_position position);
void custom_execute_ex(zend_execute_data *execute_data) ;

#endif