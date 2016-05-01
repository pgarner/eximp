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

bool verbose = false;

/**
 * Convert an array (of strings) to an ISO 8601 string, i.e.,
 *  extended: 2016-04-17T09:09:48Z
 *     basic: 20160417T090948Z
 *
 * The colons cause trouble on a FAT filesystem, in which case the answer
 * is to use the "basic format" - the same string without hyphens or colons;
 * that's still an ISO standard date.
 */
var arrayToISODate(var iArray, bool iBasic=false)
{
    // How long is it?
    int s = iArray.size();
    if (s > 6)
        s = 6;

    // Format to a stream
    const char* dsep = iBasic ? "" : "-";
    const char* tsep = iBasic ? "" : ":";
    varstream iso;
    switch (s)
    {
    case 6:
        // The most likely; three date and three time fields
        iso << iArray[0].str() << dsep
            << iArray[1].str() << dsep
            << iArray[2].str();
        iso << "T";
        iso << iArray[3].str() << tsep
            << iArray[4].str() << tsep
            << iArray[5].str();
        iso << "Z";
        break;
        //default:
        // throw lube::error("Not a valid date size");
    }

    // Pull the var out of the stream
    return var(iso);
}

/**
 * Pull an array of strings out of a single string representing a date.
 */
var dateMatch(var iStr)
{
    // Dropbox files are 2012-12-02 04.08.18
    // ISO format is     2012-12-02T04:08:18Z
    //            or     20121202T040818Z
    //
    // Use search rather than match to skip leading crap.  There must be a
    // better syntax than this; \\\\. is to escape both the compiler and the RE
    // parser.  Literal '-' must be last in a [] expression.
    var array = iStr.search(
        "(\\d\\d\\d\\d)[:-]?(\\d\\d)[:-]?(\\d\\d)"
        "[T_ -]?(\\d\\d)[:\\\\.-]?(\\d\\d)[:\\\\.-]?(\\d\\d)Z?"
    );

    // There should be 6 matches in an array of length 7
    if (array.size() != 7)
        return nil;
    array.shift();
    return array;
}

/**
 * Get the date, make and model from an EXIF record
 */
var exifData(var iPath)
{
    // Is it even a photo?
    // If there is no EXIF there's no point trying to continue
    EXIF exif(iPath);
    if (!exif.valid())
        return nil;
    if (verbose)
    {
        std::cout << std::endl;
        exif.dump();
    }

    // It's a photo, can we infer the date from exif data?
    var da = exif.date();
    if (!da)
    {
        std::cout << " [no EXIF date]";
        return nil;
    }

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
        n.replace("KODAK ", "");
        if (m)
            mm << " ";
        mm << n.str();
    }
    if (!m && !n)
        mm << "Unknown";
    m = var(mm).replace(" ", "-");

    // The metadata is date array plus make-model string
    var meta;
    meta[0] = da;
    meta[1] = m;
    return meta;
}

/**
 * Look up EXIF information to convert a path of a photo to a path in an
 * archive.
 *
 * Returns an array with three components: path, stem and extension.
 */
var target(var iPrefix, var iPath, var iBit)
{
    // First try for EXIF data
    var meta = exifData(iPath);
    if (!meta)
    {
        // Fall back on the file name
        meta[1] = "Unknown";
        meta[0] = dateMatch(iBit[1]);
        if (!meta[0])
            return nil;
    }
    var year = meta[0][0].copy();
    var month = meta[0][1].copy();

    // Check for a label after the month
    var label = iPath.search("\\/\\d\\d(-\\S+)\\/");
    if (label)
        month.append(label[1]);

    // Result
    var path;
    path.push(iPrefix);
    path.push(year);
    path.push(month);
    path.push(meta[1]);
    var ret;
    ret[2] = iBit[2].tolower().str();
    ret[1] = arrayToISODate(meta[0], true);
    ret[0] = path.join("/");
    return ret;
}

void usage()
{
    std::cerr <<
        "Usage: eximp [-r] <dir>\n" <<
        " dir: directory (or file) to import\n" <<
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
            case 'v':
                verbose = true;
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

    // Recurse the paths to find files
    for (int d=0; d<dir.size(); d++)
    {
        module mod("path");
        path* p = create(mod, dir[d]);
        var rdir = p->rdir(true);
        for (int i=0; i<rdir.size(); i++)
        {
            var s = rdir.key(i);

            // Check it's actually a file, not a directory
            if (fs::is_directory(s.str()))
                continue;
            std::cout << s.str();

            // Convert to target path
            var t = target(prefix, s, rdir[i]);
            if (!t)
            {
                std::cout << " [no usable metadata]" << std::endl;
                continue;
            }

            // Create path
            if (really)
                fs::create_directories(t[0].str());
            var ext = t.pop();
            t = t.join("/");
            t.append(ext);

            // Check for overwrite; eximp should never overwrite
            if (fs::exists(t.str()))
            {
                std::cout << " [target " << t.str() << " exists]" << std::endl;
                continue;
            }

            // Result
            std::cout << " -> " << t.str() << std::endl;
            if (really)
                fs::rename(s.str(), t.str());
        }
    }

    return 0;
}
