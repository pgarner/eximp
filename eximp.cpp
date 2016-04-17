/*
 * Copyright 2015 by Philip N. Garner
 *
 * See the file COPYING for the licence associated with this software.
 *
 * Author(s):
 *   Phil Garner, September 2015
 */

#include <lube.h>
#include <lube/path.h>

#include "exif.h"

using namespace lube;

int main(int argc, char** argv)
{
    // Get a path
    if (argc <= 1)
        throw error("not enough args");
    var dir = argv[1];
    std::cout << dir << std::endl;

    // Recurse the path to find files
    module mod("path");
    path* p = create(mod, dir);
    var rdir = p->rdir(true);
    std::cout << rdir << std::endl;

    // Try to load EXIF data
    for (int i=0; i<rdir.size(); i++)
    {
        // Load
        var f = rdir.key(i);
        std::cout << "Loading: " << f << std::endl;

        // Is it even a photo
        EXIF exif(f);
        if (!exif.valid())
        {
            std::cout << "EXIF Failed: " << f << std::endl;
            continue;
        }

        // It's a photo, can we infer the date from exif data?  Or from the
        // filename?
        var d = exif.date(rdir[i]);
        if (d)
            std::cout << "EXIF Date: " << d << std::endl;
        else
        {
            // No; dump it.
            std::cout << "Date: Failed" << std::endl;
            exif.dump();
        }

        // Try for make and model
        var m = exif.entry(EXIF_TAG_MAKE);
        var n = exif.entry(EXIF_TAG_MODEL);
        std::cout << m << "-" << n << std::endl;
    }

    return 0;
}
