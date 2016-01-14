/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:  jichao                                                            |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_APPLE_PUSH_H
#define PHP_APPLE_PUSH_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <SAPI.h>
#include <ext/standard/info.h>
#include <Zend/zend_extensions.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DEVICE_TOKEN_BIN_SIZE 32

extern zend_module_entry apple_push_module_entry;
#define phpext_apple_push_ptr &apple_push_module_entry

#define PHP_APPLE_PUSH_VERSION "1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_APPLE_PUSH_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_APPLE_PUSH_API __attribute__ ((visibility("default")))
#else
#	define PHP_APPLE_PUSH_API
#endif

PHP_MINIT_FUNCTION(apple_push);
PHP_MSHUTDOWN_FUNCTION(apple_push);
PHP_RINIT_FUNCTION(apple_push);
PHP_RSHUTDOWN_FUNCTION(apple_push);
PHP_MINFO_FUNCTION(apple_push);


#ifdef ZTS
#include "TSRM.h"
#endif

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(apple_push)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(apple_push)
*/

/* In every utility function you add that needs to use variables 
   in php_apple_push_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as APPLE_PUSH_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define APPLE_PUSH_G(v) TSRMG(apple_push_globals_id, zend_apple_push_globals *, v)
#else
#define APPLE_PUSH_G(v) (apple_push_globals.v)
#endif


#if (PHP_MAJOR_VERSION >= 5)
ZEND_BEGIN_ARG_INFO_EX(apple_push__send_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
  ZEND_ARG_INFO(0, x)
  ZEND_ARG_INFO(0, y)
ZEND_END_ARG_INFO()
#else /* PHP 4.x */
#define apple_push__send_args NULL
#endif

#if (PHP_MAJOR_VERSION >= 5)
ZEND_BEGIN_ARG_INFO_EX(apple_push__connect_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()
#else /* PHP 4.x */
#define apple_push__send_args NULL
#endif



#endif	/* PHP_APPLE_PUSH_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
