/**
 *  Copyright 2006-2013 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of the SMART Minimizing Autoing Resource Thing (SMART)
 *
 *  SMART is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SMART is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SMART. If not, see <http://www.gnu.org/licenses/>.
 */

package smart;

import java.applet.AppletContext;
import java.applet.AppletStub;
import java.net.URL;
import java.util.HashMap;

/**
 * Provides a minimalist AppletStub for use in the RuneScape client. Be sure 
 * that all needed settings are passed in the constructor.
 *
 * @author Benjamin J. Land
 */
public class ClientStub implements AppletStub {
    
    public boolean active = false;
    private HashMap<String, String> params;
    private URL documentbase;
    private URL codebase;
    
    public ClientStub(String documentBase, String codebase, HashMap<String, String> params) throws Exception {
        this.params = params;
        this.documentbase = new URL(documentBase);
        this.codebase = new URL(codebase);
    }
    
    public void setActive(boolean active) {
        this.active = active;
    }
    
    @Override
    public boolean isActive() {
        return active;
    }
    
    @Override
    public URL getDocumentBase() {
        return documentbase;
    }
    
    @Override
    public URL getCodeBase() {
        return codebase;
    }
    
    @Override
    public String getParameter(String name) {
        //System.out.println("GET PARAM: " + name + "=>" + params.get(name));
        String value = params.get(name);
        return value == null ? "" : value;
    }
    
    @Override
    public AppletContext getAppletContext() {
        return null;
    }
    
    @Override
    public void appletResize(int i, int i1) {
        //Perhaps this has changed... REGARDLESS its a valid response.
        throw new RuntimeException("Resizing the applet is a BAD idea ;)");
    }
    
}
