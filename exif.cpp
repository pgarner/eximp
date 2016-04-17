/*
 * Copyright 2015 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, September 2015
 */

#include "exif.h"

EXIF::EXIF(var iPath)
{
    mData = exif_data_new_from_file(iPath.str());
    if (!mData)
        std::cout << "Failed: " << iPath << std::endl;
    else
        // I'm not sure this is necessary, but it feels good
        exif_data_fix(mData);
}

EXIF::~EXIF()
{
    if (mData)
        exif_data_free(mData);
    mData = 0;
}

void EXIF::dump()
{
    if (valid())
        exif_data_dump(mData);
}

var EXIF::date()
{
    // date() is probably not the right name, but let's play with it anyway
    // just to figure out how the library works
    if (!valid())
        throw lube::error("EXIF class invalid");

    var r;
    ExifEntry* entry = exif_data_get_entry(mData, EXIF_TAG_DATE_TIME);
    if (!entry)
        return r;
    exif_entry_dump(entry, 2);

    switch (entry->format)
    {
    case EXIF_FORMAT_ASCII:
        r = var(entry->components, (const char*)entry->data);
        break;
    default:
        throw lube::error("Unknown format in EXIF entry");
    }

    // Parse it to get at least the ISO date and the month
    //   iPhone has format 2015:01:01 17:31:46
    //   Old Samsung has   2012-12-02 04:08:18
    // So we need to get rid of at least ':' and '-'
    // Actually, geeqie fails on that Samsung format
    r.replace("[-:]", " ");
    var s = r.split(" ");
    r = s[1];
    return r;
}
