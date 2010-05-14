{** 
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
 *} 

{==============================================================================
This file relates to Map and Screen locations and converting them to Tiles. The
methods and formulas here relate to hooks into the Runescape Client's rendering 
system, and therefore a brief description about how those work will help a lot.

The Runescape world is laid out on a large grid. A coordinate on this grid is 
called a Tile. The origin (0,0) is somewhere in the South Western corner of the
world, and the coordinates increase in the North Easternly direction. 

A loading area is a smaller grid 104x104 tiles in size whose bounds can be
variable. The origin of the loading area is stored in the client as the global
tile position (BaseX, BaseY), therefore your local Tile position is your global
Tile position minux the origin of the loading area. 

A tile is rendered as 128x128 pixels. Most positions in the renderer data are 
measured in pixels. This means the loading area is 13312x13312 pixels in size.
to convert a local Tile into a pixel measurement, simply multiply the local Tile
coordinates by 128.

Each corner of a tile has a height stored in a 2D array in the current Plane 
object. To calculate the average height of a tile, simply average the height of 
its corners. 

The client provides a rotation matrix, offset, and scaling values to transform
a "world" pixel coordinate into a "screen" coordinate. The formula for doing 
this is found in the TileToMSEx function. Since one often requires sub tile 
accuracy, offset values in the range of [0,1] are provided. (0.5,0.5) would be 
the center, and (0,0) would be the south-west corner of the tile.

The minimap is simply a perfectly verticle view of the ground as if it were 
perfectly flat. Tiles here have a size of 4x4 pixels, and the map is rotated
around your current position by an angle stored in MapAngle. The angle is in
the range [0,16384], with 0 being North and increasing counter-clockwise. The
math for converting a Tile into a "minimap" coordinate is provided in TileToMM. 
==============================================================================}

{*******************************************************************************
TRenderData - Stores the rotation matrix used by the client to render to 
              the main screen, along with offsets.
TRender - Stores information about the rendered world including ranges 
          for acceptable values, and scaling factors.
*******************************************************************************}
type
  TRenderData = record
    xOff,xX,xY,xZ : extended;
    yOff,yX,yY,yZ : extended;
    zOff,zX,zY,zZ : extended;
  end;
  TRender = record
    xScale,yScale : extended;
    xMin,xMax : extended;
    yMin,ymax : extended;
    zMax,zMin : extended;
  end;
  
{*******************************************************************************
procedure UpdateRenderInfo(var Render : TRender; var RenderData : TRenderData);
By: ???
Description: Fills a pair of TRender and TRenderData structures with data. 
*******************************************************************************}
procedure UpdateRenderInfo(var Render : TRender; var RenderData : TRenderData);
var
  tk, vp: Integer;
begin
  tk := SmartGetFieldObject(0, hook_static_Toolkit);
  vp := SmartGetFieldObject(tk, hook_toolkit_Viewport);
  with RenderData do
  begin
    xOff := SmartGetFieldInt(vp,hook_viewport_xOff);
    xX :=SmartGetFieldInt(vp,hook_viewport_xX);
    xY := SmartGetFieldInt(vp,hook_viewport_xY);
    xZ := SmartGetFieldInt(vp,hook_viewport_xZ);
	yOff := SmartGetFieldInt(vp,hook_viewport_yOff);
    yX := SmartGetFieldInt(vp,hook_viewport_yX);
    yY := SmartGetFieldInt(vp,hook_viewport_yY);
    yZ := SmartGetFieldInt(vp,hook_viewport_yZ);
	zOff := SmartGetFieldInt(vp,hook_viewport_zoff);
    zX := SmartGetFieldInt(vp,hook_viewport_zX);
    zY := SmartGetFieldInt(vp,hook_viewport_zY);
    zZ := SmartGetFieldInt(vp,hook_viewport_zZ);
  end;
  with Render do
  begin
	xScale := SmartGetFieldInt(tk, hook_toolkit_xScale);
    yScale := SmartGetFieldInt(tk, hook_toolkit_yScale);
	xMin := SmartGetFieldInt(tk, hook_toolkit_xMin);
    xMax := SmartGetFieldInt(tk, hook_toolkit_xMax);
	yMin := SmartGetFieldInt(tk, hook_toolkit_yMin);
    yMax := SmartGetFieldInt(tk, hook_toolkit_yMax);
	zMin := SmartGetFieldInt(tk, hook_toolkit_zMin);
    zMax := SmartGetFieldInt(tk, hook_toolkit_zMax);
  end;
  SmartFreeObject(tk);
  SmartFreeObject(vp);
end;

