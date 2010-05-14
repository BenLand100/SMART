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
 * Represents a class in the Client and its non-static fields. Static fields
 * are not refrenced by class, but rather are global and thus not stored here.
 *
 * @author benland100
 */
public class RSClass {
    public String className;
    public String hookName;
    public LinkedList<RSField> fields = new LinkedList<RSField>();

    /**
     * Constructs a new RSClass object given its client and hook names.
     *
     * @param name Obfscuated client name
     * @param hookName Deobfuscated hook name
     */
    public RSClass(String name, String hookName) {
        this.className = name;
        this.hookName = hookName;
    }

    /**
     * Adds a field to this class given its hook name and path, provided it
     * does not exist by that path.
     *
     * @param properName Name of the hook
     * @param path Path for the static field
     * @return RSField object representing this field
     */
    public RSField addField(String properName, String path) {
        for (RSField field : fields) {
            if (field.hookName.equals(path)) return field;
        }
        fields.add(new RSField(path,properName));
        return fields.getLast();
    }

    /**
     * Finds the RSField object for a field given its hook name.
     *
     * @param properName Name of the hook
     * @return RSField object, or null if not found
     */
    public RSField getProperField(String properName) {
        for (RSField field : fields) {
            if (field.hookName.equals(properName)) return field;
        }
        return null;
    }

    /**
     * Finds the RSField object for a field given its path.
     *
     * @param path Path for the field
     * @return RSField object, or null if not found
     */
    public RSField getPathField(String path) {
        for (RSField field : fields) {
            if (field.path.equals(path)) return field;
        }
        return null;
    }

    /**
     * Generates a string representation of this class containing all of the
     * hooked fields.
     *
     * @return String representation
     */
    public String toString() {
        StringBuilder str = new StringBuilder();
        str.append("Class ").append(className).append(" => ").append(hookName).append('\n');
        for (RSField field : fields) {
            str.append('\t').append(field).append('\n');
        }
        return str.toString();
    }

}
