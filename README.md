## Basic Renderer in C

Another dumb little project I made which may get mildly expanded on, but mostly exists as a tutorial on how to write a basic renderer using SDL and double buffering in C.

## What This Contains:

- [x] 2% AI Generated Bullshit. Triangles are fucking hard.
- [x] Buffers of adjustible size, and the Renderer struct
- [x] Safe API for creation and destruction of the renderer and buffers
- [x] Drawing of rectangles, triangles, circles, and single pixels.
- [x] Double buffering (drawing to the back, and flipping)
- [x] Simple display using SDL2

## What I Can Think of Adding:

- Gradient textures
- Z-buffering
- PNG based sprite loading
- Interactive elements (maybe, click based or keybind based events)

## Known Pitfalls:

- None that I've tracked. I'll go thru this later

## Why Did I Make This?

Another slow small project I decided to write when I felt the urge to code. A very basic software renderer written in C, my first whack at this, all in just over 200 semicolons.

I've been happy to say I'm steering away from using LLMs in my code. This entire thing was written with copilot turned off and chrome closed (though I did get pissed off with triangles and need the formula for that), and I couldn't be prouder with the result. After around 2 hours of work, I feel... strangely fulfilled. Half with the fact that I was able to complete it so fast, but also with the fact that I was able to reason through all of this without the help of the "know it all" machine.

This will likely receive updates in the coming weeks, as I plan to get back into the swing of things and get a good amount of the base for [Stick](https://github.com/gamerjamer43/Stick) done as well. Maybe I'll work this into Stick as a simple SDL hook slash game engine... who knows?

---

<div align="center">
    <sub style="margin:0">Made with 💓 by <a href="https://github.com/gamerjamer43">gamerjamer43</a></sub>
</div>