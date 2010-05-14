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

package smart.updater;

import com.sun.org.apache.bcel.internal.classfile.ClassParser;
import com.sun.org.apache.bcel.internal.classfile.Method;
import com.sun.org.apache.bcel.internal.generic.ClassGen;
import com.sun.org.apache.bcel.internal.generic.InstructionHandle;
import com.sun.org.apache.bcel.internal.generic.InstructionList;
import com.sun.org.apache.bcel.internal.generic.MethodGen;
import com.sun.org.apache.bcel.internal.generic.SIPUSH;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map.Entry;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

/**
 * Updater is the core of the updater. To add a new search, put its class name
 * in the SEARCHES field, and ensure its .class file is in the correct package.
 * Uage for this program is as follows:
 *
 * java smart.updater.Updater
 *     Simply runs the updater on a freshly downlaoded runescape.jar and
 *     writes everything to STDOUT
 *
 * java smart.updater.Updater [outputfile]
 *     Runs the updater on a freshly downloaded runescape.jar and generates a
 *     SMART compatible pascal file in the provided file name, overwriting any
 *     previous data.
 *
 * java smart.updater.Updater [downloadedjar] [outputfile]
 *     Runs the updater on the specified jar file (without downloading a new)
 *     runescape.jar, and generates a SMART compatible pascal file in the
 *     provided file name, overwriting any previous data.
 *
 * @author benland100
 */
public class Updater {

    /**
     * Searches to run go here. Must be in the smart.updater.searches package.
     * Searches are run in the order given, but may be deferred if required data
     * is not avaliable when run.
     */
    public static final String[] SEARCHES = new String[]{
        "BaseAnimableXY", "GroundPlane", "Toolkit", "Viewport", "StandardDetail",
        "MinimapAngle"
    };

    /**
     * See the class description for usage information.
     *
     * @param args
     */
    public static void main(String[] args) {
        Updater updater;
        RSClient data;
        switch (args.length) {
            case 1:
                updater = new Updater(download("http://world" + Math.round(Math.random() * 169) + ".runescape.com/runescape.jar"));
                data = updater.runSearches();
                System.out.println(data);
                saveFile(args[0],data.smartFile().getBytes());
                break;
            case 2:
                updater = new Updater(readFile(args[0]));
                data = updater.runSearches();
                System.out.println(data);
                saveFile(args[1],data.smartFile().getBytes());
                break;
            case 0:
            default:
                updater = new Updater(download("http://world" + Math.round(Math.random() * 169) + ".runescape.com/runescape.jar"));
                data = updater.runSearches();
                System.out.println(data);
                System.out.println(data.smartFile());
        }
    }

