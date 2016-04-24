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
#include <boost/filesystem/operations.hpp>
#include <wordexp.h>

#include "exif.h"

using namespace lube;
namespace fs = boost::filesystem;

/**
 * Look up EXIF information to convert a path of a photo to a path in an
 * archive.
 *
 * Returns an array with three components: path, stem and extension.
 */
var target(var iPrefix, var iPath, var iBit)
{
    // Is it even a photo?
    // If there is no EXIF there's no point trying to continue
    EXIF exif(iPath);
    if (!exif.valid())
        return nil;

    // It's a photo, can we infer the date from exif data?  Or from the
    // filename?
    var da = exif.dateArray(iBit[1]);
    if (!da)
    {
        std::cout << "Date: Failed" << std::endl;
        return nil;
    }
    var date = exif.date(da);
    var year = da[0];
    var month = da[1];

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
        n.replace("HTC ", "");
        n.replace("HTC_", "");
        n.replace("BlackBerry ", "");
        n.replace("NIKON ", "");
        if (m)
            mm << " ";
        mm << n.str();
    }
    if (!m && !n)
        mm << "Unknown";
    m = var(mm).replace(" ", "-");

    // Check for a label after the month
    var label = iPath.search("\\/\\d\\d(-\\S+)\\/");
    if (label)
        month.append(label[1]);

    // Result
    var path;
    path.push(iPrefix);
    path.push(year);
    path.push(month);
    path.push(m);
    var ret;
    ret[2] = iBit[2].tolower().str();
    ret[1] = date.str();
    ret[0] = path.join("/");
    return ret;
}

void usage()
{
    std::cerr <<
        "Usage: eximp [-r] <dir>\n" <<
        " dir: directory to import\n" <<
        "  -r: Really do it instead of reporting what would be done\n" <<
        "Nothing is actually moved unless -r is specified\n";
    exit(1);
}

int main(int argc, char** argv)
{
    // Use wordexp to expand the tilde in the prefix.  wordexp is cool; it
    // should be in lube!
    var prefix = "~/Pictures/Import";
    wordexp_t we;
    wordexp(prefix.str(), &we, 0);
    var wev(we.we_wordc, we.we_wordv);
    prefix = wev[0];
    wordfree(&we);

    // Command line
    var dir;
    bool really = false;
    for (int i=1; i<argc; i++)
    {
        if (argv[i][0] == '-')
            // It's a switch
            switch (argv[i][1])
            {
            case 'r':
                really = true;
                break;
            default:
                usage();
            }
        else
            // It's a directory
            dir.push(argv[i]);
    }

    // Check we have a path
    if (dir.size() < 1)
        usage();

    // Recurse the path to find files
    module mod("path");
    path* p = create(mod, dir[0]); // Just one for now
    var rdir = p->rdir(true);
    for (int i=0; i<rdir.size(); i++)
    {
        var s = rdir.key(i);

        // Check it's actually a file, not a directory
        if (fs::is_directory(s.str()))
            continue;
        std::cout << s.str() << " ";

        // Convert to target path
        var t = target(prefix, s, rdir[i]);
        if (!t)
        {
            std::cout << "no EXIF" << std::endl;
            continue;
        }

        // Create path
        fs::create_directories(t[0].str());
        var ext = t.pop();
        t = t.join("/");
        t.append(ext);

        // Check for overwrite; eximp should never overwrite
        if (fs::exists(t.str()))
        {
            std::cout << "target exists" << std::endl;
            continue;
        }

        // Result
        std::cout << "-> " << t.str() << std::endl;
        if (really)
            fs::rename(s.str(), t.str());
    }

    return 0;
}
