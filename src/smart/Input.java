package smart;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import smart.events.EventHandler;

/**
 * @author Brandon
 */
public class Input {

    private Component comp;
    private int cx, cy;
    private boolean mousein;
    private boolean active = false;
    private boolean leftDown = false;
    private boolean midDown = false;
    private boolean rightDown = false;
    private boolean focused = false;
    private boolean shiftDown = false;
    private boolean mouseEnabled, keysEnabled;
    private Set<int[]> keysHeld = Collections.synchronizedSet(new HashSet<int[]>());
    private KeySender keySender = new KeySender("java-key-thread");

    public Input() {
        keySender.start();
    }

    public boolean isMouseEnabled() {
        return mouseEnabled;
    }

    public boolean isKeysEnabled() {
        return keysEnabled;
    }
    
    public void setMouseEnabled(boolean Enabled) {
        this.mouseEnabled = Enabled;
        EventHandler.setBlocking(comp, mouseEnabled, keysEnabled);
    }
    
    public void setKeysEnabled(boolean Enabled) {
        this.keysEnabled = Enabled;
        EventHandler.setBlocking(comp, mouseEnabled, keysEnabled);
    }
    
    public void setActive(boolean active) {
        this.active = active;
    }
    
    public void setComponent(Component comp) {
        this.comp = comp;
        if (!this.active) {
            if (comp.getMousePosition() != null && comp.isFocusOwner() && comp.isShowing()) {
                this.cx = comp.getMousePosition().x;
                this.cy = comp.getMousePosition().y;
                this.mousein = true;
            } else {
                this.cx = -1;
                this.cy = -1;
                this.mousein = false;
            }
            this.active = true;
            this.focused = comp.isFocusOwner() && comp.isShowing();
        }
    }

    public void getFocus() {
        if (!isFocused()) {
            EventHandler.PostEvent(new FocusEvent(this.comp, FocusEvent.FOCUS_GAINED, false, null));
            this.focused = true;
            wait(100, 200);
        }
    }

    public void looseFocus() {
        if (isFocused()) {
            EventHandler.PostEvent(new FocusEvent(this.comp, FocusEvent.FOCUS_LOST, false, null));
            this.focused = false;
            wait(100, 200);
        }
    }

    public void wait(int min, int max) {
        int time = min == max ? min : (int) ((Math.random() * Math.abs(max - min)) + Math.min(min, max));
        Utilities.sleep(time);
    }

    public void wait(int mills) {
        wait(mills, mills);
    }

    public Point getMousePos() {
        return new Point(cx, cy);
    }

    private synchronized Point moveMouseImpl(int x, int y) {
        int btnMask = (leftDown ? MouseEvent.BUTTON1_DOWN_MASK : 0) | (rightDown ? (MouseEvent.BUTTON3_DOWN_MASK | MouseEvent.META_DOWN_MASK) : 0);
        if (isDragging()) {
            EventHandler.PostEvent(new MouseEvent(comp, MouseEvent.MOUSE_DRAGGED, System.currentTimeMillis(), btnMask, x, y, 0, false, 0));
        } else if (x > 0 && x < comp.getWidth() && y > 0 && y < comp.getHeight()) {
            if (mousein) {
                EventHandler.PostEvent(new MouseEvent(comp, MouseEvent.MOUSE_MOVED, System.currentTimeMillis(), btnMask, x, y, 0, false, 0));
            } else {
                mousein = true;
                EventHandler.PostEvent(new MouseEvent(comp, MouseEvent.MOUSE_ENTERED, System.currentTimeMillis(), btnMask, x, y, 0, false, 0));
            }
        } else {
            if (mousein) {
                EventHandler.PostEvent(new MouseEvent(comp, MouseEvent.MOUSE_EXITED, System.currentTimeMillis(), btnMask, x, y, 0, false, 0));
                mousein = false;
            }
        }
        cx = x;
        cy = y;
        return new Point(cx, cy);
    }

