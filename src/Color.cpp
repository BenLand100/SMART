/**
 *  Copyright 2010 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of the Scar Minimizing Autoing Resource Thing (SMART)
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

#include "Color.h"
#include "Smart.h"

PRGB target = 0;
int width = 0, height = 0;
int cmpType = 1;
double huemod = 0.2;
double satmod = 0.2;

//Defined in Smart.cpp
extern PRGB image;
extern int client_width;
extern int client_height;

/*BITMAP bmpdata(int hdc) {
    HBITMAP hbitmap = (HBITMAP) GetCurrentObject((HDC)hdc, OBJ_BITMAP);
    BITMAP bmp;
    GetObject((HBITMAP)hbitmap, sizeof(BITMAP), &bmp);
    if (bmp.bmBits == NULL) throw 1;
    return bmp;
}*/

int setTarget(PRGB newTarget, int w, int h) {
    int last = (int) target;
    if (newTarget == 0) {
        target = (PRGB) image;
        width = client_width;
        height = client_height;
    } else {
        target = newTarget;
        width = w;
        height = h;
    }
    return last;
}

HSL toHSL(RGB& rgb) {
    double r, g, b;
    double max, min;
    HSL res;
    r = (double)(rgb.r / 255);
    g = (double)(rgb.g / 255);
    b = (double)(rgb.b / 255);
    max = r > b ? r > g ? r : g : b > g ? b : g;
    min = r < b ? r < g ? r : g : b < g ? b : g;
    res.lum = 0.5 * (max + min);
    if (max == min) {
        res.hue = 0;
    } if (max == r) {
        if (g >= b) {
            res.hue = 60 * ((g - b) / (max - min));
        } else {
            res.hue = 60 * ((g - b) / (max - min)) + 360;
        }
    } else if (max == g) {
        res.hue = 60 * ((b - r) / (max - min)) + 120;
    } else {
        res.hue = 60 * ((r - g) / (max - min)) + 240;
    }
    if (res.lum == 0 || max == min) {
        res.sat = 0;
    } else if (res.lum > 0.5) {
        res.sat = (max - min) / (max + min);
    } else {
        res.sat = (max - min) / (2.0 - max + min);
    }
    return res;
}

inline int idist(int a, int b) {
    return a < b ? b - a : a - b;
}

inline float fdist(float a, float b) {
    return a < b ? b - a : a - b;
}

//use function pointers and make these into three methods... switch slows it 
//down like nobody's buisness...
bool cmpColors(RGB& x, RGB& y, int tol) {
    switch (cmpType) {
        case 0:
            if (tol < idist(x.r, y.r)) return false;
            if (tol < idist(x.g, y.g)) return false;
            if (tol < idist(x.b, y.b)) return false;
            return true;
        case 1:
            int r, g, b;
            r = x.r - y.r;
            g = x.g - y.g;
            b = x.b - y.b;
            return tol*tol >= r*r+g*g+b*b;
        case 2:
            HSL hx, hy;
            double hue = (double)tol * huemod;
            double sat = (double)tol * satmod;
            hx = toHSL(x);
            hy = toHSL(y);
            return fdist(hx.hue, hy.hue) <= hue && fdist(hx.sat, hy.sat) <= sat;
    }
    return false;
}

int getColor(int x, int y) {
    RGB c = target[y*width+x];
    return ((c.b & 0xFF) << 16) | ((c.g & 0xFF) << 8) | (c.r & 0xFF);
}

bool findColor(int& x, int& y, int color, int sx, int sy, int ex, int ey) {
    sy *= width;
    ey *= width;
    for (int offset = sy; offset <= ey; offset += width) {
        int end = ex + offset;
        for (int i = sx + offset; i <= end; i++) {
            if (target[i].color == color) {
                x = i % width;
                y = i / width;
                return true;
            }
        }
    }
    x = -1;
    y = -1;
    return false;
}


