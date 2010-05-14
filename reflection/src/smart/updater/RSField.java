package smart.updater;

/**
 *
 * @author benland100
 */
public class RSField {
    public String path;
    public String hookName;

    public RSField(String path, String hookName) {
        this.path = path;
        this.hookName = hookName;
    }

    public String toString() {
        return path + " => " + hookName;
    }

}
