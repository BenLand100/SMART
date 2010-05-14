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
import java.util.Map.Entry;
import smart.updater.RSClient;
import smart.updater.Search;
import smart.updater.Search.SearchResult;

/**
 * Locates the Toolkit class and the static CurToolkit field
 *
 * @author benland100
 */
public class Toolkit extends Search {

    public SearchResult run(RSClient data, HashMap<String, ClassGen> classes) {
        for (Entry<String, ClassGen> c : classes.entrySet()) {
            for (Method m : c.getValue().getMethods()) {
                if (m.isStatic()) {
                    ConstantPoolGen cpg = c.getValue().getConstantPool();
                    MethodGen gen = new MethodGen(m,c.getValue().getClassName(),cpg);
                    InstructionList il = gen.getInstructionList();
                    if (il == null) continue;
                    InstructionFinder f = new InstructionFinder(il);
                    Iterator e = f.search("BASTORE GETSTATIC ILOAD ILOAD ILOAD ILOAD INVOKEVIRTUAL");
                    if (e.hasNext()) {
                        InstructionHandle[] handles = (InstructionHandle[]) e.next();
                        String curtoolkit = ((GETSTATIC)handles[1].getInstruction()).getClassName(cpg) + "." + ((GETSTATIC)handles[1].getInstruction()).getFieldName(cpg);
                        String toolkit = ((GETSTATIC)handles[1].getInstruction()).getSignature(cpg).replaceAll("L|;","");
                        data.addClass("Toolkit", toolkit);
                        data.addField("CurToolkit", curtoolkit);
                        return SearchResult.Success;
                    }
                }
            }
        }
        return SearchResult.Failure;
    }
}
