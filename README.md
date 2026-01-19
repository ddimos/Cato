
# Cato
Cato is a 2D online multiplayer card game based on Cabo.

The game is built using SFML. The network code is built using my own library [nsf](https://github.com/ddimos/nsf) making it a great opportunity to try it out in practice.

# Features

- Multiplayer gameplay using client-server architecture
- Input buffer delay
- Entity interpolation
- Basic save system 
- Cross-platform (Windows, Linux, macOS)

## Requirments

- C++20
- Cmake 3.24+
- SFML 2.6
- nsf ([link](https://github.com/ddimos/nsf))

### Linux

You must install SFML dependencies before compiling.

Example (Ubuntu/Debian):
```
sudo apt-get update 
sudo apt-get install libxrandr-dev libxcursor-dev libudev-dev libopenal-dev libflac-dev libvorbis-dev libgl1-mesa-dev libegl1-mesa-dev libfreetype-dev

```

## Compilation
### Clone the repository
```
git clone https://github.com/ddimos/Cato.git
cd Cato
```
### Build with Cmake
#### Client-Release
```
mkdir build
cd build
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DLAUNCH_TARGET=Client

cmake --build build
```

#### Server-Release
```
mkdir build
cd build
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DLAUNCH_TARGET=Server

cmake --build build
```
### Build Using VS Code (cmake-variants.json)

This project includes a cmake-variants.json file for use with the VS Code CMake Tools extension.

1. Open the project folder in VS Code
2. Install the CMake Tools extension
3. Press Ctrl+Shift+P -> CMake: Select Variant
4. Choose:
    - Build Type: Debug or Release
    - Launch Target: Client or Server
5. Press Ctrl+Shift+P -> CMake: Build

VS Code will automatically apply the correct CMake options based on the selected variant.

## Launching the Game
Running from the project directory
### Client
```
./build/Cato
```
### Server
```
./build/Cato_Server
```
The server must be launched separately. It listens on port **20475** by default.

### Connecting remotely
Clients have to enter the server public address which is logged in the server terminal when launched.

Example:
_The public address: x.x.x.x:20475_

Ensure the port **is open** on the server machine. Remote clients won't be able to connect if it's not done.

### Connecting locally 
It is possible to connect to the server running on the same machine by entering the loopback address **127.0.0.1:20475**

### MacOs Note
On macOs, the game executable may be blocked by Gatekeeper.

To remove the quarantine flag, run the following command in Terminal:
```
xattr -dr com.apple.quarantine Cato.app
```

## Screenshots
coming soon...