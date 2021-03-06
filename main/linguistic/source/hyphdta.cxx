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
#include "precompiled_linguistic.hxx"

#include "linguistic/hyphdta.hxx"
#include "linguistic/lngprops.hxx"
#include "linguistic/misc.hxx"
#include <osl/mutex.hxx>


#include <rtl/ustrbuf.hxx>
#include <tools/debug.hxx>
#include <svl/lngmisc.hxx>
#include <unotools/localedatawrapper.hxx>

//using namespace utl;
using namespace osl;
using namespace rtl;
using namespace com::sun::star;
//using namespace com::sun::star::beans;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;
using namespace com::sun::star::linguistic2;

namespace linguistic
{
///////////////////////////////////////////////////////////////////////////


HyphenatedWord::HyphenatedWord(const OUString &rWord, sal_Int16 nLang, sal_Int16 nHPos,
							   const OUString &rHyphWord, sal_Int16 nPos ) :
	aWord			(rWord),
    aHyphenatedWord (rHyphWord),
    nHyphPos        (nPos),
    nHyphenationPos (nHPos),
    nLanguage       (nLang)
{
    String aSingleQuote( GetLocaleDataWrapper( nLanguage ).getQuotationMarkEnd() );
    DBG_ASSERT( 1 == aSingleQuote.Len(), "unexpectend length of quotation mark" );
    if (aSingleQuote.Len())
    { 
        // ignore typographical apostrophes (which got replaced in original
        // word when being checked for hyphenation) in results.
        OUString aTmpWord( rWord );
        OUString aTmpHyphWord( rHyphWord );
        aTmpWord        = aTmpWord    .replace( aSingleQuote.GetChar(0), '\'' );
        aTmpHyphWord    = aTmpHyphWord.replace( aSingleQuote.GetChar(0), '\'' );
        bIsAltSpelling  = aTmpWord != aTmpHyphWord;
    }
    else
        bIsAltSpelling = rWord != rHyphWord;
}


HyphenatedWord::~HyphenatedWord()
{
}


OUString SAL_CALL HyphenatedWord::getWord()
		throw(RuntimeException)
{
	MutexGuard	aGuard( GetLinguMutex() );
	return aWord;
}


Locale SAL_CALL HyphenatedWord::getLocale()
		throw(RuntimeException)
{
	MutexGuard	aGuard( GetLinguMutex() );

	Locale aRes;
	return LanguageToLocale( aRes, nLanguage );
}


sal_Int16 SAL_CALL HyphenatedWord::getHyphenationPos()
		throw(RuntimeException)
{
	MutexGuard	aGuard( GetLinguMutex() );
	return nHyphenationPos;
}


OUString SAL_CALL HyphenatedWord::getHyphenatedWord()
		throw(RuntimeException)
{
	MutexGuard	aGuard( GetLinguMutex() );
	return aHyphenatedWord;
}


sal_Int16 SAL_CALL HyphenatedWord::getHyphenPos()
		throw(RuntimeException)
{
	MutexGuard	aGuard( GetLinguMutex() );
	return nHyphPos;
}


sal_Bool SAL_CALL HyphenatedWord::isAlternativeSpelling()
		throw(RuntimeException)
{
	MutexGuard	aGuard( GetLinguMutex() );
	return bIsAltSpelling;
}


///////////////////////////////////////////////////////////////////////////


PossibleHyphens::PossibleHyphens(const OUString &rWord, sal_Int16 nLang,
			const OUString &rHyphWord,
			const Sequence< sal_Int16 > &rPositions) :
	aWord			(rWord),
	aWordWithHyphens(rHyphWord),
	aOrigHyphenPos	(rPositions),
	nLanguage		(nLang)
{
}


PossibleHyphens::~PossibleHyphens()
{
}


OUString SAL_CALL PossibleHyphens::getWord()
		throw(RuntimeException)
{
	MutexGuard	aGuard( GetLinguMutex() );
	return aWord;
}


Locale SAL_CALL PossibleHyphens::getLocale()
		throw(RuntimeException)
{
	MutexGuard	aGuard( GetLinguMutex() );
	return CreateLocale( nLanguage );
}


OUString SAL_CALL PossibleHyphens::getPossibleHyphens()
		throw(RuntimeException)
{
	MutexGuard	aGuard( GetLinguMutex() );
	return aWordWithHyphens;
}


Sequence< sal_Int16 > SAL_CALL PossibleHyphens::getHyphenationPositions()
		throw(RuntimeException)
{
	MutexGuard	aGuard( GetLinguMutex() );
	return aOrigHyphenPos;
}

///////////////////////////////////////////////////////////////////////////

}	// namespace linguistic

