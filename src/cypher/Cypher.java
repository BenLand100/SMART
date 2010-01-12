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

package cypher;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;

public class Cypher {
    
    public static byte[] encode(byte[] data, String key) {
        data = (byte[]) data.clone();
        char[] chars = key.toCharArray();
        byte[] rotate = new byte[chars.length];
        int times = 17;
        for (int i = 0; i < rotate.length; i++) {
            char c = chars[i];
            if (Character.isLetter(c)) {
                if (Character.isUpperCase(c)) {
                    rotate[i] = (byte)((c - 'A') / 26D * 0xFF);
                } else {
                    rotate[i] = (byte)((26 - (c - 'a')) / 26D * 0xFF);
                }
            } else if (Character.isDigit(c)) {
                rotate[i] = (byte)((c - '0') / 10D * 0xFF);
            } else {
                rotate[i] = (byte)c;
            }
            times += times % (rotate[i] == 0 ? 1 : rotate[i]);
        }
        times /= rotate.length;
        for (int i = 0; i < times * data.length; i++) {
            data[i%data.length] += rotate[i%rotate.length];
        }
        return data;
    }
    
    public static void genRotate(String key) {
        char[] chars = key.toCharArray();
        byte[] rotate = new byte[chars.length];
        int times = 17;
        for (int i = 0; i < rotate.length; i++) {
            char c = chars[i];
            if (Character.isLetter(c)) {
                if (Character.isUpperCase(c)) {
                    rotate[i] = (byte)((c - 'A') / 26D * 0xFF);
                } else {
                    rotate[i] = (byte)((26 - (c - 'a')) / 26D * 0xFF);
                }
            } else if (Character.isDigit(c)) {
                rotate[i] = (byte)((c - '0') / 10D * 0xFF);
            } else {
                rotate[i] = (byte)c;
            }
            times += times % (rotate[i] == 0 ? 1 : rotate[i]);
        }
        times /= rotate.length;
        System.out.println("Times: " + times);
        System.out.println("Rotate: " + Arrays.toString(rotate));
    }
    
    public static byte[] decode(byte[] data, String key) {
        data = (byte[]) data.clone();
        char[] chars = key.toCharArray();
        byte[] rotate = new byte[chars.length];
        int times = 17;
        for (int i = 0; i < rotate.length; i++) {
            char c = chars[i];
            if (Character.isLetter(c)) {
                if (Character.isUpperCase(c)) {
                    rotate[i] = (byte)((c - 'A') / 26D * 0xFF);
                } else {
                    rotate[i] = (byte)((26 - (c - 'a')) / 26D * 0xFF);
                }
            } else if (Character.isDigit(c)) {
                rotate[i] = (byte)((c - '0') / 10D * 0xFF);
            } else {
                rotate[i] = (byte)c;
            }
            times += times % (rotate[i] == 0 ? 1 : rotate[i]);
        }
        times /= rotate.length;
        for (int i = 0; i < times * data.length; i++) {
            data[i%data.length] -= rotate[i%rotate.length];
        }
        return data;
    }
    
    public static void main(String[] args) throws Exception {
        if (args.length != 2) {
            System.out.println("Usage: java cypher.Cypher [classpath] [outputfile]");
            System.exit(-1);
        }
        DataOutputStream out = new DataOutputStream(new FileOutputStream(new File(args[1])));
        
        out.writeBytes("/**\n");
        out.writeBytes(" *  Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100)\n");
        out.writeBytes(" *\n");
        out.writeBytes(" *  This file is part of the Scar Minimizing Autoing Resource Thing (SMART)\n");
        out.writeBytes(" *\n");
        out.writeBytes(" *  SMART is free software: you can redistribute it and/or modify\n");
        out.writeBytes(" *  it under the terms of the GNU General Public License as published by\n");
        out.writeBytes(" *  the Free Software Foundation, either version 3 of the License, or\n");
        out.writeBytes(" *  (at your option) any later version.\n");
        out.writeBytes(" *\n");
        out.writeBytes(" *  SMART is distributed in the hope that it will be useful,\n");
        out.writeBytes(" *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
        out.writeBytes(" *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n");
        out.writeBytes(" *  GNU General Public License for more details.\n");
        out.writeBytes(" *\n");
        out.writeBytes(" *  You should have received a copy of the GNU General Public License\n");
        out.writeBytes(" *  along with SMART. If not, see <http://www.gnu.org/licenses/>.\n");
        out.writeBytes(" */\n\n");
        out.writeBytes("//****WARNING**** This file is autogenerated; do not modify\n\n");

        HashMap<String,File> dirs = new HashMap<String,File>();
        HashMap<String,String> classVars = new HashMap<String,String>();
        String path = "";
        File file = new File(args[0]);
        while (file != null) {
            File[] files = file.listFiles();
            for (File f : files) {
                if (f.isDirectory()) {
                    dirs.put((path.length() > 0 ? (path + "/") : "") + f.getName(),f);
                    continue;
                } else if (f.getName().endsWith(".class")) {
                    System.out.println(" +" + f);
                    String name = f.getName();
                    name = name.substring(0,name.lastIndexOf("."));
                    FileInputStream fin = new FileInputStream(f);
                    ByteArrayOutputStream bos = new ByteArrayOutputStream();
                    byte[] buffer = new byte[0xFFFF];
                    int length;
                    while ((length = fin.read(buffer)) != -1) {
                        bos.write(buffer,0,length);
                    }
                    fin.close();
                    bos.close();
                    byte[] data = encode(bos.toByteArray(), "OpenSourceSucks");
                    String var = "_" + Integer.toHexString(name.hashCode()).toUpperCase();
                    out.writeBytes("const unsigned char " + var + "[" + data.length + "] = {\n\t");
                    for (int i = 0; i < data.length; i++) {
                        out.writeBytes("0x" + Integer.toHexString((int)(data[i] & 0xFF)).toUpperCase() + ", ");
                    }
                    classVars.put(path + "/" + name, var);
                    out.writeBytes("\n};\nint len" + var + " = " + data.length + ";\n\n");
                }
            }
            Iterator<String> iter = dirs.keySet().iterator();
            if (iter.hasNext()) {
                path = iter.next();
                file = dirs.remove(path);
            } else {
                break;
            }
        }
        out.writeBytes("const void* classes[] = {\n\t");
        for (String clazz : classVars.keySet()) {
            out.writeBytes("(void*)\"" + clazz + "\", (void*)&" + classVars.get(clazz) + ", (void*)&len" + classVars.get(clazz) + ",\n\t");
        }
        out.writeBytes("};\n\n");
        out.writeBytes("const int numClasses = " + classVars.size() + ";\n\n");
        out.close();
    }
    
}
