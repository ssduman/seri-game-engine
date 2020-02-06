# Maze #
* A simple 3D OpenGL game I wrote for fun. I started July, 2019 and developed from time to time. You can find older version here.
## Specifications: ##
* Solves the maze with iteration method and makes the path green if the mode activated
* Determine maze size ingame
* Program runs up to 2000x2000+ maze (around 8.000.000 blocks, takes too long to run and very slow)
* Detects whether if you solve the maze and keeps track of solving time using chronometer
* A light source is above the entrance
* Renders text on the screen 
* Quickly change textures, skyboxes or font
* Maze generator algorithm from: https://github.com/norvig/pytudes/blob/master/ipynb/Maze.ipynb
## Dependencies: ##
* <a href="https://www.glfw.org"> GLFW </a> 
* <a href="http://glew.sourceforge.net"> GLEW </a> 
* <a href="https://github.com/nothings/stb/blob/master/stb_image.h"> stb_image </a> 
* <a href="https://github.com/g-truc/glm"> glm </a> 
* <a href="https://www.freetype.org/"> freetype </a> 
## Run: ##
Create a file named *libraries* and copy dependencies into it. Click to solution (if needed, change solution platform) and 
build. Make sure create a file named *fonts* and copy any font into it named *arial.ttf*. Next, copy *shaders/*, *textures/*,
*fonts/* and *freetype.dll* next to the *Maze.exe*, run and enjoy!
## Control: ##
* Write the maze size (Format of maze size should be "*width***x***height*") and hit enter 
* Press 'C' for cheat mode on/off 
* If cheat mode is on, use arrow keys for speed up/down and also do not affect restarting (stay in same position)
* Press 'E' for escape path on/off 
* Press 'R' for restart, generates new maze, camera goes default starting point, resets chronometer
* Scroll up/down for adjusting ambient strength 

<table>
    <tr>
        <td align="center">
            <img src="https://github.com/perought/maze-game/blob/master/test/user-input.png" alt="user-input" width="384" height="216">
            <br />
            <i> give maze size, a 20x20 maze is moving circularly <i>
        </td>
        <td align="center">
            <img src="https://github.com/perought/maze-game/blob/master/test/start.png" alt="starting" width="384" height="216">
            <br />
            <i> starting point <i>
        </td>
    </tr>
    <tr>
        <td align="center">
            <img src="https://github.com/perought/maze-game/blob/master/test/passage-solution.png" alt="sol-path" width="384" height="216">
            <br />
            <i> exit way <i>
        </td>
        <td align="center">
            <img src="https://github.com/perought/maze-game/blob/master/test/solved.png" alt="solved" width="384" height="216">
            <br />
            <i> reached exit <i>
        </td>
    </tr>
</table>

<i> see test/ for more images <i>
