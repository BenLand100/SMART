package smart;

import JNI.GLDXLoader;
import JNI.PluginLoader;
import java.awt.Point;
import java.io.*;
import java.net.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import smart.Client.OperatingMode;

public class Main {

    private static final int FirstFunc = 1;
    private static final int setTransparentColor = FirstFunc + 0;
    private static final int setDebug = FirstFunc + 1;
    private static final int setGraphics = FirstFunc + 2;
    private static final int setKeyInput = FirstFunc + 3;
    private static final int setMouseInput = FirstFunc + 4;
    private static final int isActive = FirstFunc + 5;
    private static final int isKeyboardEnabled = FirstFunc + 6;
    private static final int isMouseEnabled = FirstFunc + 7;
    private static final int getMousePos = FirstFunc + 8;
    private static final int holdMouse = FirstFunc + 9;
    private static final int releaseMouse = FirstFunc + 10;
    private static final int holdMousePlus = FirstFunc + 11;
    private static final int releaseMousePlus = FirstFunc + 12;
    private static final int moveMouse = FirstFunc + 13;
    private static final int windMouse = FirstFunc + 14;
    private static final int clickMouse = FirstFunc + 15;
    private static final int clickMousePlus = FirstFunc + 16;
    private static final int isMouseButtonHeld = FirstFunc + 17;
    private static final int sendKeys = FirstFunc + 18;
    private static final int holdKey = FirstFunc + 19;
    private static final int releaseKey = FirstFunc + 20;
    private static final int isKeyDown = FirstFunc + 21;
    private static final int setMode = FirstFunc + 22;
    private static final int ExtraFuncs = FirstFunc + 23;
    private static final int Ping = ExtraFuncs + 0;
    private static final int Die = ExtraFuncs + 1;
    private static final int nVars = 7;
    private static final int sizeof_int = 4;
    private static final int sizeof_SHM_Args = 4096;
    private static Client client = null;
    private static File f = null;
    private static ClientFrame frame = null;
    private static ServerSocket listen = null;
    private static RandomAccessFile fra = null;
    private static ByteBuffer mem = null, args = null;
    private static final OperatingMode OperatingModes[] = OperatingMode.values();

    public static native int getPID();
    public static native boolean checkAlive(int tid);
    public final static Utilities utilities = new Utilities();
    private static int ID = 0;

    public static void debug(Object o) {
        System.out.println("SMART[" + ID + "]:  " + o);
    }

    public static Client getClient() {
        return frame.getClient();
    }

    public static void updateFrame() {
        frame.updateUI();
    }

    public static void destruct() {
        frame.destruct();
    }

    public static void main(String[] Arguments) {
        if (Arguments.length == 1) {
            String Root = null;
            String Params = null;
            if (Arguments[0].equalsIgnoreCase("test")) {
                String PageSource = utilities.downloadPage("http://www.runescape.com/game.ws?j=1", ClientFrame.USER_AGENT);
                String SearchParams = "<iframe id=\"game\" src=\"";
                int StartIndex = PageSource.indexOf(SearchParams) + SearchParams.length();
                int EndIndex = PageSource.indexOf("\"", StartIndex);
                PageSource = PageSource.substring(StartIndex, EndIndex);
                Root = PageSource.split(",")[0];
                Params = PageSource.split(",")[1];
                String plugin = "libsmartjni" + (System.getProperty("os.arch").contains("x86") ? "32" : "64") + ".dll";
                Arguments = new String[]{
                    plugin, Root, "," + Params, "765", "553", "s", "", "", ""
                };
            } else if (Arguments[0].equalsIgnoreCase("testold")) {
                Root = "http://oldschool44.runescape.com/";
                String plugin = "libsmartjni" + (System.getProperty("os.arch").contains("x86") ? "32" : "64") + ".dll";
                Arguments = new String[]{
                    plugin, Root, "", "765", "503", "s", "", "", ""
                };
            } 
        }

        if (Arguments.length != 9) {
            return;
        }

        String path = new File(Arguments[0]).getAbsolutePath();
        System.load(path);

        String root = Arguments[1];
        String params = Arguments[2];
        int width = Integer.parseInt(Arguments[3]);
        int height = Integer.parseInt(Arguments[4]);
        String initSequence = Arguments[5];
        String userAgent = Arguments[6];
        String pluginspath = Arguments[7];
        String pluginsinfo = Arguments[8];

        frame = new ClientFrame(ID = getPID());

        if (pluginsinfo.length() > 0) {
            PluginLoader.loadAll(pluginspath, pluginsinfo.split("[,]+"));
        }

        int sizeof_Image = width * height * 4;
        int sizeof_SHMData = (nVars * sizeof_int) + sizeof_SHM_Args + (sizeof_Image * 2);  //Total size of SHMData.
        int ImageOffset = nVars * 4 + sizeof_SHM_Args;
        int DebugOffset = ImageOffset + sizeof_Image;

        try {
            f = new File("SMART." + ID);
            if (f.exists()) {
                f.delete();
            }

            f.createNewFile();
            f.deleteOnExit();
            fra = new RandomAccessFile(f, "rwd");
            fra.setLength(sizeof_SHMData);
            fra.getChannel();

            FileChannel chan = fra.getChannel();
            mem = chan.map(FileChannel.MapMode.READ_WRITE, 0, sizeof_SHMData);

            //Create The Argument Buffer.
            mem.order(ByteOrder.LITTLE_ENDIAN);
            mem.position(nVars * sizeof_int);
            args = mem.slice();
            args.order(ByteOrder.LITTLE_ENDIAN);

            //Create The Image Buffer.
            setImgOff(ImageOffset);
            mem.position(ImageOffset);
            ByteBuffer gameBuffer = mem.slice();

            //Create The Debug Buffer.
            setDbgOff(DebugOffset);
            mem.position(DebugOffset);
            ByteBuffer debugBuffer = mem.slice();
            mem.rewind();

            listen = new ServerSocket(0);

            setID(ID);
            setPaired(0);
            setDims(width, height);
            setPort(listen.getLocalPort());
            client = frame.addClient(root, params, initSequence, userAgent, gameBuffer, debugBuffer, width, height);
        
            try {
                while (client.isActive()) {
                    Socket ctrl = listen.accept();
                    debug("Socket Connected");
                    ctrl.setSoTimeout(500);
                    InputStream in = ctrl.getInputStream();
                    OutputStream out = ctrl.getOutputStream();
                    debug("Remote TID: " + getPaired());
                    client.setDrawMouse(true);
                    while (getPaired() != 0 && checkAlive(getPaired())) {
                        try {
                            out.write(handle(in.read()));
                        } catch (SocketTimeoutException | SocketException e) {
                        } catch (Exception e) {
                            Utilities.StackTrace(e);
                            break;
                        }
                    }
                    debug("Socket disconnected; Unpairing");
                    client.setDrawMouse(false);
                    if (GLDXLoader.OpenGLLoaded || GLDXLoader.DirectXLoaded) {
                        GLDXLoader.DrawMouse(-1, -1, 0, 0, 0);
                    }
                    setPaired(0);
                    in.close();
                    out.close();
                    ctrl.close();
                }
            } catch (Exception Ex) {
                Utilities.StackTrace(Ex);
            }
        } catch (Exception Ex) {
            Utilities.StackTrace(Ex);
        }
    }

