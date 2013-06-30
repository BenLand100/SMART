package JNI;

import java.io.File;
import java.util.ArrayList;

public class PluginLoader {

    private static PluginLoader instance = new PluginLoader();
    private static ArrayList<String> Plugins = new ArrayList<>();

    private PluginLoader() {
    }

    public static PluginLoader getInstance() {
        return instance;
    }

    public static void load(String Path) {
        File plugin = new File(Path.trim());
        String pluginName = plugin.getName().toLowerCase();

        if (plugin.isFile() && !Plugins.contains(pluginName)) {
            System.load(plugin.getAbsolutePath());
            Plugins.add(pluginName);
            //Main.getFrame().printMiniConsole("[b][color=blue]Loaded Plugin: [/color][color=green]" + plugin.getName() + "[/color][/b]");

            if (pluginName.contains("opengl32.dll")) {
                GLDXLoader.OpenGLLoaded = true;
            } else if (pluginName.contains("d3d9d.dll") || pluginName.contains("d3d11d.dll")) {
                GLDXLoader.DirectXLoaded = true;
            }
        } else {
            //Main.getFrame().printMiniConsole("[b][color=red]Failed To Load Plugin: [/color][color=black]" + plugin.getName() + "[/color][/b]");
        }
    }

    public static void loadAll(String[] Paths) {
        for (String Path : Paths) {
            load(Path);
        }
    }

    public static void loadAll(String PluginsPath, String[] Paths) {
        for (String Path : Paths) {
            load(PluginsPath + "/" + Path);
        }
    }
}