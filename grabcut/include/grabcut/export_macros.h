#pragma once
#ifndef GRABCUT_EXPORT_MACROS_H
#define GRABCUT_EXPORT_MACROS_H

// This file contains minimal, required suite of macros for API tagging in more cross-compile manner

#if (defined WIN32 || defined _WIN32 || defined WINCE || defined __CYGWIN__)
#ifdef grabcut_EXPORTS // FIXME: make this work
      #define GRABCUT_EXPORT __declspec(dllexport)
   #else
      #define GRABCUT_EXPORT __declspec(dllimport)
  #endif
#elif defined __GNUC__ && __GNUC__ >= 4
#define GRABCUT_EXPORT __attribute__((visibility("default")))
#else
#define GRABCUT_EXPORT
#endif

#endif //GRABCUT_EXPORT_MACROS_H
