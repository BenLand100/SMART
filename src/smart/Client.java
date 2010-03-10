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

import java.applet.Applet;
import java.awt.AWTEvent;
import java.awt.BorderLayout;
import java.awt.Canvas;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GridLayout;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferInt;
import java.awt.image.WritableRaster;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.net.URL;
import java.net.URLClassLoader;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Stack;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.border.EmptyBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 * Contains all of the setup and control for the RuneScape client. Responsible
 * for starting that client and monitoring its progress.
 *
 * @author Benjamin J. Land
 */
public class Client implements ActionListener, ChangeListener {
    
    public static final String TITLE = "Public SMARTv5.2 - SMART Minimizing Autoing Resource Thing - By BenLand100";

    private static Hashtable<String, Client> clients = new Hashtable<String, Client>();

    public static void canvasNotify(Canvas it) {
        try {
            //System.out.println("Notify " + it.getClass().getClassLoader().toString());
            if (clients.containsKey(it.getClass().getClassLoader().toString())) {
                //System.out.println("Fired");
                clients.get(it.getClass().getClassLoader().toString()).target(it);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void main(String... args) throws Exception {
        int w = 765;
        int h = 503;
        new Client(ByteBuffer.allocate(w * h * 4), ByteBuffer.allocate(w * h * 4), w, h, "http://world79.runescape.com/","plugin.js?param=o0,a0,s0");
    }
    
    private int width = 825;
    private int height = 500;
    private URLClassLoader thisLoader;
    private ClassLoader gameLoader;
    private Applet clientApplet;
    private JFrame clientFrame;
    private EventNazi nazi;
    private IntBuffer nativeBuff;
    private IntBuffer nativeDebug;
    private Thread blitThread;
    private int refresh;
    public boolean blocking;
    public boolean active;
    public boolean debuggfx = false;
    public int transColor = 0;
    public boolean renderWhileBlocking = true;
    public boolean minimized = false;
    private final String codeRegex = "code\\=([^ ]*) ";
    private final String widthRegex = "width\\=([^ ]*) ";
    private final String heightRegex = "height\\=([^ ]*) ";
    private final String archiveRegex = "archive\\=([^ ]*) ";
    private JButton blockingbtn;
    private JButton gfxbtn;
    private JButton debugbtn;
    private JSlider refreshSlider;
    private Canvas canvas;

    public void target(Canvas it) {
        if (it == canvas) {
            return;
        }
        boolean iam = blocking;
        if (iam) {
            stopBlocking();
        }
        BlockingEventQueue.removeComponent(canvas);
        if (canvas == null) iam = true;
        canvas = it;
        canvas.setRefresh(refresh);
        if (blitThread != null) {
            blitThread.stop();
        }
        blitThread = createBlitThread();
        BlockingEventQueue.addComponent(canvas, new EventRedirect() {

            @Override
            public void dispatched(AWTEvent e) {
                if (e instanceof MouseEvent && e.getID() == MouseEvent.MOUSE_CLICKED) {
                    clientFrame.requestFocusInWindow();
                }
            }
        });
        if (iam) {
            startBlocking();
        }
        blitThread.start();
    }

    public Client(ByteBuffer imgBuffer, ByteBuffer debugBuffer, int w, int h, String root, String params) {
        try {
            width = w;
            height = h;
            System.out.println("JVM Garbage Collection Invoked");
            System.gc();
            System.out.println("Java Initilized - SMART Starting");
            nativeBuff = imgBuffer.asIntBuffer();
            nativeDebug = debugBuffer.asIntBuffer();
            initApplet(root, params);
            initFrame();
            setRefresh(60);
            active = true;
            clientFrame.setVisible(true);
        } catch (Exception e) {
            e.printStackTrace();
            JOptionPane.showMessageDialog(null, "Error starting SMART, ensure the target page has an applet decleration", "SMART", JOptionPane.ERROR_MESSAGE);
        }
    }

    public void moveMouse(int x, int y) {
        if (nazi != null) {
            nazi.moveMouse(x, y);
        }
    }

    public void windMouse(int x, int y) {
        if (nazi != null) {
            nazi.windMouse(x, y);
        }
    }

    public void clickMouse(int x, int y, boolean left) {
        if (nazi != null) {
            nazi.clickMouse(x, y, left);
        }
    }

    public void dragMouse(int x, int y) {
        if (nazi != null) {
            nazi.dragMouse(x, y);
        }
    }

    public void holdMouse(int x, int y, boolean left) {
        if (nazi != null) {
            nazi.holdMouse(x, y, left);
        }
    }

    public void releaseMouse(int x, int y, boolean left) {
        if (nazi != null) {
            nazi.releaseMouse(x, y, left);
        }
    }

    public Point getMousePos() {
        if (nazi != null) {
            return nazi.getMousePos();
        }
        return new Point(-1, -1);
    }

    public void sendKeys(String string) {
        if (nazi != null) {
            nazi.sendKeys(string);
        }
    }

    public void holdKey(int keycode) {
        if (nazi != null) {
            nazi.holdKey(keycode);
        }
    }

    public boolean isKeyDown(int keycode) {
        return nazi != null ? nazi.isKeyDown(keycode) : false;
    }

    public void releaseKey(int keycode) {
        if (nazi != null) {
            nazi.releaseKey(keycode);
        }
    }

    /**
     * Stops sending the image and accepting commands from Delphi.
     * @result True if success
     */
    public boolean stopBlocking() {
        if (blocking) {
            try {
                blocking = false;
                if (canvas != null) {
                    BlockingEventQueue.setBlocking(canvas, false);
                }
                nazi = null;
                blockingbtn.setEnabled(true);
                blockingbtn.setText("Enable SMART");
                gfxbtn.setEnabled(false);
                debugbtn.setEnabled(false);
                System.out.println("SMART Disabled");
                return true;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return false;
    }

    private Thread createBlitThread() {
        try {
            canvas.setBackground(new Color(0xFE, 0xFE, 0xFE));
            final Graphics canvasGraphics = canvas.getCanvasGraphics();
            canvasGraphics.setColor(Color.RED);
            final BufferedImage buffer = new BufferedImage(width, height, BufferedImage.TYPE_INT_BGR);
            System.out.println("Replacing Canvas Drawing Surface");
            canvas.setBuffer(buffer);
            Field rasterField = BufferedImage.class.getDeclaredField("raster");
            rasterField.setAccessible(true);
            WritableRaster bufferRaster = (WritableRaster) rasterField.get(buffer);
            final int[] bufferData = ((DataBufferInt) bufferRaster.getDataBuffer()).getData();
            final BufferedImage debug = new BufferedImage(width, height, BufferedImage.TYPE_INT_BGR);
            WritableRaster debugRaster = (WritableRaster) rasterField.get(debug);
            final Graphics debugGraphics = debug.getGraphics();
            final int[] debugData = ((DataBufferInt) debugRaster.getDataBuffer()).getData();
            return new Thread("Smart_Image_Transfer") {

                @Override
                public void run() {
                    int[] temp = new int[384795];
                    try {
                        System.out.println("Transfer Thread Entered");
                        while (active) {
                            sleep(refresh);
                            while (blocking) {
                                sleep(refresh);
                                nativeBuff.rewind();
                                for (int i = 0; i < 384795; ++i) {
                                    if (bufferData[i] != 0xFEFEFE) {
                                        temp[i] = bufferData[i] << 8;
                                    }
                                }
                                nativeBuff.put(temp);
                                if (renderWhileBlocking && !minimized) {
                                    if (debuggfx) {
                                        debugGraphics.drawImage(buffer, 0, 0, null);
                                        nativeDebug.rewind();
                                        for (int i = 0; i < 384795; ++i) {
                                            int color = nativeDebug.get() >> 8;
                                            if (color != transColor) {
                                                debugData[i] = color;
                                            }
                                        }
                                        canvasGraphics.drawImage(debug, 0, 0, null);
                                    } else {
                                        canvasGraphics.drawImage(buffer, 0, 0, null);
                                    }
                                    Point p = getMousePos();
                                    canvasGraphics.fillOval(p.x - 2, p.y - 2, 4, 4);
                                }
                            }
                            canvasGraphics.drawImage(buffer, 0, 0, null);
                        }
                        System.out.println("Transfer Thread Exited");
                    } catch (IllegalThreadStateException e) {
                        System.out.println("Transfer Thread Died");
                    } catch (Exception e) {
                        System.out.println("Transfer Thread Died");
                        e.printStackTrace();
                        blitThread.start();
                    }
                }
            };
        } catch (Exception e) {
            throw new RuntimeException("Could not replace drawing surface");
        }
    }

    /**
     * Starts sending the image and accepting commands from Delphi. 
     *
     * @result True if success
     */
    public boolean startBlocking() {
        if (!blocking) {
            try {
                blocking = true;
                System.out.println("Disabeling Events");
                BlockingEventQueue.setBlocking(canvas, true);
                BlockingEventQueue.ensureBlocking();
                nazi = EventNazi.getNazi(canvas);
                System.out.println("Starting Image Transfer");
                blockingbtn.setEnabled(true);
                gfxbtn.setEnabled(true);
                debugbtn.setEnabled(true);
                blockingbtn.setText("Disable SMART");
                System.out.println("SMART Enabled");
                return true;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return false;
    }

    /**
     * Frees any remaining refrences and exits the client.
     */
    public void destroy() {
        stopBlocking();
        if (active) {
            System.out.println("Destroying SMART");
            active = false;
            BlockingEventQueue.removeComponent(canvas);
            clientFrame.setVisible(false);
            clientApplet.stop();
            clientApplet.destroy();
            //clientFrame.removeAll();
            //clientFrame.dispose();
            clientApplet = null;
            clientFrame = null;
        } else {
            System.out.println("SMART already destroyed");
        }
        System.out.println("JVM Garbage Collection Invoked");
        System.gc();
    }

    /**
     * Creates the frame that contains the RuneScape client.
     */
    private void initFrame() {
        System.out.println("Setting up Frame");
        clientFrame = new JFrame(TITLE);
        clientFrame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
        clientFrame.addWindowListener(new WindowAdapter() {

            @Override
            public void windowDeiconified(WindowEvent e) {
                minimized = false;
            }

            @Override
            public void windowIconified(WindowEvent e) {
                minimized = true;
            }

            @Override
            public void windowClosing(WindowEvent e) {
                destroy();
            }
        });
        clientFrame.addKeyListener(new KeyAdapter() {

            @Override
            public void keyPressed(KeyEvent e) {
                if (nazi != null) {
                    nazi.holdKey(e.getKeyCode());
                }
            }

            @Override
            public void keyReleased(KeyEvent e) {
                if (nazi != null) {
                    nazi.releaseKey(e.getKeyCode());
                }
            }
        });
        clientFrame.setLayout(new BorderLayout());
        clientFrame.add(clientApplet);
        refreshSlider = new JSlider(JSlider.VERTICAL, 1, 100, 96);
        refreshSlider.addChangeListener(this);
        refresh = 500 / refreshSlider.getValue() + 20;
        clientFrame.add(refreshSlider, BorderLayout.EAST);
        JPanel south = new JPanel();
        south.setBorder(new EmptyBorder(3, 3, 3, 3));
        south.setLayout(new GridLayout());
        clientFrame.add(south, BorderLayout.SOUTH);
        blockingbtn = new JButton("Enable SMART");
        blockingbtn.addActionListener(this);
        south.add(blockingbtn);
        gfxbtn = new JButton("Disable Graphics");
        gfxbtn.addActionListener(this);
        south.add(gfxbtn);
        debugbtn = new JButton("Enable Debug");
        debugbtn.addActionListener(this);
        south.add(debugbtn);
        System.out.println("Client INIT");
        clientApplet.init();
        System.out.println("Client START");
        clientApplet.start();
        System.out.println("Client WAITING");
        try {
            while (clientApplet.getComponentCount() < 1) {
                Thread.sleep(100);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        gameLoader = clientApplet.getComponent(0).getClass().getClassLoader();
        clients.put(gameLoader.toString(), this);
        System.out.println("Client Fully Initialized");
        clientFrame.pack();
        clientFrame.setResizable(false);
        clientFrame.setLocationRelativeTo(null);
    }

    private String parseArg(String arg) {
        return Pattern.matches("\".*\"", arg) ? arg.substring(1, arg.length() - 1) : arg;
    }

    /**
     * Sets up the RuneScape client with all needed (and some extra) settings.
     */
    private void initApplet(String root, String params) throws Exception {
        String jsInfoPage = downloadHTML(root + params);
        jsInfoPage = jsInfoPage.substring(Math.max(jsInfoPage.indexOf("<applet"), jsInfoPage.indexOf("write('<app")), jsInfoPage.indexOf("</applet>"));
        System.out.println("Applet Loader Parsed");
        thisLoader = new URLClassLoader(new URL[]{new URL(root +  parseArg(search(jsInfoPage, archiveRegex, 1)))});
        clientApplet = (Applet) (thisLoader.loadClass(parseArg(search(jsInfoPage, codeRegex, 1)).split("\\.")[0]).newInstance());
        HashMap<String, String> paramMap = new HashMap<String, String>();
        paramMap.put("width", parseArg(search(jsInfoPage, widthRegex, 1)));
        paramMap.put("height", parseArg(search(jsInfoPage, heightRegex, 1)));
        Matcher matcher = Pattern.compile("<param name\\=([^ ]*) value\\=([^>]*)>").matcher(jsInfoPage);
        while (matcher.find()) {
            paramMap.put(matcher.group(1), parseArg(matcher.group(2)));
        }
        ClientStub stub = new ClientStub(root +  parseArg(search(jsInfoPage, archiveRegex, 1)), root, paramMap);
        clientApplet.setStub(stub);
        clientApplet.setPreferredSize(new Dimension(width, height));
        stub.active = true;
        System.out.println("Applet Parameters Forwarded");
    }

    /**
     * Searches for regex in what and returns the group specified.
     */
    private static String search(String what, String regex, int group) {
        Matcher matcher = Pattern.compile(regex).matcher(what);
        matcher.find();
        return matcher.group(group);
    }

    /**
     * Returns a string of the data at the specified address.
     */
    private static String downloadHTML(String address) {
        try {
            URL url = new URL(address);
            BufferedReader in = new BufferedReader(new InputStreamReader(url.openStream()));
            StringBuilder builder = new StringBuilder();
            String inputLine;
            while ((inputLine = in.readLine()) != null) {
                builder.append(inputLine).append("\n");
            }
            in.close();
            return builder.toString();
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    public void setGraphics(boolean on) {
        renderWhileBlocking = on;
        if (renderWhileBlocking) {
            debugbtn.setEnabled(true);
            gfxbtn.setText("Disable Graphics");
        } else {
            debugbtn.setEnabled(false);
            gfxbtn.setText("Enable Graphics");
        }
    }

    public void setDebug(boolean on) {
        debuggfx = on;
        if (debuggfx) {
            debugbtn.setText("Disable Debug");
        } else {
            debugbtn.setText("Enable Debug");
        }
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == blockingbtn) {
            blockingbtn.setEnabled(false);
            if (blocking) {
                stopBlocking();
            } else {
                startBlocking();
            }
        } else if (e.getSource() == gfxbtn) {
            setGraphics(!renderWhileBlocking);
        } else if (e.getSource() == debugbtn) {
            setDebug(!debuggfx);
        }
    }

    public void setRefresh(int x) {
        x = x < 1 ? 1 : x > 100 ? 100 : x;
        if (!refreshSlider.getValueIsAdjusting()) {
            refreshSlider.setValue(x);
        }
        if (canvas != null) {
            canvas.setRefresh(500 / x - 5);
        }
        refresh = 500 / x + 20;
    }

    public int getRefresh() {
        return refreshSlider.getValue();
    }

    @Override
    public void stateChanged(ChangeEvent e) {
        setRefresh(refreshSlider.getValue());
    }

    public Object getFieldObject(Object o, String path) {
        try {
            String[] parts = path.split("\\.");
            Stack<String> stack = new Stack<String>();
            Field field;
            if (o == null) {
                Class c = gameLoader.loadClass(parts[0]);
                for (int i = parts.length - 1; i > 0; i--) {
                    stack.push(parts[i]);
                }
                field = c.getDeclaredField(stack.pop());
                field.setAccessible(true);
                o = field.get(null);
            } else {
                for (int i = parts.length - 1; i >= 0; i--) {
                    stack.push(parts[i]);
                }
            }
            if (!stack.empty()) {
                while (!stack.empty()) {
                    String theField = stack.pop();
                    Class theClass = o.getClass();
                    field = null;
                    while (field == null && theClass != Object.class) {
                        try {
                            field = theClass.getDeclaredField(theField);
                        } catch (Exception e) {
                            try {
                                theClass = theClass.getSuperclass();
                            } catch (Exception x) {
                                break;
                            }
                        }
                    }
                    field.setAccessible(true);
                    o = field.get(o);
                }
            }
            return o;
        } catch (Exception e) {
            System.out.println("Field not found: " + path);
        }
        return null;
    }

    public boolean getFieldBoolean(Object o, String path) {
        o = getFieldObject(o, path);
        if (o != null && o instanceof Boolean) {
            return ((Boolean) o).booleanValue();
        }
        return false;
    }

    public int getFieldLongH(Object o, String path) {
        o = getFieldObject(o, path);
        if (o != null && o instanceof Long) {
            return (int) ((((Long) o).longValue() >> 32) & 0xFFFFFFFF);
        }
        return -1;
    }

    public int getFieldLongL(Object o, String path) {
        o = getFieldObject(o, path);
        if (o != null && o instanceof Long) {
            return (int) (((Long) o).longValue() & 0xFFFFFFFF);
        }
        return -1;
    }

    public int getFieldInt(Object o, String path) {
        o = getFieldObject(o, path);
        if (o != null && o instanceof Integer) {
            return ((Integer) o).intValue();
        }
        return -1;
    }

    public float getFieldFloat(Object o, String path) {
        o = getFieldObject(o, path);
        if (o != null && o instanceof Float) {
            return ((Float) o).floatValue();
        }
        return -1F;
    }

    public double getFieldDouble(Object o, String path) {
        o = getFieldObject(o, path);
        if (o != null && o instanceof Double) {
            return ((Double) o).doubleValue();
        }
        return -1D;
    }

    public int getFieldByte(Object o, String path) {
        o = getFieldObject(o, path);
        if (o != null && o instanceof Integer) {
            return ((Byte) o).intValue();
        }
        return -1;
    }

    public Object getFieldArray3DObject(Object o, String path, int x, int y, int z) {
        o = getFieldObject(o, path);
        try {
            return Array.get(Array.get(Array.get(o, x), y), z);
        } catch (Exception e) {
            return null;
        }
    }

    public int getFieldArray3DByte(Object o, String path, int x, int y, int z) {
        o = getFieldObject(o, path);
        try {
            return Array.getByte(Array.get(Array.get(o, x), y), z);
        } catch (Exception e) {
            return -1;
        }
    }

    public int getFieldArray3DChar(Object o, String path, int x, int y, int z) {
        o = getFieldObject(o, path);
        try {
            return Array.getChar(Array.get(Array.get(o, x), y), z);
        } catch (Exception e) {
            return -1;
        }
    }

    public int getFieldArray3DShort(Object o, String path, int x, int y, int z) {
        o = getFieldObject(o, path);
        try {
            return Array.getShort(Array.get(Array.get(o, x), y), z);
        } catch (Exception e) {
            return -1;
        }
    }

    public int getFieldArray3DInt(Object o, String path, int x, int y, int z) {
        o = getFieldObject(o, path);
        try {
            return Array.getInt(Array.get(Array.get(o, x), y), z);
        } catch (Exception e) {
            return -1;
        }
    }

    public float getFieldArray3DFloat(Object o, String path, int x, int y, int z) {
        o = getFieldObject(o, path);
        try {
            return Array.getFloat(Array.get(Array.get(o, x), y), z);
        } catch (Exception e) {
            return -1F;
        }
    }


    public double getFieldArray3DDouble(Object o, String path, int x, int y, int z) {
        o = getFieldObject(o, path);
        try {
            return Array.getDouble(Array.get(Array.get(o, x), y), z);
        } catch (Exception e) {
            return -1D;
        }
    }

    public boolean getFieldArray3DBoolean(Object o, String path, int x, int y, int z) {
        o = getFieldObject(o, path);
        try {
            return Array.getBoolean(Array.get(Array.get(o, x), y), z);
        } catch (Exception e) {
            return false;
        }
    }

    public int getFieldArray3DLongL(Object o, String path, int x, int y, int z) {
        o = getFieldObject(o, path);
        try {
            return (int) (Array.getLong(Array.get(Array.get(o, x), y), z) & 0xFFFFFFFF);
        } catch (Exception e) {
            return -1;
        }
    }

    public int getFieldArray3DLongH(Object o, String path, int x, int y, int z) {
        o = getFieldObject(o, path);
        try {
            return (int) ((Array.getLong(Array.get(Array.get(o, x), y), z) >> 32) & 0xFFFFFFFF);
        } catch (Exception e) {
            return -1;
        }
    }

    public Object getFieldArray2DObject(Object o, String path, int x, int y) {
        o = getFieldObject(o, path);
        try {
            return Array.get(Array.get(o, x), y);
        } catch (Exception e) {
            return null;
        }
    }

    public int getFieldArray2DByte(Object o, String path, int x, int y) {
        o = getFieldObject(o, path);
        try {
            return Array.getByte(Array.get(o, x), y);
        } catch (Exception e) {
            return -1;
        }
    }

    public int getFieldArray2DChar(Object o, String path, int x, int y) {
        o = getFieldObject(o, path);
        try {
            return Array.getChar(Array.get(o, x), y);
        } catch (Exception e) {
            return -1;
        }
    }

    public int getFieldArray2DShort(Object o, String path, int x, int y) {
        o = getFieldObject(o, path);
        try {
            return Array.getShort(Array.get(o, x), y);
        } catch (Exception e) {
            return -1;
        }
    }

    public int getFieldArray2DInt(Object o, String path, int x, int y) {
        o = getFieldObject(o, path);
        try {
            return Array.getInt(Array.get(o, x), y);
        } catch (Exception e) {
            return -1;
        }
    }

    public float getFieldArray2DFloat(Object o, String path, int x, int y) {
        o = getFieldObject(o, path);
        try {
            return Array.getFloat(Array.get(o, x), y);
        } catch (Exception e) {
            return -1F;
        }
    }

    public double getFieldArray2DDouble(Object o, String path, int x, int y) {
        o = getFieldObject(o, path);
        try {
            return Array.getDouble(Array.get(o, x), y);
        } catch (Exception e) {
            return -1D;
        }
    }

    public boolean getFieldArray2DBoolean(Object o, String path, int x, int y) {
        o = getFieldObject(o, path);
        try {
            return Array.getBoolean(Array.get(o, x), y);
        } catch (Exception e) {
            return false;
        }
    }

    public int getFieldArray2DLongL(Object o, String path, int x, int y) {
        o = getFieldObject(o, path);
        try {
            return (int) (Array.getLong(Array.get(o, x), y) & 0xFFFFFFFF);
        } catch (Exception e) {
            return -1;
        }
    }

    public int getFieldArray2DLongH(Object o, String path, int x, int y) {
        o = getFieldObject(o, path);
        try {
            return (int) ((Array.getLong(Array.get(o, x), y) >> 32) & 0xFFFFFFFF);
        } catch (Exception e) {
            return -1;
        }
    }

    public int getFieldArrayByte(Object o, String path, int index) {
        o = getFieldObject(o, path);
        try {
            return (int) Array.getByte(o, index);
        } catch (Exception e) {
            return -1;
        }
    }

    public int getFieldArrayShort(Object o, String path, int index) {
        o = getFieldObject(o, path);
        try {
            return (int) Array.getShort(o, index);
        } catch (Exception e) {
            return -1;
        }
    }

    public int getFieldArrayChar(Object o, String path, int index) {
        o = getFieldObject(o, path);
        try {
            return (int) Array.getChar(o, index);
        } catch (Exception e) {
            return -1;
        }
    }

    public int getFieldArrayInt(Object o, String path, int index) {
        o = getFieldObject(o, path);
        try {
            return Array.getInt(o, index);
        } catch (Exception e) {
            return -1;
        }
    }

    public float getFieldArrayFloat(Object o, String path, int index) {
        o = getFieldObject(o, path);
        try {
            return Array.getFloat(o, index);
        } catch (Exception e) {
            return -1F;
        }
    }

    public double getFieldArrayDouble(Object o, String path, int index) {
        o = getFieldObject(o, path);
        try {
            return Array.getDouble(o, index);
        } catch (Exception e) {
            return -1D;
        }
    }

    public int getFieldArrayLongH(Object o, String path, int index) {
        o = getFieldObject(o, path);
        try {
            return (int) ((Array.getLong(o, index) >> 32) & 0xFFFFFFFF);
        } catch (Exception e) {
            return -1;
        }
    }

    public int getFieldArrayLongL(Object o, String path, int index) {
        o = getFieldObject(o, path);
        try {
            return (int) (Array.getLong(o, index) & 0xFFFFFFFF);
        } catch (Exception e) {
            return -1;
        }
    }

    public boolean getFieldArrayBoolean(Object o, String path, int index) {
        o = getFieldObject(o, path);
        try {
            return Array.getBoolean(o, index);
        } catch (Exception e) {
            return false;
        }
    }

    public Object getFieldArrayObject(Object o, String path, int index) {
        o = getFieldObject(o, path);
        try {
            return Array.get(o, index);
        } catch (Exception e) {
            return null;
        }
    }

    public int getFieldArraySize(Object o, String path) {
        o = getFieldObject(o, path);
        try {
            return Array.getLength(o);
        } catch (Exception e) {
            return -1;
        }
    }
}
