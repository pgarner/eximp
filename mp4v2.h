/*
 * Copyright 2017 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, August 2017
 */

#ifndef MP4V2_H
#define MP4V2_H

#include <lube.h>
#include <mp4v2/mp4v2.h>

class MP4
{
public:
    MP4();
    MP4(var iFileName);
    ~MP4();
    bool valid() { return MP4_IS_VALID_FILE_HANDLE(mHandle); };
    void dump();
    var date();
    var tag(var iKey);
private:
    void load(var iFileName);
    MP4FileHandle mHandle;
    var mTag;
};

#endif // MP4V2_H
