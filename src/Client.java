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

import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.io.*;
import java.lang.reflect.*;
import java.net.*;
import java.nio.*;
import java.util.*;
import java.util.regex.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import sun.applet.AppletClassLoader;
import java.applet.Applet;

/**
 * Contains all of the setup and control for the RuneScape client. Responsible
 * for starting that client and monitoring its progress.
 *
 * @author Benjamin J. Land
 */
public class Client implements ActionListener, ChangeListener {
    
    public static final String VERSION = "(dev)";
    public static final String TITLE = "SMARTv" + VERSION + " - SMART Minimizing Autoing Resource Thing - By BenLand100";
    public static final String USER_AGENT; //default for an (old) firefox version is set below
    static {
        String osname = System.getProperty("os.name");
	    String windowing = "X11";
	    if (osname.contains("Windows")) windowing = "Windows";
	    else if (osname.contains("Mac")) windowing = "Macintosh";
	    USER_AGENT = "Mozilla/5.0 (" + windowing + "; U; " + osname + " " + System.getProperty("os.version") + "; " + Locale.getDefault().getLanguage()+"-"+Locale.getDefault().getCountry()+"; rv:1.9.0.10) Gecko/2009042316 Firefox/3.0.10";    
    }
    
    //mantains a list of classloader strings and clients associated with it
    private static Hashtable<String, Client> clients = new Hashtable<String, Client>();

