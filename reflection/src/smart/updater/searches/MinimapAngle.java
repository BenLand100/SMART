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

package smart.updater.searches;

import com.sun.org.apache.bcel.internal.classfile.Method;
import com.sun.org.apache.bcel.internal.generic.ClassGen;
import com.sun.org.apache.bcel.internal.generic.ConstantPoolGen;
import com.sun.org.apache.bcel.internal.generic.GETSTATIC;
import com.sun.org.apache.bcel.internal.generic.InstructionHandle;
import com.sun.org.apache.bcel.internal.generic.InstructionList;
import com.sun.org.apache.bcel.internal.generic.MethodGen;
import com.sun.org.apache.bcel.internal.util.InstructionFinder;
import java.util.HashMap;
import java.util.Iterator;
import smart.updater.RSClient;
import smart.updater.Search;
import smart.updater.Search.SearchResult;

/**
 * Finds the MapAngle hook, and nothing else. This just happened to be an easy
 * accurate hook.
 *
 * @author benland100
 */
public class MinimapAngle extends Search {

    public SearchResult run(RSClient data, HashMap<String, ClassGen> classes) {
        for (ClassGen c : classes.values()) {
            ConstantPoolGen cpg = c.getConstantPool();
            if (cpg.lookupFloat(16384.0000f) != -1) {
                for (Method m : c.getMethods()) {
                    if (m.isStatic()) {
                        MethodGen gen = new MethodGen(m,c.getClassName(),cpg);
                        InstructionList il = gen.getInstructionList();
                        if (il == null) continue;
                        InstructionFinder f = new InstructionFinder(il);
                        Iterator e = f.search("GETSTATIC LDC FSUB PUTSTATIC");
                        if (e.hasNext()) {
                            InstructionHandle[] handles = (InstructionHandle[]) e.next();
                            data.addField("MapAngle", ((GETSTATIC)handles[0].getInstruction()).getClassName(cpg) + "." + ((GETSTATIC)handles[0].getInstruction()).getFieldName(cpg));
                            return SearchResult.Success;
                        }
                    }
                }
            }
        }
        return SearchResult.Failure;
    }
}
