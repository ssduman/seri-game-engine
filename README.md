# Seri Game Engine #
* A 2D/3D OpenGL Game Engine. Started July, 2019 and developed from time to time.
## Dependencies: ##
* [GLFW](https://www.glfw.org)
* [GLEW](http://glew.sourceforge.net)
* [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)
* [glm](https://github.com/g-truc/glm)
* [freetype](https://www.freetype.org)
* [assimp](https://github.com/assimp/assimp)
* [ImGui](https://github.com/ocornut/imgui) (for editor)
## Run: ##
Create a file named *libraries* and copy dependencies into it. Click to solution, run and enjoy!
## Specifications for Maze: ##
* Solves the maze with iteration method and makes the path green if the mode activated
* Determine maze size ingame
* Program runs up to 2000x2000+ maze (around 8.000.000 blocks, takes too long to run and very slow)
* Detects whether if you solve the maze and keeps track of solving time using chronometer
* A light source is above the entrance
* Renders text on the screen 
* Quickly change textures, skyboxes or font
* Fonts: [En Bloc](https://vrtxrry.itch.io/en-bloc) and [DungeonFont](https://vrtxrry.itch.io/dungeonfont)
* [Maze generator algorithm](https://github.com/norvig/pytudes/blob/master/ipynb/Maze.ipynb)
## Control for Maze: ##
* Write the maze size (Format of maze size should be "*width***x***height*") and hit enter 
* Press 'C' for cheat mode on/off 
* If cheat mode is on, use arrow keys for speed up/down and also do not affect restarting (stay in same position)
* Press 'E' for escape path on/off 
* Press 'R' for restart, generates new maze, camera goes default starting point, resets chronometer
* Scroll up/down for adjusting ambient strength 

<table>
    <tr>
        <td align="center">
            <img src="https://github.com/ssduman/seri-game-engine/blob/master/images/user-input.png" alt="user-input" width="384" height="216">
            <br />
            <i> dimension of the maze <i>
        </td>
        <td align="center">
            <img src="https://github.com/ssduman/seri-game-engine/blob/master/images/passage-solution.png" alt="sol-path" width="384" height="216">
            <br />
            <i> exit of the maze <i>
        </td>
    </tr>
    <tr>
        <td align="center">
            <img src="https://github.com/ssduman/seri-game-engine/blob/master/images/editor.png" alt="sol-path" width="384" height="216">
            <br />
            <i> editor <i>
        </td>
        <td align="center">
            <img src="https://github.com/ssduman/seri-game-engine/blob/master/images/snake.png" alt="solved" width="384" height="216">
            <br />
            <i> snake <i>
        </td>
    </tr>
    <tr>
        <td align="center">
            <img src="https://github.com/ssduman/seri-game-engine/blob/master/images/tetris.png" alt="new-font" width="284" height="384">
            <br />
            <i> tetris <i>
        </td>
        <td align="center">
            <img src="https://github.com/ssduman/seri-game-engine/blob/master/images/mics.png" alt="solved-new-font" width="284" height="384">
            <br />
            <i> miscellaneous <i>
        </td>
    </tr>
</table>

<i> see images/ for more images <i>
