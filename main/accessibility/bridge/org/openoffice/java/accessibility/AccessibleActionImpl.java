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



package org.openoffice.java.accessibility;

import com.sun.star.accessibility.XAccessibleAction;

/** The AccessibleActionWrapper maps the calls to the java AccessibleAction interface
 *  to the corresponding methods of the UNO XAccessibleAction interface.
 */
public class AccessibleActionImpl implements javax.accessibility.AccessibleAction {
    
    protected XAccessibleAction unoObject;
    
    /** Creates new AccessibleActionWrapper */
    public AccessibleActionImpl(XAccessibleAction xAccessibleAction) {
        unoObject = xAccessibleAction;
    }
    
    public boolean doAccessibleAction(int param) {
        try {
            return unoObject.doAccessibleAction(param);
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            return false;
        } catch (com.sun.star.uno.RuntimeException e) {
            return false;
        }
    }
    
    public java.lang.String getAccessibleActionDescription(int param) {
        try {
            return unoObject.getAccessibleActionDescription(param);
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            return null;
        } catch (com.sun.star.uno.RuntimeException e) {
            return null;
        }
    }
    
    public int getAccessibleActionCount() {
        try {
            return unoObject.getAccessibleActionCount();
        } catch (com.sun.star.uno.RuntimeException e) {
            return 0;
        }
    }
}
