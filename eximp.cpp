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

    // Recurse the path to find files
    module mod("path");
    path* p = create(mod, dir);
    var rdir = p->rdir(true);

    // Try to load EXIF data
    for (int i=0; i<rdir.size(); i++)
    {
        // Load
        var f = rdir.key(i);
        std::cout << f.str() << " ";

        // Is it even a photo?
        EXIF exif(f);
        if (!exif.valid())
        {
            // If there is no EXIF there's no point trying to continue
            std::cout << "no EXIF" << std::endl;
            continue;
        }

        // It's a photo, can we infer the date from exif data?  Or from the
        // filename?
        var d = exif.dateArray(rdir[i]);
        var s;
        if (!d)
        {
            std::cout << "Date: Failed" << std::endl;
            continue;
        }
        else
            s = exif.date(d);

        // Try for make and model
        // ...this seems way too long
        varstream mm;
        var m = exif.entry(EXIF_TAG_MAKE);
        if (m)
            mm << m.str();
        var n = exif.entry(EXIF_TAG_MODEL);
        if (n)
        {
            // Models don't need makes
            n.replace("Canon ", "");
            if (m)
                mm << " ";
            mm << n.str();
        }
        if (!m && !n)
            mm << "Unknown";
        m = var(mm).replace(" ", "-");

        // Result
        std::cout << "-> " << m.str() << "/" << s.str() << std::endl;
    }

    return 0;
}