{*******************************************************************************
function GetTileHeight(tile: TTile) : integer;
By: BenLand100
Description: finds the average height of the global tile provided the tile is in
             the current loading area and on a loaded plane.
*******************************************************************************}
function GetTileHeight(tile: TTile) : integer;
var
  x, y, CurPlane, PlaneInstance : integer;
begin
  x:= tile.x - SmartGetFieldInt(0,hook_static_BaseX);
  y:= tile.y - SmartGetFieldInt(0,hook_static_BaseY);
  if ((x < 0) or (x > 104) or (y < 0) or (y > 104)) then
  begin
    result := 0;
    exit;
  end;
  CurPlane:= SmartGetFieldInt(0,hook_static_LoadedPlane);
  if ((CurPlane < 3) and ((SmartGetFieldArray3DByte(0,hook_static_GroundSettingsArray,1,x,y) and 2) <> 0)) then
    CurPlane:= CurPlane + 1;
  PlaneInstance := SmartGetFieldArrayObject(0,hook_static_PlaneArray,CurPlane);
  result:= (SmartGetFieldArray2DInt(PlaneInstance,hook_plane_TileHeights,x,y) +
            SmartGetFieldArray2DInt(PlaneInstance,hook_plane_TileHeights,x + 1,y) +
            SmartGetFieldArray2DInt(PlaneInstance,hook_plane_TileHeights,x,y + 1) +
            SmartGetFieldArray2DInt(PlaneInstance,hook_plane_TileHeights,x + 1,y + 1)) / 4;
  writeln(inttostr(result));
end;


{*******************************************************************************
function TileToMM(tile: TTile): TPoint;
By: BenLand100
Description: Converts the global tile position to a screen location on the 
             minimap, taking map rotation into account.
*******************************************************************************}
function TileToMM(tile: TTile): TPoint;
var
  angle, x, y: extended;
  temp: TPoint;
begin
   angle:= -GetMinimapAngleRad();
   temp := GetMyPos();
   x:= (tile.x - temp.x) * 4 - 2;
   y:= (temp.y - tile.y) * 4 - 2;
   result.x:= round(x*cos(angle) + y*sin(angle)) + 628;
   result.y:= round(y*cos(angle) - x*sin(angle)) + 87;
end;

{*******************************************************************************
function TileToMSEx(tile: TTile; offx, offy : extended; height : integer): TPoint;
By: BenLand100
Description: Converts the global tile position, a position on that tile in the 
             range [(0,0), (1,1)] with (0.5, 0.5) being center and a height above
             ground level to a screen location on the main screen, taking camera 
             position and rotation into account.
*******************************************************************************}
function TileToMSEx(tile: TTile; offx, offy : extended; height : integer) : TPoint;
var
  rd : TRenderData;
  r : TRender;
  pixelX, pixelY, pixelZ, x, y, z: extended;
begin;
  result:= point(-1,-1);
  UpdateRenderInfo(r,rd);
  pixelX:= (tile.x - SmartGetFieldInt(0,hook_static_BaseX) + offX) * 128.0;
  pixelY:= GetTileHeight(tile) - height;
  pixelZ:= (tile.y - SmartGetFieldInt(0,hook_static_BaseY) + offY) * 128.0;
  z:= rd.zOff + ((rd.zX * pixelX + rd.zY * pixelY + rd.zZ * pixelZ) / 32768.0);
  if ((z < r.zMin) or (z > r.zMax)) then
    exit;
  x:= r.xScale * (rd.xOff + ((rd.xX * pixelX + rd.xY * pixelY + rd.xZ * pixelZ) / 32768.0)) / z;
  y:= r.yScale * (rd.yOff + ((rd.yX * pixelX + rd.yY * pixelY + rd.yZ * pixelZ) / 32768.0)) / z;
  if ((x >= r.xMin) and (x <= r.xMax) and (y >= r.yMin) and (y <= r.yMax)) then
  begin
    result.x:= Round(x - r.xMin);
    result.y:= Round(y - r.yMin);
  end else
  begin
    //Because sometimes the renderer is doing funny stuff, a single repeat will
    //often fix this error, since it should work if it got this far.
    result:= TileToMSEx(tile,offx,offy,height);
  end;
end;

{*******************************************************************************
function TileToMS(tile: TTile; height : integer): TPoint;
By: BenLand100
Description: Converts the center of a global tile position and a height above
             ground level to a screen location on the main screen, taking camera 
             position and rotation into account.
*******************************************************************************}
function TileToMS(tile: TTile; height : integer) : TPoint;
begin
  result:= TileToMSEx(tile, 0.5, 0.5, height);
end;
