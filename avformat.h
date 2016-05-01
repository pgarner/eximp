/*
 * Copyright 2016 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, May 2016
 */

#ifndef AVFORMAT_H
#define AVFORMAT_H

#include <lube.h>
extern "C" {
# include <libavformat/avformat.h>
}

class AVFormat
{
public:
    AVFormat();
    AVFormat(var iFileName);
    ~AVFormat();
    bool valid() { return mContext; };
    void dump();
    var date();
    var tag(var iKey);
private:
    void load(var iFileName);
    AVFormatContext *mContext;
    var mTag;
};

#endif // AVFORMAT_H
