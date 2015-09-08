/*
 *  app-svc
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Jayoun Lee <airjany@samsung.com>, Sewook Park <sewook7.park@samsung.com>, Jaeho Lee <jaeho81.lee@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>

#include "appsvc_db.h"
#include "internal.h"


#define SVC_DB_PATH	"/opt/dbspace/.appsvc.db"
#define APP_INFO_DB_PATH	"/opt/dbspace/.pkgmgr_parser.db"

#define QUERY_MAX_LEN	8192
#define URI_MAX_LEN	4096
#define BUF_MAX_LEN	1024

#define APPSVC_COLLATION "appsvc_collation"

static sqlite3 *svc_db = NULL;
static sqlite3 *app_info_db = NULL;



/**
 * db initialize
 */
static int __init(void)
{
	int rc;

	if (svc_db) {
		_D("Already initialized\n");
		return 0;
	}

	rc = sqlite3_open(SVC_DB_PATH, &svc_db);
	if(rc) {
		_E("Can't open database: %d, %s, extended: %d", rc, sqlite3_errmsg(svc_db), sqlite3_extended_errcode(svc_db));
		goto err;
	}

	// Enable persist journal mode
	rc = sqlite3_exec(svc_db, "PRAGMA journal_mode = PERSIST", NULL, NULL, NULL);
	if(SQLITE_OK!=rc){
		_E("Fail to change journal mode\n");
		goto err;
	}

	return 0;
err:
	sqlite3_close(svc_db);
	svc_db = NULL;
	return -1;
}

static int __collate_appsvc(void *ucol, int str1_len, const void *str1, int str2_len, const void *str2)
{
	char *saveptr1 = NULL;
	char *saveptr2 = NULL;
	char *dup_str1;
	char *dup_str2;
	char *token;
	char *in_op;
	char *in_uri;
	char *in_mime;
	char *op;
	char *uri;
	char *mime;
	int i;

	if(str1 == NULL || str2 == NULL)
		return -1;

	dup_str1 = strdup(str1);
	dup_str2 = strdup(str2);

	in_op = strtok_r(dup_str2, "|", &saveptr1);
	in_uri = strtok_r(NULL, "|", &saveptr1);
	in_mime = strtok_r(NULL, "|", &saveptr1);

	if(!(in_op && in_uri && in_mime)) {
		_D("op(%s) uri(%s) mime(%s)", in_op, in_uri, in_mime);
		free(dup_str1);
		free(dup_str2);
		return -1;
	}

	token = strtok_r(dup_str1, ";", &saveptr1);

	if(token == NULL) {
		free(dup_str1);
		free(dup_str2);
		return -1;
	}

	do {
		//_D("token : %s", token);
		op = strtok_r(token, "|", &saveptr2);
		uri = strtok_r(NULL, "|", &saveptr2);
		mime = strtok_r(NULL, "|", &saveptr2);

		if(!(op && uri && mime)) {
			_D("op(%s) uri(%s) mime(%s)", op, uri, mime);
			continue;
		}

		if( (strcmp(op, in_op) == 0) && (strcmp(mime, in_mime) == 0) ) {
			SECURE_LOGD("%s %s %s %s %s %s", op, in_op, mime, in_mime, uri, in_uri);
			if(strcmp(uri, in_uri) == 0) {
				free(dup_str1);
				free(dup_str2);
				return 0;
			} else {
				for(i=0; uri[i]!=0; i++) {
					if(uri[i] == '*') {
						uri[i] = 0;
						if(strstr(in_uri, uri)) {
							SECURE_LOGD("in_uri : %s | uri : %s", in_uri, uri);
							free(dup_str1);
							free(dup_str2);
							return 0;
						}
					}
				}
			}
		}
	} while( (token = strtok_r(NULL, ";", &saveptr1)) );

	free(dup_str1);
	free(dup_str2);

	return -1;
}

