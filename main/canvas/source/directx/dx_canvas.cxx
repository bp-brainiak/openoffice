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
#include "precompiled_canvas.hxx"

#include <ctype.h> // don't ask. msdev breaks otherwise...
#include <canvas/debug.hxx>
#include <canvas/verbosetrace.hxx>
#include <canvas/canvastools.hxx>
#include <tools/diagnose_ex.h>

#include <osl/mutex.hxx>

#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XSystemDependentWindowPeer.hpp>
#include <com/sun/star/registry/XRegistryKey.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/NoSupportException.hpp>

#include <toolkit/helper/vclunohelper.hxx>
#include <cppuhelper/factory.hxx>
#include <cppuhelper/implementationentry.hxx>
#include <comphelper/servicedecl.hxx>

#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/point/b2dpoint.hxx>
#include <basegfx/tools/canvastools.hxx>
#include <basegfx/numeric/ftools.hxx>

#include "dx_graphicsprovider.hxx"
#include "dx_winstuff.hxx"
#include "dx_canvas.hxx"

#include <vcl/sysdata.hxx>

#define CANVAS_TECH "GDI+"
#define CANVAS_SERVICE_NAME        		 "com.sun.star.rendering.Canvas."      		 CANVAS_TECH
#define CANVAS_IMPLEMENTATION_NAME 		 "com.sun.star.comp.rendering.Canvas." 		 CANVAS_TECH
#define BITMAPCANVAS_SERVICE_NAME        "com.sun.star.rendering.BitmapCanvas."      CANVAS_TECH
#define BITMAPCANVAS_IMPLEMENTATION_NAME "com.sun.star.comp.rendering.BitmapCanvas." CANVAS_TECH


using namespace ::com::sun::star;

namespace dxcanvas
{
    /// Actual canonical implementation of the GraphicsProvider interface
    class GraphicsProviderImpl : public GraphicsProvider
    {
        GraphicsSharedPtr mpGraphics;
    public:
        explicit GraphicsProviderImpl( Gdiplus::Graphics* pGraphics ) : mpGraphics( pGraphics ) {}
        virtual GraphicsSharedPtr getGraphics() { return mpGraphics; }
    };

    Canvas::Canvas( const uno::Sequence< uno::Any >&                aArguments,
                    const uno::Reference< uno::XComponentContext >& rxContext ) :
        maArguments(aArguments),
        mxComponentContext( rxContext )
    {
    }

