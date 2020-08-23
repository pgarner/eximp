/*
 * Copyright 2016 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, May 2016
 */

#include <iostream>
#include "avformat.h"
extern "C" {
# include <libavutil/dict.h>
}

AVFormat::AVFormat()
{
    mContext = 0;
}

AVFormat::AVFormat(var iFileName)
{
    mContext = 0;
    load(iFileName);
}

AVFormat::~AVFormat()
{
    if (mContext)
        avformat_close_input(&mContext);
    mContext = 0;    
}

void AVFormat::load(var iFileName)
{
    int ret = avformat_open_input(&mContext, iFileName.str(), 0, 0);
    if (ret)
    {
        mContext = 0;
        return;
    }

    // Copy the metadata tags
    AVDictionaryEntry *tag = 0;
    while ((tag = av_dict_get(
                mContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX
            )))
        mTag[tag->key] = tag->value;
}

void AVFormat::dump()
{
    std::cout << mTag << std::endl;
}

var AVFormat::date()
{
    // Date information is in 'creation_time' tag
    return mTag["creation_time"].copy();
}

var AVFormat::tag(var iKey)
{
    return mTag[iKey];
}
