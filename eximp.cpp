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
    var rdir = p->rdir();
    std::cout << rdir << std::endl;

    // Try to load EXIF data
    for (int i=0; i<rdir.size(); i++)
    {
        var f = rdir.key(i);
        std::cout << "Loading: " << f << std::endl;
        EXIF exif(f);
        if (!exif.valid())
            continue;
        exif.dump();
        var d = exif.date();
        std::cout << "Date: " << d << std::endl;
    }

    return 0;
}
