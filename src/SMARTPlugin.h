#ifndef _SMART_PLUGIN
#define _SMART_PLUGIN


typedef void (*SMARTGetMousePos)(int &x, int &y);
typedef void (*SMARTSetCapture)(bool enabled);
 
typedef struct {
        int version;
        void *img,*dbg;
        int width, height;
        SMARTGetMousePos getMousePos;
        SMARTSetCapture setCapture;
} SMARTInfo;
 
typedef void (*SMARTPluginInit)(SMARTInfo *ptr);


#endif
