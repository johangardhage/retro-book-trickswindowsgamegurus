# retro-book-trickswindowsgamegurus

Demos from Tricks of the Windows Game Programming Gurus, brought back from the 90's.

![Screenshot](/screenshots/demo12_3.png "demo12_3")

## Prerequisites

To build the demo programs, you must first install the following tools:

- [GCC](https://gcc.gnu.org/)
- [Ninja](https://ninja-build.org/)
- [SDL2](https://www.libsdl.org/)

### Install dependencies

#### openSUSE

`$ sudo zypper install ninja gcc-c++ libSDL2-devel`

#### Ubuntu

`$ sudo apt install ninja-build g++ libsdl2-dev`

#### Windows

* Manually download and install [MSYS2](https://www.msys2.org/).
* Run `View advanced system settings` from the Start menu and add a PATH to `C:\msys64\mingw64\bin`.
* Run `MSYS2 MSYS` from the Start menu. Update the package database and base packages with `pacman -Syu`.
* Run `MSYS2 MSYS` from the Start menu again. Update the rest of the base packages with `pacman -Syu`.
* Install the development tools with `pacman -S git mingw-w64-x86_64-gcc mingw-w64-x86_64-ninja mingw-w64-x86_64-SDL2`.
* Close the `MSYS2 MSYS` window and run `MSYS MinGW 64-bit` from the Start menu.
* Clone the git repository with `git clone https://github.com/johangardhage/retro-book-gardens.git`.
* Finally, to be able to build the demos on Windows, edit the file `build.ninja` and uncomment the line [#  command = $cc $in $windows -o $out.](build.ninja#L10).

## Build instructions

To build the demo programs, run:

`$ ninja`

A `build` directory will be created, containing the demo programs.

## Usage

```
Usage: demo [OPTION]...

Options:
 -h, --help           Display this text and exit
 -w, --window         Render in a window
     --fullwindow     Render in a fullscreen window
 -f, --fullscreen     Render in fullscreen
 -v, --vsync          Enable sync to vertical refresh
     --novsync        Disable sync to vertical refresh
 -l, --linear         Render using linear filtering
     --nolinear       Render using nearest pixel sampling
 -c, --showcursor     Show mouse cursor
     --nocursor       Hide mouse cursor
     --showfps        Show frame rate in window title
     --nofps          Hide frame rate
     --capfps=VALUE   Limit frame rate to the specified VALUE
```

## License

Licensed under MIT license. See [LICENSE](LICENSE) for more information.

## Authors

Original code written by Andre LaMothe for [Tricks of the Windows Game Programming Gurus](https://www.amazon.com/Tricks-Windows-Game-Programming-Gurus-dp-0672323699/dp/0672323699) (Sams)

## Screenshots

![Screenshot](/screenshots/demo7_8.png "demo7_8")
![Screenshot](/screenshots/demo8_1.png "demo8_1")
![Screenshot](/screenshots/demo8_4.png "demo8_4")
![Screenshot](/screenshots/demo8_7.png "demo8_7")
![Screenshot](/screenshots/demo8_9.png "demo8_9")
![Screenshot](/screenshots/demo8_10.png "demo8_10")
![Screenshot](/screenshots/demo9_1.png "demo9_1")
![Screenshot](/screenshots/demo9_2.png "demo9_2")
![Screenshot](/screenshots/demo9_4.png "demo9_4")
![Screenshot](/screenshots/demo12_1.png "demo12_1")
![Screenshot](/screenshots/demo12_2.png "demo12_2")
![Screenshot](/screenshots/demo12_3.png "demo12_3")
![Screenshot](/screenshots/demo12_6.png "demo12_6")
![Screenshot](/screenshots/demo12_8.png "demo12_8")
![Screenshot](/screenshots/demo13_1.png "demo13_1")
![Screenshot](/screenshots/demo13_2.png "demo13_2")
![Screenshot](/screenshots/demo13_3.png "demo13_3")
![Screenshot](/screenshots/demo13_4.png "demo13_4")
![Screenshot](/screenshots/demo13_5.png "demo13_5")
![Screenshot](/screenshots/demo13_6.png "demo13_6")
![Screenshot](/screenshots/demo13_7.png "demo13_7")
![Screenshot](/screenshots/demo13_9.png "demo13_9")
![Screenshot](/screenshots/demo13_11.png "demo13_11")
![Screenshot](/screenshots/freakout.png "freakout")
