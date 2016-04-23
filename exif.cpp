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
    if (mData)
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

var EXIF::entry(ExifTag iTag)
{
    if (!valid())
        throw lube::error("EXIF class invalid");
    var r;
    ExifEntry* e = exif_data_get_entry(mData, iTag);
    if (!e)
        return r;
    // Debuggin'
    //exif_entry_dump(e, 2);

    switch (e->format)
    {
    case EXIF_FORMAT_ASCII:
        r = var(e->components, (const char*)e->data);
        break;
    default:
        throw lube::error("EXIF::entry: Unknown format");
    }
    return r;
}

/**
 * Helper function to return the date as an array of strings
 */
var EXIF::dateArray(var iStr)
{
    if (!valid())
        throw lube::error("EXIF class invalid");

    // Try for the DATE_TIME EXIF tag, then DATE_TIME_ORIGINAL; otherwise use
    // the input hint, which is probably the filename
    var r = entry(EXIF_TAG_DATE_TIME);
    if (!r)
        r = entry(EXIF_TAG_DATE_TIME_ORIGINAL);
    if (!r)
        r = iStr.copy();

    // Parse it to get at least the ISO date and the month
    //   iPhone has format 2015:01:01 17:31:46
    //   Old Nokia has     2010:06:09 12:22:02
    //   Old Samsung has   2012-12-02 04:08:18
    //   Dropbox files are 2012-12-02 04.08.18
    //   ISO format is     2012-12-02T04:08:18Z
    // So we need to get rid of at least ':', '-' and '.', and the ISO tags.
    // Actually, geeqie fails on that Samsung format
    r.replace("[-:.TZ]", " ");
    var s = r.strip().split(" ");
    if (s[0].size() != 4) return lube::nil;
    if (s[1].size() != 2) return lube::nil;
    if (s[2].size() != 2) return lube::nil;
    return s;
}

/**
 * Convert the array to an ISO 8601 string
 * ie. 2016-04-17T09:09:48Z
 *
 * The colons may cause trouble on a FAT filesystem, in which case the answer
 * is to use the same string without hyphens or colons; that's still an ISO
 * standard date.
 */
var arrayToISODate(var d)
{
    // How long is it?
    int s = d.size();
    if (s > 6)
        s = 6;

    // Format to a stream
    varstream iso;
    switch (s)
    {
    case 6:
        // The most likely; three date and three time fields
        iso << d[0].str() << "-" << d[1].str() << "-" << d[2].str();
        iso << "T";
        iso << d[3].str() << ":" << d[4].str() << ":" << d[5].str();
        iso << "Z";
        break;
        //default:
        // throw lube::error("Not a valid date size");
    }

    // Pull the var out of the stream
    return var(iso);
}

/**
 * Get the date, but as an ISO string rather than whatever format is in the
 * file.
 */
var EXIF::date(var iArray)
{
    var s = iArray ? iArray : dateArray();
    var r = arrayToISODate(s);
    return r;
}