    void Canvas::initialize()
    {
        // #i64742# Only perform initialization when not in probe mode
        if( maArguments.getLength() == 0 )
            return;

        VERBOSE_TRACE( "Canvas::initialize called" );

        // At index 1, we expect a HWND handle here, containing a
        // pointer to a valid window, on which to output
        // At index 2, we expect the current window bound rect
        ENSURE_ARG_OR_THROW( maArguments.getLength() >= 6 &&
                             maArguments[5].getValueTypeClass() == uno::TypeClass_SEQUENCE, 
                             "SpriteCanvas::initialize: wrong number of arguments, or wrong types" );

        uno::Sequence<sal_Int8> aSeq;
        maArguments[5] >>= aSeq;

        const SystemGraphicsData* pSysData=reinterpret_cast<const SystemGraphicsData*>(aSeq.getConstArray());
        if( !pSysData || !pSysData->hDC )
            throw lang::NoSupportException(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                                     "Passed SystemGraphicsData or HDC invalid!")),
                NULL);            
        
        // setup helper
        maDeviceHelper.init( pSysData->hDC,
                             *this );
        maCanvasHelper.setDevice( *this );
        maCanvasHelper.setTarget( 
            GraphicsProviderSharedPtr(
                new GraphicsProviderImpl( 
                    Gdiplus::Graphics::FromHDC(pSysData->hDC))));

        maArguments.realloc(0);
    }

    void SAL_CALL Canvas::disposing()
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        mxComponentContext.clear();

        // forward to parent
        CanvasBaseT::disposing();
    }

    ::rtl::OUString SAL_CALL Canvas::getServiceName(  ) throw (uno::RuntimeException)
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( CANVAS_SERVICE_NAME ) );
    }

    BitmapCanvas::BitmapCanvas( const uno::Sequence< uno::Any >&                aArguments,
                                const uno::Reference< uno::XComponentContext >& rxContext ) :
        maArguments(aArguments),
        mxComponentContext( rxContext ),
        mpTarget()
    {
    }

    void BitmapCanvas::initialize()
    {
        // #i64742# Only perform initialization when not in probe mode
        if( maArguments.getLength() == 0 )
            return;

        VERBOSE_TRACE( "BitmapCanvas::initialize called" );

        // At index 1, we expect a HWND handle here, containing a
        // pointer to a valid window, on which to output
        // At index 2, we expect the current window bound rect
        ENSURE_ARG_OR_THROW( maArguments.getLength() >= 6 &&
                             maArguments[5].getValueTypeClass() == uno::TypeClass_SEQUENCE, 
                             "SpriteCanvas::initialize: wrong number of arguments, or wrong types" );

        uno::Sequence<sal_Int8> aSeq;
        maArguments[5] >>= aSeq;

        const SystemGraphicsData* pSysData=reinterpret_cast<const SystemGraphicsData*>(aSeq.getConstArray());
        if( !pSysData || !pSysData->hDC )
            throw lang::NoSupportException(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                                     "Passed SystemGraphicsData or HDC invalid!")),
                NULL);            
        
        // setup helper
        maDeviceHelper.init( pSysData->hDC,
                             *this );
        maCanvasHelper.setDevice( *this );

        // check whether we can actually provide a BitmapCanvas
        // here. for this, check whether the HDC has a bitmap
        // selected.
        HBITMAP hBmp;
        hBmp=(HBITMAP)GetCurrentObject(pSysData->hDC, OBJ_BITMAP);
        if( !hBmp || GetObjectType(pSysData->hDC) != OBJ_MEMDC )
        {
            throw lang::NoSupportException(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                                     "Passed HDC is no mem DC/has no bitmap selected!")),
                NULL);
        }

        mpTarget.reset( new DXBitmap(
                            BitmapSharedPtr(
                                Gdiplus::Bitmap::FromHBITMAP(
                                    hBmp, 0) ),
                            false ));

        maCanvasHelper.setTarget( mpTarget );

        maArguments.realloc(0);
    }

    void SAL_CALL BitmapCanvas::disposing()
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        mpTarget.reset();
        mxComponentContext.clear();

        // forward to parent
        BitmapCanvasBaseT::disposing();
    }

    ::rtl::OUString SAL_CALL BitmapCanvas::getServiceName(  ) throw (uno::RuntimeException)
    {
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( BITMAPCANVAS_SERVICE_NAME ) );
    }

    IBitmapSharedPtr BitmapCanvas::getBitmap() const
    {
        return mpTarget;
    }

    static uno::Reference<uno::XInterface> initCanvas( Canvas* pCanvas )
    {
        uno::Reference<uno::XInterface> xRet(static_cast<cppu::OWeakObject*>(pCanvas));
        pCanvas->initialize();
        return xRet;
    }

    namespace sdecl = comphelper::service_decl;
    sdecl::class_<Canvas, sdecl::with_args<true> > serviceImpl1(&initCanvas);
    const sdecl::ServiceDecl dxCanvasDecl(
        serviceImpl1,
        CANVAS_IMPLEMENTATION_NAME,
        CANVAS_SERVICE_NAME );

    static uno::Reference<uno::XInterface> initBitmapCanvas( BitmapCanvas* pCanvas )
    {
        uno::Reference<uno::XInterface> xRet(static_cast<cppu::OWeakObject*>(pCanvas));
        pCanvas->initialize();
        return xRet;
    }

    namespace sdecl = comphelper::service_decl;
    sdecl::class_<BitmapCanvas, sdecl::with_args<true> > serviceImpl2(&initBitmapCanvas);
    const sdecl::ServiceDecl dxBitmapCanvasDecl(
        serviceImpl2,
        BITMAPCANVAS_IMPLEMENTATION_NAME,
        BITMAPCANVAS_SERVICE_NAME );
}

// The C shared lib entry points
COMPHELPER_SERVICEDECL_EXPORTS2(dxcanvas::dxCanvasDecl,
                                dxcanvas::dxBitmapCanvasDecl);
