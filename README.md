# FontViewer
A cross platform tool to view font information.

## Build
**Prerequisites**
* cmake,python
* Open source libraries: Qt5, freetype, harfbuzz, boost, icu, iconv, openssl, podofo(optional)
* Homebrew (macOS) or vcpkg (Windows) is recommended to manage the libraries.

**Get the Repo:**

    git clone https://github.com/frinkr/FontViewer.git
    
**CMake gen:**

    cd FontViewer
    python cmake-gen.py
    
Checkout the build directory to find the project files.

### Usage
Intuited & Simple

![Alt text](Resources/Screenshots/FontBrowser.png?raw=true "Font Browser")
![Alt text](Resources/Screenshots/Main.png?raw=true "Main Window")

### License
MIT
