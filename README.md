# AssaultCube External ESP

This is my first C++ project and my first attempt at making something in the game hacking space.

The goal was to create an external ESP (Extra Sensory Perception) for [AssaultCube](https://assault.cubers.net/), which shows enemy positions on screen.

![esp_test_small_small](https://github.com/user-attachments/assets/c25499b9-19d9-489b-a0ad-f31c22c317e8)

## What I learned

The toughest part was learning C++ itself — memory handling, structures, and all the little rules that come with the language.

I also had no prior experience with the Windows API, so figuring out how to draw overlays on screen was a steep climb.

Another big challenge was translating 3D game state (enemy/player positions, camera direction, etc.) into something that could be accurately drawn on a 2D screen.  
I had to read and understand how model, view, and projection matrices work in game rendering. I chose to read the model-view-projection matrix directly from the game memory, although technically it's possible to recalculate it by knowing a few parameters. I did try building it myself, but couldn’t get it quite right, so in the end, I just read it from memory.

## Code quality

I know the code might not be optimal, clean, or efficient in many places. The goal was to just **get something working**, as fast as possible, and learn by doing. This was more about understanding concepts than writing perfect code.

## Tools and Help

I used AI (like ChatGPT) here and there, especially for learning C++ best practices, refactoring ideas, and understanding tricky parts of Windows programming and rendering concepts.

## Disclaimer

This project is for **educational purposes only**. Please don’t use it to cheat in online games or ruin the experience for others.
