package smart;

import JNI.GLDXLoader;
import java.awt.*;
import java.awt.image.*;
import java.lang.reflect.Field;
import java.nio.*;
import java.util.HashMap;
import java.util.Map;
import smart.events.EventHandler;

/**
 * @author Brandon
 */
public class Client {

    private boolean active;
    private String initseq;
    private Canvas canvas;
    private OperatingMode Mode;
    public int transColor;
    private ClientApplet loader;
    private boolean renderWhileBlocking, debugfx, drawMouse;
    private BufferedImage gameImage, debugImage;
    private IntBuffer gameBuffer, debugBuffer;
    public static boolean minimized;
    private Input input = new Input();
    private static HashMap<Integer, Client> Clients = new HashMap<>();

    public enum OperatingMode {
        SafeMode, OpenGL, DirectX
    }

    public Client(String root, String params, String initseq, String userAgent, ByteBuffer gameBuffer, ByteBuffer debugBuffer, int width, int height) {       
        this.initseq = (initseq != null && initseq.length() > 0) ? initseq : null;
        this.Mode = OperatingMode.SafeMode;
        this.loader = new ClientApplet(root, params, userAgent, width, height);
        this.gameBuffer = gameBuffer.order(ByteOrder.LITTLE_ENDIAN).asIntBuffer();
        this.debugBuffer = debugBuffer.order(ByteOrder.LITTLE_ENDIAN).asIntBuffer();
        this.gameImage = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        this.debugImage = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
    }

    public ClientApplet getLoader() {
        return this.loader;
    }
    
    public void start() {
        this.loader.start();   
        while(this.loader.getApplet().getComponentCount() < 1) {
            Utilities.sleep(100);
        }
        
        Clients.put(getHashCode((Canvas)this.loader.getApplet().getComponent(0)), this);
        this.active = true;
    }
    
    public void stop() {
        if (this.active) {
            this.active = false;
            /*for (Map.Entry<Integer, Client> entry : Clients.entrySet()) {
                if (entry.getValue() == this) {
                    Clients.remove(entry.getKey());
                    break;
                }
            }
            EventHandler.remove(this.canvas);
            this.loader.stop();*/
            this.loader = null;
        }
    }
    
    public static int getHashCode(Canvas canvas) {
        return canvas.getClass().getClassLoader().hashCode();
    }

    public void updateClient(Canvas canvas) {
        try {
            if (canvas != null && Clients.containsKey(getHashCode(canvas))) {
                Client client = Clients.get(getHashCode(canvas));
                if (client != null) {
                    client.updateCanvas(canvas);
                }
            }
        } catch (Exception e) {
            Utilities.StackTrace(e);
        }
    }

    private void updateCanvas(Canvas canvas) {
        if (canvas != null && canvas != this.canvas) {
            this.input.setComponent(canvas);
            EventHandler.remove(this.canvas);
            EventHandler.setBlocking(this.canvas = canvas, input.isMouseEnabled(), input.isKeysEnabled());
            
            if (initseq != null) {
                this.sendKeys(initseq, 90, 60);
                this.initseq = null;
                Main.debug("Init Sequence Dispatched.");
            }
        }
    }

