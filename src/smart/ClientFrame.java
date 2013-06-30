package smart;

import java.awt.*;
import java.awt.event.*;
import java.net.URLEncoder;
import java.nio.ByteBuffer;
import java.util.Locale;
import javax.swing.*;
import smart.Client.OperatingMode;

/**
 * @author Brandon
 */
public class ClientFrame extends JFrame {

    private static final long serialVersionUID = -4985152588153565884L;
    public static final String USER_AGENT;
    private static final String VERSION = "8.1";
    private static final String TITLE = "SMARTv" + VERSION + " Custom Revision";
    private static final Utilities utility = Main.utilities;
    private static final Image trayIcon;
    private static final ImageIcon graphicsIcon, paintIcon;
    private static final ImageIcon keyboardIcon, mouseIcon;

    static {
        String osname = System.getProperty("os.name");
        String windowing = "X11";
        if (osname.contains("Windows")) {
            windowing = "Windows";
        } else if (osname.contains("Mac")) {
            windowing = "Macintosh";
        }
        
        USER_AGENT = "Mozilla/5.0 (" + windowing + "; U; " + osname + " " + System.getProperty("os.version") + "; " + Locale.getDefault().getLanguage() + "-" + Locale.getDefault().getCountry() + "; rv:9.0.1) Gecko/20100101 Firefox/9.0.1";
        graphicsIcon = new ImageIcon(utility.scaleImage(16, 16, utility.loadResourceImage("/icons/Graphics.png")));
        paintIcon = new ImageIcon(utility.scaleImage(16, 16, utility.loadResourceImage("/icons/Painting.png")));
        keyboardIcon = new ImageIcon(utility.scaleImage(16, 16, utility.loadResourceImage("/icons/Keyboard.png")));
        mouseIcon = new ImageIcon(utility.scaleImage(16, 16, utility.loadResourceImage("/icons/Mouse.png")));
        trayIcon = utility.loadResourceImage("/icons/SystemTray.png");
    }
    
    
    private Client client;
    private static JToolBar toolbar;    
    private static SystemTray systemTray;
    private static TrayIcon systemTrayIcon;
    private JComboBox<String> ModeSelector;
    private JButton TrayBtn;
    private JToggleButton GraphicsBtn, DebugBtn;
    private JToggleButton MouseBtn, KeyboardBtn;

    public ClientFrame(int ID) {
        String response = Main.utilities.downloadPage("http://blanddns.no-ip.org:81/smart.php?version=" + URLEncoder.encode(VERSION), USER_AGENT);
        Main.debug("Registration Response: " + ((response == null) ? "Unsuccessful" : response.replaceAll("\n|\r", "")));

        this.setTitle(TITLE + "    ID: [" + ID + "]");
        JPopupMenu.setDefaultLightWeightPopupEnabled(false);
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        Utilities.setCustomTheme(UIManager.getSystemLookAndFeelClassName());
        this.setLayout(new BorderLayout(0, 0));
        this.setToolBar();
        this.setSystemTray();
        this.addListeners();
        this.pack();
        this.setResizable(false);
    }

    public Client addClient(String root, String params, String initseq, String userAgent, ByteBuffer gameBuffer, ByteBuffer debugBuffer, int width, int height) {
        this.client = new Client(root, params, initseq, userAgent == null || userAgent.isEmpty() ? USER_AGENT : userAgent, gameBuffer, debugBuffer, width, height);
        this.add(client.getLoader(), BorderLayout.CENTER);
        this.client.setDebug(false);
        this.client.setGraphics(true);
        this.client.setKeyInput(false);
        this.client.setMouseInput(false);
        this.updateUI();
        this.client.start();
        this.pack();
        this.setSize(this.getWidth(), this.getHeight() + 1);
        this.setLocationRelativeTo(null);
        this.setVisible(true);
        System.gc();
        return this.client;
    }

    public void removeClient() {
        if (this.client != null) {
            this.setVisible(false);
            this.client.stop();
            this.client = null;
            System.gc();
        }
    }
    
    public void destruct() {
        this.removeClient();
        this.dispose();
        System.exit(0);
    }
    
    public Client getClient() {
        return this.client;
    }
    
