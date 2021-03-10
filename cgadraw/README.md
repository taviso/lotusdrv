# cgadraw

This is a stripped down version of libcaca 0.99 used for adding drawing
primitives to the CGA framebuffer.

You might ask why not just use full libcaca, but it requires around `8 * width
* height` bytes for the canvas, plus another `width * height` to get the data
out of a canvas, and some management overhead.

If you want to add frames or any kind of transformation, double that again at
least.

That is a totally reasonable amount normally, but in a tiny display driver
it's really pushing the limits of what I can scratch together.
