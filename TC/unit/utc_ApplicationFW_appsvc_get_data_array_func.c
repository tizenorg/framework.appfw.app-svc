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

#include <tet_api.h>
#include <appsvc.h>
#include <bundle.h>
#include <unistd.h>

static void startup(void);
static void cleanup(void);

void (*tet_startup) (void) = startup;
void (*tet_cleanup) (void) = cleanup;

static void utc_appsvc_get_data_array_func_01(void);
static void utc_appsvc_get_data_array_func_02(void);
static void utc_appsvc_get_data_array_func_03(void);

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

struct tet_testlist tet_testlist[] = {
	{utc_appsvc_get_data_array_func_01, POSITIVE_TC_IDX},
	{utc_appsvc_get_data_array_func_02, NEGATIVE_TC_IDX},
	{utc_appsvc_get_data_array_func_03, NEGATIVE_TC_IDX},
	{NULL, 0}
};

static void startup(void)
{
	tet_infoline("Testing API appsvc_set_uri start");
}

static void cleanup(void)
{
	tet_infoline("Testing API appsvc_set_uri ends ");

}

/**
 * @brief Positive test case of appsvc_get_data_array()
 */
static void utc_appsvc_get_data_array_func_01(void)
{
	const char **ret_array = NULL;
	const char *images[] = { "/opt/media/a.jpg",
		"/opt/media/b.jpg", "/opt/media/c.jpg"
	};
	bundle *b = NULL;
	int len = 0;
	int ret = APPSVC_RET_OK;

	b = bundle_create();
	if (b == NULL) {
		tet_infoline("Test initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}
	ret = appsvc_add_data_array(b, APPSVC_DATA_SELECTED, images, 3);
	if (ret != APPSVC_RET_OK) {
		tet_infoline("Test initialization failed!!!");
		bundle_free(b);
		tet_result(TET_UNINITIATED);
		return;
	}

	ret_array = appsvc_get_data_array(b, APPSVC_DATA_SELECTED, &len);
	if (ret_array != NULL) {
		tet_infoline("appsvc_get_data_array test PASS!");
		tet_result(TET_PASS);
	} else {
		tet_infoline("appsvc_get_data_array test FAIL!");
		tet_printf("ret_array = NULL");
		tet_result(TET_FAIL);
	}

	bundle_free(b);
	return;
}

/**
 * @brief Negative test case of appsvc_get_data_array()
 */
static void utc_appsvc_get_data_array_func_02(void)
{
	int ret = APPSVC_RET_OK;
	const char **ret_array = NULL;
	const char *images[] = { "/opt/media/a.jpg",
		"/opt/media/b.jpg", "/opt/media/c.jpg"
	};
	bundle *b = NULL;
	int len = 0;
	b = bundle_create();
	if (b == NULL) {
		tet_infoline("Test initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}
	ret = appsvc_add_data_array(b, APPSVC_DATA_SELECTED, images, 3);
	if (ret != APPSVC_RET_OK) {
		tet_infoline("Test initialization failed!!!");
		bundle_free(b);
		tet_result(TET_UNINITIATED);
		return;
	}

	ret_array = appsvc_get_data_array(b, NULL, &len);
	if (ret_array == NULL) {
		tet_infoline("appsvc_get_data_array test PASS!");
		tet_printf("ret_array = NULL");
		tet_result(TET_PASS);
	} else {
		tet_infoline("appsvc_get_data_array test FAIL!");
		tet_result(TET_FAIL);
	}
	bundle_free(b);
	return;
}

/**
 * @brief Negative test case of appsvc_get_data_array()
 */
static void utc_appsvc_get_data_array_func_03(void)
{
	const char **ret_array = NULL;
	int len = 0;

	ret_array = appsvc_get_data_array(NULL, NULL, &len);
	if (ret_array == NULL) {
		tet_infoline("appsvc_get_data_array test PASS!");
		tet_printf("ret_array = NULL");
		tet_result(TET_PASS);
	} else {
		tet_infoline("appsvc_get_data_array test FAIL!");
		tet_result(TET_FAIL);
	}

	return;
}
