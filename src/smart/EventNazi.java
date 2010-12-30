/**
 *  Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100)
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

import java.awt.Component;
import java.awt.KeyboardFocusManager;
import java.awt.Point;
import java.awt.event.FocusEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.util.HashMap;
import java.util.HashSet;

/**
 * The EventNazi is responsible for creating events to send to the client. It
 * automaticaly handles focus just as a normal client would, by loosing when you
 * click off, and getting when you click on. It abstracts key control into two
 * methods, one to send a string, and one to hold a key. It abstracts mouse
 * control into three, one for moving, one for dragging, and one for clicking.
 *
 * This class also contains the newest in my line of mouse algorithms, WindMouse.
 * As the name would suggest, the mouse is givin randomness as if a smooth but
 * shifting wind were pushing it around. All the while, it is being "pulled"
 * towards the destination as if by gravity. Supprisingly this is a very good
 * emuation of human mouse movement.
 *
 * The comment "Scratch" denotes pieces of the mouse debug output code that
 * should be commented out when not using.
 *
 * @author Benjamin J. Land
 */
public class EventNazi {
    
    /**
     * Contains all created and active EventNazi instances.
     */
    private static final HashMap<Component,EventNazi> nazies = new HashMap<Component,EventNazi>();
    
    /**
     * Returns the EventNazi for the specified Component, if it exists. Otherwise
     * it creates the Nazi and sets it up. Only the BlockingEventQueue should
     * use this method.
     * @param comp The component to create a nazi for.
     * @result The new/old EventNazi
     */
    public static EventNazi createNazi(Component comp) {
        EventNazi nazi = nazies.containsKey(comp) ? nazies.get(comp) : new EventNazi(comp);
        if (!nazi.active) {
            if (comp.getMousePosition() != null && comp.isFocusOwner() && comp.isShowing()) {
                nazi.cx = comp.getMousePosition().x;
                nazi.cy = comp.getMousePosition().y;
                nazi.mousein = true;
            } else {
                nazi.cx = -1;
                nazi.cy = -1;
                nazi.mousein = false;
            }
            nazi.active = BlockingEventQueue.isBlocking(comp);
            nazi.focused = comp.isFocusOwner() && comp.isShowing();
        }
        nazies.put(comp, nazi);
        return nazi;
    }
    
    /**
     * Returns the EventNazi for the specified Component, if it exists. Null
     * otherwise. Other classes should use this method.
     * @param comp The comonent to check for.
     * @result See description
     */
    public static EventNazi getNazi(Component comp) {
        return nazies.get(comp);
    }
    
    private Component comp;
    private KeySender keySender;
    private boolean active;
    private int cx, cy;
    private HashSet<int[]> keysHeld;
    private boolean mousein;
    private boolean leftDown;
    private boolean rightDown;
    private boolean focused;
    private boolean shiftDown;
    
    //Scratch
    /*private JFrame frame = new JFrame ("WindMouse Debug");
    private BufferedImage scratch = new BufferedImage(765,503,BufferedImage.TYPE_INT_ARGB);
    private Graphics g = scratch.getGraphics();*/
    
    /**
     * Creates a new instance of EventNazi initialized to a "null" but ready
     * state. Private because EventNazi need to be created by the createNazi
     * method.
     */
    private EventNazi(Component comp) {
        //Scratch
        /*frame.setDefaultCloseOperation(frame.DISPOSE_ON_CLOSE);
        frame.setLayout(new GridLayout());
        frame.add(new JLabel(new ImageIcon(scratch)));
        frame.pack();
        frame.setVisible(true);
        g.setColor(Color.BLACK);*/
        nazies.put(comp, this);
        this.comp = comp;
        active = false;
        leftDown = false;
        rightDown = false;
        focused = false;
        shiftDown = false;
        keysHeld = new HashSet<int[]>();
        keySender = new KeySender("java-key-thread");
        keySender.start();
    }
    
    /**
     * Returns the component that this EventNazi is controlling.
     * @result This EventNazi's component.
     */
    public Component getComponent() {
        return comp;
    }
    
    public void wait(int min, int max) {
        int time = min == max ? min : (int)((Math.random() * Math.abs(max - min)) + Math.min(min, max));
        try { Thread.sleep(time); } catch (Exception e) { e.printStackTrace(); }
    }
    
    public void wait(int mills) {
        wait(mills, mills);
    }
    
