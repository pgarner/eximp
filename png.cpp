/*
 * Copyright 2016 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, April 2016
 */

#include <cassert>
#include <iomanip>
#include "png.h"
#include "exif.h"

void PNG::load(var iFileName)
{
    // Open the file and check the header indicates it's actually a PNG file
    FILE *fp = fopen(iFileName.str(), "rb");
    const int headersize = 8;
    char header[headersize];
    if (!fp)
        return;
    if (fread(header, 1, headersize, fp) != headersize)
        return;
    if (png_sig_cmp((png_const_bytep)header, 0, headersize))
        return;

    // Create the PNG structure without callbacks
    mData = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, (png_voidp)0, 0, 0
    );
    if (!mData)
        return;

    // And the PNG information fields
    mInfo = png_create_info_struct(mData);
    if (!mInfo)
        return;

    // Give it the file pointer
    png_init_io(mData, fp);
    png_set_sig_bytes(mData, headersize);

    // Low level read: only read the header information
    png_read_info(mData, mInfo);
    fclose(fp);
}

PNG::PNG()
{
    mData = 0;
    mInfo = 0;
}

PNG::PNG(var iFileName)
    : PNG()
{
    load(iFileName);
}

PNG::~PNG()
{
    if (mData)
        png_destroy_read_struct(&mData, &mInfo, (png_infopp)NULL);
    mData = 0;
    mInfo = 0;    
}


void PNG::dump()
{
    // Text chunks
    png_textp textp = 0;
    int ntext = 0;
    png_get_text(mData, mInfo, &textp, &ntext);
    var text;
    for (int i=0; i<ntext; i++)
        text[textp[i].key] = textp[i].text;
    std::cout << "Text: " << text << std::endl;

    // EXIF
    // The library reports "png_get_eXIf does not work; use png_get_eXIf_1"
    png_bytep exif_data = 0;
    png_uint_32 num_exif = 0;
    png_uint_32 pngRet = png_get_eXIf_1(mData, mInfo, &num_exif, &exif_data);
    if (pngRet)
    {
        assert(exif_data);
        EXIF exif(exif_data, num_exif);
        exif.dump();
        std::cout << "EXIF date: " << exif.date() << std::endl;
    }
}

static var format(int iNumber, int iWidth)
{
    varstream vs;
    vs << std::setw(iWidth) << std::setfill('0');
    vs << iNumber;
    return var(vs);
}

/**
 * Try for modification time.  png_get_tIME() should return PNG_INFO_tIME on
 * success or 0 otherwise.  Then we handle time as an array of six strings like
 * (my) exif for the moment.
 */
var PNG::date()
{
    if (!valid())
        return lube::nil;

    png_timep timep = 0;
    png_uint_32 pngRet = png_get_tIME(mData, mInfo, &timep);
    if (!pngRet)
        return lube::nil;
    assert(timep);
    var time;
    time[0] = format(timep->year, 4);
    time[1] = format(timep->month, 2);
    time[2] = format(timep->day, 2);
    time[3] = format(timep->hour, 2);
    time[4] = format(timep->minute, 2);
    time[5] = format(timep->second, 2);
    return time;
}
