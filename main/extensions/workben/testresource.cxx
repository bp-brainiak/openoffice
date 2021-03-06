/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_extensions.hxx"

#include <smart/com/sun/star/registry/XImplementationRegistration.hxx>
#include <smart/com/sun/star/script/XInvocation.hxx>

#include <rtl/ustring.hxx>
#include <vos/dynload.hxx>
#include <vos/diagnose.hxx>
#include <usr/services.hxx>
#include <vcl/svapp.hxx>

using namespace rtl;
using namespace vos;
using namespace usr;

class MyApp : public Application
{
public:
	void		Main();
};

MyApp aMyApp;

// -----------------------------------------------------------------------

void MyApp::Main()
{
	XMultiServiceFactoryRef xSMgr = createRegistryServiceManager();
	registerUsrServices( xSMgr );
	setProcessServiceManager( xSMgr );

	XInterfaceRef x = xSMgr->createInstance( L"com.sun.star.registry.ImplementationRegistration" );
	XImplementationRegistrationRef xReg( x, USR_QUERY );
	sal_Char szBuf[1024];
	ORealDynamicLoader::computeModuleName( "res", szBuf, 1024 );
	UString aDllName( StringToOUString( szBuf, CHARSET_SYSTEM ) );
	xReg->registerImplementation( L"com.sun.star.loader.SharedLibrary", aDllName, XSimpleRegistryRef() );

	x = xSMgr->createInstance( L"com.sun.star.resource.VclStringResourceLoader" );
	XInvocationRef xResLoader( x, USR_QUERY );
	XIntrospectionAccessRef xIntrospection = xResLoader->getIntrospection();
	UString aFileName( L"TestResource" );
	UsrAny aVal;
	aVal.setString( aFileName );
	xResLoader->setValue( L"FileName", aVal );

	Sequence< UsrAny > Args( 1 );
	Sequence< INT16 > OutPos;
	Sequence< UsrAny > OutArgs;
	Args.getArray()[0].setINT32( 1000 );

	BOOL b = xResLoader->invoke( L"hasString", Args, OutPos, OutArgs ).getBOOL();
	VOS_ENSHURE( b, "hasString" );

	UString aStr = xResLoader->invoke( L"getString", Args, OutPos, OutArgs ).getString();
	VOS_ENSHURE( aStr == L"Hello", "getString" );
	
	Args.getArray()[0].setINT32( 1001 );
	b = xResLoader->invoke( L"hasString", Args, OutPos, OutArgs ).getBOOL();
	VOS_ENSHURE( !b, "!hasString" );

	xReg->revokeImplementation( aDllName, XSimpleRegistryRef() );
}