    public Point getMousePos() {
        return new Point(cx,cy);
    }
    
    private synchronized Point moveMouseImpl(int x, int y) {
        int btnMask = (leftDown ? MouseEvent.BUTTON1_DOWN_MASK : 0) | (rightDown ? (MouseEvent.BUTTON3_DOWN_MASK | MouseEvent.META_DOWN_MASK) : 0);
        if (isDragging()) {
            BlockingEventQueue.sendUnblocked(new MouseEvent(comp,MouseEvent.MOUSE_DRAGGED,System.currentTimeMillis(),btnMask,x,y,0,false,0));
        } else if (x > 0 && x < comp.getWidth() && y > 0 && y < comp.getHeight()) {
            if (mousein) {
                BlockingEventQueue.sendUnblocked(new MouseEvent(comp,MouseEvent.MOUSE_MOVED,System.currentTimeMillis(),btnMask,x,y,0,false,0));
            } else {
                mousein = true;
                BlockingEventQueue.sendUnblocked(new MouseEvent(comp,MouseEvent.MOUSE_ENTERED,System.currentTimeMillis(),btnMask,x,y,0,false,0));
            }
        } else {
            if (mousein) {
                BlockingEventQueue.sendUnblocked(new MouseEvent(comp,MouseEvent.MOUSE_EXITED,System.currentTimeMillis(),btnMask,x,y,0,false,0));
                mousein = false;
            } else {
                //Mouse outside and still out, no event.
            }
        }
        cx = x;
        cy = y;
        return new Point(cx, cy);
    }
    
    /**
     * Internal mouse movement algorithm. Do not use this without credit to either
     * Benjamin J. Land or BenLand100. This is synchronized to prevent multiple
     * motions and bannage.
     * @param xs The x start
     * @param ys The y start
     * @param xe The x destination
     * @param ye The y destination
     * @param gravity Strength pulling the position towards the destination
     * @param wind Strength pulling the position in random directions
     * @param minWait Minimum relative time per step
     * @param maxWait Maximum relative time per step
     * @param maxStep Maximum size of a step, prevents out of control motion
     * @param targetArea Radius of area around the destination that should
     *                   trigger slowing, prevents spiraling
     * @result The actual end point
     */
    private synchronized Point windMouseImpl(double xs, double ys, double xe, double ye, double gravity, double wind, double minWait, double maxWait, double maxStep, double targetArea) {
        //System.out.println(targetArea);
        final double sqrt3 = Math.sqrt(3);
        final double sqrt5 = Math.sqrt(5);
        double dist, veloX = 0, veloY = 0, windX = 0, windY = 0;
        while ((dist = Math.hypot(xs - xe,ys - ye)) >= 1) {
            wind = Math.min(wind, dist);
            if (dist >= targetArea) {
                windX = windX / sqrt3 + (Math.random() * (wind * 2D + 1D) - wind) / sqrt5;
                windY = windY / sqrt3 + (Math.random() * (wind * 2D + 1D) - wind) / sqrt5;
            } else {
                windX /= sqrt3;
                windY /= sqrt3;
                if (maxStep < 3) {
                    maxStep = Math.random() * 3 + 3D;
                } else {
                    maxStep /= sqrt5;
                }
                //System.out.println(maxStep + ":" + windX + ";" + windY);
            }
            veloX += windX + gravity * (xe - xs) / dist;
            veloY += windY + gravity * (ye - ys) / dist;
            double veloMag = Math.hypot(veloX, veloY);
            if (veloMag > maxStep) {
                double randomDist = maxStep / 2D + Math.random() * maxStep / 2D;
                veloX = (veloX / veloMag) * randomDist;
                veloY = (veloY / veloMag) * randomDist;
            }
            xs += veloX;
            ys += veloY;
            int mx = (int) Math.round(xs);
            int my = (int) Math.round(ys);
            if (cx != mx || cy != my) {
                //Scratch
                /*g.drawLine(cx,cy,mx,my);
                frame.repaint();*/
                //MouseJacking
                /*try {
                    Robot r = new Robot();
                    r.mouseMove(mx,my);
                } catch (Exception e) { } */
                moveMouseImpl(mx, my);
            }
            double step = Math.hypot(xs - cx, ys - cy);
            try {
                Thread.sleep(Math.round((maxWait - minWait) * (step / maxStep) + minWait));
            } catch (InterruptedException ex) {  }
        }
        //System.out.println(Math.abs(xe - cx) + ", " + Math.abs(ye - cy));
        return new Point(cx, cy);
    }
    
