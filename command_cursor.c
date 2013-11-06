/**
 *  Copyright 2013 MongoDB, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <php.h>
#include <zend_interfaces.h>
#include <zend_exceptions.h>

#include "cursor.h"
#include "cursor_shared.h"

/* externs */
extern zend_class_entry *mongo_ce_MongoClient;
extern zend_class_entry *mongo_ce_Exception, *mongo_ce_CursorException;
extern zend_object_handlers mongo_default_handlers;

ZEND_EXTERN_MODULE_GLOBALS(mongo)

zend_class_entry *mongo_ce_CommandCursor = NULL;


MONGO_ARGINFO_STATIC ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_OBJ_INFO(0, connection, MongoClient, 0)
	ZEND_ARG_INFO(0, database_and_collection_name)
	ZEND_ARG_INFO(0, query)
	ZEND_ARG_INFO(0, array_of_fields_OR_object)
ZEND_END_ARG_INFO()

MONGO_ARGINFO_STATIC ZEND_BEGIN_ARG_INFO_EX(arginfo_no_parameters, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

MONGO_ARGINFO_STATIC ZEND_BEGIN_ARG_INFO_EX(arginfo_limit, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

MONGO_ARGINFO_STATIC ZEND_BEGIN_ARG_INFO_EX(arginfo_batchsize, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

MONGO_ARGINFO_STATIC ZEND_BEGIN_ARG_INFO_EX(arginfo_skip, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

MONGO_ARGINFO_STATIC ZEND_BEGIN_ARG_INFO_EX(arginfo_add_option, 0, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

/* {{{ Cursor flags */
MONGO_ARGINFO_STATIC ZEND_BEGIN_ARG_INFO_EX(arginfo_set_flag, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, bit)
	ZEND_ARG_INFO(0, set)
ZEND_END_ARG_INFO()

MONGO_ARGINFO_STATIC ZEND_BEGIN_ARG_INFO_EX(arginfo_immortal, 0, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, liveForever)
ZEND_END_ARG_INFO()
/* }}} */

MONGO_ARGINFO_STATIC ZEND_BEGIN_ARG_INFO_EX(arginfo_setReadPreference, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, read_preference)
	ZEND_ARG_ARRAY_INFO(0, tags, 0)
ZEND_END_ARG_INFO()

/* {{{ MongoCommandCursor::__construct(MongoClient connection, string ns [, array query])
   Constructs a MongoCursor */
PHP_METHOD(MongoCommandCursor, __construct)
{
	zval *zlink = 0, *zcommand = 0;
	char *ns;
	int   ns_len;
	mongo_command_cursor *cmd_cursor;
	mongoclient          *link;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Osa", &zlink, mongo_ce_MongoClient, &ns, &ns_len, &zcommand) == FAILURE) {
		return;
	}

	if (!php_mongo_is_valid_namespace(ns, ns_len)) {
		php_mongo_cursor_throw(mongo_ce_CursorException, NULL, 21 TSRMLS_CC, "An invalid 'ns' argument is given (%s)", ns);
		return;
	}

	cmd_cursor = (mongo_command_cursor*) zend_object_store_get_object(getThis() TSRMLS_CC);
	link = (mongoclient*) zend_object_store_get_object(zlink TSRMLS_CC);
	MONGO_CHECK_INITIALIZED(link->manager, MongoClient);

	/* ns */
	cmd_cursor->ns = estrdup(ns);

	/* query */
	cmd_cursor->query = zcommand;
	zval_add_ref(&zcommand);

	/* reset iteration pointer and flags */
	php_mongo_cursor_reset(cmd_cursor TSRMLS_CC);
	cmd_cursor->special = 0;
}
/* }}} */

static zend_function_entry MongoCommandCursor_methods[] = {
	PHP_ME(MongoCursor, __construct, arginfo___construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
	PHP_ME(MongoCursor, hasNext, arginfo_no_parameters, ZEND_ACC_PUBLIC)
	PHP_ME(MongoCursor, getNext, arginfo_no_parameters, ZEND_ACC_PUBLIC)

	/* options */
	PHP_ME(MongoCursor, limit, arginfo_limit, ZEND_ACC_PUBLIC)
	PHP_ME(MongoCursor, batchSize, arginfo_batchsize, ZEND_ACC_PUBLIC)
	PHP_ME(MongoCursor, skip, arginfo_skip, ZEND_ACC_PUBLIC)

	/* meta options */
	PHP_ME(MongoCursor, addOption, arginfo_add_option, ZEND_ACC_PUBLIC)
	PHP_ME(MongoCursor, snapshot, arginfo_no_parameters, ZEND_ACC_PUBLIC)
	PHP_ME(MongoCursor, explain, arginfo_no_parameters, ZEND_ACC_PUBLIC)

	/* flags */
	PHP_ME(MongoCursor, setFlag, arginfo_set_flag, ZEND_ACC_PUBLIC)
	PHP_ME(MongoCursor, immortal, arginfo_immortal, ZEND_ACC_PUBLIC)

	/* read preferences */
	PHP_ME(MongoCursor, getReadPreference, arginfo_no_parameters, ZEND_ACC_PUBLIC)
	PHP_ME(MongoCursor, setReadPreference, arginfo_setReadPreference, ZEND_ACC_PUBLIC)

	/* query */
	PHP_ME(MongoCursor, info, arginfo_no_parameters, ZEND_ACC_PUBLIC)
	PHP_ME(MongoCursor, dead, arginfo_no_parameters, ZEND_ACC_PUBLIC)

	/* iterator funcs */
	PHP_ME(MongoCursor, current, arginfo_no_parameters, ZEND_ACC_PUBLIC)
	PHP_ME(MongoCursor, key, arginfo_no_parameters, ZEND_ACC_PUBLIC)
	PHP_ME(MongoCursor, next, arginfo_no_parameters, ZEND_ACC_PUBLIC)
	PHP_ME(MongoCursor, rewind, arginfo_no_parameters, ZEND_ACC_PUBLIC)
	PHP_ME(MongoCursor, valid, arginfo_no_parameters, ZEND_ACC_PUBLIC)
	PHP_ME(MongoCursor, reset, arginfo_no_parameters, ZEND_ACC_PUBLIC)

	PHP_FE_END
};

/* The PHP_MONGO_OBJ_NEW macro needs a specifically named function for free, but
 * as mongo_cursor is both used for normal and command cursors, we need this extra
 * intermediary function */
void php_mongo_command_cursor_free(void *object TSRMLS_DC)
{
	php_mongo_cursor_free(object TSRMLS_CC);
}

static zend_object_value php_mongo_command_cursor_new(zend_class_entry *class_type TSRMLS_DC)
{
	PHP_MONGO_OBJ_NEW(mongo_command_cursor);
}

void mongo_init_MongoCommandCursor(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "MongoCommandCursor", MongoCommandCursor_methods);
	ce.create_object = php_mongo_command_cursor_new;
	mongo_ce_CommandCursor = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(mongo_ce_CommandCursor TSRMLS_CC, 1, zend_ce_iterator);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
