/*
 * Copyright 2020 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, August 2020
 */

#include <cassert>
#include <cstring>

#include "heif.h"
#include "exif.h"

/*
 * For the moment this uses the C interface.  If it were to get complicated it
 * may make sense to use the C++ interface.  At least, the function names would
 * be shorter.
 */

HEIF::HEIF(var iPath) : HEIF()
{
    struct heif_error err;

    // Load the image into a context
    mData = heif_context_alloc();
    err = heif_context_read_from_file(mData, iPath.str(), NULL);
    if (err.code != heif_error_Ok)
    {
        heif_context_free(mData);
        mData = 0;
        return;
    }

    // Pull the exif information out of the image
    int nImages = heif_context_get_number_of_top_level_images(mData);
    if (nImages < 1)
        return;
    struct heif_image_handle *handle;
    heif_context_get_primary_image_handle(mData, &handle);
    int nMeta = heif_image_handle_get_number_of_metadata_blocks(handle, "Exif");
    heif_item_id meta[nMeta];
    heif_image_handle_get_list_of_metadata_block_IDs(
        handle, "Exif", meta, nMeta
    );
    if (nMeta < 1)
        return;
    
    int nData = heif_image_handle_get_metadata_size(handle, meta[0]);
    unsigned char exifData[nData];
    heif_image_handle_get_metadata(handle, meta[0], exifData);
    mExif.load(exifData+4, nData-4);
}

HEIF::~HEIF()
{
    if (mData)
        heif_context_free(mData);
    mData = 0;
}

void HEIF::dump()
{
    if (!valid())
        return;

    // Gather some information
    int nImages = heif_context_get_number_of_top_level_images(mData);
    std::cout << "HEIF: " << nImages << " top level image(s)" << std::endl;
    heif_item_id image[nImages];
    heif_context_get_list_of_top_level_image_IDs(mData, image, nImages);
    for (int i=0; i<nImages; ++i)
    {
        struct heif_image_handle *handle;
        heif_context_get_image_handle(mData, image[i], &handle);
        int nMeta =
            heif_image_handle_get_number_of_metadata_blocks(handle, NULL);
        std::cout << "Image " << i << " has "
                  << nMeta << " metadata blocks" << std::endl;
        heif_item_id meta[nMeta];
        heif_image_handle_get_list_of_metadata_block_IDs(
            handle, NULL, meta, nMeta
        );
        for (int m=0; m<nMeta; ++m)
        {
            const char* str =
                heif_image_handle_get_metadata_type(handle, meta[m]);
            std::cout << " ID: " << m << " " << str << std::endl;
            if (!strcmp(str, "Exif"))
            {
                // Dump the exif data
                int nData =
                    heif_image_handle_get_metadata_size(handle, meta[m]);
                unsigned char exifData[nData];

                // From heif.h:
                // The data is returned exactly as stored in the HEIF file.
                // For Exif data, you probably have to skip the first four
                // bytes of the data, since they indicate the offset to the
                // start of the TIFF header of the Exif data.
                heif_image_handle_get_metadata(handle, meta[m], exifData);
                EXIF exif(exifData+4, nData-4);
                exif.dump();
                std::cout << "EXIF date: " << exif.date() << std::endl;
            }
        }
    }
}