bool findColorTol(int& x, int& y, int color, int sx, int sy, int ex, int ey, int tol) {
    RGB find;
    find.color = color;
    sy *= width;
    ey *= width;
    for (int offset = sy; offset <= ey; offset += width) {
        int end = ex + offset;
        for (int i = sx + offset; i <= end; i++) {
            if (cmpColors(target[i], find, tol)) {
                x = i % width;
                y = i / width;
                return true;
            }
        }
    }
    x = -1;
    y = -1;
    return false;
}

bool findColorSpiral(int& x, int& y, int color, int sx, int sy, int ex, int ey) {
    OutOfBoundsCheck(x, y, sx, sy, ex, ey);
    int cx = x, cy = y;
    char dir = 0; //0-left; 1-down; 2-right; 3-up
    int layer = 0;
    bool ul = false, ur = false, dl = false, dr = false;
    Loop:
        if (target[cy*width+cx].color == color) {
            x = cx;
            y = cy;
            return true;
        }
        if (cx == sx && cy == sy) ul = true;
        if (cx == ex && cy == sy) ur = true;
        if (cx == sx && cy == ey) dl = true;
        if (cx == ex && cy == ey) dr = true;
        if (ul && ur && dl && dr) {
            return false;
        }
        switch (dir) {
            case 0:
                if (cx == x + 1 + layer) {
                    layer++;
                    dir = 1;
                    break;
                } else if (cx == ex && cx != x + layer) {
                    goto SkipLoop;
                }
                break;
            case 1:
                if (cy == y + layer) {
                    dir = 2;
                    break;
                } else if (cy == ey) {
                    goto SkipLoop;
                }
                break;
            case 2:
                if (cx == x - layer) {
                    dir = 3;
                    break;
                } else if (cx == sx) {
                    goto SkipLoop;
                }
                break;
            case 3:
                if (cy == y - layer) {
                    dir = 0;
                    break;
                } else if (cy == sy) {
                    goto SkipLoop;
                }
                break;
        }
        switch (dir) {
            case 0: cx++; break;
            case 1: cy++; break;
            case 2: cx--; break;
            case 3: cy--; break;
        }
        goto Loop;
        SkipLoop:
            if (dir == 3) dir = 0; else dir++;
            switch (dir) { //Its been incremented
                case 1:
                    if (dr) {
                        goto SkipLoop;
                    } else {
                        cx = ex;
                        cy = y + layer;
                    }
                    break;
                case 2:
                    if (dl) {
                        goto SkipLoop;
                    } else {
                        cx = x - layer;
                        cy = ey;
                    }
                    break;
                case 3:
                    if (ul) {
                        goto SkipLoop;
                    } else {
                        cx = sx;
                        cy = y - layer;
                    }
                    break;
                case 0:
                    if (ur) {
                        layer++;
                        goto SkipLoop;
                    } else {
                        layer++;
                        cx = x + layer;
                        cy = sy;
                    }
                    break;
            }
            if (dir == 3) dir = 0; else dir++;
            goto Loop;
}

