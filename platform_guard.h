#ifndef PLATFORM_GUARD_H
#define PLATFORM_GUARD_H

#ifndef __WIN32
#ifndef __DARWIN
#ifndef __LINUX

#error "Unsupported platform."

#endif // __LINUX
#endif // __DARWIN
#endif // __WIN32

#endif // PLATFORM_GUARD_H
