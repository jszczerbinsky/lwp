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
lwp 1920 1080 3 /usr/share/lwp/wallpapers/test
```

## Usage
```shell
lwp [screen width] [screen height] [layers count] [wallpepr dir]
```

## Creating wallpapers for lwp
- Create a directory for the wallpaper
- Save each layer as: `1.bmp`, `2.bmp`, `3.bmp`... (`1.bmp` is bottom most layer)
