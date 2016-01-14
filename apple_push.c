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
  | Author: jichao                                                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_apple_push.h"

SSL *ssl;

/* If you declare any globals in php_apple_push.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(apple_push)
*/

/* True global resources - no need for thread safety here */
static int le_apple_push;


static zend_class_entry * apple_push_ce_ptr = NULL;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("apple_push.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_apple_push_globals, apple_push_globals)
    STD_PHP_INI_ENTRY("apple_push.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_apple_push_globals, apple_push_globals)
PHP_INI_END()
*/
/* }}} */

static bool send_payload(SSL *sslPtr, char *devicetokenbinary, char *payloadBuff, size_t payloadLength)
{
    bool rtn = 0;
    if (sslPtr && devicetokenbinary && payloadBuff && payloadLength)
    {   
        /*
        int tokenSegementLen = sizeof(uint8_t) + sizeof(uint16_t) + DEVICE_TOKEN_BIN_SIZE;
        int payloadSegementLen = sizeof(uint8_t) + sizeof(uint16_t) + PAYLOAD_SIZE;
        int identifierSegementLen = sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t);
        int expirationSegementLen = sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t);
        int prioritySegementLen = sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint8_t);
        */
       
        int frameDataLen = 4096;

        char frameData[frameDataLen];
        char *frameDataPt = frameData;
        
    
        //token 数据段
        uint8_t itemTokenId = 1;
        *frameDataPt++ = itemTokenId;
        
        uint16_t itemTokenLen = htons(DEVICE_TOKEN_BIN_SIZE);
        memcpy(frameDataPt, &itemTokenLen, sizeof(uint16_t));
        frameDataPt += sizeof(uint16_t);

        memcpy(frameDataPt, devicetokenbinary, DEVICE_TOKEN_BIN_SIZE);
        frameDataPt += DEVICE_TOKEN_BIN_SIZE;
        //token 数据段结束

        //payload数据段
        uint8_t itemPayloadId = 2;
        *frameDataPt++ = itemPayloadId;

        uint16_t itemPayloadLen = htons(payloadLength);
        memcpy(frameDataPt, &itemPayloadLen, sizeof(uint16_t));
        frameDataPt += sizeof(uint16_t);

        memcpy(frameDataPt, payloadBuff, payloadLength);
        frameDataPt += payloadLength;
        //playload数据段结束
        

        //identifier数据段
        uint8_t itemIdentifierId = 3;
        *frameDataPt++ = itemIdentifierId;
        
        uint32_t identifer = htonl(0);
        uint16_t itemIdentifierLen = htons(sizeof(identifer));

        memcpy(frameDataPt, &itemIdentifierLen, sizeof(uint16_t));
        frameDataPt += sizeof(uint16_t);
        
        memcpy(frameDataPt, &identifer, sizeof(uint32_t));
        frameDataPt += sizeof(uint32_t);
        //identifier数据段结束

        //Expiration数据段
        uint8_t itemExpirationId = 4;
        *frameDataPt++ = itemExpirationId;
        
        uint32_t curtime = htonl(time(NULL)+86400);
        uint16_t itemExpirationLen = htons(sizeof(curtime)); 
        memcpy(frameDataPt, &itemExpirationLen, sizeof(uint16_t));
        frameDataPt += sizeof(uint16_t);

        memcpy(frameDataPt, &curtime, sizeof(uint32_t));
        frameDataPt += sizeof(uint32_t);
        //Expiration结束
        
        //priority数据段
        uint8_t itemPriorityId = 5;
        *frameDataPt++ = itemPriorityId;
    
        uint8_t itemPriority = 10;

        uint16_t itemPriorityLen = htons(sizeof(itemPriority));
        memcpy(frameDataPt, &itemPriorityLen, sizeof(uint16_t));
        frameDataPt += sizeof(uint16_t);


        memcpy(frameDataPt, &itemPriority, sizeof(uint8_t));
        frameDataPt += sizeof(uint8_t);
        //priority 数据段结束
        
        int frameLen;
        frameLen = frameDataPt - frameData;
        
        int binLen = sizeof(uint8_t) + sizeof(uint32_t) + frameLen;
        char binData[binLen];

        char * binPt = binData;
        *binPt++ = 2;
        
        uint32_t frameBinLen = htonl(frameLen);
        memcpy(binPt, &frameBinLen, sizeof(uint32_t));
        binPt += sizeof(uint32_t);

        memcpy(binPt, frameData, frameLen);
        binPt += frameLen; 

        if (SSL_write(sslPtr, binData, (binPt - binData)) > 0)
            rtn = 1;  
    }
    return rtn;
}


void string_to_bytes( const char *token, char *bytes)
{
    int val;
    while (*token)
    {
        sscanf(token, "%2x", &val);
        *(bytes++) = (char)val;
        token += 2;
    }
}
/* {{{ proto string connect()
    Connection apple apple_push service  */
PHP_METHOD(apple_push, init)
{
    zend_class_entry * _this_ce;

    zval * _this_zval = NULL;
    const char * x = NULL;
    int x_len = 0;
    const char * y = NULL;
    int y_len = 0;
    BIO *bio;
    SSL_CTX  *ssl_ctx;
    SSL_library_init();
    char *certificate_dir,*passphraseStr,*gatewayurl;
    zval *certificate = zend_read_static_property(apple_push_ce_ptr, ZEND_STRL("certificate"), sizeof ("certificate"));
    spprintf(&certificate_dir, 0, "%s", Z_STRVAL_P(certificate));
    zval *passphrase = zend_read_static_property(apple_push_ce_ptr, ZEND_STRL("passphrase"), sizeof ("passphrase"));
    spprintf(&passphraseStr, 0, "%s", Z_STRVAL_P(passphrase));
    zval *gateway = zend_read_static_property(apple_push_ce_ptr, ZEND_STRL("gateway"), sizeof ("gateway"));
    spprintf(&gatewayurl, 0, "%s", Z_STRVAL_P(gateway));
    ssl_ctx = SSL_CTX_new(SSLv23_method());
    if (ssl_ctx == NULL)
    {
        zend_throw_exception_ex(apple_push_ce_ptr, 0 TSRMLS_CC, "SSL_INIT_FAIL",  2 TSRMLS_CC);
        return ;
    }
    if (SSL_CTX_use_certificate_chain_file(ssl_ctx, certificate_dir) != 1)
    {
        zend_throw_exception_ex(apple_push_ce_ptr, 0 TSRMLS_CC, "CERTIFICATE_NOT_FOUNT",  2 TSRMLS_CC);
        return ;
    }
    SSL_CTX_set_default_passwd_cb_userdata(ssl_ctx,passphraseStr);

    if (SSL_CTX_use_PrivateKey_file(ssl_ctx, certificate_dir, SSL_FILETYPE_PEM) != 1)
    {
        zend_throw_exception_ex(apple_push_ce_ptr, 0 TSRMLS_CC, "CERTIFICATE_PASSWORD_FAIL",  2 TSRMLS_CC);
        return ;
    }
    bio = BIO_new_connect(gatewayurl);
    if(!bio)
    {
        zend_throw_exception_ex(apple_push_ce_ptr, 0 TSRMLS_CC, "SSL_CONNECT_FAIL",  2 TSRMLS_CC);
        return ;
    }
    if (BIO_do_connect(bio) <= 0)
    {
        zend_throw_exception_ex(apple_push_ce_ptr, 0 TSRMLS_CC, "SSL_CONNECT_FAIL",  2 TSRMLS_CC);
        return ;
    }
    if (!(ssl = SSL_new(ssl_ctx)))
    {
        zend_throw_exception_ex(apple_push_ce_ptr, 0 TSRMLS_CC, "SSL_CONNECT_FAIL",  2 TSRMLS_CC);
        return ;
    }
    SSL_set_bio(ssl, bio, bio);
    if (SSL_connect(ssl) <= 0)
    {
        zend_throw_exception_ex(apple_push_ce_ptr, 0 TSRMLS_CC, "SSL_CONNECT_FAIL",  2 TSRMLS_CC);
        return ;
    }
    RETURN_TRUE;
}
/* }}} */

/* {{{ proto string send(string token,string message)
     Send the apple_push data format  */
PHP_METHOD(apple_push, send)
{
    zend_class_entry * _this_ce;
    zval * _this_zval = NULL;
    const char * x = NULL;
    int x_len = 0;
    const char * y = NULL;
    int y_len = 0;
    if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "ss",   &x, &x_len,&y,&y_len) == FAILURE)
    {
        return;
    }
    char tokenbytes[32];
    string_to_bytes(x, tokenbytes);
    if(send_payload(ssl, tokenbytes, (char*)y,strlen( (char*)y))>=1)
    {
        RETURN_TRUE;
    }
    else
    {
        RETVAL_FALSE;
    }
}
/* }}} */