    /**
     * Moves the mouse from the current position to the specified position.
     * Approximates human movement in a way where smoothness and accuracy are
     * relative to speed, as it should be.
     * @param x The x destination
     * @param y The y destination
     * @result The actual end point
     */
    public Point windMouse(int x, int y) {
        synchronized (this) {
            if (canInteract()) {
                double speed = (Math.random() * 15D + 15D) / 10D;
                return windMouseImpl(cx,cy,x,y,9D,3D,5D/speed,10D/speed,10D*speed,8D*speed);
            }
            return null;
        }
    }
    
    /**
     * Moves the mouse from the current position to the specified position.
     * Approximates human movement in a way where smoothness and accuracy are
     * relative to speed, as it should be.
     * @param x The x destination
     * @param y The y destination
     * @result The actual end point
     */
    public Point moveMouse(int x, int y) {
        synchronized (this) {
            if (canInteract()) {
                return moveMouseImpl(x,y);
            }
            return null;
        }
    }
    
    /**
     * Drags the mouse from the current position to the specified position.
     * @param x The x destination
     * @param y The y destination
     * @result The actual end point
     */
    public Point dragMouse(int x, int y) {
        synchronized (this) {
            if (canHold(true)) {
                leftDown = true;
                int btnMask = MouseEvent.BUTTON1_DOWN_MASK | (rightDown ? (MouseEvent.BUTTON3_DOWN_MASK | MouseEvent.META_DOWN_MASK) : 0);
                int btn = MouseEvent.BUTTON1;
                if (!focused) getFocus();
                BlockingEventQueue.sendUnblocked(new MouseEvent(comp,MouseEvent.MOUSE_PRESSED,System.currentTimeMillis(),btnMask,cx,cy,1,false,btn));
                try { Thread.sleep((int)(Math.random() * 56 + 90)); } catch (Exception ex) { }
                Point end = moveMouse(x,y);
                try { Thread.sleep((int)(Math.random() * 56 + 90)); } catch (Exception ex) { }
                BlockingEventQueue.sendUnblocked(new MouseEvent(comp,MouseEvent.MOUSE_RELEASED,System.currentTimeMillis(),btnMask,end.x,end.y,1,false,btn));
                leftDown = false;
                if (!mousein) looseFocus(false);
                return end;
            }
            return null;
        }
    }
    
    /**
     * Holds the mouse at the specified position after moving from the current
     * position to the specified position.
     * @param x The x destination
     * @param y The y destination
     * @result The actual end point
     */
    public Point holdMouse(int x, int y, boolean left) {
        synchronized (this) {
            if (canHold(left)) {
                int btnMask = ((leftDown || left) ? MouseEvent.BUTTON1_DOWN_MASK : 0) | ((rightDown || !left) ? (MouseEvent.BUTTON3_DOWN_MASK | MouseEvent.META_DOWN_MASK) : 0);
                int btn = left ? MouseEvent.BUTTON1 : MouseEvent.BUTTON3;
                Point end = moveMouse(x,y);
                if (mousein) {
                    BlockingEventQueue.sendUnblocked(new MouseEvent(comp,MouseEvent.MOUSE_PRESSED,System.currentTimeMillis(),btnMask,cx,cy,1,false,btn));
                    if (!focused) {
                        wait(25,50);
                        getFocus();
                    }
                    if (left) leftDown = true; else rightDown = true;
                }
                return end;
            }
            return null;
        }
    }
    
    /**
     * Releases the mouse at the specified position after moving from the current
     * position to the specified position.
     * @param x The x destination
     * @param y The y destination
     * @result The actual end point
     */
    public Point releaseMouse(int x, int y, boolean left) {
        synchronized (this) {
            if (canRelease(left)) {
                int btnMask = ((leftDown || left) ? MouseEvent.BUTTON1_DOWN_MASK : 0) | ((rightDown || !left) ? (MouseEvent.BUTTON3_DOWN_MASK | MouseEvent.META_DOWN_MASK) : 0);
                int btn = left ? MouseEvent.BUTTON1 : MouseEvent.BUTTON3;
                Point end = moveMouse(x,y);
                if (mousein) {
                    long time = System.currentTimeMillis();
                    BlockingEventQueue.sendUnblocked(new MouseEvent(comp,MouseEvent.MOUSE_RELEASED,time,btnMask,end.x,end.y,1,false,btn));
                    BlockingEventQueue.sendUnblocked(new MouseEvent(comp,MouseEvent.MOUSE_CLICKED,time,btnMask,end.x,end.y,1,false,btn));
                    if (left) leftDown = false; else rightDown = false;
                } else {
                    looseFocus(false);
                }
                return end;
            }
            return null;
        }
    }
    