    private void setSystemTray() {
        if (SystemTray.isSupported()) {
            systemTray = SystemTray.getSystemTray();

            PopupMenu trayMenu = new PopupMenu();
            MenuItem TrayMenuItem = new MenuItem("Show");
            TrayMenuItem.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    Client.minimized = false;
                    JNI.Natives.addToTaskBar(JNI.Natives.getWindowHandle(ClientFrame.this));
                    systemTray.remove(systemTrayIcon);
                    ClientFrame.this.setState(JFrame.NORMAL);
                }
            });
            
            trayMenu.add(TrayMenuItem);
            TrayMenuItem = new MenuItem("Exit");
            TrayMenuItem.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    ClientFrame.this.destruct();
                    System.exit(0);
                }
            });

            trayMenu.add(TrayMenuItem);     
            systemTrayIcon = new TrayIcon(trayIcon, VERSION, trayMenu);
            systemTrayIcon.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    Client.minimized = false;
                    JNI.Natives.addToTaskBar(JNI.Natives.getWindowHandle(ClientFrame.this));
                    systemTray.remove(systemTrayIcon);
                    ClientFrame.this.setState(JFrame.NORMAL);
                }
            });
            systemTrayIcon.setImageAutoSize(true);
            systemTrayIcon.setToolTip("Smartv" + VERSION + "   ID: [" + Main.getPID() + "]");
        }
    }

    private void setToolBar() {
        this.add(toolbar = new JToolBar(), BorderLayout.NORTH);
        toolbar.setLayout(new BoxLayout(toolbar, BoxLayout.X_AXIS));
        toolbar.add(ModeSelector = new JComboBox<>(new String[]{"SafeMode", "OpenGL", "DirectX"}));
        ModeSelector.setPreferredSize(new Dimension(10, 20));
        toolbar.add(Box.createHorizontalGlue());
        toolbar.add(Box.createHorizontalGlue());
        toolbar.add(Box.createHorizontalGlue());
        toolbar.add(TrayBtn = new JButton(new ImageIcon(utility.scaleImage(16, 16, trayIcon))));
        toolbar.add(GraphicsBtn = new JToggleButton(graphicsIcon));
        toolbar.add(DebugBtn = new JToggleButton(paintIcon));
        toolbar.add(MouseBtn = new JToggleButton(mouseIcon));
        toolbar.add(KeyboardBtn = new JToggleButton(keyboardIcon));

        toolbar.setFloatable(false);
        toolbar.setFocusable(false);
        TrayBtn.setFocusable(false);
        GraphicsBtn.setFocusable(false);
        DebugBtn.setFocusable(false);
        MouseBtn.setFocusable(false);
        KeyboardBtn.setFocusable(false);
        ModeSelector.setFocusable(false);

        ModeSelector.setSelectedIndex(0);
        Dimension Size = new Dimension(22, 22);
        TrayBtn.setPreferredSize(Size);
        GraphicsBtn.setPreferredSize(Size);
        DebugBtn.setPreferredSize(Size);
        MouseBtn.setPreferredSize(Size);
        KeyboardBtn.setPreferredSize(Size);

        TrayBtn.setToolTipText("Minimize To System Tray");
        GraphicsBtn.setToolTipText("Disable Graphics");
        DebugBtn.setToolTipText("Disable Painting");
        MouseBtn.setToolTipText("Disable Mouse");
        KeyboardBtn.setToolTipText("Disable Keyboard");
        ModeSelector.setToolTipText("Change Operation Mode");
    }
    
    private void updateToolTips(Client client) {
        KeyboardBtn.setToolTipText(client.isKeysEnabled() ? "Disable Keyboard" : "Enable Keyboard");
        MouseBtn.setToolTipText(client.isMouseEnabled() ? "Disable Mouse" : "Enable Mouse");
        GraphicsBtn.setToolTipText(client.isRenderBlocking() ? "Disable Graphics" : "Enable Graphics");
        DebugBtn.setToolTipText(client.isDebugFx() ? "Disable Painting" : "Enable Painting");
        ModeSelector.setToolTipText("Graphics Modes");
    }
    
    public void updateUI() {
        if (client != null) {
            this.updateButtonStates(client);
            this.updateToolTips(client);
        }
    }
    
    private void updateButtonStates(Client client) {
        KeyboardBtn.setSelected(!client.isKeysEnabled());
        KeyboardBtn.getModel().setPressed(!client.isKeysEnabled());
        MouseBtn.setSelected(!client.isMouseEnabled());
        MouseBtn.getModel().setPressed(!client.isMouseEnabled());

        GraphicsBtn.setSelected(client.isRenderBlocking());
        GraphicsBtn.getModel().setPressed(client.isRenderBlocking());
        DebugBtn.setSelected(client.isDebugFx());
        DebugBtn.getModel().setPressed(client.isDebugFx());
        ModeSelector.setSelectedItem(client.getMode().toString());
    }
    
    private void addListeners() {
        this.addWindowListener(new WindowAdapter() {
            @Override
            public void windowActivated(WindowEvent e) {
                ClientFrame.this.requestFocusInWindow();
            }

            @Override
            public void windowDeiconified(WindowEvent e) {
                ClientFrame.this.requestFocusInWindow();
                Client.minimized = false;
            }

            @Override
            public void windowIconified(WindowEvent e) {
                Client.minimized = true;
            }

            @Override
            public void windowClosing(WindowEvent e) {
                ClientFrame.this.destruct();
                System.exit(0);
            }
        });

        ModeSelector.addItemListener(new ItemListener() {
            @Override
            public void itemStateChanged(ItemEvent e) {
                if (e.getStateChange() == ItemEvent.SELECTED) {
                    if (client != null) {
                        client.setMode(OperatingMode.valueOf((String) ModeSelector.getSelectedItem()));
                    }
                }
            }
        });

        GraphicsBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (client != null) {
                    client.setGraphics(!client.isRenderBlocking());
                    DebugBtn.setEnabled(client.isRenderBlocking());
                }
            }
        });

        DebugBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (client != null) {
                    client.setDebug(!client.isDebugFx());
                }
            }
        });

        KeyboardBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (client != null) {
                    client.setKeyInput(!client.isKeysEnabled());
                }
            }
        });

        MouseBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (client != null) {
                    client.setMouseInput(!client.isMouseEnabled());
                }
            }
        });
        
        TrayBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (!Client.minimized) {
                    try {
                        if (SystemTray.isSupported()) {
                            systemTray.add(systemTrayIcon);
                            JNI.Natives.removeFromTaskBar(JNI.Natives.getWindowHandle(ClientFrame.this));
                            Client.minimized = true;
                        }
                        ClientFrame.this.setState(JFrame.ICONIFIED);
                    } catch (Exception ex) {
                        Utilities.StackTrace(ex);
                    }
                }
            }
        });
    }
}