static int __init_app_info_db(void)
{
	int rc;

	if (app_info_db) {
		_D("Already initialized\n");
		return 0;
	}

	rc = sqlite3_open(APP_INFO_DB_PATH, &app_info_db);
	if(rc) {
		_E("Can't open database: %d, %s, extended: %d", rc, sqlite3_errmsg(app_info_db), sqlite3_extended_errcode(app_info_db));
		goto err;
	}

	// Enable persist journal mode
	rc = sqlite3_exec(app_info_db, "PRAGMA journal_mode = PERSIST", NULL, NULL, NULL);
	if(SQLITE_OK!=rc){
		_D("Fail to change journal mode\n");
		goto err;
	}

	sqlite3_create_collation(app_info_db, APPSVC_COLLATION, SQLITE_UTF8, NULL,
                __collate_appsvc);

	return 0;
err:
	sqlite3_close(app_info_db);
	app_info_db = NULL;
	return -1;
}


static int __fini(void)
{
	if (svc_db) {
		sqlite3_close(svc_db);
		svc_db = NULL;
	}
	return 0;
}


int _svc_db_add_app(const char *op, const char *mime_type, const char *uri, const char *pkg_name)
{
	char m[BUF_MAX_LEN];
	char u[URI_MAX_LEN];
	const char insert_query[] = "insert into appsvc( operation, mime_type, uri, pkg_name) values(?,?,?,?)";
	sqlite3_stmt* p_statement;
	int result;

	if(__init()<0)
		return -1;

	if(op == NULL )
		return -1;

	if(mime_type==NULL)
		strncpy(m,"NULL",BUF_MAX_LEN-1);
	else
		strncpy(m,mime_type,BUF_MAX_LEN-1);

	if(uri==NULL)
		strncpy(u,"NULL",URI_MAX_LEN-1);
	else
		strncpy(u,uri,URI_MAX_LEN-1);


    result = sqlite3_prepare_v2(svc_db, insert_query, strlen(insert_query), &p_statement, NULL);
    if (result != SQLITE_OK) {
        _E("Sqlite3 error [%d] : <%s> preparing <%s> querry\n", result, sqlite3_errmsg(svc_db), insert_query);
		return -1;
    }

	sqlite3_bind_text(p_statement, 1, op, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(p_statement, 2, m, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(p_statement, 3, u, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(p_statement, 4, pkg_name, -1, SQLITE_TRANSIENT);


	result = sqlite3_step(p_statement);
	if (result != SQLITE_DONE) {
		_E("Sqlite3 error [%d] : <%s> executing statement\n", result, sqlite3_errmsg(svc_db));
	}

	result = sqlite3_finalize(p_statement);
	if (result != SQLITE_OK) {
		_E("Sqlite3 error [%d] : <%s> finalizing statement\n", result, sqlite3_errmsg(svc_db));
	}

	__fini();
	return 0;
}

int _svc_db_delete_with_pkgname(const char *pkg_name)
{
	const char delete_query[] = "delete from appsvc where pkg_name = ?;";
	sqlite3_stmt* p_statement;
	int result;

	if(pkg_name == NULL) {
		_E("Invalid argument: data to delete is NULL\n");
		return -1;
	}

	if(__init()<0)
		return -1;

	result = sqlite3_prepare_v2(svc_db, delete_query, strlen(delete_query), &p_statement, NULL);
	if (result != SQLITE_OK) {
		_E("Sqlite3 error [%d] : <%s> preparing <%s> querry\n", result, sqlite3_errmsg(svc_db), delete_query);
		return -1;
	}

	sqlite3_bind_text(p_statement, 1, pkg_name, -1, SQLITE_TRANSIENT);

	result = sqlite3_step(p_statement);
	if (result != SQLITE_DONE) {
		_E("Sqlite3 error [%d] : <%s> executing statement\n", result, sqlite3_errmsg(svc_db));
	}

	result = sqlite3_finalize(p_statement);
	if (result != SQLITE_OK) {
		_E("Sqlite3 error [%d] : <%s> finalizing statement\n", result, sqlite3_errmsg(svc_db));
	}

	__fini();

	return 0;
}

int _svc_db_delete_all()
{
	char query[QUERY_MAX_LEN];
	char* error_message = NULL;

	if(__init()<0)
		return -1;

	snprintf(query, QUERY_MAX_LEN, "delete from appsvc;");

	if (SQLITE_OK != sqlite3_exec(svc_db, query, NULL, NULL, &error_message))
	{
	 	_E("Don't execute query = %s, error message = %s\n", query, error_message);
		return -1;
	}

	__fini();

	return 0;
}

int _svc_db_is_defapp(const char *pkg_name)
{
	char query[QUERY_MAX_LEN];
	sqlite3_stmt *stmt;
	int cnt = 0;
	int ret = -1;

	if(pkg_name == NULL) {
		_E("Invalid argument: data to delete is NULL\n");
		return 0;
	}

	if(__init()<0)
		return 0;

	snprintf(query, QUERY_MAX_LEN,
		"select count(*) from appsvc where pkg_name = '%s';", pkg_name);

	ret = sqlite3_prepare(svc_db, query, sizeof(query), &stmt, NULL);
	if (ret != SQLITE_OK) {
		_E("prepare error, ret = %d, extended = %d\n", ret, sqlite3_extended_errcode(svc_db));
		return -1;
	}

	ret = sqlite3_step(stmt);
	if (ret == SQLITE_ROW) {
		cnt = sqlite3_column_int(stmt, 0);
	}
	sqlite3_finalize(stmt);

	__fini();

	if(cnt < 1) return 0;

	return 1;
}

char* _svc_db_get_app(const char *op, const char *mime_type, const char *uri)
{
	char m[BUF_MAX_LEN];
	char u[URI_MAX_LEN];
	char query[QUERY_MAX_LEN];
	sqlite3_stmt* stmt;
	int ret;
	char* pkgname;
	char* ret_val = NULL;

	if(op == NULL )
		return NULL;

	if(mime_type==NULL)
		strncpy(m,"NULL",BUF_MAX_LEN-1);
	else
		strncpy(m,mime_type,BUF_MAX_LEN-1);

	if(uri==NULL)
		strncpy(u,"NULL",URI_MAX_LEN-1);
	else
		strncpy(u,uri,URI_MAX_LEN-1);

//	if(doubt_sql_injection(mime_type))
//		return NULL;

	if(__init() < 0)
		return NULL;


	snprintf(query, QUERY_MAX_LEN, "select pkg_name from appsvc where operation='%s' and mime_type='%s' and uri='%s'",\
				op,m,u);

	SECURE_LOGD("query : %s\n",query);

	ret = sqlite3_prepare(svc_db, query, strlen(query), &stmt, NULL);
	if ( ret != SQLITE_OK) {
		_E("prepare error, ret = %d, extended = %d\n", ret, sqlite3_extended_errcode(svc_db));
		goto db_fini;
	}

	ret = sqlite3_step(stmt);
	if (ret != SQLITE_ROW) {
		_D("no result");
		goto stmt_finialize;
	}

	pkgname = (char*) sqlite3_column_text(stmt, 0);
	if(pkgname) {
		ret_val = malloc(BUF_MAX_LEN);
		if (ret_val == NULL) {
			_E("out of memory");
			goto stmt_finialize;
		}

		strncpy(ret_val, (const char *)sqlite3_column_text(stmt, 0),BUF_MAX_LEN-1);
	}

	SECURE_LOGD("pkgname : %s\n",pkgname);

stmt_finialize :
	ret = sqlite3_finalize(stmt);
	if ( ret != SQLITE_OK) {
		_D("finalize error(%d)", ret);
	}

db_fini :
	__fini();

	return ret_val;
}

int _svc_db_get_list_with_condition(char *op, char *uri, char *mime, GSList **pkg_list)
{
	char query[QUERY_MAX_LEN];
	sqlite3_stmt* stmt;
	int ret;
	GSList *iter = NULL;
	char *str = NULL;
	char *pkgname = NULL;
	int found;

	if(__init_app_info_db()<0)
		return 0;

	snprintf(query, QUERY_MAX_LEN, "select ac.app_id from package_app_app_control as ac, package_app_info ai where ac.app_id = ai.app_id and ac.app_control like '%%%s|%s|%s%%' and ai.app_disable='false'", op,uri,mime);
	SECURE_LOGD("query : %s\n",query);

	ret = sqlite3_prepare(app_info_db, query, strlen(query), &stmt, NULL);
	if ( ret != SQLITE_OK) {
		_E("prepare error, ret = %d, extended = %d\n", ret, sqlite3_extended_errcode(app_info_db));
		return -1;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		str = (char *)sqlite3_column_text(stmt, 0);
		if (str == NULL) {
			continue;
		}

		found = 0;
		for (iter = *pkg_list; iter != NULL; iter = g_slist_next(iter)) {
			pkgname = (char *)iter->data;
			if (pkgname && strncmp(str, pkgname, MAX_PACKAGE_STR_SIZE-1) == 0) {
				found = 1;
				break;
			}
		}
		if(found == 0) {
			pkgname = strdup(str);
			*pkg_list = g_slist_append(*pkg_list, (void *)pkgname);
			_D("%s is added",pkgname);
		}
	}

	ret = sqlite3_finalize(stmt);

	return 0;
}

int _svc_db_get_list_with_collation(char *op, char *uri, char *mime, GSList **pkg_list)
{
	char query[QUERY_MAX_LEN];
	sqlite3_stmt* stmt;
	int ret;
	GSList *iter = NULL;
	char *str = NULL;
	char *pkgname = NULL;
	int found;

	if(__init_app_info_db()<0)
		return 0;

	snprintf(query, QUERY_MAX_LEN, "select ac.app_id from package_app_app_control as ac, package_app_info ai where ac.app_id = ai.app_id and ac.app_control='%s|%s|%s' collate appsvc_collation and ai.app_disable='false'", op,uri,mime);
	SECURE_LOGD("query : %s\n",query);

	ret = sqlite3_prepare(app_info_db, query, strlen(query), &stmt, NULL);
	if ( ret != SQLITE_OK) {
		_E("prepare error, ret = %d, extended = %d\n", ret, sqlite3_extended_errcode(app_info_db));
		return -1;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		str = (char *)sqlite3_column_text(stmt, 0);
		if (str == NULL) {
			continue;
		}

		found = 0;
		for (iter = *pkg_list; iter != NULL; iter = g_slist_next(iter)) {
			pkgname = (char *)iter->data;
			if (strncmp(str,pkgname, MAX_PACKAGE_STR_SIZE-1) == 0) {
				found = 1;
				break;
			}
		}
		if(found == 0) {
			pkgname = strdup(str);
			*pkg_list = g_slist_append(*pkg_list, (void *)pkgname);
			_D("%s is added",pkgname);
		}
	}

	ret = sqlite3_finalize(stmt);

	return 0;
}

int _svc_db_get_list_with_all_defapps(GSList **pkg_list)
{
	char query[QUERY_MAX_LEN];
	sqlite3_stmt* stmt;
	int ret;
	GSList *iter = NULL;
	char *str = NULL;
	char *pkgname = NULL;
	int found;

	if(__init()<0)
		return 0;

	snprintf(query, QUERY_MAX_LEN, "select pkg_name from appsvc");

	ret = sqlite3_prepare(svc_db, query, sizeof(query), &stmt, NULL);
	if ( ret != SQLITE_OK) {
		_E("prepare error, ret = %d, extended = %d\n", ret, sqlite3_extended_errcode(svc_db));
		return -1;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		str = (char *)sqlite3_column_text(stmt, 0);
		if (str == NULL) {
			continue;
		}

		found = 0;
		for (iter = *pkg_list; iter != NULL; iter = g_slist_next(iter)) {
			pkgname = (char *)iter->data;
			if (pkgname && strncmp(str, pkgname, MAX_PACKAGE_STR_SIZE-1) == 0) {
				found = 1;
				break;
			}
		}
		if(found == 0) {
			pkgname = strdup(str);
			*pkg_list = g_slist_append(*pkg_list, (void *)pkgname);
			_D("[%s] is def app",pkgname);
		}
	}

	ret = sqlite3_finalize(stmt);

	return 0;
}

