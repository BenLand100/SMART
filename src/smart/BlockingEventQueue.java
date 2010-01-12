/**
 *  Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of the Scar Minimizing Autoing Resource Thing (SMART)
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

import java.awt.AWTEvent;
import java.awt.Component;
import java.awt.EventQueue;
import java.awt.Toolkit;
import java.awt.event.FocusEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.WindowEvent;
import java.util.HashMap;
import java.util.LinkedList;

/**
 * Smart blocks events to the client by overriding the AWTEventQueue. This 
 * queue is responsible for sending events to anything and everything in java.
 * Therefore, by checking the "source" of the event, we can determin whether or 
 * not to block it. Also, we can inject events and distribute them exactly as 
 * AWT would, so it is no different to the client. 
 *
 * This class was written for another project, and is easily portable through 
 * the provided methods. Feel free to use this class with credit.
 *
 * @author Benjamin J. Land
 */
public class BlockingEventQueue extends EventQueue {
    
    /**
     * This one single instance is all that will ever be used in this instance
     * of Java. Makes sure we don't mess ourselves up.
     */
    private static final BlockingEventQueue instance = new BlockingEventQueue();
    /**
     * Maintains a list of all components added to the queue's blocking list
     */
    private static final LinkedList<Component> blockedComponents = new LinkedList<Component>();
    /**
     * Keeps track of whether or not the component is being blocked at the 
     * moment.
     */
    private static final HashMap<Component,Boolean> blocking = new HashMap<Component,Boolean>();
    
    private static final HashMap<Component,EventRedirect> redirects = new HashMap<Component,EventRedirect>();
    
    /**
     * Adds a component to the queue and set it to start blocking
     * @param comp The component to be blocked
     */
    public static void addComponent(Component comp, EventRedirect redirect) {
        if (!blockedComponents.contains(comp)) {
            blockedComponents.add(comp);
            redirects.put(comp,redirect);
            setBlocking(comp, true);
        }
    }
    
    /**
     * Removes a component to the queue and set it to stop blocking
     * @param comp The component to be removed
     */
    public static void removeComponent(Component comp) {
        if (blockedComponents.contains(comp)) {
            setBlocking(comp,false);
            blockedComponents.remove(comp);
            blocking.remove(comp);
        }
    }
    
    /**
     * Changes the blocking status of a component. Creates an event nazi if 
     * nessessary.
     * @param comp The component to be changed
     * @param isBlocking Whether or not the component should be blocking
     */
    public static void setBlocking(Component comp, boolean isBlocking) {
        if (!blockedComponents.contains(comp))
            throw new RuntimeException("Component not found: " + comp.getName());
        if (isBlocking) {
            blocking.put(comp, isBlocking);
            if (EventNazi.getNazi(comp) == null) 
                EventNazi.createNazi(comp);
        } else {
            if (EventNazi.getNazi(comp) != null) EventNazi.getNazi(comp).destroy();
            blocking.put(comp, isBlocking);
        }
    }
    
    /**
     * Checks to see if a component is blocking
     * @param comp The component to be checked
     * @result True if blocking
     */
    public static boolean isBlocking(Component comp) {
        if (!blockedComponents.contains(comp))
            return false;
        return blocking.get(comp);
    }
    
    /**
     * Call this method whenever you want to make sure something is being blocked.
     * Needs to be called at least once per program...
     */
    public static void ensureBlocking() {
        if (!(Toolkit.getDefaultToolkit().getSystemEventQueue() instanceof BlockingEventQueue)) {
            Toolkit.getDefaultToolkit().getSystemEventQueue().push(instance);
        }
    }
    
    /**
     * Send an event skiping blocking. Just wraps the event and unwraps it in
     * the queue.
     * @param e The event to send unblocked
     */
    public static void sendUnblocked(AWTEvent e) {
        instance.postEvent(new UnblockedEvent(e));
    }
    
    /**
     * If it gets called more than once, you've pretty much screwed yourself.
     */
    private BlockingEventQueue() {
    }
    
    /**
     * The backbone to event blocking. Simple concept really.
     * @param event The next event in the queue
     */
    protected void dispatchEvent(AWTEvent event) {
        if (event instanceof UnblockedEvent) {
            AWTEvent e = ((UnblockedEvent)event).getEvent();
            ((Component)e.getSource()).dispatchEvent(e);
        } else if (blockedComponents.contains(event.getSource()) && blocking.get(event.getSource())) {
            if (event instanceof MouseEvent || event instanceof KeyEvent || event instanceof WindowEvent || event instanceof FocusEvent) {
                redirects.get(event.getSource()).dispatched(event);
                return;
            }
            super.dispatchEvent(event);
        } else {
            super.dispatchEvent(event);
        }
    }
    
}
