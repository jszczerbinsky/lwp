#ifndef PLATFORM_GUARD_H
#define PLATFORM_GUARD_H

#ifdef __WIN32
#define NTDDI_VERSION 0x0A000000
#endif

#ifndef __WIN32
#ifndef __LINUX

#error "Unsupported platform."

#endif // __LINUX
#endif // __WIN32

#endif // PLATFORM_GUARD_H