bool findColorSpiralTol(int& x, int& y, int color, int sx, int sy, int ex, int ey, int tol) {
    OutOfBoundsCheck(x, y, sx, sy, ex, ey);
    RGB find;
    find.color = color;
    int cx = x, cy = y;
    char dir = 0; //0-left; 1-down; 2-right; 3-up
    int layer = 0;
    bool ul = false, ur = false, dl = false, dr = false;
    Loop:
        if (cmpColors(target[cy*width+cx], find, tol)) {
            x = cx;
            y = cy;
            return true;
        }
        if (cx == sx && cy == sy) ul = true;
        if (cx == ex && cy == sy) ur = true;
        if (cx == sx && cy == ey) dl = true;
        if (cx == ex && cy == ey) dr = true;
        if (ul && ur && dl && dr) {
            return false;
        }
        switch (dir) {
            case 0:
                if (cx == x + 1 + layer) {
                    layer++;
                    dir = 1;
                    break;
                } else if (cx == ex && cx != x + layer) {
                    goto SkipLoop;
                }
                break;
            case 1:
                if (cy == y + layer) {
                    dir = 2;
                    break;
                } else if (cy == ey) {
                    goto SkipLoop;
                }
                break;
            case 2:
                if (cx == x - layer) {
                    dir = 3;
                    break;
                } else if (cx == sx) {
                    goto SkipLoop;
                }
                break;
            case 3:
                if (cy == y - layer) {
                    dir = 0;
                    break;
                } else if (cy == sy) {
                    goto SkipLoop;
                }
                break;
        }
        switch (dir) {
            case 0: cx++; break;
            case 1: cy++; break;
            case 2: cx--; break;
            case 3: cy--; break;
        }
        goto Loop;
        SkipLoop:
            if (dir == 3) dir = 0; else dir++;
            switch (dir) { //Its been incremented
                case 1:
                    if (dr) {
                        goto SkipLoop;
                    } else {
                        cx = ex;
                        cy = y + layer;
                    }
                    break;
                case 2:
                    if (dl) {
                        goto SkipLoop;
                    } else {
                        cx = x - layer;
                        cy = ey;
                    }
                    break;
                case 3:
                    if (ul) {
                        goto SkipLoop;
                    } else {
                        cx = sx;
                        cy = y - layer;
                    }
                    break;
                case 0:
                    if (ur) {
                        layer++;
                        goto SkipLoop;
                    } else {
                        layer++;
                        cx = x + layer;
                        cy = sy;
                    }
                    break;
            }
            if (dir == 3) dir = 0; else dir++;
            goto Loop;
}

/*
bool findBitmapTolIn(int hdc, int& x, int& y, int sx, int sy, int ex, int ey, int tol) {
    BITMAP bmp = bmpdata(hdc);
    PRGB bpx = (PRGB) bmp.bmBits;
    int bh = bmp.bmHeight;
    int bw = bmp.bmWidth;
    int scan = width - bw;
    int last = bh * bw - 1;
    int scanlast = (bh - 1) * width + bw - 1;
    ey -= bh;
    ex -= bw;
    sy *= width;
    ey *= width;
    for (int offset = sy; offset <= ey; offset += width) {
        int end = ex + offset;
        for (int i = sx + offset; i <= end; i++) {
            if (cmpColors(bpx[0], target[i], tol) && cmpColors(bpx[last], target[i+scanlast], tol)) {
                int imginc = i;
                int bmpinc = 0;
                for (int yinc = 0; yinc < bh; yinc++) {
                    for (int xinc = 0; xinc < bw; xinc++) {
                        if (!cmpColors(bpx[bmpinc++], target[imginc++], tol)) goto Skip;
                    }
                    imginc += scan;
                }
                x = i % width;
                y = i / width;
                return true;
                Skip:
                continue;
            }
        }
    }
    x = -1;
    y = -1;
    return false;
}

bool findBitmapIn(int hdc, int& x, int& y, int sx, int sy, int ex, int ey) {
    BITMAP bmp = bmpdata(hdc);
    int* bpx = (int*) bmp.bmBits;
    int bh = bmp.bmHeight;
    int bw = bmp.bmWidth;
    int scan = width - bw;
    int last = bh * bw - 1;
    int scanlast = (bh - 1) * width + bw - 1;
    ey -= bh;
    ex -= bw;
    sy *= width;
    ey *= width;
    for (int offset = sy; offset <= ey; offset += width) {
        int end = ex + offset;
        for (int i = sx + offset; i <= end; i++) {
            if (bpx[0] == target[i].color && bpx[last] == target[i+scanlast].color) {
                int imginc = i;
                int bmpinc = 0;
                for (int yinc = 0; yinc < bh; yinc++) {
                    for (int xinc = 0; xinc < bw; xinc++) {
                        if (bpx[bmpinc++] != target[imginc++].color) goto Skip;
                    }
                    imginc += scan;
                }
                x = i % width;
                y = i / width;
                return true;
                Skip:			
                continue;
            }
        }
    }
    x = -1;
    y = -1;
    return false;
}

bool findBitmap(int hdc, int& x, int& y) {
    return findBitmapIn(hdc, x, y, 0, 0, (width - 1), (height - 1));
}
*/
