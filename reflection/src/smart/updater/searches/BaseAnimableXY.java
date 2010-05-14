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
import com.sun.org.apache.bcel.internal.generic.GETSTATIC;
import com.sun.org.apache.bcel.internal.generic.InstructionHandle;
import com.sun.org.apache.bcel.internal.generic.InstructionList;
import com.sun.org.apache.bcel.internal.generic.MethodGen;
import com.sun.org.apache.bcel.internal.generic.Type;
import com.sun.org.apache.bcel.internal.util.InstructionFinder;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map.Entry;
import smart.updater.RSClass;
import smart.updater.RSClient;
import smart.updater.Search;

/**
 * Finds the Animable, Player, and Character classes, and the non-static hooks
 * for the Animable class. Also finds the BaseX, BaseY, and MyPlayer static
 * hooks.
 *
 * @author benland100
 */
public class BaseAnimableXY extends Search {

    public SearchResult run(RSClient data, HashMap<String, ClassGen> classes) {
        for (Entry<String, ClassGen> c : classes.entrySet()) {
            for (Method m : c.getValue().getMethods()) {
                if (m.isStatic() && m.getReturnType() == Type.VOID) {
                    ConstantPoolGen cpg = c.getValue().getConstantPool();
                    MethodGen gen = new MethodGen(m, c.getValue().getClassName(), cpg);
                    InstructionList il = gen.getInstructionList();
                    if (il == null) {
                        continue;
                    }
                    InstructionFinder f = new InstructionFinder(il);
                    Iterator e = f.search("GETSTATIC GETFIELD ISTORE (GETSTATIC GETFIELD BIPUSH ISHR GETSTATIC IADD ISTORE)+");
                    if (e.hasNext()) {
                        InstructionHandle[] handles = (InstructionHandle[]) e.next();
                        String myplayer = ((GETSTATIC) handles[0].getInstruction()).getClassName(cpg) + "." + ((GETSTATIC) handles[0].getInstruction()).getFieldName(cpg);
                        String animable = ((GETFIELD) handles[1].getInstruction()).getClassName(cpg);
                        String plane = ((GETFIELD) handles[1].getInstruction()).getFieldName(cpg);
                        String animx = ((GETFIELD) handles[4].getInstruction()).getFieldName(cpg);
                        String animy = ((GETFIELD) handles[11].getInstruction()).getFieldName(cpg);
                        String basex = ((GETSTATIC) handles[7].getInstruction()).getClassName(cpg) + "." + ((GETSTATIC) handles[7].getInstruction()).getFieldName(cpg);
                        String basey = ((GETSTATIC) handles[14].getInstruction()).getClassName(cpg) + "." + ((GETSTATIC) handles[14].getInstruction()).getFieldName(cpg);
                        RSClass animableClass = data.addClass("Animable", animable);
                        animableClass.addField("PixelX", animx);
                        animableClass.addField("PixelY", animy);
                        animableClass.addField("Plane", plane);
                        data.addField("MyPlayer", myplayer);
                        data.addField("BaseX", basex);
                        data.addField("BaseY", basey);
                        String player = ((GETSTATIC) handles[0].getInstruction()).getType(cpg).getSignature().replaceAll("L|;", "");
                        data.addClass("Player", player);
                        data.addClass("Character", classes.get(player).getSuperclassName());
                        return SearchResult.Success;
                    }
                }
            }
        }
        return SearchResult.Failure;
    }
}
