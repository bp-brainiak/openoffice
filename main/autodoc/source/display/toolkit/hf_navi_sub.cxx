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

#include <precomp.h>
#include <toolkit/hf_navi_sub.hxx>


// NOT FULLY DEFINED SERVICES


HF_NaviSubRow::HF_NaviSubRow( Xml::Element & o_rOut )
    :   HtmlMaker(o_rOut),
		aRow(),
        pMyRow(0)
{
    Setup_Row();
}

HF_NaviSubRow::~HF_NaviSubRow()
{
}

void
HF_NaviSubRow::AddItem( const String &      i_sText,
                        const String &      i_sLink,
                        bool                i_bSwitchOn )
{
    aRow.push_back( SubRow_Item( SubRow_Data(i_sText,i_sLink),
								 i_bSwitchOn ));
}

void
HF_NaviSubRow::SwitchOn( int i_nIndex )
{
    if ( i_nIndex < int(aRow.size()) )
        aRow[i_nIndex].second = true;
}

void
HF_NaviSubRow::Setup_Row()
{
    Html::Table *
                pTable = new Html::Table;
    CurOut()
        >> *pTable
           << new Html::ClassAttr("navisub") 
           << new Xml::AnAttribute( "border", "0" )
           << new Xml::AnAttribute( "cellpadding", "0" );
    pMyRow = &pTable->AddRow();
}

void
HF_NaviSubRow::Produce_Row()
{
    for ( SubRow::const_iterator it = aRow.begin();
          it != aRow.end();
          ++it )
    {
        Xml::Element &
            rCell = *pMyRow
                     >> *new Html::TableCell
                         << new Html::ClassAttr("navisub");
        StreamLock sl(100);
        Xml::Element &
            rGoon = (*it).second
                            ?   ( rCell
                                  >> *new Html::Link( sl()
                                                      << "#"
                                                      << (*it).first.second
                                                      << c_str )
                                     << new Html::ClassAttr("navisub")
                                )
                            :   rCell;
        rGoon
            << (*it).first.first;
    }
}


