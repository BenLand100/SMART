package JNI;

public class Natives {

    public static final int PAGE_READONLY = 0x02;
    public static final int PAGE_READWRITE = 0x04;
    public static final int PAGE_WRITECOPY = 0x08;
    public static final int PAGE_EXECUTE_READ = 0x20;
    public static final int PAGE_EXECUTE_READWRITE = 0x40;
    public static final int PAGE_EXECUTE_WRITECOPY = 0x80;
    public static final int FILE_MAP_COPY = 0x0001;
    public static final int FILE_MAP_WRITE = 0x0002;
    public static final int FILE_MAP_READ = 0x0004;
    public static final int FILE_MAP_EXECUTE = 0x0020;
    public static final int FILE_MAP_ALL_ACCESS = 0xF001F;
    public static final int INVALID_HANDLE_VALUE = -1;

    public static native long createFileMapping(long hFile, int lpAttributes, int dwMaximumSizeHigh, int dwMaximumSizeLow, String lpName);

    public static native long openFileMapping(int dwDesiredAccess, boolean bInheritHandle, String lpName);

    public static native long mapViewOfFile(long hFileMap, int dwDesiredAccess, int dwFileOffsetHigh, int dwFileOffsetLow, int dwNumberOfBytesToMap);

    public static native long unmapViewOfFile(long hFileMap, long Size);

    public static native long closeHandle(long hFile);
    
    public static native void addToTaskBar(long WindowHandle);
    
    public static native void removeFromTaskBar(long WindowHandle);
    
    public static long getWindowHandle(java.awt.Frame frame) {
        return (Long)invokeMethod(invokeMethod(frame, "getPeer"), "getHWnd");
    }
    
    protected static Object invokeMethod(Object o, String methodName) {
        Class c = o.getClass();
        for (java.lang.reflect.Method m : c.getMethods()) {
            if (m.getName().equals(methodName)) {
                try {
                    return m.invoke(o);
                } catch (IllegalAccessException | IllegalArgumentException | java.lang.reflect.InvocationTargetException e) {
                    e.printStackTrace();
                    break;
                }
            }
        }
        return null;
    }
}