static zend_function_entry apple_push_methods[] =
{
    PHP_ME(apple_push, init, apple_push__connect_args, /**/ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    PHP_ME(apple_push, send, apple_push__send_args, /**/ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    {
        NULL, NULL, NULL
    }
};

static void class_init_apple_push(void)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "apple_push", apple_push_methods);
    apple_push_ce_ptr = zend_register_internal_class(&ce);
    zend_declare_property_string(apple_push_ce_ptr, "certificate", strlen("certificate"), "null", ZEND_ACC_STATIC TSRMLS_CC);
    zend_declare_property_string(apple_push_ce_ptr, "gateway", strlen("gateway"), "gateway.push.apple.com:2195", ZEND_ACC_STATIC TSRMLS_CC);
    zend_declare_property_string(apple_push_ce_ptr, "passphrase", strlen("passphrase"), "", ZEND_ACC_STATIC TSRMLS_CC);

}

/* {{{ php_apple_push_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_apple_push_init_globals(zend_apple_push_globals *apple_push_globals)
{
	apple_push_globals->global_value = 0;
	apple_push_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(apple_push)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
    //ZEND_INIT_MODULE_GLOBALS(apple_push, php_apple_push_init_globals, php_apple_push_shutdown_globals)
    //REGISTER_INI_ENTRIES();
    REGISTER_LONG_CONSTANT("apple_push_version", 1.0, CONST_PERSISTENT | CONST_CS);
    class_init_apple_push();
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(apple_push)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(apple_push)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(apple_push)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(apple_push)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "apple_push support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ apple_push_functions[]
 *
 * Every user visible function must have an entry in apple_push_functions[].
 */
const zend_function_entry apple_push_functions[] = {
	//PHP_FE(confirm_apple_push_compiled,	NULL)		/* For testing, remove later. */
	//PHP_FE_END	/* Must be the last line in apple_push_functions[] */
};
/* }}} */

/* {{{ apple_push_module_entry
 */
zend_module_entry apple_push_module_entry = {
	STANDARD_MODULE_HEADER,
	"apple_push",
	apple_push_functions,
	PHP_MINIT(apple_push),
	PHP_MSHUTDOWN(apple_push),
	PHP_RINIT(apple_push),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(apple_push),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(apple_push),
	PHP_APPLE_PUSH_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_APPLE_PUSH
ZEND_GET_MODULE(apple_push)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
