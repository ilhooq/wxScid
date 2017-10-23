
# WxScid

Wxwidget Interface for SCID

## Code features

Dockable interface : with AUI

Plugin API : Extend app with plugins

Bind Scripting language : Lua with wxLua ? 

I18n: Internationalize app with Gettext

XML : use xrc to build dialogs / frames

## Compilation

```bash
# Generate configure script
./autogen.sh
# Build
make
# Run
WX_WXSCID_DATA_DIR=${RESOURCE_DIR} ./src/wxScid
```

## Cross compilation under Linux with MXE

### Win32

```bash
CXXFLAGS="-std=c++11" ./autogen.sh \
--host=i686-w64-mingw32.static \
--prefix=${MXE_PATH}/usr/i686-w64-mingw32.static \
--with-wx-config=${MXE_PATH}/usr/i686-w64-mingw32.static/bin/wx-config
```

