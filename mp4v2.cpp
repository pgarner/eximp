/*
 * Copyright 2017 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, August 2017
 */

#include <iostream>
#include "mp4v2.h"

MP4::MP4()
{
    mHandle = MP4_INVALID_FILE_HANDLE;
}

MP4::MP4(var iFileName)
{
    mHandle = MP4_INVALID_FILE_HANDLE;
    load(iFileName);
}

MP4::~MP4()
{
    if (mHandle)
        MP4Close(mHandle);
    mHandle = MP4_INVALID_FILE_HANDLE;
}

void MP4::load(var iFileName)
{
    // Open the file
    mHandle = MP4ReadProvider(iFileName.str());
    if (!MP4_IS_VALID_FILE_HANDLE(mHandle))
        return;

    // Copy the metadata tags
    MP4ItmfItemList* itemList;
    itemList = MP4ItmfGetItems(mHandle);
    int nItems = itemList->size;
    MP4ItmfItem* item = itemList->elements;
    for (int i=0; i<nItems; i++)
    {
        std::cout << item[i].code << std::endl;
    }
    MP4ItmfItemListFree(itemList);
}

void MP4::dump()
{
    std::cout << mTag << std::endl;
}

var MP4::date()
{
    // Date information is in 'creation_time' tag
    return mTag["creation_time"].copy();
}

var MP4::tag(var iKey)
{
    return mTag[iKey];
}
