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


#ifndef CHART2_REGRESSIONCURVECALCULATOR_HXX
#define CHART2_REGRESSIONCURVECALCULATOR_HXX

#include <cppuhelper/implbase1.hxx>

#include <com/sun/star/chart2/XRegressionCurveCalculator.hpp>
#include <com/sun/star/util/XNumberFormatter.hpp>

namespace chart
{

class RegressionCurveCalculator : public
    ::cppu::WeakImplHelper1< ::com::sun::star::chart2::XRegressionCurveCalculator >
{
public:
	RegressionCurveCalculator();
	virtual ~RegressionCurveCalculator();

    static bool isLinearScaling(
        const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XScaling > & xScaling );
    static bool isLogarithmicScaling(
        const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XScaling > & xScaling );

protected:
    virtual ::rtl::OUString ImplGetRepresentation(
        const ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatter >& xNumFormatter,
        ::sal_Int32 nNumberFormatKey ) const = 0;

    ::rtl::OUString getFormattedString(
        const ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatter >& xNumFormatter,
        ::sal_Int32 nNumberFormatKey,
        double fNumber ) const;

    double m_fCorrelationCoeffitient;

    // ____ XRegressionCurveCalculator ____
    virtual void SAL_CALL recalculateRegression(
        const ::com::sun::star::uno::Sequence< double >& aXValues,
        const ::com::sun::star::uno::Sequence< double >& aYValues )
        throw (::com::sun::star::uno::RuntimeException) = 0;
    virtual double SAL_CALL getCurveValue( double x )
        throw (::com::sun::star::lang::IllegalArgumentException,
               ::com::sun::star::uno::RuntimeException) = 0;

    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::geometry::RealPoint2D > SAL_CALL getCurveValues(
        double min,
        double max,
        ::sal_Int32 nPointCount,
        const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XScaling >& xScalingX,
        const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XScaling >& xScalingY,
        ::sal_Bool bMaySkipPointsInCalculation )
        throw (::com::sun::star::lang::IllegalArgumentException,
               ::com::sun::star::uno::RuntimeException);
    virtual double SAL_CALL getCorrelationCoefficient()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getRepresentation()
        throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getFormattedRepresentation(
        const ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatsSupplier >& xNumFmtSupplier,
        ::sal_Int32 nNumberFormatKey )
        throw (::com::sun::star::uno::RuntimeException);
};

} //  namespace chart

// CHART2_REGRESSIONCURVECALCULATOR_HXX
#endif
