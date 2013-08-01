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
 
//Hopefully Oracle will play nice if I keep their copyright.
/*
 * Copyright (c) 1995, 2010, Oracle and/or its affiliates. All rights reserved.
 * ORACLE PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 */
 
package java.awt;

import java.awt.image.BufferStrategy;
import java.awt.peer.CanvasPeer;
import java.awt.image.BufferedImage;
import javax.accessibility.*;
import smart.Client;

/**
 *
 * Dirty little hack that makes a new Canvas class right where I want it.
 * Uses the code from Oracle's API with some additions. 
 *
 * @author Benjamin J. Land
 */
public class Canvas extends Component implements Accessible {

	@Override
    public void setVisible(boolean visible) {
        Client.canvasNotify(this);
        super.setVisible(visible);
    }

    private static final String base = "canvas";
    private static int nameCounter = 0;
    private BufferedImage buffer = null;
    
    public BufferedImage getBuffer() {
        return buffer;
    }
    
    public void setBuffer(BufferedImage buffer) {
        this.buffer = buffer;
    }
    
    @Override
    public Graphics getGraphics() {
        return buffer == null ?  super.getGraphics() : buffer.getGraphics();
    }
    
    public Graphics getCanvasGraphics() {
        return super.getGraphics();
    }
    

    private static final long serialVersionUID = -2284879212465893870L;

    public Canvas() {
    }

    public Canvas(GraphicsConfiguration config) {
        this();
        setGraphicsConfiguration(config);
    }

    @Override
    void setGraphicsConfiguration(GraphicsConfiguration gc) {
        synchronized(getTreeLock()) {
            CanvasPeer peer = (CanvasPeer)getPeer();
            if (peer != null) {
                gc = peer.getAppropriateGraphicsConfiguration(gc);
            }
            super.setGraphicsConfiguration(gc);
        }
    }

    String constructComponentName() {
        synchronized (Canvas.class) {
            return base + nameCounter++;
        }
    }
    
    public void addNotify() {
        synchronized (getTreeLock()) {
            if (peer == null)
                peer = getToolkit().createCanvas(this);
            super.addNotify();
        }
    }

    public void paint(Graphics g) {
        g.clearRect(0, 0, width, height);
    }

    public void update(Graphics g) {
        g.clearRect(0, 0, width, height);
        paint(g);
    }

    boolean postsOldMouseEvents() {
        return true;
    }

    public void createBufferStrategy(int numBuffers) {
        super.createBufferStrategy(numBuffers);
    }

    public void createBufferStrategy(int numBuffers,
        BufferCapabilities caps) throws AWTException {
        super.createBufferStrategy(numBuffers, caps);
    }

    public BufferStrategy getBufferStrategy() {
        return super.getBufferStrategy();
    }

    public AccessibleContext getAccessibleContext() {
        if (accessibleContext == null) {
            accessibleContext = new AccessibleAWTCanvas();
        }
        return accessibleContext;
    }

    protected class AccessibleAWTCanvas extends AccessibleAWTComponent {
        private static final long serialVersionUID = -6325592262103146699L;

        public AccessibleRole getAccessibleRole() {
            return AccessibleRole.CANVAS;
        }

    }
}
