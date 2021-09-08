// udp_server.h : Include file for standard system include files,
// or project specific include files.

#pragma once
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

typedef bool bool32;
typedef float float32;
typedef double float64;

const uint16 PORT = 9999;
const uint32 SOCKET_BUFFER_SIZE = 1024;


#include <iostream>
#include <stdio.h>
#include <winsock2.h>
// TODO: Reference additional headers your program requires here.

