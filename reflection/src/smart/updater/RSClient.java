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

import java.util.LinkedList;

/**
 * RSClient organizes the paths and names of hooks into a Runescape client. All
 * static fields are stored here, as well as references to classes which contain
 * their own non-static fields.
 *
 * @author benland100
 */
public class RSClient {
    public LinkedList<RSField> staticfields = new LinkedList<RSField>();
    public LinkedList<RSClass> classes = new LinkedList<RSClass>();
    public final int version;

    /**
     * Constructs a new RSClient object.
     *
     * @param version Client version
     */
    public RSClient(int version) {
        this.version = version;
    }

    /**
     * Adds a static field to this client given its hook name and path, provided
     * it does not exist by that path.
     *
     * @param properName Name of the hook
     * @param path Path for the static field
     * @return RSField object representing this field
     */
    public RSField addField(String properName, String path) {
        for (RSField field : staticfields) {
            if (field.hookName.equals(path)) return field;
        }
        staticfields.add(new RSField(path,properName));
        return staticfields.getLast();
    }

    /**
     * Finds the RSField object for a static field given its hook name.
     *
     * @param properName Name of the hook
     * @return RSField object, or null if not found
     */
    public RSField getProperField(String properName) {
        for (RSField field : staticfields) {
            if (field.hookName.equals(properName)) return field;
        }
        return null;
    }

    /**
     * Finds the RSField object for a static field given its path.
     *
     * @param path Path for the static field
     * @return RSField object, or null if not found
     */
    public RSField getPathField(String path) {
        for (RSField field : staticfields) {
            if (field.path.equals(path)) return field;
        }
        return null;
    }

    /**
     * Stores a class hook in the client provided it does not already exist
     * by the given obfsucated name.
     *
     * @param properName The name of the hook
     * @param clientName The class being hooked
     * @return RSClass object representing this class
     */
    public RSClass addClass(String properName, String clientName) {
        for (RSClass clazz : classes) {
            if (clazz.hookName.equals(clientName)) return clazz;
        }
        classes.add(new RSClass(clientName,properName));
        return classes.getLast();
    }

    /**
     * Finds the RSClass object by its "proper" name, e.g. deobfuscated name.
     *
     * @param properName Deobfuscated name of the class
     * @return RSClass object, or null if not found
     */
    public RSClass getProperClass(String properName) {
        for (RSClass clazz : classes) {
            if (clazz.hookName.equals(properName)) return clazz;
        }
        return null;
    }

    /**
     * Finds the RSClass object by its "client" name, e.g. obfuscated name.
     *
     * @param clientName Obfuscated name of the class
     * @return RSClass object, or null if not found
     */
    public RSClass getClientClass(String clientName) {
        for (RSClass clazz : classes) {
            if (clazz.className.equals(clientName)) return clazz;
        }
        return null;
    }

    /**
     * Generates a pascal source file defining the constants for all known hooks
     * organized in an intuitive way.
     * 
     * @return String representing the file
     */
    public String smartFile() {
        StringBuilder buf = new StringBuilder();
        buf.append(pascal_gpl);
        buf.append("const\n");
        buf.append("\tClientVersion = ").append(version).append(";\n");
        buf.append("\n{Static Hooks}\n");
        for (RSField field : staticfields) {
            buf.append("\thook_static_").append(field.hookName).append(" = '").append(field.path).append("';\n");
        }
        for (RSClass clazz : classes) {
            String name = clazz.hookName;
            buf.append("\n{Class ").append(name).append("}\n");
            buf.append("\tclass_").append(name).append(" = '").append(clazz.className).append("';\n");
            name = name.toLowerCase();
            for (RSField field : clazz.fields) {
                buf.append("\thook_").append(name).append("_").append(field.hookName).append(" = '").append(field.path).append("';\n");
            }
        }
        return buf.toString();
    }

    /**
     * Generates a brief rundown of the values for the various hooks. Useful for
     * debuggin purposes and runtime information.
     *
     * @return String version of this object
     */
    public String toString() {
        StringBuilder str = new StringBuilder();
        str.append("Static Fields:\n");
        for (RSField field : staticfields) {
            str.append('\t').append(field).append('\n');
        }
        str.append('\n');
        for (RSClass clazz : classes) {
            str.append(clazz).append('\n');
        }
        return str.toString();
    }

    public static String pascal_gpl =
        "{** \n" +
        " *  Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100)\n" +
        " *  \n" +
        " *  This file is part of the SMART Minimizing Autoing Resource Thing (SMART)\n" +
        " *  \n" +
        " *  SMART is free software: you can redistribute it and/or modify\n" +
        " *  it under the terms of the GNU General Public License as published by\n" +
        " *  the Free Software Foundation, either version 3 of the License, or\n" +
        " *  (at your option) any later version.\n" +
        " *  \n" +
        " *  SMART is distributed in the hope that it will be useful,\n" +
        " *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n" +
        " *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n" +
        " *  GNU General Public License for more details.\n" +
        " *  \n" +
        " *  You should have received a copy of the GNU General Public License\n" +
        " *  along with SMART. If not, see <http://www.gnu.org/licenses/>.\n" +
        " *} \n\n";

}
