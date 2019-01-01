/*
 * Copyright 2016 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, April 2016
 */

// Beware: libpng is actually /usr/include/png.h and defines PNG_H
#ifndef LUBE_PNG_H
#define LUBE_PNG_H

#include <lube.h>
#include <png.h>

class PNG
{
public:
    PNG();
    PNG(var iFileName);
    ~PNG();
    bool valid() { return mData; };
    void dump();
    var date();
private:
    png_structp mData;
    png_infop mInfo;
    void load(var iFileName);
};

#endif // LUBE_PNG_H
