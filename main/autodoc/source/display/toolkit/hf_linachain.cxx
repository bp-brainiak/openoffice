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
#include <toolkit/hf_linachain.hxx>


// NOT FULLY DEFINED SERVICES
#include <toolkit/out_position.hxx>



HF_LinkedNameChain::HF_LinkedNameChain( Xml::Element & o_rOut )
    :   HtmlMaker( o_rOut
                        >> *new Html::Paragraph
                            << new Html::ClassAttr("namechain") )
{
}

HF_LinkedNameChain::~HF_LinkedNameChain()
{
}

void
HF_LinkedNameChain::Produce_CompleteChain( const output::Position & i_curPosition,
										   F_LinkMaker              i_linkMaker ) const
{
    produce_Level(i_curPosition.RelatedNode(), i_curPosition, i_linkMaker);
}

void
HF_LinkedNameChain::Produce_CompleteChain_forModule( const output::Position &  i_curPosition,
													 F_LinkMaker               i_linkMaker ) const
{
	if (i_curPosition.Depth() == 0)
		return;
    produce_Level(*i_curPosition.RelatedNode().Parent(), i_curPosition, i_linkMaker);
}



namespace
{

StreamStr aLinkBuf(200);

}

void
HF_LinkedNameChain::produce_Level( output::Node &           i_levelNode,
								   const output::Position & i_startPosition,
								   F_LinkMaker              i_linkMaker ) const
{
    if ( i_levelNode.Depth() > 0 )
    {
        produce_Level( *i_levelNode.Parent(),
                       i_startPosition,
                       i_linkMaker );
    }

    aLinkBuf.reset();

    String
        sFileName = (*i_linkMaker)(i_levelNode.Name());
    output::Position
        aLevelPos(i_levelNode, sFileName);

    i_startPosition.Get_LinkTo(aLinkBuf, aLevelPos);

    if ( i_levelNode.Depth() > 0 )
    {
        CurOut()
        >> *new Html::Link(aLinkBuf.c_str())
            << new Html::ClassAttr("namechain")
            << i_levelNode.Name();
        CurOut() << " :: ";
    }
    else
    {
        CurOut()
        >> *new Html::Link(aLinkBuf.c_str())
            << new Html::ClassAttr("namechain")
            << "::";
        CurOut() << " ";
    }
}
