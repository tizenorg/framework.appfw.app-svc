/*
 * app-svc
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Jyotsna Dhumale <jyotsna.a@samsung.com>
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

/**
 *
 * @ingroup SLP_PG
 * @defgroup AppSvc_PG App-Svc
 * @{

<h1 class="pg">Introduction</h1>
AppSvc is a library which provides various features to applications. Its main purpose is to launch suitable application based on combination of MIME type, operation and URI.

<h1 class="pg">AppSvc system context view</h1>
\image html appsvc_diag.png "Picture 1. AppSvc System Context View"
\image rtf appsvc_diag.png "Picture 1. AppSvc System context View"

Applications requests app-svc to launch application based on mime type(image/jpeg, image/png, video/3gp, video/3gpp etc), operation (send, view etc) and URI. App-Svc finds the suitable application by referring to default app DB and launches that app. If default app not set, it searches and lists suitable apps by referring to AIL DB using app-selector.

<h1 class="pg">API list and description</h1>
<ul>
	<li>appsvc_set_operation() : Sets operation in the bundle </li>
	<li>appsvc_set_uri() : Sets URI in the bundle</li>
	<li>appsvc_set_mime() : Sets MIME type in the bundle</li>
	<li>appsvc_add_data() : Sets an extra data to launch application based on appsvc</li>
	<li>appsvc_add_data_array() : Sets an extra array data to launch application based on appsvc</li>
	<li>appsvc_set_pkgname() : Sets package name in the bundle</li>
	<li>appsvc_set_appid() : Sets app-id in the bundle</li>
	<li>appsvc_run_service() : Launches application based on parameters set in the bundle</li>
	<li>appsvc_get_list() : Lists suitable applications matching the bundle parameters</li>
	<li>appsvc_get_operation() : Gets operation from bundle</li>
	<li>appsvc_get_uri() : Gets URI from bundle</li>
	<li>appsvc_get_mime() : Gets MIME from bundle</li>
	<li>appsvc_get_pkgname() : Gets package name from bundle</li>
	<li>appsvc_get_appid() : Gets app-id from bundle</li>
	<li>appsvc_get_data() : Gets data from bundle</li>
	<li>appsvc_get_data_array() : Gets data array from bundle</li>
	<li>appsvc_create_result_bundle() : Creates appsvc result bundle based on bundle received in reset event</li>
	<li>appsvc_send_result() : Sends appsvc result to caller with bundle</li>
	<li>appsvc_set_defapp() : Sets default app</li>
	<li>appsvc_unset_defapp() : Unsets default app</li>
	<li>appsvc_is_defapp() : Checks application is default application or not</li>
	<li>appsvc_data_is_array() : Checks extra data is array or not</li>
</ul>

 * @}
 */
