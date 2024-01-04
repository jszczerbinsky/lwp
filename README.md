
<div align="center">

# Layered WallPaper

Layered WallPaper allows You to create multi-layered parallax wallpapers.<br />
Each layer moves with Your mouse cursor, creating this beautiful effect.

[![](https://img.shields.io/github/v/release/jszczerbinsky/lwp?style=for-the-badge)](https://github.com/jszczerbinsky/lwp/releases/latest)
[![](https://img.shields.io/github/downloads/jszczerbinsky/lwp/total?style=for-the-badge)](https://github.com/jszczerbinsky/lwp/releases/latest)
[![](https://img.shields.io/github/actions/workflow/status/jszczerbinsky/lwp/build.yml?style=for-the-badge)](https://github.com/jszczerbinsky/lwp)
[![](https://img.shields.io/discord/1077955077974794322?label=DISCORD&style=for-the-badge)](https://discord.gg/JmkCqjYFQa)

[Installation](#installation) •
[Configuration](#configuration) •
[Creating Wallpapers](#creating-wallpapers)

<a href="https://www.buymeacoffee.com/jszczerbinsky" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me A Coffee" style="height: 60px !important;width: 217px !important;" ></a>

https://user-images.githubusercontent.com/38699473/220888934-09788a6b-873c-469b-b147-b345e8a8949a.mp4

</div>

## Installation

<details>
  <summary>Linux</summary>
  
  #### Installation steps
  
  - Install `SDL2` using Your package manager
  - If You are using `Wayland`, You also must install `XWayland`
  - Download `.tar.gz` package from [releases](https://github.com/jszczerbinsky/lwp/releases/latest)
  - Extract the content to `/`:
  ```shell
  sudo tar -o -xvf [archive name].tar.gz --directory /
  ```
  - Test Layered WallPaper by running `lwp`
  - Setting `reload_rootwindow=1` in config file may be necessary on some distributions for Layered WallPaper to work properly (see [configuration](#configuration))
  - To make Layered WallPaper run on startup, add `lwp &` command to Your desktop enviroment `.rc` file
  
  #### Build from source instead
  - Install `SDL2` using Your package manager. On some distributions `SDL2` doesn't contain development files, so it may be also necessary to install development version of `SDL2`
  - If You are using `Wayland`, You also must install `XWayland`
  - Install `CMake`
  - Clone the repository and prepare a `build` directory:

  ```shell
  git clone https://github.com/jszczerbinsky/lwp
  cd lwp
  mkdir build
  cd build
  ```
  - Compile the project and generate a `.tar.gz` package
  ```shell
  cmake ../
  cmake --build .
  cpack
  ```
  - Extract `.tar.gz` package
  ```shell
  sudo tar -o -xvf [archive name].tar.gz --directory /
  ```
  - Test Layered WallPaper by running `lwp`
  - Setting `reload_rootwindow=1` in config file may be necessary on some distributions for Layered WallPaper to work properly (see [configuration](#configuration))
  - To make Layered WallPaper run on startup, add `lwp &` command to Your desktop enviroment `.rc` file
  
</details>

<details>
  <summary>macOS</summary>
  
  #### Installation steps
  - Download and run the installer from [releases](https://github.com/jszczerbinsky/lwp/releases/latest)
  - Drag and drop Layered_WallPaper into Applications
  - To make Layered WallPaper run on startup, run Toggle_Autorun.command
    - To stop running Layered WallPaper on startup, run it again
  
  #### Build from source instead
  - Install `SDL2` (homebrew: `brew install sdl2`)
  - To build this project, You need to install `cmake` (homebrew: `brew install cmake`)
  - Clone the repository:
    ```zsh
    git clone https://github.com/jszczerbinsky/lwp
    cd lwp
    
    ```
  - Compile and generate installer
    ```zsh
    mkdir build
    cd build
    cmake ../
    cmake --build .
    cpack -G DragNDrop
    ```
  - DMG installer should appear, open it and drag Layered_WallPaper into Applications
  - To make Layered WallPaper run on startup, run Toggle_Autorun.command
    - To stop running Layered WallPaper on startup, run it again
    
</details>

<details>
  <summary>Windows</summary>
  
  #### Installation steps
  - Download and run the installer from [releases](https://github.com/jszczerbinsky/lwp/releases/latest)
  - Layered WallPaper should run immediately after the installation
  
  #### Build from source instead
  To compile Layered WallPaper on Windows you need to install [MSYS2](https://www.msys2.org/). After the installation follow the guide for setting up [GTK development enviroment](https://www.gtk.org/docs/installations/windows#using-gtk-from-msys2-packages). From now on continue using MSYS2 MinGW terminal (make sure you're using `MSYS2 MINGW64`/`MSYS2 MINGW32` instead of `MSYS2`).

##### Install the remaining dependencies
There is a problem with the newest version of SDL2 on MSYS2 repository (see [issue](https://github.com/haskell-game/sdl2/issues/277)), that leads to a compilation error, thus You should install an older version of SDL2 instead.
```shell
# For 64bit:
curl -O https://repo.msys2.org/mingw/x86_64/mingw-w64-x86_64-SDL2-2.0.14-2-any.pkg.tar.zst
pacman -U mingw-w64-x86_64-SDL2-2.0.14-2-any.pkg.tar.zst

# For 32bit:
curl -O https://repo.msys2.org/mingw/i686/mingw-w64-i686-SDL2-2.0.14-2-any.pkg.tar.zst
pacman -U mingw-w64-i686-SDL2-2.0.14-2-any.pkg.tar.zst
```

You also have to install cmake
```shell
# For 64bit:
pacman -S mingw-w64-x86_64-cmake

# For 32bit:
pacman -S mingw-w64-i686-cmake
```

##### Clone the repository
```shell
git clone https://github.com/jszczerbinsky/lwp
cd lwp
mkdir build
cd build
```

##### Compile and install
- Download [SDL2](https://github.com/libsdl-org/SDL/releases/latest) runtime package and unpack it.
- Compile the program
```shell
# Remember to use unix path format (instead of C:/path/to/dir use /c/path/to/dir)
cmake -DSDL2_RUNTIME_DIR=/path/to/dir ../
cmake --build .
# Prepare the DLLs, that will be shipped with the program.
# The script is going to ask you which version of gdbus.exe should it use (32bit or 64bit)
../distributeDLLs.sh
cpack
```
After this the installer should appear in the current directory.
  
</details>

## Configuration

#### Create a configuration file
<details>
  <summary>Linux</summary>
  
  - Copy default config file to `.config/lwp/lwp.cfg`:
  ```shell
  mkdir ~/.config/lwp
  cp /etc/lwp.cfg ~/.config/lwp/lwp.cfg
  ```
  
</details>
<details>
  <summary>macOS</summary>

  - Copy default config file to `~/.config/lwp/lwp.cfg`:
    ```zsh
    mkdir -p ~/.config/lwp
    cp /opt/lwp/lwp.cfg ~/.config/lwp/
    ```
</details>
<details>
  <summary>Windows</summary>
  
  - Press <kbd>⊞ Win</kbd> + <kbd>R</kbd>
  - Type `%appdata%` and press `Ok`
  - Create new directory and name it `lwp`
  - Copy file `C:\Program Files\lwp\defaultWin.cfg` to directory created in the previous step and rename it to `lwp.cfg`
  - Open `lwp.cfg` in notepad
  
</details>

#### Using config file

- Do not put spaces between `=` and values
- Do not leave trailing spaces
- Comments start with `#`
- Do not put strings in quotation marks

#### Available options:

|  Type  |      Name    | Description |
| ------ | ------------ | ----------- |
| int    | reload_rootwindow | Set this to 1 if You are using a compositor (linux only) |
| float  | smooth       | Smooth movement multipler |
| int    | monitors     | Monitors count |
| int    | monitor[n]_x | Position of nth monitor in X axis |
| int    | monitor[n]_y | Position of nth monitor in Y axis |
| int    | monitor[n]_w | Width of nth monitor |
| int    | monitor[n]_h | Height of nth monitor |
| string | monitor[n]_wallpaper   | Absolute path to the wallpaper directory |
| int    | monitor[n]_wallpaper_x | Position of the wallpaper relative to the monitor |
| int    | monitor[n]_wallpaper_y | Position of the wallpaper relative to the monitor |
| int    | monitor[n]_wallpaper_w | Wallpaper resolution |
| int    | monitor[n]_wallpaper_h | Wallpaper resolution |
| int    | target_fps     | How many times per second should the wallpaper render (imprecise, hence "target") |

## Creating Wallpapers

Want to share Your wallpaper? Join our [discord](https://discord.gg/JmkCqjYFQa)!

Parallax wallpapers are not popular. Because of this if You want some cool parallax wallpaper, You have to either find a parallax game background on the internet and use it as a wallpaper or cut some real wallpaper into layers using Gimp or Photoshop.

#### How to create a wallpaper for Layered WallPaper
- Create a directory for Your wallpaper
- Save each layer to this directory as `.bmp` file and name them `1.bmp`, `2.bmp` ... (`1.bmp` is bottom most layer)
- Create a wallpaper config file and name it `wallpaper.cfg` (You can make a copy from `C:\Program Files\lwp\wallpapers\default-fullhd\wallpaper.cfg`)

#### Example:

File structure:
```
C:
 └  MyCoolWallpaperDirectory
         └  wallpaper.cfg
         └  1.bmp
         └  2.bmp
         └  3.bmp
```

#### Config file:

|  Type  |      Name    | Description |
| ------ | ------------ | ----------- |
| int    | count        | Wallpaper layers count |
| float  | movement_x    | Mouse sensitivity in X axis |
| float  | movement_y    | Mouse sensitivity in Y axis |
| float  | movement[n]_x | Mouse sensitivity in X axis for nth layer (optional)|
| float  | movement[n]_y | Mouse sensitivity in Y axis for nth layer (optional)|
| int    | repeat_x     | Repeat the wallpaper in X axis |
| int    | repeat_y     | Repeat the wallpaper in Y axis |

## License

MIT License

Copyright (c) 2022 Jakub Szczerbiński

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.