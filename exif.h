/*
 * Copyright 2015 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, September 2015
 */

#ifndef EXIF_H
#define EXIF_H

#include <lube.h>
#include <exif-data.h>

class EXIF
{
public:
    EXIF() { mData = 0; };
    EXIF(var iPath);
    ~EXIF();
    bool valid() { return mData; };
    void dump();
    var entry(ExifTag iTag);
    var date(var iStr=lube::nil);
private:
    ExifData* mData;
};

#endif // EXIF_H
