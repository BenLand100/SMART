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

import com.sun.org.apache.bcel.internal.generic.ClassGen;
import java.util.HashMap;

/**
 * This is the superclass of all bytecode searches. To make a new search, extend
 * this class, place it in the proper package, and add it to Updater's list of
 * searches to run.
 *
 * @author benland100
 */
public abstract class Search {

    public enum SearchResult { MissingInfo, Failure, Success };

    /**
     *
     * @param data RSClient storing found data
     * @param classes HashMap of avaliable classes
     * @return Result of the run. MissingInfo results will be deferred until
     *         information is avaliable
     */
    public abstract SearchResult run(RSClient data, HashMap<String,ClassGen> classes);

}
