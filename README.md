
# WxScid

Wxwidget Interface for [Scid](http://scid.sourceforge.net/)

## Code features

**Implemented:**

* Dockable interface with [AUI](https://wiki.wxwidgets.org/WxAUI)

* XML : use xrc to build dialogs / frames

**Todo:**

* I18n: Internationalize app with Gettext

* Plugin API : Extend app with plugins

* Bind Scripting language : Lua with wxLua ? 


## Compilation

```bash
# Generate configure script
./autogen.sh
# Build
make
# Run
WX_WXSCID_DATA_DIR=${RESOURCE_DIR} ./src/wxScid
```
## Compilation using GTK-3 toolkit

Dowload latest stable version of wxWidgets and configure it with these options :

```bash
./configure --prefix=${your_install_prefix} --with-gtk=3
make && make install
```
Then compile wxScid with these options :

```bash
mkdir m4 && cd m4 && wget https://raw.githubusercontent.com/wxWidgets/wxWidgets/master/wxwin.m4
./autogen.sh --with-wx-config=${your_install_prefix}/bin/wx-config --with-toolkit=gtk3 --prefix=${your_install_prefix}
make
```
To run the app :

```bash
LD_LIBRARY_PATH=${your_install_prefix}/lib WX_WXSCID_DATA_DIR=${RESOURCE_DIR} ./src/wxScid
```

## Cross compilation under Linux with MXE

### Win32

```bash
CXXFLAGS="-std=c++11" ./autogen.sh \
--host=i686-w64-mingw32.static \
--prefix=${MXE_PATH}/usr/i686-w64-mingw32.static \
--with-wx-config=${MXE_PATH}/usr/i686-w64-mingw32.static/bin/wx-config
```

## Compilation with docker

```bash
# Build image
docker build -t wxscid-builder .

# Run and Build container
docker run -ti --name wxcidbuilder -v "$PWD":/home/ilhooq/wxScid wxscid-builder bash

# Start container
docker start wxcidbuilder

# Connect to container
docker exec -ti wxcidbuilder bash

```

