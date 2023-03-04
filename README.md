<div align="center">

# LWP

LWP allows You to create multi-layered parallax wallpapers.<br />
Each layer moves with Your mouse cursor, creating this beautiful effect.

[![](https://img.shields.io/github/v/release/jszczerbinsky/lwp?style=for-the-badge)](https://github.com/jszczerbinsky/lwp/releases/latest)
[![](https://img.shields.io/github/downloads/jszczerbinsky/lwp/total?style=for-the-badge)](https://github.com/jszczerbinsky/lwp/releases/latest)
[![](https://img.shields.io/github/actions/workflow/status/jszczerbinsky/lwp/build.yml?style=for-the-badge)](https://github.com/jszczerbinsky/lwp)
[![](https://img.shields.io/discord/1077955077974794322?label=DISCORD&style=for-the-badge)](https://discord.gg/gHpbkXJD)

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
  
  - Download `.tar.gz` package from [releases](https://github.com/jszczerbinsky/lwp/releases/latest)
  - Extract the content to `/`:
  ```shell
  sudo tar -o -xvf [archive name].tar.gz --directory /
  ```
  - Test lwp by running `lwp`
  - To make lwp run on startup, add `lwp &` command to Your desktop enviroment `.rc` file
  
  #### Build from source instead
  - Install `SDL2` using Your package manager
  - Clone the repository and install `lwp`:

  ```shell
  git clone https://github.com/jszczerbinsky/lwp
  cd lwp
  make
  sudo make install
  ```
  - Test lwp by running `lwp`
  - To make lwp run on startup, add `lwp &` command to Your desktop enviroment `.rc` file
  
</details>

<details>
  <summary>macOS</summary>
  
  #### Build from source
  - Install `SDL2` (homebrew: `brew install sdl2`)
  - Clone the repository and install `lwp`:
    ```zsh
    git clone https://github.com/jszczerbinsky/lwp
    cd lwp
    make
    sudo make install
    ```
  - Test lwp by running `/opt/lwp/bin/lwp`
  - To make lwp run on startup, run `make install-launchd`
    - To stop running lwp on startup, run `make uninstall-launchd`
    
</details>

<details>
  <summary>Windows</summary>
  
  #### Installation steps
  - Download `.zip` package from [releases](https://github.com/jszczerbinsky/lwp/releases/latest)
  - Extract the package
  - Run `install.bat` as Administrator
  - Lwp should run immediately after the installation
  
  #### Build from source instead
  - You need `MinGW` with [SDL2](https://github.com/libsdl-org/SDL/releases/latest)-devel

  ```shell
  git clone https://github.com/jszczerbinsky/lwp
  cd lwp
  build.bat
  ```
  - Download [SDL2](https://github.com/libsdl-org/SDL/releases/latest) and put `SDL2.dll` in `data` directory
  - Run `install.bat` as Administrator
  - Lwp should run immediately after the installation
  
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

Want to share Your wallpaper? Join our [discord](https://discord.gg/gHpbkXJD)!

Parallax wallpapers are not popular. Because of this if You want some cool parallax wallpaper, You have to either find a parallax game background on the internet and use it as a wallpaper or cut some real wallpaper into layers using Gimp or Photoshop.

#### How to create a wallpaper for lwp
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