    public Graphics drawGraphics(Graphics2D G) {
        if (this.active) {
            switch (this.Mode) {
                case SafeMode:
                    try {
                        Field rasterField = BufferedImage.class.getDeclaredField("raster");
                        rasterField.setAccessible(true);
                        WritableRaster bufferRaster = (WritableRaster) rasterField.get(this.gameImage);
                        final int[] bufferData = ((DataBufferInt) bufferRaster.getDataBuffer()).getData();
                        WritableRaster debugRaster = (WritableRaster) rasterField.get(this.debugImage);
                        final int[] debugData = ((DataBufferInt) debugRaster.getDataBuffer()).getData();
                        final Graphics debugGraphics = this.debugImage.getGraphics();
                        debugGraphics.setColor(Color.green);
                        int Length = this.gameImage.getHeight() * this.gameImage.getWidth();

                        gameBuffer.rewind();
                        gameBuffer.put(bufferData, 0, Length);
                        if (renderWhileBlocking && !minimized) {
                            final Point P = getMousePos();
                            if (debugfx) {
                                debugBuffer.rewind();
                                for (int I = 0; I < Length; ++I) {
                                    int color = debugBuffer.get();
                                    if (color != transColor) {
                                        debugData[I] = color;
                                    } else {
                                        debugData[I] = bufferData[I];
                                    }
                                }
                            } else {
                                debugGraphics.drawImage(this.gameImage, 0, 0, null);
                            }

                            //Draw Mouse...
                            if (this.drawMouse) {
                                debugGraphics.fillOval(P.x - 2, P.y - 2, 4, 4);
                            }

                            if (G != null) {
                                G.drawImage(this.debugImage, 0, 0, null);
                            }
                        }
                    } catch (NoSuchFieldException | SecurityException | IllegalArgumentException | IllegalAccessException e) {
                        Utilities.StackTrace(e);
                    }
                    break;

                case OpenGL:
                    if (GLDXLoader.OpenGLLoaded) {
                        GLDXLoader.GetOpenGLStaticBuffer(gameBuffer);
                    }
                    break;

                case DirectX:
                    if (GLDXLoader.DirectXLoaded) {
                        GLDXLoader.GetDirectXStaticBuffer(gameBuffer);
                    }
                    break;

                default:
                    return G;
            }

            return this.gameImage.getGraphics();
        }
        return null;
    }
    
    public boolean isActive() {
        return this.active;
    }
    
    public boolean isKeysEnabled() {
        return input.isKeysEnabled();
    }
    
    public boolean isMouseEnabled() {
        return input.isMouseEnabled();
    }
    
    public boolean isRenderBlocking() {
        return renderWhileBlocking;
    }
    
    public boolean isDebugFx() {
        return this.debugfx;
    }
    
    public void moveMouse(int x, int y) {
        if (!this.isMouseEnabled()) {
            this.input.moveMouse(x, y);
            
            if (GLDXLoader.OpenGLLoaded || GLDXLoader.DirectXLoaded) {
                GLDXLoader.DrawMouse(x, y, 0, 255, 0);
            }
        }
    }

    public void windMouse(int x, int y) {
        if (!this.isMouseEnabled()) {
            this.input.windMouse(x, y);
            
            if (GLDXLoader.OpenGLLoaded || GLDXLoader.DirectXLoaded) {
                GLDXLoader.DrawMouse(x, y, 0, 255, 0);
            }
        }
    }

    public void clickMouse(int x, int y, int button) {
        this.input.clickMouse(x, y, button);
    }

    public void holdMouse(int x, int y, int button) {
        this.input.holdMouse(x, y, button);
    }

    public void releaseMouse(int x, int y, int button) {
        this.input.releaseMouse(x, y, button);
    }

    public boolean isMouseButtonHeld(int button) {
        return this.input.isMouseButtonHeld(button);
    }

    public Point getMousePos() {
        return this.input.getMousePos();
    }

    public void sendKeys(String string, int keywait, int keymodwait) {
        this.input.sendKeys(string, keywait, keymodwait);
    }

    public void holdKey(int keycode) {
        this.input.holdKey(keycode);
    }

    public boolean isKeyDown(int keycode) {
        return this.input.isKeyDown(keycode);
    }

    public void releaseKey(int keycode) {
        this.input.releaseKey(keycode);
    }

    public void setGraphics(boolean On) {
        renderWhileBlocking = On;
        Main.updateFrame();
    }

    public void setDebug(boolean on) {
        debugfx = on;
        Main.updateFrame();
    }

    public void setMouseInput(boolean Enabled) {
        this.input.setMouseEnabled(Enabled);
        Main.updateFrame();
    }

    public void setKeyInput(boolean Enabled) {
        this.input.setKeysEnabled(Enabled);
        Main.updateFrame();
    }
    
    public void setMode(OperatingMode Mode) {
        this.Mode = Mode;
    }
    
    public OperatingMode getMode() {
        return this.Mode;
    }
    
    public void setDrawMouse(boolean drawMouse) {
        this.drawMouse = drawMouse;
    }
}
