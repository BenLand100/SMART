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

{*******************************************************************************
TTile - equivlent to a TPoint, but signifies a global tile position in the 
        Runescape client. Not to be used for any other purpose.
*******************************************************************************}
type 
  TTile = TPoint;


{*******************************************************************************
function TrimHTML(s: string): string;
By: lordsaturn
Description: Removes the HTML junk from a string.
*******************************************************************************}
function TrimHTML(S : string): string;
var
  P, P2: integer;
begin
  P := Pos('<', S);
  P2 := Pos('>', S);
  Result := S;
  while (P <> 0) and (P2 <> 0) do
  begin
    Delete(Result, P, P2 - P + 1);
    P := Pos('<', Result);
    P2 := Pos('>', Result);
  end;
end;

{*******************************************************************************
procedure GetJavaString(javaString: integer; var pascalString: string; maxLength: integer);
By: BenLand100
Description: Returns a string from a javaString with a specified  maxLenght. Do 
             not let it overflow, bad things will happen.
*******************************************************************************}
function GetJavaString(javaString: integer; maxLength: integer): string;
Begin
  SetLength(result, maxLength);
  SetLength(result, SmartStringFromString(javaString, result));
End;

{*******************************************************************************
function GetMyPos: TTile;
By: BenLand100
Description: Returns the global tile position of your player.
*******************************************************************************}
function GetMyPos: TTile;
var
  me: Integer;
begin
  me := SmartGetFieldObject(0, hook_static_MyPlayer);
  Result.x := SmartGetFieldInt(0, hook_static_BaseX) + SmartGetFieldInt(me, hook_animable_PixelX) / 128;
  Result.y := SmartGetFieldInt(0, hook_static_BaseY) + SmartGetFieldInt(me, hook_animable_PixelY) / 128;
  SmartFreeObject(me);
end;

{*******************************************************************************
function GetMinimapAngleDeg: extended;
By: BenLand100
Description: Returns the current Map angle in degrees. North: 0 deg, increases
             in the counter clockwise direction
*******************************************************************************}
function GetMinimapAngleDeg: extended;
begin
  result:= SmartGetFieldFloat(0, hook_static_MapAngle) * 45.0 / 2048.0;
end;

{*******************************************************************************
function GetMinimapAngleRad: extended;
By: BenLand100
Description: Returns the current map angle in radians. North: 0 rad, increases
             in the counter clockwise direction
*******************************************************************************}
function GetMinimapAngleRad: extended;
begin
  result:= SmartGetFieldFloat(0, hook_static_MapAngle) * pi / 8192.0;
end;
