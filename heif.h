/*
 * Copyright 2015 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, September 2015
 */

#ifndef LUBE_HEIF_H
#define LUBE_HEIF_H

#include <lube.h>
#include <heif.h>

#include "exif.h"

class HEIF
{
public:
    HEIF() { mData = 0; };
    HEIF(var iPath);
    ~HEIF();
    bool valid() { return mData; };
    void dump();
    var entry(ExifTag iTag) { return mExif.entry(iTag); };
    var date() { return mExif.date(); };
private:
    struct heif_context* mData;
    EXIF mExif;
};

#endif // LUBE_HEIF_H
