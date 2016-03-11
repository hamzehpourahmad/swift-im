## Status ##
Due to the constant changes in MMP (binary protocol used for messaging) I've discontinued the development of this project.
Furthermore, the protocol is proprietary so it's very hard to maintain project. Official page (http://agent.mail.ru/ru/developers/index.html) has a documentation, but it's outdated and seems to be incorrect.
Feel free to fork and/or explore.

## About ##

Swift-IM is an open-source, cross-platform instant messenger for Mail.ru.
It was developed to provide a Mail.ru instant messenger for Linux, particularly
for GNOME users :) Despite this Swift-IM could run under Windows, OSX
and every platform that supports GTK+. It uses GTK+ toolkit and gtkmm - official
C++ interface for GTK+. You need GTK+ and gtkmm runtime libraries to run Swift-IM.

**Swift-IM 0.2 alpha released!**
News in this version:
  * Totally rewritten protocol-specific part
  * Emoticons support
  * Multilingual interface aka i18n
  * UI improvements
  * Fresh logo :)
  * Many bugfixes!
Also, released installer for Windows!


## Features ##

Swift-IM supports only few basic features at this moment.
The basic features of Swift-IM are:
  * sending and receiving plain text messages
  * emoticons
  * basic statuses
  * avatar support
  * mailbox status support

**Swift-IM-0.2a under Ubuntu 9.04"**

![http://i36.tinypic.com/11qjw90.png](http://i36.tinypic.com/11qjw90.png)

![http://i36.tinypic.com/25k23qf.png](http://i36.tinypic.com/25k23qf.png)


## Installing ##

### Installing under Windows ###
There is an installer for windows. See [Downloads](http://code.google.com/p/swift-im/downloads/list) section.

### Installing from sources ###

For compiling Swift-IM yourself, you will need the gtkmm (>= 2.16.x) libraries
and header files. You can find it at http://www.gtkmm.org/.
Also you need, of course a C++ compiler and CMake (>= 2.6) build system.
You can download CMake at http://www.cmake.org/.
For more details, see INSTALL file in source packages.


## License ##

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses>.