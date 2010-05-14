program New;
{.include Reflection.pas}

var
   x,y: integer;
   tile: TTile;
   
begin
  SmartSetup('http://world19.runescape.com/', 'plugin.js?param=0,a1,m0', 765,503);
  for x:= -2 to 2 do
  begin
    for y:= -2 to 2 do
    begin
      tile:= GetMyPos;
      tile.x:= tile.x + x;
      tile.y:= tile.y + y;
      tile:= TileToMS(tile,0);
      writeln('(' + inttostr(tile.x) + ',' + inttostr(tile.y) + ')');
      MoveMouse(tile.x,tile.y);
      Wait(500);
    end;
  end;
end.
