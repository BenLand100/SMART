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
import com.sun.org.apache.bcel.internal.generic.GETFIELD;
import com.sun.org.apache.bcel.internal.generic.InstructionHandle;
import com.sun.org.apache.bcel.internal.generic.InstructionList;
import com.sun.org.apache.bcel.internal.generic.MethodGen;
import com.sun.org.apache.bcel.internal.util.InstructionFinder;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map.Entry;
import smart.updater.RSClass;
import smart.updater.RSClient;
import smart.updater.Search;
import smart.updater.Search.SearchResult;

/**
 * Finds the Viewpot and SDViewport classes and their non-static hooks
 *
 * @author benland100
 */
public class Viewport extends Search {

    public SearchResult run(RSClient data, HashMap<String, ClassGen> classes) {
        for (Entry<String, ClassGen> c : classes.entrySet()) {
            for (Method m : c.getValue().getMethods()) {
                if (!m.isStatic()) {
                    ConstantPoolGen cpg = c.getValue().getConstantPool();
                    MethodGen gen = new MethodGen(m,c.getValue().getClassName(),cpg);
                    InstructionList il = gen.getInstructionList();
                    if (il == null) continue;
                    InstructionFinder f = new InstructionFinder(il);
                    Iterator e = f.search("(ALOAD ICONST ALOAD GETFIELD ALOAD GETFIELD ILOAD IMUL ALOAD GETFIELD ILOAD IMUL IADD ALOAD GETFIELD ILOAD IMUL IADD BIPUSH ISHR IADD IASTORE)+");
                    if (e.hasNext()) {
                        InstructionHandle[] handles = (InstructionHandle[]) e.next();
                        String sdviewport = ((GETFIELD)handles[3].getInstruction()).getClassName(cpg);
                        if (!sdviewport.equals(c.getKey())) continue;
                        data.addClass("Viewport",classes.get(sdviewport).getSuperclassName());
                        RSClass sdviewportClass = data.addClass("SDViewport", sdviewport);
                        sdviewportClass.addField("xOff", ((GETFIELD)handles[3].getInstruction()).getFieldName(cpg));
                        sdviewportClass.addField("xX", ((GETFIELD)handles[5].getInstruction()).getFieldName(cpg));
                        sdviewportClass.addField("xY", ((GETFIELD)handles[9].getInstruction()).getFieldName(cpg));
                        sdviewportClass.addField("xZ", ((GETFIELD)handles[14].getInstruction()).getFieldName(cpg));
                        sdviewportClass.addField("yOff", ((GETFIELD)handles[25].getInstruction()).getFieldName(cpg));
                        sdviewportClass.addField("yX", ((GETFIELD)handles[27].getInstruction()).getFieldName(cpg));
                        sdviewportClass.addField("yY", ((GETFIELD)handles[31].getInstruction()).getFieldName(cpg));
                        sdviewportClass.addField("yZ", ((GETFIELD)handles[36].getInstruction()).getFieldName(cpg));
                        sdviewportClass.addField("zOff", ((GETFIELD)handles[47].getInstruction()).getFieldName(cpg));
                        sdviewportClass.addField("zX", ((GETFIELD)handles[49].getInstruction()).getFieldName(cpg));
                        sdviewportClass.addField("zY", ((GETFIELD)handles[53].getInstruction()).getFieldName(cpg));
                        sdviewportClass.addField("zZ", ((GETFIELD)handles[58].getInstruction()).getFieldName(cpg));
                        return SearchResult.Success;
                    }
                }
            }
        }
        return SearchResult.Failure;
    }
}