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

static void utc_appsvc_create_result_bundle_func_01(void);
static void utc_appsvc_create_result_bundle_func_02(void);
static void utc_appsvc_create_result_bundle_func_03(void);
static void utc_appsvc_create_result_bundle_func_04(void);

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

struct tet_testlist tet_testlist[] = {
	{utc_appsvc_create_result_bundle_func_01, POSITIVE_TC_IDX},
	{utc_appsvc_create_result_bundle_func_02, NEGATIVE_TC_IDX},
	{utc_appsvc_create_result_bundle_func_03, NEGATIVE_TC_IDX},
	{utc_appsvc_create_result_bundle_func_04, NEGATIVE_TC_IDX},
	{NULL, 0}
};

static void startup(void)
{
	tet_infoline("Testing API appsvc_create_result_bundle starts");
}

static void cleanup(void)
{
	tet_infoline("Testing API appsvc_create_result_bundle ends");
}

/**
 * @brief Positive test case of appsvc_create_result_bundle()
 */
static void utc_appsvc_create_result_bundle_func_01(void)
{
	int ret = APPSVC_RET_OK;
	bundle *inb = NULL;
	bundle *outb = NULL;
	inb = bundle_create();
	if (inb == NULL) {
		tet_infoline("Test initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}
	ret = bundle_add(inb, "__AUL_ORG_CALLER_PID__", "2000");
	if (ret != 0) {
		tet_infoline("Test initialization failed!!!");
		bundle_free(inb);
		tet_result(TET_UNINITIATED);
		return;
	}

	ret = appsvc_create_result_bundle(inb, &outb);
	if (ret == APPSVC_RET_OK) {
		tet_infoline("appsvc_create_result_bundle test PASS!");
		tet_printf("ret = %d", ret);
		tet_result(TET_PASS);
	} else {
		tet_infoline("appsvc_create_result_bundle test FAIL!");
		tet_printf("ret = %d", ret);
		tet_result(TET_FAIL);
	}
	bundle_free(inb);
	if (outb != NULL) {
		bundle_free(outb);
	}
	return;
}

/**
 * @brief Negative test case of appsvc_create_result_bundle()
 */
static void utc_appsvc_create_result_bundle_func_02(void)
{
	int ret = APPSVC_RET_OK;
	bundle *outb = NULL;
	ret = appsvc_create_result_bundle(NULL, &outb);
	if (ret == APPSVC_RET_EINVAL) {
		tet_infoline("appsvc_create_result_bundle test PASS!");
		tet_printf("ret = %d", ret);
		tet_result(TET_PASS);
	} else {
		tet_infoline("appsvc_create_result_bundle test FAIL!");
		tet_printf("ret = %d", ret);
		tet_result(TET_FAIL);
	}
	if (outb != NULL) {
		bundle_free(outb);
	}
	return;
}

/**
 * @brief Negative test case of appsvc_create_result_bundle()
 */
static void utc_appsvc_create_result_bundle_func_03(void)
{
	int ret = APPSVC_RET_OK;
	bundle *inb = NULL;
	bundle *outb = NULL;
	inb = bundle_create();
	if (inb == NULL) {
		tet_infoline("Test initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}
	ret = appsvc_create_result_bundle(inb, &outb);
	if (ret == APPSVC_RET_EINVAL) {
		tet_infoline("appsvc_create_result_bundle test PASS!");
		tet_printf("ret = %d", ret);
		tet_result(TET_PASS);
	} else {
		tet_infoline("appsvc_create_result_bundle test FAIL!");
		tet_printf("ret = %d", ret);
		tet_result(TET_FAIL);
	}
	bundle_free(inb);
	if (outb != NULL) {
		bundle_free(outb);
	}
}

/**
 * @brief Negative test case of appsvc_create_result_bundle()
 */
static void utc_appsvc_create_result_bundle_func_04(void)
{
	int ret = APPSVC_RET_OK;
	bundle *inb = NULL;
	inb = bundle_create();
	if (inb == NULL) {
		tet_infoline("Test initialization failed!!!");
		tet_result(TET_UNINITIATED);
		return;
	}

	ret = appsvc_create_result_bundle(inb, NULL);
	if (ret == APPSVC_RET_EINVAL) {
		tet_infoline("appsvc_create_result_bundle test PASS!");
		tet_printf("ret = %d", ret);
		tet_result(TET_PASS);
	} else {
		tet_infoline("appsvc_create_result_bundle test FAIL!");
		tet_printf("ret = %d", ret);
		tet_result(TET_FAIL);
	}
	bundle_free(inb);
}
