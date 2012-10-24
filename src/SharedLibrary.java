/**  © 2012, Brandon T. All Rights Reserved.
  *	
  *  This file is part of the GLHook Library.
  *  GLHook is free software: you can redistribute it and/or modify
  *  it under the terms of the GNU General Public License as published by
  *  the Free Software Foundation, either version 3 of the License, or
  *  (at your option) any later version.
  *
  *  GLHook is distributed in the hope that it will be useful,
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *  GNU General Public License for more details.
  *
  *  You should have received a copy of the GNU General Public License
  *  along with GLHook.  If not, see <http://www.gnu.org/licenses/>.
  */

package smart;

import java.awt.geom.AffineTransform;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;

class SharedLibrary {

    private ByteBuffer Buffer = null;
    private BufferedImage Image = null;
	public boolean LibraryLoaded = false;
    private static native void GetGLBuffer(IntBuffer Buffer);      
    private int ByteSize = 0, Width = 0, Height = 0, BitsPerPixel = 32;
    
    public SharedLibrary(int ImageWidth, int ImageHeight) {
        this.Width = ImageWidth; this.Height = ImageHeight;
        //ByteSize = ((ImageWidth * BitsPerPixel + 31) / 32) * 4 * ImageHeight;
		ByteSize = ImageWidth * ImageHeight * 4;
		Buffer = ByteBuffer.allocateDirect(ByteSize);
    }
    
    public void SetSize(int ImageWidth, int ImageHeight) {
        Buffer = null;
        this.Width = ImageWidth; this.Height = ImageHeight;
        //ByteSize = ((ImageWidth * BitsPerPixel + 31) / 32) * 4 * ImageHeight;
        ByteSize = ImageWidth * ImageHeight * 4;
		Buffer = ByteBuffer.allocateDirect(ByteSize);
    }
    
    public ByteBuffer GetBuffer() {
		Buffer.rewind();
        GetGLBuffer(Buffer.asIntBuffer());
        return Buffer;
    }
    
    public void GetBuffer(ByteBuffer Buffer) {
		Buffer.rewind();
        GetGLBuffer(Buffer.asIntBuffer());
    }
    
    public void GetBuffer(IntBuffer Buffer) {
		Buffer.rewind();
        GetGLBuffer(Buffer);
    }
    
    public BufferedImage GetImage() {
		Buffer.rewind();
        GetGLBuffer(Buffer.asIntBuffer());  
        return ImageFromByteBuffer(Buffer, Width, Height);
    }
    
    public BufferedImage GetImage(ByteBuffer Buffer) {
        GetGLBuffer(Buffer.asIntBuffer());
        Buffer.rewind();
        return ImageFromByteBuffer(Buffer, Width, Height);
    }
    
    public BufferedImage GetImage(IntBuffer Buffer) {
        GetGLBuffer(Buffer);
        Buffer.rewind();    
        this.Buffer.rewind();
        this.Buffer.asIntBuffer().put(Buffer);
        return ImageFromByteBuffer(this.Buffer, Width, Height);
    }
    
    private BufferedImage ImageFromBytes(byte[] Bytes, int Width, int Height) {
        Image = new BufferedImage(Width, Height, BufferedImage.TYPE_INT_BGR);
        WritableRaster raster = (WritableRaster) Image.getData();
        raster.setPixels(0, 0, Width, Height, ByteBufferToIntBuffer(Bytes));
        Image.setData(raster);
        Image = VerticalFlip(Image);
        return Image;
    }
    
    private BufferedImage ImageFromByteBuffer(ByteBuffer ByteBuffer, int Width, int Height) {
        byte Bytes[] = new byte[ByteSize];
        ByteBuffer.get(Bytes);
        return ImageFromBytes(Bytes, Width, Height);
    }

    private int[] ByteBufferToIntBuffer(byte[] Data) {
        int IntBuffer[] = new int[Data.length];
       
        for (int I = 0; I < Data.length; I++) {
            IntBuffer[I] = (int)Data[I] & 0xFF;
        }
        return IntBuffer;
    }
    
    public BufferedImage VerticalFlip(BufferedImage imageIn) {
        AffineTransform tx = AffineTransform.getScaleInstance(1, -1);
        tx.translate(0, -Image.getHeight(null));
        AffineTransformOp op = new AffineTransformOp(tx, AffineTransformOp.TYPE_NEAREST_NEIGHBOR);
        imageIn = op.filter(imageIn, null);
        return imageIn;
    }

    public BufferedImage HorizontalFlip(BufferedImage imageIn) {
        AffineTransform tx = AffineTransform.getScaleInstance(-1, 1);
        tx.translate(-Image.getHeight(null), 0);
        AffineTransformOp op = new AffineTransformOp(tx, AffineTransformOp.TYPE_NEAREST_NEIGHBOR);
        imageIn = op.filter(imageIn, null);
        return imageIn;
    }
    
    public BufferedImage AxisFlip(BufferedImage imageIn) {
        AffineTransform tx = AffineTransform.getScaleInstance(-1, -1);
        tx.translate(-Image.getWidth(null), -Image.getWidth(null));
        AffineTransformOp op = new AffineTransformOp(tx, AffineTransformOp.TYPE_NEAREST_NEIGHBOR);
        imageIn = op.filter(imageIn, null);
        return imageIn;
    }
}