    /**
     * Clicks the mouse at the specified position after moving from the current
     * position to the specified position.
     * @param x The x destination
     * @param y The y destination
     * @result The actual end point
     */
    public Point clickMouse(int x, int y, boolean left) {
        synchronized (this) {
            if (canClick(left)) {
                int btnMask = ((leftDown || left) ? MouseEvent.BUTTON1_DOWN_MASK : 0) | ((rightDown || !left) ? (MouseEvent.BUTTON3_DOWN_MASK | MouseEvent.META_DOWN_MASK) : 0);
                int btn = left ? MouseEvent.BUTTON1 : MouseEvent.BUTTON3;
                Point end = moveMouse(x,y);
                if (mousein) {
                    BlockingEventQueue.sendUnblocked(new MouseEvent(comp,MouseEvent.MOUSE_PRESSED,System.currentTimeMillis(),btnMask,cx,cy,1,false,btn));
                    if (!focused) {
                        wait(25,50);
                        getFocus();
                    }
                    try { Thread.sleep((int)(Math.random() * 56 + 90)); } catch (Exception ex) { }
                    long time = System.currentTimeMillis();
                    BlockingEventQueue.sendUnblocked(new MouseEvent(comp,MouseEvent.MOUSE_RELEASED,time,btnMask,end.x,end.y,1,false,btn));
                    BlockingEventQueue.sendUnblocked(new MouseEvent(comp,MouseEvent.MOUSE_CLICKED,time,btnMask,end.x,end.y,1,false,btn));
                    leftDown = false;
                } else {
                    looseFocus(false);
                }
                return end;
            }
            return null;
        }
    }
    
    /**
     * Tests if a character requires the shift key to be pressed.
     * @param c Char to check for
     * @result True if shift is required
     */
    private boolean isShiftChar(char c) {
        String special = "~!@#$%^&*()_+|{}:\"<>?";
        return special.indexOf(c) != -1 || (c - 'A' >= 0 && c - 'A' <= 25);
    }
    
    /**
     * Converts a char into a KeyCode value for MouseEvent
     * @param c Char to convert
     * @result c's KeyCode
     */
    private int toKeyCode(char c) {
        final String special = "~!@#$%^&*()_+|{}:\"<>?";
        final String normal = "`1234567890-=\\[];',./";
        int index = special.indexOf(c);
        return Character.toUpperCase(index == -1 ? c : normal.charAt(index));
    }
    
