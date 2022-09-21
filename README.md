# Layered Wallpaper Engine
Lwp allows You to set multi-layered wallpapers. Each of those layers will move with Your mouse cursor, creating this beautiful visual effect:

![](preview.gif)

## Installation

- Install `SDL2` using Your package manager
- Clone the repository and install `lwp`:

```shell
git clone https://github.com/jszczerbinsky/lwp
cd lwp
make
sudo make install
```
- Test lwp (replace "1920" and "1080" with Your screen resolution):

```shell
lwp 3 /usr/share/lwp/wallpapers/test 0 0 1920 1080
```

## Usage
Make sure there is no other wallpaper engine running (e.g. `feh`)
```shell
lwp [layers count] [wallpepr dir] [x] [y] [width] [height]
```
You can run `lwp` on multiple monitors by appending `[x] [y] [width] [height]` for each monitor at the end of the line

## Creating wallpapers for lwp
- Create a directory for the wallpaper
- Save each layer as: `1.bmp`, `2.bmp`, `3.bmp`... (`1.bmp` is bottom most layer)
