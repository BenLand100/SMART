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

import com.sun.org.apache.bcel.internal.classfile.Field;
import com.sun.org.apache.bcel.internal.classfile.Method;
import com.sun.org.apache.bcel.internal.generic.ClassGen;
import com.sun.org.apache.bcel.internal.generic.Type;
import com.sun.org.apache.bcel.internal.generic.ArrayType;
import com.sun.org.apache.bcel.internal.generic.ConstantPoolGen;
import com.sun.org.apache.bcel.internal.generic.GETFIELD;
import com.sun.org.apache.bcel.internal.generic.InstructionHandle;
import com.sun.org.apache.bcel.internal.generic.InstructionList;
import com.sun.org.apache.bcel.internal.generic.MethodGen;
import com.sun.org.apache.bcel.internal.util.InstructionFinder;
import java.util.HashMap;
import java.util.Iterator;
import smart.updater.RSClass;
import smart.updater.RSClient;
import smart.updater.Search;
import smart.updater.Search.SearchResult;

/**
 * Finishes up Toolkit by finding SDToolkit and its non static fields, then 
 * finds the SDPlane and its non static fields.
 *
 * @author benland100
 */
public class StandardDetail extends Search {

    public SearchResult run(RSClient data, HashMap<String, ClassGen> classes) {
        if (data.getProperClass("Toolkit") == null || data.getProperClass("Plane") == null || data.getProperClass("SDViewport") == null) {
            return SearchResult.MissingInfo;
        }
        return toolkit(data, classes) & plane(data, classes) ? SearchResult.Success : SearchResult.Failure;
    }

    private boolean toolkit(RSClient data, HashMap<String, ClassGen> classes) {
        String toolkit = data.getProperClass("Toolkit").className;
        classloop:
        for (ClassGen clazz : classes.values()) {
            if (clazz.getSuperclassName().equals(toolkit)) {
                for (Field field : clazz.getFields()) {
                    if (!field.isStatic() && field.getType() instanceof ArrayType && ((ArrayType) field.getType()).getBasicType() == Type.FLOAT) {
                        continue classloop;
                    }
                    if (!field.isStatic() && field.getName().equals("nativeid")) {
                        continue classloop;
                    }
                }
                RSClass sdtoolkit = data.addClass("SDToolkit", clazz.getClassName());
                for (Method m : clazz.getMethods()) {
                    if (!m.isStatic()) {
                        for (Type t : m.getArgumentTypes()) {
                            if (t instanceof ArrayType && ((ArrayType) t).getBasicType() == Type.INT) {
                                ConstantPoolGen cpg = clazz.getConstantPool();
                                MethodGen gen = new MethodGen(m, clazz.getClassName(), cpg);
                                InstructionList il = gen.getInstructionList();
                                if (il == null) {
                                    continue;
                                }
                                InstructionFinder f = new InstructionFinder(il);
                                Iterator e = f.search("ALOAD GETFIELD GETFIELD ALOAD GETFIELD GETFIELD");
                                if (e.hasNext()) {
                                    InstructionHandle[] handles = (InstructionHandle[]) e.next();
                                    String getviewport = ((GETFIELD) handles[1].getInstruction()).getFieldName(cpg);
                                    e = f.search("ILOAD ALOAD GETFIELD if_icmplt ILOAD ALOAD GETFIELD if_icmple", handles[handles.length - 1]);
                                    if (e.hasNext()) {
                                        handles = (InstructionHandle[]) e.next();
                                        String zmin = ((GETFIELD) handles[2].getInstruction()).getFieldName(cpg);
                                        String zmax = ((GETFIELD) handles[6].getInstruction()).getFieldName(cpg);
                                        e = f.search("ALOAD GETFIELD ALOAD GETFIELD GETFIELD ALOAD GETFIELD GETFIELD", handles[handles.length - 1]);
                                        if (e.hasNext()) {
                                            handles = (InstructionHandle[]) e.next();
                                            String xscale = ((GETFIELD) handles[1].getInstruction()).getFieldName(cpg);
                                            e = f.search("ALOAD GETFIELD ALOAD GETFIELD GETFIELD ALOAD GETFIELD GETFIELD", handles[handles.length - 1]);
                                            if (e.hasNext()) {
                                                handles = (InstructionHandle[]) e.next();
                                                String yscale = ((GETFIELD) handles[1].getInstruction()).getFieldName(cpg);
                                                e = f.search("ILOAD ALOAD GETFIELD if_icmplt ILOAD ALOAD GETFIELD if_icmpgt", handles[handles.length - 1]);
                                                if (e.hasNext()) {
                                                    handles = (InstructionHandle[]) e.next();
                                                    String xmin = ((GETFIELD) handles[2].getInstruction()).getFieldName(cpg);
                                                    String xmax = ((GETFIELD) handles[6].getInstruction()).getFieldName(cpg);
                                                    e = f.search("ILOAD ALOAD GETFIELD if_icmplt ILOAD ALOAD GETFIELD if_icmpgt", handles[handles.length - 1]);
                                                    if (e.hasNext()) {
                                                        handles = (InstructionHandle[]) e.next();
                                                        String ymin = ((GETFIELD) handles[2].getInstruction()).getFieldName(cpg);
                                                        String ymax = ((GETFIELD) handles[6].getInstruction()).getFieldName(cpg);
                                                        sdtoolkit.addField("xMin", xmin);
                                                        sdtoolkit.addField("xMax", xmax);
                                                        sdtoolkit.addField("yMin", ymin);
                                                        sdtoolkit.addField("yMax", ymax);
                                                        sdtoolkit.addField("zMin", zmin);
                                                        sdtoolkit.addField("zMax", zmax);
                                                        sdtoolkit.addField("xScale", xscale);
                                                        sdtoolkit.addField("yScale", yscale);
                                                        for (Field field : clazz.getFields()) {
                                                            if (!field.isStatic() && field.getSignature().contains(data.getProperClass("SDViewport").className)) {
                                                                sdtoolkit.addField("Viewport", field.getName());
                                                                return true;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    private boolean plane(RSClient data, HashMap<String, ClassGen> classes) {
        String plane = data.getProperClass("Plane").className;
        classloop: for (ClassGen clazz : classes.values()) {
            if (clazz.getSuperclassName().equals(plane)) {
                int twodintarrayhits = 0;
                Field groundheights = null;
                for (Field field : clazz.getFields()) {
                    if (!field.isStatic() && field.getType() instanceof ArrayType && ((ArrayType)field.getType()).getSignature().equals("[[[I")) {
                        continue classloop;
                    }
                    if (!field.isStatic() && field.getType() instanceof ArrayType && ((ArrayType)field.getType()).getSignature().equals("[[I")) {
                        twodintarrayhits++;
                        groundheights = field;
                    }
                }
                if (twodintarrayhits == 1) {
                    RSClass sdplane = data.addClass("SDPlane", clazz.getClassName());
                    sdplane.addField("GroundHeights", groundheights.getName());
                    return true;
                }
            }
        }
        return false;
    }
}
