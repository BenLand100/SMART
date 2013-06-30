package smart;

import java.awt.Image;
import java.awt.image.*;
import java.io.*;
import java.net.URL;
import java.net.URLConnection;
import javax.imageio.ImageIO;
import javax.swing.*;

/**
 * @author Brandon
 */
public class Utilities {
    
    private static final StringWriter TextWriter = new StringWriter();
    private static final PrintWriter Printer = new PrintWriter(TextWriter);
    
    public static void StackTrace(Exception e) {
        e.printStackTrace(Printer);
        System.out.println(TextWriter.toString());
    }

    public static void sleep(long Time) {
        try {
            Thread.sleep(Time);
        } catch (Exception e) {
            StackTrace(e);
        }
    }

    public static void setCustomTheme(String Theme) {
        try {
            UIManager.setLookAndFeel(Theme);
        } catch (ClassNotFoundException | InstantiationException | IllegalAccessException | UnsupportedLookAndFeelException e) {
            StackTrace(e);
        }
    }

    public Image scaleImage(int width, int height, Image Img) {
        return Img.getScaledInstance(width, height, Image.SCALE_SMOOTH);
    }

    public BufferedImage loadResourceImage(String ResourcePath) {
        try {
            return ImageIO.read(getClass().getResource(ResourcePath));
        } catch (Exception e) {
            StackTrace(e);
        }
        return null;
    }

    public ImageIcon loadResourceIcon(String ResourcePath) {
        return new ImageIcon(getClass().getResource(ResourcePath));
    }

    public Image grayScale(Image Img, int GrayPercentage) {
        ImageProducer Producer = new FilteredImageSource(Img.getSource(), new GrayFilter(true, GrayPercentage));
        return java.awt.Toolkit.getDefaultToolkit().createImage(Producer);
    }
    
    public String downloadPage(String Address, String UserAgent) {
        try {
            return downloadPage(new URL(Address), UserAgent);
        } catch(Exception e) {
            StackTrace(e);
        }
        return null;
    }
    
    public String downloadPage(URL Address, String UserAgent) {
        try {
            URLConnection Connection = Address.openConnection();
            Connection.addRequestProperty("User-Agent", UserAgent);

            String Line;
            StringBuilder Builder = new StringBuilder();
            try (BufferedReader Reader = new BufferedReader(new InputStreamReader(Connection.getInputStream()))) {
                while ((Line = Reader.readLine()) != null) {
                    Builder.append(Line).append("\n");
                }
            }
            return Builder.toString();
        } catch (Exception e) {
            StackTrace(e);
            return null;
        }
    }
}
