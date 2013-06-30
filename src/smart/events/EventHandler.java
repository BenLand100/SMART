package smart.events;

import java.awt.AWTEvent;
import java.awt.Canvas;
import java.awt.Component;
import java.awt.EventQueue;
import java.awt.Toolkit;
import java.awt.event.FocusEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;
import java.awt.event.WindowEvent;
import java.util.HashMap;

public class EventHandler extends EventQueue {

    private static EventHandler instance = new EventHandler();
    private static HashMap<Component, boolean[]> Block = new HashMap<>();

    private EventHandler() {
    }

    public static EventHandler getInstance() {
        return instance;
    }

    public static void setBlocking(Component component, boolean MouseInput, boolean KeyInput) {
        if (component != null) {
            synchronized (Block) {
                Block.put(component, new boolean[]{MouseInput, KeyInput});
            }
        }

        if (!(Toolkit.getDefaultToolkit().getSystemEventQueue() instanceof EventHandler)) {
            Toolkit.getDefaultToolkit().getSystemEventQueue().push(instance);
        }
    }

    public static void remove(Component component) {
        if (component != null) {
            synchronized (Block) {
                if (Block.containsKey(component)) {
                    Block.remove(component);
                }
            }
        }
    }

    public static void clear() {
        Block.clear();
    }

    public static void PostEvent(AWTEvent e) {
        if (e != null && e.getSource() != null) {
            instance.postEvent(new UnblockedEvent(e));
        }
    }

    @Override
    protected void dispatchEvent(AWTEvent event) {
        if (event instanceof UnblockedEvent) {
            AWTEvent e = ((UnblockedEvent) event).event;
            ((Component) e.getSource()).dispatchEvent(e);
        } else if (Block.containsKey(event.getSource())) {
            boolean Input[] = Block.get(event.getSource());
            if (event instanceof KeyEvent) {
                if (Input[1]) {
                    super.dispatchEvent(event);
                }
            } else if (event instanceof MouseEvent || event instanceof MouseWheelEvent) {
                if (Input[0]) {
                    super.dispatchEvent(event);
                } else if (Input[1] && event.getID() == MouseEvent.MOUSE_CLICKED && event.getSource() instanceof Canvas) {
                    Canvas canvas = ((Canvas) event.getSource());
                    if (canvas.getMousePosition() != null && canvas.isShowing() && !canvas.isFocusOwner()) {
                        canvas.requestFocusInWindow();
                    }
                }
            } else if ((event instanceof FocusEvent || event instanceof WindowEvent) && Input[0]) {
                super.dispatchEvent(event);
            } else {
                super.dispatchEvent(event);
            }
        } else {
            super.dispatchEvent(event);
        }
    }
}
