/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "swipe",
  "params" : [["directionLR","int","`-1` for left, `1` for right, `0` for up/down"],
              ["directionUD","int","`-1` for up, `1` for down, `0` for left/right (Bangle.js 2 only)"]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"swipe\", callback: SwipeCallback): void;"
}
Emitted when a swipe on the touchscreen is detected (a movement from
left->right, right->left, down->up or up->down)

Bangle.js 1 is only capable of detecting left/right swipes as it only contains a
2 zone touchscreen.
*/
/*TYPESCRIPT
type TouchCallback = (button: number, xy?: { x: number, y: number }) => void;
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "touch",
  "params" : [
    ["button","int","`1` for left, `2` for right"],
    ["xy","JsVar","Object of form `{x,y,type}` containing touch coordinates (if the device supports full touch). Clipped to 0..175 (LCD pixel coordinates) on firmware 2v13 and later.`type` is only available on Bangle.js 2 and is an integer, either 0 for swift touches or 2 for longer ones."]
  ],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"touch\", callback: TouchCallback): void;"
}
Emitted when the touchscreen is pressed
*/
/*TYPESCRIPT
type DragCallback = (event: {
  x: number;
  y: number;
  dx: number;
  dy: number;
  b: 1 | 0;
}) => void;
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "drag",
  "params" : [["event","JsVar","Object of form `{x,y,dx,dy,b}` containing touch coordinates, difference in touch coordinates, and an integer `b` containing number of touch points (currently 1 or 0)"]],
  "ifdef" : "BANGLEJS",
  "typescript" : "on(event: \"drag\", callback: DragCallback): void;"
}
Emitted when the touchscreen is dragged or released

The touchscreen extends past the edge of the screen and while `x` and `y`
coordinates are arranged such that they align with the LCD's pixels, if your
finger goes towards the edge of the screen, `x` and `y` could end up larger than
175 (the screen's maximum pixel coordinates) or smaller than 0. Coordinates from
the `touch` event are clipped.
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "stroke",
  "params" : [["event","JsVar","Object of form `{xy:Uint8Array([x1,y1,x2,y2...])}` containing touch coordinates"]],
  "ifdef" : "BANGLEJS_Q3",
  "typescript" : "on(event: \"stroke\", callback: (event: { xy: Uint8Array, stroke?: string }) => void): void;"
}
Emitted when the touchscreen is dragged for a large enough distance to count as
a gesture.

If Bangle.strokes is defined and populated with data from `Unistroke.new`, the
`event` argument will also contain a `stroke` field containing the most closely
matching stroke name.

For example:

```
Bangle.strokes = {
  up : Unistroke.new(new Uint8Array([57, 151, ... 158, 137])),
  alpha : Unistroke.new(new Uint8Array([161, 55, ... 159, 161])),
};
Bangle.on('stroke',o=>{
  print(o.stroke);
  g.clear(1).drawPoly(o.xy);
});
// Might print something like
{
  "xy": new Uint8Array([149, 50, ... 107, 136]),
  "stroke": "alpha"
}
```
*/
/*JSON{
  "type" : "event",
  "class" : "Bangle",
  "name" : "midnight",
  "ifdef" : "BANGLEJS"
}
Emitted at midnight (at the point the `day` health info is reset to 0).

Can be used for housekeeping tasks that don't want to be run during the day.
*/




