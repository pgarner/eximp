/*
 * Copyright 2016 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, April 2016
 */

#include "png.h"

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
    std::cout << "Loaded: " << iFileName.str() << std::endl;
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

var PNG::date()
{
    if (!valid())
        return lube::nil;

    // Try for modification time
    png_timep timep;
    png_get_tIME(mData, mInfo, &timep);
    if (timep)
        std::cout << "Time: "
                  << timep->year << "-"
                  << timep->minute
                  << std::endl;

    // Text chunks
    png_textp text;
    int ntext;
    png_get_text(mData, mInfo, &text, &ntext);
    std::cout << "Text: " << ntext << std::endl;
    for (int i=0; i<ntext; i++)
    {
        std::cout << "Key: " << text[i].key << std::endl;
        std::cout << "Txt: " << text[i].text << std::endl;
    }
    return lube::nil;
}