    private static void setPort(int port) {
        mem.putInt(0 * sizeof_int, port);
    }

    private static void setID(int id) {
        mem.putInt(1 * sizeof_int, id);
    }

    private static int getPaired() {
        return mem.getInt(4 * sizeof_int);
    }

    private static void setPaired(int tid) {
        mem.putInt(4 * sizeof_int, tid);
    }

    private static void setImgOff(int off) {
        mem.putInt(5 * sizeof_int, off);
    }

    private static void setDbgOff(int off) {
        mem.putInt(6 * sizeof_int, off);
    }

    private static void setDims(int width, int height) {
        mem.putInt(2 * sizeof_int, width);
        mem.putInt(3 * sizeof_int, height);
    }

    private static int handle(int funid) {
        switch (funid) {
            case setTransparentColor:
                client.transColor = args.getInt(0 * 4);
                break;
            case setDebug:
                client.setDebug(args.getInt(0 * 4) != 0);
                break;
            case setGraphics:
                client.setGraphics(args.getInt(0 * 4) != 0);
                break;
            case setKeyInput:
                client.setKeyInput(args.getInt(0 * 4) != 0);
                break;
            case setMouseInput:
                client.setMouseInput(args.getInt(0 * 4) != 0);
            case isActive:
                args.putInt(0 * 4, client.isActive() ? 1 : 0);
                break;
            case isKeyboardEnabled:
                args.putInt(0 * 4, client.isKeysEnabled() ? 1 : 0);
                break;
            case isMouseEnabled:
                args.putInt(0 * 4, client.isMouseEnabled() ? 1 : 0);
                break;
            case getMousePos: {
                Point pt = client.getMousePos();
                args.putInt(0 * 4, pt.x);
                args.putInt(1 * 4, pt.y);
            }
            break;
            case holdMouse:
                client.holdMouse(args.getInt(0 * 4), args.getInt(1 * 4), args.getInt(2 * 4) != 0 ? 1 : 3);
                break;
            case releaseMouse:
                client.releaseMouse(args.getInt(0 * 4), args.getInt(1 * 4), args.getInt(2 * 4) != 0 ? 1 : 3);
                break;
            case holdMousePlus:
                client.holdMouse(args.getInt(0 * 4), args.getInt(1 * 4), args.getInt(2 * 4));
                break;
            case releaseMousePlus:
                client.releaseMouse(args.getInt(0 * 4), args.getInt(1 * 4), args.getInt(2 * 4));
                break;
            case moveMouse:
                client.moveMouse(args.getInt(0 * 4), args.getInt(1 * 4));
                break;
            case windMouse:
                client.windMouse(args.getInt(0 * 4), args.getInt(1 * 4));
                break;
            case clickMouse:
                client.clickMouse(args.getInt(0 * 4), args.getInt(1 * 4), args.getInt(2 * 4) != 0 ? 1 : 3);
                break;
            case clickMousePlus:
                client.clickMouse(args.getInt(0 * 4), args.getInt(1 * 4), args.getInt(2 * 4));
                break;
            case isMouseButtonHeld:
                args.putInt(0 * 4, client.isMouseButtonHeld(args.getInt(0 * 4)) ? 1 : 0);
                break;
            case sendKeys: {
                StringBuilder keys = new StringBuilder();
                byte c;
                args.position(8);
                while ((c = args.get()) != 0) {
                    keys.append((char) c);
                }
                client.sendKeys(keys.toString(), args.getInt(0 * 4), args.getInt(1 * 4));
            }
            break;
            case holdKey:
                client.holdKey(args.getInt(0 * 4));
                break;
            case releaseKey:
                client.releaseKey(args.getInt(0 * 4));
                break;
            case isKeyDown:
                args.putInt(0 * 4, client.isKeyDown(args.getInt(0 * 4)) ? 1 : 0);
                break;
            case setMode:
                client.setMode(OperatingModes[args.getInt(0 * 4)]);
                frame.updateUI();
                break;
            case Ping:
                break;
            case Die:
                frame.destruct();
                break;
            default:
                return -1;
        }
        return funid;
    }
}
