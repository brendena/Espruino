var g = Graphics.createArrayBuffer(32,32,8);
g.dump = _=>{
  var s = "";
  var b = new Uint8Array(g.buffer);
  var n = 0;
  for (var y=0;y<g.getHeight();y++) {
    s+="\n";
    for (var x=0;x<g.getWidth();x++) 
      s+=".#"[b[n++]?1:0];
  }
  return s;
}
g.print = _=>{
  print("`"+g.dump()+"`");
}
var ok = true;
function SHOULD_BE(a) {
  var b = g.dump();
  if (a!=b) {
    console.log("GOT :"+b+"\nSHOULD BE:"+a+"\n================");
    ok = false;
  }
}

var img = {
  width : 8, height : 8, bpp : 8,
  transparent : 0,
  buffer : new Uint8Array([
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,
  ]).buffer
};

g.clear();
g.drawImage(img,16,16,{scale:2,rotate:Math.PI/4});
//g.print();
SHOULD_BE(`
................................
................................
................................
................................
................#...............
...............###..............
..............#####.............
.............#######............
............###.#####...........
...........###...#####..........
..........###.....#####.........
.........###.......#####........
........###.........#####.......
.......###...........#####......
......###.............#####.....
......##...............####.....
.......##...............##......
........##.............##.......
.........##...........##........
..........##.........##.........
...........##.......##..........
............##.....##...........
.............##...##............
..............##.##.............
...............###..............
................#...............
................................
................................
................................
................................
................................
................................`);

result = ok;
