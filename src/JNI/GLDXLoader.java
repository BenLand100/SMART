package JNI;

import java.nio.IntBuffer;

public class GLDXLoader {

    public static boolean OpenGLLoaded = false;
    public static boolean DirectXLoaded = false;

    public static native void GetOpenGLBuffer(IntBuffer Buffer);

    public static native void GetOpenGLStaticBuffer(IntBuffer Buffer);

    public static native void GetDirectXBuffer(IntBuffer Buffer);

    public static native void GetDirectXStaticBuffer(IntBuffer Buffer);

    public static native void DrawMouse(int X, int Y, int R, int G, int B);
}