    /**
     * Holds a key. Should be used for any key that needs to be held, not
     * sending text.
     * @param code KeyCode for the key
     * @param mills MS to send key for
     */
    public void holdKey(int code) {
        synchronized (this) {
            if (canInteract()) {
                if (!focused) getFocus();
                long startTime = System.currentTimeMillis();
                int[] dat = new int[] {code, (int) (startTime & 0xFFFFFFFF)};
                if (!isKeyHeld(dat)) {
                    if (KeyEvent.VK_SHIFT == code) shiftDown = true;
                    BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_PRESSED, startTime, shiftDown ? KeyEvent.SHIFT_DOWN_MASK : 0, code, KeyEvent.CHAR_UNDEFINED, KeyEvent.KEY_LOCATION_STANDARD));
                    char c = shiftDown ? Character.toUpperCase((char)code) : Character.toLowerCase((char)code);
                    if (Character.isLetterOrDigit(c) || Character.isSpaceChar(c))
                        BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_TYPED, startTime, shiftDown ? KeyEvent.SHIFT_DOWN_MASK : 0, 0, (char)c, KeyEvent.KEY_LOCATION_UNKNOWN));
                    setKeyHeld(dat, true);
                }
            }
        }
    }
    
    /**
     * Release a key. Should be used for any key that needs to be held, not
     * sending text. Will only release it if its already held.
     * @param code KeyCode for the key
     * @param mills MS to send key for
     */
    public void releaseKey(int code) {
        synchronized (this) {
            if (canInteract()) {
                if (!focused) getFocus();
                long startTime = System.currentTimeMillis();
                int[] dat = new int[] {code};
                if (isKeyHeld(dat)) {
                    setKeyHeld(dat, false);
                    BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_RELEASED, startTime, 0, code, KeyEvent.CHAR_UNDEFINED, KeyEvent.KEY_LOCATION_STANDARD));
                    if (KeyEvent.VK_SHIFT == code) shiftDown = false;
                }
            }
        }
    }
    
    /**
     * Allows KeyEvents to be easily and safely passed through the blocking 
     * mechanism. Also ensures the target component is the target of this nazi
     */
    public void passKeyEvent(KeyEvent e) {
		synchronized (this) {
            if (canInteract()) {
                if (!focused) getFocus();  
                BlockingEventQueue.sendUnblocked(new KeyEvent(comp, e.getID(), e.getWhen(), e.getModifiers(), e.getKeyCode(), e.getKeyChar(), e.getKeyLocation()));
            }
        }  
    }
    
    
    public boolean isKeyDown(int code) {
        int[] dat = new int[] {code};
        return isKeyHeld(dat);
    }
    
    /**
     * Sends a string to the client like a person would type it.
     * In Scar you can use Chr(10) for enter, not Chr(13)
     * Not to be used for arrow keys, but can be used with F keys or the like
     * @param text String to send to the client
     */
    public void sendKeys(String text) {
        synchronized (this) {
            if (canInteract()) {
                char[] chars = text.toCharArray();
                if (!focused) getFocus();
                for (char c : chars) {
                    int code = toKeyCode(c);
                    int keyLoc = Character.isDigit(c) ? Math.random() > 0.5D ? KeyEvent.KEY_LOCATION_NUMPAD : KeyEvent.KEY_LOCATION_STANDARD : KeyEvent.KEY_LOCATION_STANDARD;
                    if (isShiftChar(c)) {
                        int shiftLoc = Math.random() > 0.5D ? KeyEvent.KEY_LOCATION_RIGHT : KeyEvent.KEY_LOCATION_LEFT;
                        BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_PRESSED, System.currentTimeMillis(), KeyEvent.SHIFT_MASK, KeyEvent.VK_SHIFT, KeyEvent.CHAR_UNDEFINED, shiftLoc));
                        try { Thread.sleep((int)(Math.random() * 20 + 60)); } catch (Exception e) { e.printStackTrace(); }
                        long time = System.currentTimeMillis();
                        BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_PRESSED, time, KeyEvent.SHIFT_MASK, code, c, keyLoc));
                        BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_TYPED, time, KeyEvent.SHIFT_MASK, 0, c, KeyEvent.KEY_LOCATION_UNKNOWN));
                        try { Thread.sleep((int)(Math.random() * 20 + 90)); } catch (Exception e) { e.printStackTrace(); }
                        BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_RELEASED, System.currentTimeMillis(), KeyEvent.SHIFT_MASK, code, c, keyLoc));
                        try { Thread.sleep((int)(Math.random() * 20 + 60)); } catch (Exception e) { e.printStackTrace(); }
                        BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_RELEASED, System.currentTimeMillis(), 0, KeyEvent.VK_SHIFT, KeyEvent.CHAR_UNDEFINED, shiftLoc));
                    } else {
                        long time = System.currentTimeMillis();
                        BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_PRESSED, time, 0, code, c, keyLoc));
                        BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_TYPED, time, 0, 0, c, KeyEvent.KEY_LOCATION_UNKNOWN));
                        try { Thread.sleep((int)(Math.random() * 20 + 90)); } catch (Exception e) { e.printStackTrace(); }
                        BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_RELEASED, System.currentTimeMillis(), 0, code, c, keyLoc));
                    }
                }
            }
        }
    }
    
    /**
     * Sends an event that emulates a user alt+tabbing into RuneScape.
     */
    private void getFocus() {
        synchronized (this) {
            if (!focused) {
                BlockingEventQueue.sendUnblocked(new FocusEvent(comp, FocusEvent.FOCUS_GAINED, false, null));
                focused = true;
                wait(100,200);
            }
        }
    }
    
    /**
     * Sends an event that emulates a user alt+tabbing out of RuneScape.
     */
    private void looseFocus(boolean tabbed) {
        synchronized (this) {
            if (focused) {
                if (tabbed) {
                    BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_PRESSED,System.currentTimeMillis(),KeyEvent.ALT_DOWN_MASK,KeyEvent.VK_ALT,KeyEvent.CHAR_UNDEFINED,KeyEvent.KEY_LOCATION_LEFT));
                    wait(100,200);
                    BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_PRESSED,System.currentTimeMillis(),KeyEvent.ALT_DOWN_MASK,KeyEvent.VK_TAB,'\t',KeyEvent.KEY_LOCATION_STANDARD));
                    wait(100,200);
                    BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_RELEASED,System.currentTimeMillis(),0,KeyEvent.VK_ALT,KeyEvent.CHAR_UNDEFINED,KeyEvent.KEY_LOCATION_LEFT));
                    wait(10,50);
                }
                BlockingEventQueue.sendUnblocked(new FocusEvent(comp, FocusEvent.FOCUS_LOST, false, null));
                BlockingEventQueue.sendUnblocked(new FocusEvent(comp, FocusEvent.FOCUS_LOST, false, null));
                focused = false;
                wait(100,200);
            }
        }
    }
    
    private void setKeyHeld(int[] dat, boolean held) {
        if (held) {
            keysHeld.add(dat);
        } else {
            for (int[] entry : keysHeld) {
                if (entry[0] == dat[0]) {
                    keysHeld.remove(entry);
                }
            }
        }
    }
    
    private boolean isKeyHeld(int[] dat) {
        for (int[] entry : keysHeld) {
            if (entry[0] == dat[0]) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * Kills this EvetNazi without making bad events possible.
     */
    public void destroy() {
        //Scratch
        /*frame.dispose();*/
        keySender.stop();
        for (int[] dat : keysHeld) {
            releaseKey(dat[1]);
        }
        Point pos;
        if ((pos = comp.getMousePosition()) != null && !mousein) {
            moveMouse(pos.x,pos.y);
        } else if (focused && pos == null && mousein) {
            moveMouse(-10,-10);
        }
        if (focused) {
            //Its impossible for the comp to have focus, you clicked elsewhere
            //in order to stop blocking...
            //Don't forget to clear native focus also...
            KeyboardFocusManager.getCurrentKeyboardFocusManager().clearGlobalFocusOwner();
            looseFocus(true);
        }
        nazies.remove(comp);
        comp = null;
        active = false;
    }
    
    public boolean isActive() {
        if (!BlockingEventQueue.isBlocking(comp)) destroy();
        return active;
    }
    
    public boolean isFocused() {
        return focused;
    }
    
    public boolean isDragging() {
        return leftDown || rightDown;
    }
    
    public boolean isDown(boolean left) {
        return left ? leftDown : rightDown;
    }
    
    public boolean canInteract() {
        return active;
    }
    
    public boolean canClick(boolean left) {
        return canInteract() && (left ? !leftDown : !rightDown);
    }
    
    public boolean canHold(boolean left) {
        return canInteract() && (left ? !leftDown : !rightDown);
    }
    
    public boolean canRelease(boolean left) {
        return canInteract() && (left ? leftDown : rightDown);
    }
    
    private class KeySender extends Thread {
        
        public KeySender(String name) {
            super(name);
        }
        
        public void run() {
            while (true) {
                try { Thread.sleep(30 + (int)(Math.random()*5)); } catch (InterruptedException ex) { }
                int minTime = (int) (System.currentTimeMillis() & 0xFFFFFFFF) - 1000;
                for (int[] dat : keysHeld)  {
                    if (dat[1] < minTime) {
                        BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_PRESSED, System.currentTimeMillis(), shiftDown ? KeyEvent.SHIFT_DOWN_MASK : 0, dat[0], KeyEvent.CHAR_UNDEFINED, KeyEvent.KEY_LOCATION_STANDARD));
                        char c = shiftDown ? Character.toUpperCase((char)dat[0]) : Character.toLowerCase((char)dat[0]);
                        if (Character.isLetterOrDigit(c) || Character.isSpaceChar(c))
                            BlockingEventQueue.sendUnblocked(new KeyEvent(comp, KeyEvent.KEY_TYPED, System.currentTimeMillis(), shiftDown ? KeyEvent.SHIFT_DOWN_MASK : 0, 0, (char)c, KeyEvent.KEY_LOCATION_UNKNOWN));
                    }
                }
            }
        }
        
    }
    
}