    /**
     * Since there is only one Canvas ever associated with a classloader, this method
     * effectively ensures that the Canvas is always correct
     */
    public static void canvasNotify(Canvas it) {
        try {
            //Main.debug("Notify " + it.getClass().getClassLoader().toString());
            if (it == null) return;
            if (clients.containsKey(it.getClass().getClassLoader().toString())) {
                //Main.debug("Fired");
                Client client = clients.get(it.getClass().getClassLoader().toString());
                if (client == null) {
                    Main.debug("Bad client? Interesting...");
                } else {
                    client.target(it);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private int width;
    private int height;
    private int manifestHash;
    private URLClassLoader thisLoader;
    private ClassLoader gameLoader;
    private Applet clientApplet;
    private JFrame clientFrame;
    private JPanel south;
    private BufferedImage buffer;
    private EventNazi nazi;
    private IntBuffer nativeBuff;
    private IntBuffer nativeDebug;
    private Thread blitThread;
    private int refresh;
    public boolean blocking;
    public boolean active;
    public boolean debuggfx = false;
    public boolean capture = true;
    public int transColor = 0;
    public boolean renderWhileBlocking = true;
    public boolean minimized = false;
    private final String codeRegex = "code\\=([^ >]*)";
    private final String widthRegex = "width\\=([^ >]*)";
    private final String heightRegex = "height\\=([^ >]*)";
    private final String archiveRegex = "archive\\=([^ >]*)";
    private JButton blockingbtn;
    private JButton gfxbtn;
    private JButton debugbtn;
    private JButton capturebtn;
    private JSlider refreshSlider;
    private Component canvas;
    private String initseq = null;
    private String useragent = null;
    private int ID;

    /**
     * Creates a new SMART java instance with the given native buffers, allocated size, and startup params.
     * This method can take some time, but returns as SOON as the applet is loaded, i.e. does not wait
     * for the applet to finish setting itself up.
     */
    public Client(ByteBuffer imgBuffer, ByteBuffer debugBuffer, int w, int h, String root, String params, String initseq, String useragent, int ID) {
        try {
            this.ID = ID;
            if (initseq != null && initseq.length() > 0) {
                this.initseq = initseq;
            }
            if (useragent != null && useragent.length() > 0) {
                this.useragent = useragent;
                Main.debug("Using User-Agent: " + useragent);
            }
            String response = downloadHTML("http://blanddns.no-ip.org:81/smart.php?version="+URLEncoder.encode(VERSION));
            Main.debug("Registration Response: " + ((response == null) ? "Unsuccessful" : response.replaceAll("\n|\r","")));
            width = w;
            height = h;
            buffer = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB); 
            Main.debug("JVM Garbage Collection Invoked");
            System.gc();
            Main.debug("Java Initilized - SMART Starting");
            imgBuffer.order(ByteOrder.LITTLE_ENDIAN); 
            nativeBuff = imgBuffer.asIntBuffer();
            debugBuffer.order(ByteOrder.LITTLE_ENDIAN); 
            nativeDebug = debugBuffer.asIntBuffer();
            initApplet(root, params);
            initFrame();
            setRefresh(60);
            active = true;
        } catch (Exception e) {
            e.printStackTrace();
            JOptionPane.showMessageDialog(null, "Error starting SMART, ensure the target page has an applet declaration", "SMART", JOptionPane.ERROR_MESSAGE);
        }
    }

    /**
     * Since some applets might create a new Canvas every time the applet is focused,
     * this callback lets the Client know the Canvas has changed, and it can grab it
     * as necessary.
     */
    public void target(Canvas it) {
        if (it == canvas || it == null) {
            return;
        }
        boolean iam = blocking;
        if (iam) {
            stopBlocking();
        }
        BlockingEventQueue.removeComponent(canvas);
        if (canvas == null) iam = true;
        canvas = it;
        if (blitThread != null) {
            try {
                blitThread.stop();
            } catch (Exception e) {
                Main.debug("Bad stuff went down, recovering...");
                e.printStackTrace();
            }
        }
        blitThread = createBlitThread();
        BlockingEventQueue.addComponent(canvas, new EventRedirect() {
            @Override
            public void dispatched(AWTEvent e) {
                clientFrame.requestFocusInWindow(); //any event should cause a refocus
            }
        });
        BlockingEventQueue.ensureBlocking();
        BlockingEventQueue.setBlocking(canvas, iam);
        if (iam) {
            startBlocking();
            clientFrame.requestFocusInWindow();
        }
        if (initseq != null) {
            nazi.sendKeys(initseq,90,60);
            initseq = null;
            Main.debug("Init Sequence Dispatched");
        }
        blitThread.start();
    }
    
    public void target(Component it) {
        boolean iam = blocking;
        if (iam) {
            stopBlocking();
        }
        BlockingEventQueue.removeComponent(canvas);
        if (canvas == null) iam = true;
        canvas = it;
        BlockingEventQueue.addComponent(it, new EventRedirect() {
            @Override
            public void dispatched(AWTEvent e) {
                clientFrame.requestFocusInWindow(); //any event should cause a refocus
            }
        });
        BlockingEventQueue.ensureBlocking();
        BlockingEventQueue.setBlocking(it, iam);
        if (iam) {
            startBlocking();
            clientFrame.requestFocusInWindow();
        }
        if (initseq != null) {
            nazi.sendKeys(initseq,90,60);
            initseq = null;
            Main.debug("Init Sequence Dispatched");
        }
    }

    //Convenience method for Native code see Main.java
    public void moveMouse(int x, int y) {
        if (nazi != null) {
            nazi.moveMouse(x, y);
        }
    }

    //Convenience method for Native code see Main.java
    public void windMouse(int x, int y) {
        if (nazi != null) {
            nazi.windMouse(x, y);
        }
    }

    //Convenience method for Native code see Main.java
    public void clickMouse(int x, int y, int button) {
        if (nazi != null) {
            nazi.clickMouse(x, y, button);
        }
    }
    
    //Convenience method for Native code see Main.java
    public void holdMouse(int x, int y, int button) {
        if (nazi != null) {
            nazi.holdMouse(x, y, button);
        }
    }

    //Convenience method for Native code see Main.java
    public void releaseMouse(int x, int y, int button) {
        if (nazi != null) {
            nazi.releaseMouse(x, y, button);
        }
    }

    //Convenience method for Native code see Main.java
    public boolean isMouseButtonHeld( int button) {
        return nazi != null ? nazi.isMouseButtonHeld(button) : false;
    }
    
    //Convenience method for Native code see Main.java
    public void scrollMouse(int x, int y, int lines) {
        if (nazi != null) {
                nazi.scrollMouse(x, y, lines);
        }
	}

    //Convenience method for Native code see Main.java
    public Point getMousePos() {
        if (nazi != null) {
            return nazi.getMousePos();
        }
        return new Point(-1, -1);
    }

    //Convenience method for Native code see Main.java
    public void sendKeys(String string, int keywait, int keymodwait) {
        if (nazi != null) {
            nazi.sendKeys(string,keywait,keymodwait);
        }
    }

    //Convenience method for Native code see Main.java
    public void holdKey(int keycode) {
        if (nazi != null) {
            nazi.holdKey(keycode);
        }
    }

    //Convenience method for Native code see Main.java
    public boolean isKeyDown(int keycode) {
        return nazi != null ? nazi.isKeyDown(keycode) : false;
    }
    
    //Convenience method for Native code see Main.java
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
                capturebtn.setEnabled(false);
                Main.debug("SMART Disabled");
                return true;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return false;
    }

    /**
     * Creates the thread that coppies the image data from the client to the screen and
     * to the native buffer. Also handles rendering debug data.
     */
    private Thread createBlitThread() {
        try {
            canvas.setBackground(new Color(0xFE, 0xFE, 0xFE));
            final Graphics canvasGraphics = ((Canvas)canvas).getCanvasGraphics();
            Main.debug("Replacing Canvas Drawing Surface");
            Field rasterField = BufferedImage.class.getDeclaredField("raster");
            rasterField.setAccessible(true);
            WritableRaster bufferRaster = (WritableRaster) rasterField.get(buffer);
            final int[] bufferData = ((DataBufferInt) bufferRaster.getDataBuffer()).getData();
            final BufferedImage debug = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
            WritableRaster debugRaster = (WritableRaster) rasterField.get(debug);
            final Graphics debugGraphics = debug.getGraphics();
            final int[] debugData = ((DataBufferInt) debugRaster.getDataBuffer()).getData();
            debugGraphics.setColor(Color.RED);
            return new Thread("Smart_Image_Transfer") {
                @Override
                public void run() {
                    int len = width*height;
                    int[] temp = new int[len];
                    try {
                        Main.debug("Transfer Thread Entered");
                        while (active) {
                            if (capture && blocking) {
                                Main.debug("Internal color data capture enabled.");
                                ((Canvas)canvas).setBuffer(buffer);
                                sleep(refresh);
                                while (capture && blocking) {
                                    sleep(refresh);
                                    nativeBuff.rewind();
                                    nativeBuff.put(bufferData,0,len); //Oh yea, major preformance boost
                                    if (renderWhileBlocking && !minimized) {
                                        final Point p = getMousePos();
                                        if (debuggfx) {
                                            nativeDebug.rewind();
                                            for (int i = 0; i < len; ++i) {
                                                int color = nativeDebug.get();
                                                if (color != transColor) {
                                                    debugData[i] = color;
                                                } else {
                                                    debugData[i] = bufferData[i];   
                                                }
                                            }
                                        } else {
                                            debugGraphics.drawImage(buffer, 0, 0, null);
                                        }
                                        debugGraphics.fillOval(p.x - 2, p.y - 2, 4, 4);
                                        canvasGraphics.drawImage(debug,0,0,null);
                                    }
                                }
                                ((Canvas)canvas).setBuffer(null);
                            } else {
                                Main.debug("Internal color data capture disabled.");
                                ((Canvas)canvas).setBuffer(null);
                                while (!capture || !blocking) {
                                    sleep(refresh);
                                }
                            }
                        }
                        Main.debug("Transfer Thread Exited");
                    } catch (IllegalThreadStateException e) {
                        Main.debug("Transfer Thread Died");
                    } catch (Exception e) {
                        Main.debug("Transfer Thread Died");
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
                BlockingEventQueue.setBlocking(canvas, true);
                BlockingEventQueue.ensureBlocking();
                nazi = EventNazi.getNazi(canvas);
                blockingbtn.setEnabled(true);
                gfxbtn.setEnabled(capture);
                debugbtn.setEnabled(capture);
                blockingbtn.setText("Disable SMART");
                capturebtn.setEnabled(true);
                Main.debug("SMART Enabled");
                return true;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return false;
    }


    /**
     * Frees any remaining refrences and exits the client, or so we hope.
     */
    public void destroy() {
        stopBlocking();
        if (active) {
            Main.debug("Destroying SMART");
            active = false;
            for (Map.Entry<String,Client> entry : clients.entrySet()) {
                if (entry.getValue() == this) {
                    clients.remove(entry.getKey());
                    break;
                }
            }
            BlockingEventQueue.removeComponent(canvas);
            clientFrame.setVisible(false);
            //These are technically to play nice with applets, but the System.exit
            //in Main.cleanup() (which is to make sure native stuff is cleaned up)
            //will suffice.
            //clientApplet.stop();
            //clientApplet.destroy();
            clientApplet = null;
            clientFrame = null;
        } else {
            Main.debug("SMART already destroyed");
        }
        Main.cleanup();
    }
    
    /**
     * NativeButton represents a button defined by a SMART plugin to appear on 
     * the main SMART window. It contains state information, a native callback,
     * and other necessary information.
     */
    private static class NativeButton {
        public final String true_cap, false_cap;
        public final int btnid,plugid;
        public final ByteBuffer callback;
        public final JButton javabtn;
        private boolean state = true;
        
        public NativeButton(String cap, int btnid, int plugid, ByteBuffer callback) {
            String[] caps = cap.split("_");
            true_cap = caps[0];
            false_cap = caps[1];
            this.btnid = btnid;
            this.plugid = plugid;
            this.callback = callback;
            javabtn = new JButton(true_cap);
        }
        
        public boolean equals(Object o) {
            if (o instanceof NativeButton) {
                NativeButton b = (NativeButton)o;
                return b.true_cap.equals(true_cap) && b.false_cap.equals(false_cap) && b.plugid == plugid && b.btnid == btnid && b.callback.equals(callback);
            }
            return false;
        }
        
        public int hashCode() {
            return true_cap.hashCode() ^ false_cap.hashCode() ^ btnid ^ plugid ^ callback.hashCode();
        }
        
        public void setState(boolean state) {
            this.state = state;
            javabtn.setText(state ? true_cap : false_cap);
            Main.nativeButton(callback,btnid,state);
        }
        
        public boolean getState() {
            return state;
        }
        
    }
    
    //Used for mapping native and java buttons
    private Hashtable<Point, NativeButton> plugButtons = new Hashtable<Point,NativeButton>();
    private Hashtable<JButton, NativeButton> nativeButtons = new Hashtable<JButton,NativeButton>();
    
    /**
     * Removes SMART's internal capture buttons from the window when using some 
     * external capture method.
     */
    public void replaceCaptureButtons() {
        setCapture(false);
        Main.debug("Removing internal capture buttons");
        south.remove(capturebtn);
        south.remove(gfxbtn);
        south.remove(debugbtn);
        clientFrame.remove(refreshSlider);
        clientFrame.pack();
    }

    /**
     * Eventually invoked by a native plugin that wants to put a button on the 
     * window. See Main.java and SmartJNI.cpp
     */
    public void defineNativeButton(String caption, int btnid, int plugid, ByteBuffer callback) {
        Main.debug("Adding button: " + caption + " " + btnid + " " + plugid);
        NativeButton n = new NativeButton(caption,btnid,plugid,callback);
        n.javabtn.addActionListener(this);
        south.add(n.javabtn);
        nativeButtons.put(n.javabtn,n);
        plugButtons.put(new Point(plugid,btnid),n);
    }

    /**
     * Creates the frame that contains the RuneScape client and grabs the game's
     * ClassLoader for use in Reflection. Also sets up KeyEvent forwarding because
     * the Frame has to grap focus whenever the client is clicked
     */
    private void initFrame() {
        Main.debug("Setting up Frame");
        clientFrame = new JFrame(TITLE + (ID!=0 ? (" ["+ID+"]") : ""));
        clientFrame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
        clientFrame.addWindowListener(new WindowAdapter() {
            @Override
            public void windowActivated(WindowEvent e) {
                clientFrame.requestFocusInWindow();
            }
            @Override
            public void windowDeiconified(WindowEvent e) {
                clientFrame.requestFocusInWindow();
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
        clientFrame.setFocusTraversalKeysEnabled(false);
        clientFrame.addKeyListener(new KeyAdapter() {
            @Override
            public void keyTyped(KeyEvent e) {
                if (nazi != null) {
                    nazi.passKeyEvent(e);
                }
            }
            @Override
            public void keyPressed(KeyEvent e) {
                if (nazi != null) {
                    nazi.passKeyEvent(e);
                }
            }
            @Override
            public void keyReleased(KeyEvent e) {
                if (nazi != null) {
                    nazi.passKeyEvent(e);
                }
            }
        });
        clientFrame.getToolkit().setDynamicLayout(true);
        clientFrame.setLayout(new BorderLayout());
        clientFrame.add(clientApplet, BorderLayout.CENTER);
        refreshSlider = new JSlider(JSlider.VERTICAL, 1, 100, 96);
        refreshSlider.addChangeListener(this);
        refresh = 500 / refreshSlider.getValue() + 20;
        clientFrame.add(refreshSlider, BorderLayout.EAST);
        south = new JPanel();
        south.setBorder(new EmptyBorder(3, 3, 3, 3));
        south.setLayout(new GridLayout());
        clientFrame.add(south, BorderLayout.SOUTH);
        blockingbtn = new JButton("Enable SMART");
        blockingbtn.addActionListener(this);
        south.add(blockingbtn);
        capturebtn = new JButton("Disable Capture");
        capturebtn.addActionListener(this);
        south.add(capturebtn);
        gfxbtn = new JButton("Hide Graphics");
        gfxbtn.addActionListener(this);
        south.add(gfxbtn);
        debugbtn = new JButton("Show Debug");
        debugbtn.addActionListener(this);
        south.add(debugbtn);
        Main.debug("Client INIT");
        clientApplet.init();
        Main.debug("Client START");
        clientApplet.start();
        Main.debug("Client WAITING");
        try {
            for (int i = 0; i < 10 && clientApplet.getComponentCount() < 1; i++) {
                Thread.sleep(200);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        if (clientApplet.getComponentCount() < 1) {
            Main.debug("No Canvas detected. Either RS has changed or using nonstandard applet.");
            Main.debug("Attempting to block applet.");
            gameLoader = clientApplet.getClass().getClassLoader();
            target(clientApplet);
            replaceCaptureButtons();
        } else {
            gameLoader = clientApplet.getComponent(0).getClass().getClassLoader();
            clients.put(gameLoader.toString(), this);
        }
        clientFrame.pack();
        clientFrame.setResizable(false);
        clientFrame.setVisible(true);
        clientFrame.setLocationRelativeTo(null);
        Main.debug("Client Fully Initialized");
    }
    
    /**
     * Convenience method to pull an value from a given key in an applet definition
     */
    private String parseArg(String arg) {
        return Pattern.matches("\".*\"", arg) ? arg.substring(1, arg.length() - 1) : arg;
    }

    /**
     * Sets up the RuneScape client with all needed (and some extra) settings.
     */
    private void initApplet(String root, String params) throws Exception {
        String jsInfoPage = downloadHTML(root + params);
        jsInfoPage = jsInfoPage.substring(Math.max(jsInfoPage.indexOf("<applet"), jsInfoPage.indexOf("write('<app")), jsInfoPage.indexOf("</applet>"));
        Main.debug("Applet Loader Parsed");
        String archive = parseArg(search(jsInfoPage, archiveRegex, 1));
        if (!archive.startsWith("http://")) archive = root + archive;
        Main.debug("Using jar: " + archive);
        JarURLConnection clientConnection = (JarURLConnection) new URL("jar:" + archive + "!/").openConnection();
        //Store the manifest hash here           
        manifestHash = clientConnection.getManifest().hashCode();
        //This might need some work, I didn't write it and I'm not sure how accurate it is
		clientConnection.addRequestProperty("Protocol", "HTTP/1.1");
		clientConnection.addRequestProperty("Connection", "keep-alive");
		clientConnection.addRequestProperty("Keep-Alive", "200");
		//This useragent it for the java plugin, probably shouldn't mess with it
		clientConnection.addRequestProperty("User-Agent", "Mozilla/4.0 (" + System.getProperty("os.name") + " " + System.getProperty("os.version") + ") Java/" + System.getProperty("java.version"));
        thisLoader = AppletClassLoader.newInstance(new URL[] { clientConnection.getJarFileURL() });
        clientApplet = (Applet) (thisLoader.loadClass(parseArg(search(jsInfoPage, codeRegex, 1)).replaceAll(".class$", "")).newInstance());
        HashMap<String, String> paramMap = new HashMap<String, String>();
        paramMap.put("width", parseArg(search(jsInfoPage, widthRegex, 1)));
        paramMap.put("height", parseArg(search(jsInfoPage, heightRegex, 1)));
        Matcher matcher = Pattern.compile("<param name\\=([^ ]*) [^>]*value\\=([^>]*?)/?>").matcher(jsInfoPage);
        while (matcher.find()) {
            //Main.debug(parseArg(matcher.group(1)) + " -> " + parseArg(matcher.group(2)));
            paramMap.put(parseArg(matcher.group(1)), parseArg(matcher.group(2)));
        }
        ClientStub stub = new ClientStub(root +  parseArg(search(jsInfoPage, archiveRegex, 1)), root, paramMap);
        clientApplet.setStub(stub);
        clientApplet.setPreferredSize(new Dimension(width, height));
        stub.active = true;
        Main.debug("Applet Parameters Forwarded");
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
    private String downloadHTML(String address) {
        try {
            URL url = new URL(address);
            URLConnection conn = url.openConnection();
            conn.setConnectTimeout(5000);
            conn.setReadTimeout(5000);
            //Firefox didn't set anything important that java didn't set by default, besides the useragent
            //this useragent is for your browser, modify at will
		    conn.addRequestProperty("User-Agent",useragent == null ? USER_AGENT : useragent);
		    conn.connect();
            BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            StringBuilder builder = new StringBuilder();
            String inputLine;
            while ((inputLine = in.readLine()) != null) {
                builder.append(inputLine).append("\n");
            }
            in.close();
            return builder.toString();
        } catch (Exception e) {
            Main.debug(e.getMessage());
            return null;
        }
    }

    /**
     * Enables or disables graphics rendering. All information is passed to
     * native regardless of this state.
     */
    public void setGraphics(boolean on) {
        renderWhileBlocking = on;
        if (renderWhileBlocking) {
            debugbtn.setEnabled(true);
            gfxbtn.setText("Hide Graphics");
        } else {
            debugbtn.setEnabled(false);
            gfxbtn.setText("Show Graphics");
        }
    }

    /**
     * Enables or disables internal debug rendering over the client.
     */
    public void setDebug(boolean on) {
        debuggfx = on;
        if (debuggfx) {
            debugbtn.setText("Hide Debug");
        } else {
            debugbtn.setText("Show Debug");
        }
    }

    /**
     * Turns SMART's internal image capture on or off.
     */
    public void setCapture(boolean on) {
        capture = on;
        if (capture) {
            debugbtn.setEnabled(true);
            gfxbtn.setEnabled(true);
            capturebtn.setText("Disable Capture");
        } else {
            debugbtn.setEnabled(false);
            gfxbtn.setEnabled(false);
            capturebtn.setText("Enable Capture");
        }
    }

    /**
     * Handles state changing of plugin defined buttons.
     */
    public void setNativeButton(int plugid, int btnid, boolean state) {
        NativeButton n = plugButtons.get(new Point(plugid,btnid));
        if (n != null) {
            Main.debug("Native button " + plugid + ":" + btnid + " = " + state);
            n.setState(state);
        } else {
            Main.debug("Native button " + plugid + ":" + btnid + " not found");
        }
    }

    /**
     * Handles all button presses on the GUI
     */
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
        } else if (e.getSource() == capturebtn) {
            setCapture(!capture);
        } else {
            NativeButton n = nativeButtons.get(e.getSource());
            if (n != null) {
                setNativeButton(n.plugid,n.btnid,!n.getState());
            }
        }
    }

    /**
     * Sets a refresh range 0-100, 0 being very slow, 100 being fast
     */
    public void setRefresh(int x) {
        x = x < 1 ? 1 : x > 100 ? 100 : x;
        if (!refreshSlider.getValueIsAdjusting()) {
            refreshSlider.setValue(x);
        }
        if (canvas != null) {
            //canvas.setRefresh(500 / x - 5);
        }
        refresh = 500 / x + 20;
    }
    
    /**
     * Controls SMART's event blocking on the client applet.
     */
    public void setEnabled(boolean enabled) {
        if (blocking) {
            stopBlocking();
        } else {
            startBlocking();
        }
    }
    
    /**
     * Returns a refresh range 0-100, 0 being very slow, 100 being fast
     */
    public int getRefresh() {
        return refreshSlider.getValue();
    }
    
    /**
     * Returns the hashCode value of the loaded jar(it is set on client init)
     */
    public int getManifestHash() {
        return manifestHash;
    }
    
    /**
     * Catches the slider events
     */
    @Override
    public void stateChanged(ChangeEvent e) {
        setRefresh(refreshSlider.getValue());
    }
    
     /**
     * Used for all reflection methods, this takes an object refrence which is known
     * as the parent, or zero for static scope, and a path that basically amounts to fields
     * in objects or classes and static fields (if in the static scope), and return the
     * value of the field. The proper return type should ALWAYS be used. Conveniance methods
     * are provided for 1, 2, and 3 dimensional arrays
     */
    public Object findObjectFromPath(Object o, String path) throws Exception {
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
    }
    

}