    private synchronized Point windMouseImpl(double xs, double ys, double xe, double ye, double gravity, double wind, double minWait, double maxWait, double maxStep, double targetArea) {
        final double sqrt3 = Math.sqrt(3);
        final double sqrt5 = Math.sqrt(5);


        double dist, veloX = 0, veloY = 0, windX = 0, windY = 0;
        while ((dist = Math.hypot(xs - xe, ys - ye)) >= 1) {
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
                moveMouseImpl(mx, my);
            }
            double step = Math.hypot(xs - cx, ys - cy);
            Utilities.sleep(Math.round((maxWait - minWait) * (step / maxStep) + minWait));
        }
        return new Point(cx, cy);
    }

    public synchronized Point windMouse(int x, int y) {
        if (canInteract()) {
            double speed = (Math.random() * 15D + 15D) / 10D;
            return windMouseImpl(cx, cy, x, y, 9D, 3D, 5D / speed, 10D / speed, 10D * speed, 8D * speed);
        }
        return null;
    }

    public synchronized Point moveMouse(int x, int y) {
        if (canInteract()) {
            return moveMouseImpl(x, y);
        }
        return null;
    }

    public synchronized Point holdMouse(int x, int y, int button) {
        if (canHold(button)) {
            int btnMask = ((leftDown || button == 1) ? MouseEvent.BUTTON1_DOWN_MASK : 0) | ((midDown || button == 2) ? (MouseEvent.BUTTON2_DOWN_MASK | MouseEvent.META_DOWN_MASK) : 0) | ((rightDown || button == 3) ? (MouseEvent.BUTTON3_DOWN_MASK | MouseEvent.META_DOWN_MASK) : 0);
            int btn = 0;
            switch (button) {
                case 1:
                    btn = MouseEvent.BUTTON1;
                    break;
                case 2:
                    btn = MouseEvent.BUTTON2;
                    break;
                case 3:
                    btn = MouseEvent.BUTTON3;
                    break;
            }
            Point end = moveMouse(x, y);
            if (mousein) {
                EventHandler.PostEvent(new MouseEvent(comp, MouseEvent.MOUSE_PRESSED, System.currentTimeMillis(), btnMask, cx, cy, 1, false, btn));
                if (!focused) {
                    wait(25, 50);
                    getFocus();
                }
                switch (button) {
                    case 1:
                        leftDown = true;
                        break;
                    case 2:
                        midDown = true;
                        break;
                    case 3:
                        rightDown = true;
                        break;
                }
            }
            return end;
        }
        return null;
    }

    public synchronized Point releaseMouse(int x, int y, int button) {
        if (canRelease(button)) {
            int btnMask = ((leftDown || button == 1) ? MouseEvent.BUTTON1_DOWN_MASK : 0) | ((midDown || button == 2) ? (MouseEvent.BUTTON2_DOWN_MASK | MouseEvent.META_DOWN_MASK) : 0) | ((rightDown || button == 3) ? (MouseEvent.BUTTON3_DOWN_MASK | MouseEvent.META_DOWN_MASK) : 0);
            int btn = 0;
            switch (button) {
                case 1:
                    btn = MouseEvent.BUTTON1;
                    break;
                case 2:
                    btn = MouseEvent.BUTTON2;
                    break;
                case 3:
                    btn = MouseEvent.BUTTON3;
                    break;
            }
            Point end = moveMouse(x, y);
            if (mousein) {
                long time = System.currentTimeMillis();
                EventHandler.PostEvent(new MouseEvent(comp, MouseEvent.MOUSE_RELEASED, time, btnMask, end.x, end.y, 1, false, btn));
                EventHandler.PostEvent(new MouseEvent(comp, MouseEvent.MOUSE_CLICKED, time, btnMask, end.x, end.y, 1, false, btn));
                switch (button) {
                    case 1:
                        leftDown = false;
                        break;
                    case 2:
                        midDown = false;
                        break;
                    case 3:
                        rightDown = false;
                        break;
                }
            } else {
                looseFocus();
            }
            return end;
        }
        return null;
    }

    public synchronized Point clickMouse(int x, int y, int button) {
        if (canClick(button)) {
            int btnMask = ((leftDown || button == 1) ? MouseEvent.BUTTON1_DOWN_MASK : 0) | ((midDown || button == 2) ? (MouseEvent.BUTTON2_DOWN_MASK | MouseEvent.META_DOWN_MASK) : 0) | ((rightDown || button == 3) ? (MouseEvent.BUTTON3_DOWN_MASK | MouseEvent.META_DOWN_MASK) : 0);
            int btn = 0;
            switch (button) {
                case 1:
                    btn = MouseEvent.BUTTON1;
                    break;
                case 2:
                    btn = MouseEvent.BUTTON2;
                    break;
                case 3:
                    btn = MouseEvent.BUTTON3;
                    break;
            }
            Point end = moveMouse(x, y);
            if (mousein) {
                EventHandler.PostEvent(new MouseEvent(comp, MouseEvent.MOUSE_PRESSED, System.currentTimeMillis(), btnMask, cx, cy, 1, false, btn));
                switch (button) {
                    case 1:
                        leftDown = true;
                        break;
                    case 2:
                        midDown = true;
                        break;
                    case 3:
                        rightDown = true;
                        break;
                }
                if (!focused) {
                    wait(25, 50);
                    getFocus();
                }
                Utilities.sleep((int) (Math.random() * 56 + 90));
                long time = System.currentTimeMillis();
                EventHandler.PostEvent(new MouseEvent(comp, MouseEvent.MOUSE_RELEASED, time, btnMask, end.x, end.y, 1, false, btn));
                EventHandler.PostEvent(new MouseEvent(comp, MouseEvent.MOUSE_CLICKED, time, btnMask, end.x, end.y, 1, false, btn));
                switch (button) {
                    case 1:
                        leftDown = false;
                        break;
                    case 2:
                        midDown = false;
                        break;
                    case 3:
                        rightDown = false;
                        break;
                }
            } else {
                looseFocus();
            }
            return end;
        }
        return null;
    }

    public synchronized boolean isMouseButtonHeld(int button) {
        switch (button) {
            case 1:
                return leftDown;
            case 2:
                return midDown;
            case 3:
                return rightDown;
        }
        return false;
    }

    private boolean isShiftChar(char c) {
        String special = "~!@#$%^&*()_+|{}:\"<>?";
        return special.indexOf(c) != -1 || (c - 'A' >= 0 && c - 'A' <= 25);
    }
    public static int[] typable_vk_keycode = new int[0xff];

    static {
        typable_vk_keycode[32] = 32;
        for (int c = (int) 'A'; c <= (int) 'Z'; c++) {
            typable_vk_keycode[c] = c;
        }
        for (int c = (int) '0'; c <= (int) '9'; c++) {
            typable_vk_keycode[c] = c;
        }
        typable_vk_keycode[186] = ';';
        typable_vk_keycode[187] = '=';
        typable_vk_keycode[188] = ',';
        typable_vk_keycode[189] = '-';
        typable_vk_keycode[190] = '.';
        typable_vk_keycode[191] = '/';
        typable_vk_keycode[192] = '`';
        typable_vk_keycode[219] = '[';
        typable_vk_keycode[220] = '\\';
        typable_vk_keycode[221] = ']';
        typable_vk_keycode[222] = '\'';
        typable_vk_keycode[226] = ',';
    }

    private int toKeyCode(char c) {
        final String special = "~!@#$%^&*()_+|{}:\"<>?";
        final String normal = "`1234567890-=\\[];',./";
        int index = special.indexOf(c);
        return Character.toUpperCase(index == -1 ? c : normal.charAt(index));
    }

    private char toChar(int vk, boolean shift) {
        int code = typable_vk_keycode[vk];
        final String special = "~!@#$%^&*()_+|{}:\"<>?";
        final String normal = "`1234567890-=\\[];',./";
        int index = normal.indexOf((char) code);
        if (index == -1) {
            return shift ? Character.toUpperCase((char) code) : Character.toLowerCase((char) code);
        } else {
            return shift ? special.charAt(index) : (char) code;
        }
    }

    private boolean isTypableCode(int vk) {
        return vk < 0xff && typable_vk_keycode[vk] != 0;
    }

    public synchronized void holdKey(int code) {
        if (canInteract()) {
            if (!focused) {
                getFocus();
            }
            long startTime = System.currentTimeMillis();
            int[] dat = new int[]{code, (int) (startTime & 0xFFFFFFFF)};
            if (!isKeyHeld(dat)) {
                if (KeyEvent.VK_SHIFT == code) {
                    shiftDown = true;
                }
                EventHandler.PostEvent(new KeyEvent(comp, KeyEvent.KEY_PRESSED, startTime, shiftDown ? KeyEvent.SHIFT_DOWN_MASK : 0, code, KeyEvent.CHAR_UNDEFINED, KeyEvent.KEY_LOCATION_STANDARD));
                if (isTypableCode(code)) {
                    //System.out.println("Trying to type " + code + " as '" + toChar(code,shiftDown) + "'");
                    EventHandler.PostEvent(new KeyEvent(comp, KeyEvent.KEY_TYPED, startTime, shiftDown ? KeyEvent.SHIFT_DOWN_MASK : 0, 0, toChar(code, shiftDown), KeyEvent.KEY_LOCATION_UNKNOWN));
                }
                setKeyHeld(dat, true);
            }
        }
    }

    public synchronized void releaseKey(int code) {
        if (canInteract()) {
            if (!focused) {
                getFocus();
            }
            long startTime = System.currentTimeMillis();
            int[] dat = new int[]{code};
            if (isKeyHeld(dat)) {
                setKeyHeld(dat, false);
                EventHandler.PostEvent(new KeyEvent(comp, KeyEvent.KEY_RELEASED, startTime, 0, code, KeyEvent.CHAR_UNDEFINED, KeyEvent.KEY_LOCATION_STANDARD));
                if (KeyEvent.VK_SHIFT == code) {
                    shiftDown = false;
                }
            }
        }
    }

    public synchronized void passKeyEvent(KeyEvent e) {
        if (canInteract()) {
            if (!focused) {
                getFocus();
            }
            EventHandler.PostEvent(new KeyEvent(comp, e.getID(), e.getWhen(), e.getModifiers(), e.getKeyCode(), e.getKeyChar(), e.getKeyLocation()));
        }
    }

    public boolean isKeyDown(int code) {
        int[] dat = new int[]{code};
        return isKeyHeld(dat);
    }

    public synchronized void sendKeys(String text, int keywait, int keymodwait) {
        if (canInteract()) {
            char[] chars = text.toCharArray();
            if (!focused) {
                getFocus();
            }
            for (char c : chars) {
                int code = toKeyCode(c);
                int keyLoc = Character.isDigit(c) ? Math.random() > 0.5D ? KeyEvent.KEY_LOCATION_NUMPAD : KeyEvent.KEY_LOCATION_STANDARD : KeyEvent.KEY_LOCATION_STANDARD;
                if (isShiftChar(c)) {
                    int shiftLoc = Math.random() > 0.5D ? KeyEvent.KEY_LOCATION_RIGHT : KeyEvent.KEY_LOCATION_LEFT;
                    EventHandler.PostEvent(new KeyEvent(comp, KeyEvent.KEY_PRESSED, System.currentTimeMillis(), KeyEvent.SHIFT_MASK, KeyEvent.VK_SHIFT, KeyEvent.CHAR_UNDEFINED, shiftLoc));
                    Utilities.sleep((int) ((Math.random() * 0.1 + 1) * keymodwait));
                    long time = System.currentTimeMillis();
                    EventHandler.PostEvent(new KeyEvent(comp, KeyEvent.KEY_PRESSED, time, KeyEvent.SHIFT_MASK, code, c, keyLoc));
                    EventHandler.PostEvent(new KeyEvent(comp, KeyEvent.KEY_TYPED, time, KeyEvent.SHIFT_MASK, 0, c, KeyEvent.KEY_LOCATION_UNKNOWN));
                    Utilities.sleep((int) ((Math.random() * 0.1 + 1) * keywait));
                    EventHandler.PostEvent(new KeyEvent(comp, KeyEvent.KEY_RELEASED, System.currentTimeMillis(), KeyEvent.SHIFT_MASK, code, c, keyLoc));
                    Utilities.sleep((int) ((Math.random() * 0.1 + 1) * keymodwait));
                    EventHandler.PostEvent(new KeyEvent(comp, KeyEvent.KEY_RELEASED, System.currentTimeMillis(), 0, KeyEvent.VK_SHIFT, KeyEvent.CHAR_UNDEFINED, shiftLoc));
                } else {
                    long time = System.currentTimeMillis();
                    EventHandler.PostEvent(new KeyEvent(comp, KeyEvent.KEY_PRESSED, time, 0, code, c, keyLoc));
                    EventHandler.PostEvent(new KeyEvent(comp, KeyEvent.KEY_TYPED, time, 0, 0, c, KeyEvent.KEY_LOCATION_UNKNOWN));
                    Utilities.sleep((int) ((Math.random() * 0.1 + 1) * keywait));
                    EventHandler.PostEvent(new KeyEvent(comp, KeyEvent.KEY_RELEASED, System.currentTimeMillis(), 0, code, c, keyLoc));
                }
            }
        }
    }

    private synchronized void setKeyHeld(int[] dat, boolean held) {
        synchronized (keysHeld) {
            if (held) {
                keysHeld.add(dat);
            } else {
                HashSet<int[]> remove = new HashSet<>();
                for (int[] entry : keysHeld) {
                    if (entry[0] == dat[0]) {
                        remove.add(entry);
                    }
                }
                keysHeld.removeAll(remove);
            }
        }
    }

    private synchronized boolean isKeyHeld(int[] dat) {
        synchronized (keysHeld) {
            for (int[] entry : keysHeld) {
                if (entry[0] == dat[0]) {
                    return true;
                }
            }
            return false;
        }
    }

    public synchronized void destroy() {
        keySender.stop();
        synchronized (keysHeld) {
            for (int[] dat : keysHeld) {
                releaseKey(dat[1]);
            }
        }
        Point pos;
        if ((pos = comp.getMousePosition()) != null && !mousein) {
            moveMouse(pos.x, pos.y);
        } else if (focused && pos == null && mousein) {
            moveMouse(-10, -10);
        }
        if (focused) {
            looseFocus();
        }
        comp = null;
    }

    public synchronized boolean isFocused() {
        return focused;
    }

    public synchronized boolean isDragging() {
        return leftDown || midDown || rightDown;
    }

    public synchronized boolean isDown(int button) {
        switch (button) {
            case 1:
                return leftDown;
            case 2:
                return midDown;
            case 3:
                return rightDown;
        }
        return false;
    }

    public synchronized boolean canInteract() {
        return active;
    }

    public synchronized boolean canClick(int button) {
        switch (button) {
            case 1:
                return canInteract() && !leftDown;
            case 2:
                return canInteract() && !midDown;
            case 3:
                return canInteract() && !rightDown;
        }
        return false;
    }

    public synchronized boolean canHold(int button) {
        switch (button) {
            case 1:
                return canInteract() && !leftDown;
            case 2:
                return canInteract() && !midDown;
            case 3:
                return canInteract() && !rightDown;
        }
        return false;
    }

    public synchronized boolean canRelease(int button) {
        switch (button) {
            case 1:
                return canInteract() && leftDown;
            case 2:
                return canInteract() && midDown;
            case 3:
                return canInteract() && rightDown;
        }
        return false;
    }

    private class KeySender extends Thread {

        public KeySender(String name) {
            super(name);
        }

        @Override
        public void run() {
            while (true) {
                Utilities.sleep(30 + (int) (Math.random() * 5));
                int minTime = (int) (System.currentTimeMillis() & 0xFFFFFFFF) - 1000;
                synchronized (keysHeld) {
                    for (int[] dat : keysHeld) {
                        if (dat[1] < minTime) {
                            EventHandler.PostEvent(new KeyEvent(comp, KeyEvent.KEY_PRESSED, System.currentTimeMillis(), shiftDown ? KeyEvent.SHIFT_DOWN_MASK : 0, dat[0], KeyEvent.CHAR_UNDEFINED, KeyEvent.KEY_LOCATION_STANDARD));
                            if (isTypableCode(dat[0])) {
                                EventHandler.PostEvent(new KeyEvent(comp, KeyEvent.KEY_TYPED, System.currentTimeMillis(), shiftDown ? KeyEvent.SHIFT_DOWN_MASK : 0, 0, toChar(dat[0], shiftDown), KeyEvent.KEY_LOCATION_UNKNOWN));
                            }
                        }
                    }
                }
                yield();
            }
        }
    }
}
