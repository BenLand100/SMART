package smart;

import java.applet.*;
import java.awt.*;
import java.io.IOException;
import java.net.*;
import java.util.HashMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.swing.*;
import sun.applet.AppletClassLoader;

/**
 * @author Brandon
 */
public class ClientApplet extends JPanel implements AppletStub {

    private boolean active = false;
    private URL documentBase;
    private URL codeBase;
    private Applet applet;
    private ClassLoader loader;
    private HashMap<String, String> parameters = new HashMap<>();
    private static final Pattern codeRegex = Pattern.compile("code=(.*) ");
    private static final Pattern archiveRegex = Pattern.compile("archive=(.*) ");
    private static final Pattern parameterRegex = Pattern.compile("<param name=\"([^\\s]+)\"\\s+value=\"([^>]*)\">");

    public ClientApplet(String Root, String Parameters, String UserAgent, int width, int height) {
        try {
            this.setLayout(new BorderLayout(0, 0));
            String PageSource = Main.utilities.downloadPage(new URL(Root + Parameters), UserAgent);
            
            Matcher CodeMatcher = codeRegex.matcher(PageSource);
            Matcher ArchiveMatcher = archiveRegex.matcher(PageSource);

            if (ArchiveMatcher.find() && CodeMatcher.find()) {
                JarURLConnection Connection = (JarURLConnection) new URL("jar:" + Root + ArchiveMatcher.group(1) + "!/").openConnection();
                Connection.addRequestProperty("Accept", "text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5");
                Connection.addRequestProperty("Accept-Charset", "ISO-8859-1,UTF-8;q=0.7,*;q=0.7");
                Connection.addRequestProperty("Accept-Language", "de,en;q=0.7,en-us;q=0.3");
                Connection.addRequestProperty("Connection", "keep-alive");
                Connection.addRequestProperty("Host", "runescape");
                Connection.addRequestProperty("Keep-Alive", "300");
                Connection.addRequestProperty("User-Agent", "Mozilla/4.0 (" + System.getProperty("os.name") + " " + System.getProperty("os.version") + ") Java/" + System.getProperty("java.version"));
                Main.debug("Using Jar: " + Root + ArchiveMatcher.group(1) + "\n");
                
                this.documentBase = new URL(Root);
                this.codeBase = new URL(Root + ArchiveMatcher.group(1));
                Matcher ParameterMatcher = parameterRegex.matcher(PageSource);
                while(ParameterMatcher.find()) {
                    parameters.put(ParameterMatcher.group(1), ParameterMatcher.group(2));
                    Main.debug("Parameter: " + ParameterMatcher.group(1) + "            Value:  " + ParameterMatcher.group(2));
                }
                
                loader = AppletClassLoader.newInstance(new URL[]{Connection.getJarFileURL()});
                applet = (Applet)loader.loadClass(CodeMatcher.group(1).replaceAll(".class", "")).newInstance();
                applet.setStub(this);
                applet.setPreferredSize(new Dimension(width, height));
                this.add(applet, BorderLayout.CENTER);
                this.active = true;
            }
        } catch (IOException | ClassNotFoundException | InstantiationException | IllegalAccessException e) {
            Utilities.StackTrace(e);
            JOptionPane.showMessageDialog(null, "Please Check Your Internet Connection.", "Error Loading..", JOptionPane.ERROR_MESSAGE);
            Main.destruct();
        }
    }
    
    public Applet getApplet() {
        return this.applet;
    }
    
    public void start() {
        this.applet.init();
        System.out.println();
        Main.debug("Applet Initialized.");
        this.applet.start();
        Main.debug("Applet Started.");
    }
    
    public void stop() {
        if (this.applet != null) {
            this.applet.stop();
            this.applet.destroy();
            this.applet = null;
            this.loader = null;
            System.out.println();
            Main.debug("Destruction Sequence Completed.");
        }
    }

    @Override
    public boolean isActive() {
        return active;
    }

    @Override
    public URL getDocumentBase() {
        return documentBase;
    }

    @Override
    public URL getCodeBase() {
        return codeBase;
    }

    @Override
    public String getParameter(String name) {
        String result = parameters.get(name);
        return result == null ? "" : result;
    }

    @Override
    public AppletContext getAppletContext() {
        return null;
    }

    @Override
    public void appletResize(int width, int height) {
    }
}