    /**
     * Saves data to a file, erasing all previous information.
     *
     * @param file Path to file
     * @param data Data to save
     */
    public static void saveFile(String file, byte[] data) {
        try {
            File f = new File(file);
            if (f.exists()) {
                f.delete();
            }
            f.createNewFile();
            FileOutputStream out = new FileOutputStream(f);
            out.write(data);
            out.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Reads data from a file and returns the bytes
     *
     * @param file Path to file
     * @return Data read
     */
    public static byte[] readFile(String file) {
        try {
            FileInputStream in = new FileInputStream(new File(file));
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            byte[] buffer = new byte[1024];
            int len;
            while ((len = in.read(buffer, 0, 1024)) > -1) {
                out.write(buffer, 0, len);
            }
            in.close();
            out.close();
            return out.toByteArray();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }


    /**
     * Reads data from a URL and returns the bytes
     *
     * @param address Valid URL
     * @return Data read
     */
    public static byte[] download(String address) {
        ByteArrayOutputStream out = null;
        InputStream in = null;
        try {
            URL url = new URL(address);
            URLConnection urlc = url.openConnection();
            urlc.setConnectTimeout(1000);
            urlc.setReadTimeout(1000);
            out = new ByteArrayOutputStream();
            in = urlc.getInputStream();
            byte[] buffer = new byte[1024];
            int numRead;
            while ((numRead = in.read(buffer)) != -1) {
                out.write(buffer, 0, numRead);
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (in != null) {
                    in.close();
                }
                if (out != null) {
                    out.close();
                }
            } catch (IOException e) {
            }
        }
        if (out != null) {
            return out.toByteArray();
        }
        return null;
    }

    /**
     * HashMap of raw binary class data by class name, just in case.
     */
    private HashMap<String, byte[]> rawclasses = new HashMap<String, byte[]>();

    /**
     * HashMap of ClassGens by class name, for use in the Searches.
     */
    private HashMap<String, ClassGen> javaclasses = new HashMap<String, ClassGen>();

    /**
     * Creates an Updater instance by parsing the classes out of a byte[] from
     * a runescape.jar. This constructor does not do any bytecode searching, it
     * simply parses classes.
     *
     * @param jar byte[] from a runescape.jar
     */
    public Updater(byte[] jar) {
        try {
            ZipInputStream in = new ZipInputStream(new ByteArrayInputStream(jar));
            ZipEntry entry = in.getNextEntry();
            byte[] buffer = new byte[1024];
            while (entry != null) {
                String entryName = entry.getName();
                if (entryName.endsWith(".class")) {
                    ByteArrayOutputStream data = new ByteArrayOutputStream();
                    int len;
                    while ((len = in.read(buffer, 0, 1024)) > -1) {
                        data.write(buffer, 0, len);
                    }
                    rawclasses.put(entryName.replaceAll("\\\\", ".").replace(".class", ""), data.toByteArray());
                    data.close();
                }
                in.closeEntry();
                entry = in.getNextEntry();
            }
            in.close();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
        try {
            for (Entry<String, byte[]> entry : rawclasses.entrySet()) {
                ByteArrayInputStream in = new ByteArrayInputStream(entry.getValue());
                ClassParser parser = new ClassParser(in, entry.getKey());
                javaclasses.put(entry.getKey(), new ClassGen(parser.parse()));
                in.close();
            }
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    /**
     * Searches the client class for the version. Usually works fine.
     *
     * @return Client version
     */
    public int getVersion() {
        ClassGen client = javaclasses.get("client");
        if (client == null)
            return -1;
        for (Method m : client.getMethods()) {
            if (m.getName().equals("main")) {
                MethodGen gen = new MethodGen(m, client.getClassName(), client.getConstantPool());
                InstructionList il = gen.getInstructionList();
                InstructionHandle i = il.getStart();
                do {
                    if (i.getInstruction() instanceof SIPUSH) {
                        int value = ((SIPUSH) i.getInstruction()).getValue().intValue();
                        if (value >= 400 && value <= 800 && value != 503 && value != 768) {
                            return value;
                        }
                    }
                } while ((i = i.getNext()) != null);
            }
        }
        return -1;
    }

    /**
     * Runs all the specified searches deferring as necessary and prints out
     * some nice debug information.
     * 
     * @return RSClient instance containing the hooks found.
     */
    public RSClient runSearches() {
        long start = System.currentTimeMillis();
        System.out.println("=== SMART Public Updater by BenLand100 ===");
        System.out.println();
        int version = getVersion();
        if (version == -1) System.out.println("Warning: version not found, hooks may fail");
        System.out.println("Preparing to run searches on:");
        System.out.println("  * " + rawclasses.size() + " class files");
        System.out.println("  * From client version: " + version);
        System.out.println();
        RSClient data = new RSClient(version);
        LinkedList<Search> deferred = new LinkedList<Search>();
        for (String searchName : SEARCHES) {
            Search search = null;
            try {
                search = (Search) Class.forName("smart.updater.searches." + searchName).newInstance();
                System.out.print(searchName + "... ");
                switch (search.run(data, javaclasses)) {
                    case Success:
                        System.out.println(" completed successfully");
                        break;
                    case Failure:
                        System.out.println(" failed to find its hooks");
                        break;
                    case MissingInfo:
                        System.out.println(" needs to be deferred");
                        deferred.add(search);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        int lastsize = deferred.size();
        while (deferred.size() > 0) {
            System.out.println("Running deferred searches");
            for (Search search : deferred) {
                System.out.print(search.getClass().getSimpleName() + "... ");
                switch (search.run(data, javaclasses)) {
                    case Success:
                        System.out.println(" completed after deferring");
                        deferred.remove(search);
                        break;
                    case Failure:
                        System.out.println(" failed to find its hooks");
                        deferred.remove(search);
                        break;
                    case MissingInfo:
                        System.out.println(" needs to be deferred again");
                }
            }
            if (lastsize == deferred.size()) {
                break;
            }
            lastsize = deferred.size();
        }
        for (Search search : deferred) {
            System.out.println(search.getClass().getSimpleName() + " failed to find its hooks");
        }
        System.out.println();
        System.out.println("Finished after " + (System.currentTimeMillis() - start) + "ms");
        System.out.println();
        return data;
    }
}
