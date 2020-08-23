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
#include <lube/config.h>
#include <boost/filesystem/operations.hpp>
#include <wordexp.h>

#include "exif.h"
#include "heif.h"
#include "png.h"

#ifdef HAVE_AVFORMAT
# include "avformat.h"
#endif

// #include "isobm.h"
// #include "mp4v2.h"

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

/*
 * Format the exif make model information
 */
var catMakeModel(var iMake, var iModel)
{
    // Try for make and model
    // ...this seems way too long
    varstream mm;
    if (iMake)
        mm << iMake.str();
    if (iModel)
    {
        // Models don't need makes
        iModel.replace("Canon ", "");
        iModel.replace("HTC ", "");
        iModel.replace("HTC_", "");
        iModel.replace("BlackBerry ", "");
        iModel.replace("NIKON ", "");
        iModel.replace("KODAK ", "");
        if (iMake)
            mm << " ";
        mm << iModel.str();
    }
    if (!iMake && !iModel)
        mm << "Unknown";
    var m = var(mm).replace(" ", "-");
    return m;
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

    // Make & model
    var m = exif.entry(EXIF_TAG_MAKE);
    var n = exif.entry(EXIF_TAG_MODEL);
    m = catMakeModel(m, n);

    // The metadata is date array plus make-model string
    var meta;
    meta[0] = da;
    meta[1] = m;
    return meta;
}

/**
 * Get the date from a PNG file
 */
var pngData(var iPath)
{
    // Is it a PNG file?
    PNG png(iPath);
    if (!png.valid())
        return nil;
    if (verbose)
    {
        std::cout << std::endl;
        png.dump();
    }

    // It's a picture, can we infer the date?
    var da = png.date();
    if (!da)
    {
        std::cout << " [no PNG date]";
        return nil;
    }
    var meta;
    meta[0] = da;
    meta[1] = "Unknown";
    return meta;
}

/**
 * Read a HEIF file
 */
var heifData(var iPath)
{
    HEIF heif(iPath);
    if (!heif.valid())
        return nil;
    if (verbose)
    {
        std::cout << std::endl;
        heif.dump();
    }

    // It's a picture, can we infer the date?
    var da = heif.date();
    if (!da)
    {
        std::cout << " [no HEIF date]";
        return nil;
    }

    // Make & model
    var m = heif.entry(EXIF_TAG_MAKE);
    var n = heif.entry(EXIF_TAG_MODEL);
    m = catMakeModel(m, n);

    var meta;
    meta[0] = da;
    meta[1] = m;
    return meta;
}

#if 0
/**
 * Get the date from an ISO file
 */
var isoData(var iPath)
{
    ISOBM isobm(iPath);
    if (!isobm.valid())
        return nil;
    if (verbose)
    {
        std::cout << std::endl;
        isobm.dump();
    }

    // Meta
    var da = isobm.date();
    if (!da)
    {
        std::cout << " [no ISO date]";
        return nil;
    }
    var meta;
    meta[0] = da;
    meta[1] = "Unknown";
    return meta;
}
#endif

#ifdef HAVE_AVFORMAT
/**
 * Get the date and encoder from an AVFormat record
 */
var avData(var iPath)
{
    AVFormat av(iPath);
    if (!av.valid())
        return nil;
    if (verbose)
    {
        std::cout << std::endl;
        av.dump();
    }

    // Date is in the AV file
    var date = av.date();
    if (!date)
    {
        std::cout << " [no AV date]";
        return nil;
    }
    var da = dateMatch(date);

    // Recently, on iPhone at least, make & model exist
    var mm;
    mm[0] = av.tag("make");
    mm[1] = av.tag("model");
    if (mm[0])
        mm = mm.join("-");
    else
        // Fall back to 'encoder'
        mm = av.tag("encoder");
    if (!mm)
        mm = "Unknown";

    // Date array plus make-model
    var meta;
    meta[0] = da;
    meta[1] = var(mm).replace(" ", "-");
    return meta;
}
#endif

#if 0
/**
 * Get the date from an MP4 file
 */
var mp4Data(var iPath)
{
    MP4 mp4(iPath);
    if (!mp4.valid())
        return nil;
    if (verbose)
    {
        std::cout << std::endl;
        mp4.dump();
    }

    // Meta
    var meta = 0;
    return meta;
}
#endif


/**
 * Look up information to convert a path of a photo to a path in an archive.
 *
 * Returns an array with three components: path, stem and extension.
 */
var target(var iPrefix, var iPath, var iBit)
{
    // First try for EXIF data
    var meta = exifData(iPath);
    if (!meta)
        // Try PNG data
        meta = pngData(iPath);
    if (!meta)
        // Try HEIF data
        meta = heifData(iPath);
#ifdef HAVE_AVFORMAT
    if (!meta)
        // Try AVFormat data
        meta = avData(iPath);
#endif
#if 0
    if (!meta)
        // Try ISO data
        meta = isoData(iPath);
    if (!meta)
        // Try MP4 data
        meta = mp4Data(iPath);
#endif
    if (verbose)
        std::cout << "Meta: " << meta << std::endl;
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
    var label = iPath.search("\\/\\d\\d(-[^\\/]+)\\/");
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
    Option o("EXIMP: metadata based media importer");
    o(" <args> is a list of directories (at least one) that are recursed");
    o('t', "Import target directory", prefix);
    o('r', "Rename files; only works on the same filesystem");
    o('c', "Copy files; works across filesystems");
    o('m', "Move files; works across filesystems using copy then remove");
    o('v', "Be verbose; dump EXIF records and the like");
    o('n', "Append version numbers to files (deals with duplicates)");
    o('d', "Write out duplicates in the form of a diff command");
    o("Nothing is actually done unless -r, -c or -m are specified");
    o.parse(argc, argv);
    var dir = o.args();
    verbose = o['v'].defined();

    // Check for nonsense options
    int n = 0;
    if (o['r']) n++;
    if (o['c']) n++;
    if (o['m']) n++;
    if (n > 1)
        o.usage(1);

    // Check we have a path
    if (dir.size() < 1)
        o.usage(1);

    // Recurse the paths to find files
    pathmodule mod;
    for (int d=0; d<dir.size(); d++)
    {
        path& p = mod.create(dir[d]);
        var rdir = p.rdir(true);
        for (int i=0; i<rdir.size(); i++)
        {
            var s = rdir.key(i);

            // Check it's actually a file, not a directory
            if (fs::is_directory(s.str()))
                continue;
            std::cout << s.str();

            // Convert to target path
            var base = target(o['t'], s, rdir[i]);
            if (!base)
            {
                std::cout << " [no usable metadata]" << std::endl;
                continue;
            }

            // Create path as base and extension
            if (o['r'] || o['c'] || o['m'])
                fs::create_directories(base[0].str());
            var ext = base.pop();
            base = base.join("/");

            // Check if the unversioned or zero-versioned file exists and keep
            // the path
            var path = base.copy();
            path.append("-0");
            path.append(ext);
            bool exists = fs::exists(path.str());
            if (!exists)
            {
                path = base.copy();
                path.append(ext);
                exists = fs::exists(path.str());
            }

            // Check for overwrite; eximp should never overwrite
            if (!o['n'] && exists)
            {
                if (o['d'])
                    std::cout
                        << " [exists]\n"
                        << "diff " << s << " " << path
                        << std::endl;
                else
                    std::cout
                        << " [target " << path.str() << " exists]"
                        << std::endl;
                continue;
            }

            // Build a versioned path
            if (o['n'])
            {
                int version = exists ? 1 : 0;
                do
                {
                    if (version > 99)
                    {
                        std::cout
                            << " Probable error: version number > 99"
                            << std::endl;
                        exit(1);
                    }
                    path = base.copy();
                    varstream v;
                    v << "-" << version++;
                    path.append(v);
                    path.append(ext);
                }
                while (fs::exists(path.str()));
            }

            // Result
            std::cout << " -> " << path.str() << std::endl;
            if (o['r'])
                fs::rename(s.str(), path.str());
            else if (o['c'])
                fs::copy(s.str(), path.str());
            else if (o['m'])
            {
                boost::system::error_code ec;
                fs::copy(s.str(), path.str(), ec);
                if (ec.value() == 0)
                    fs::remove(s.str());
                else
                    std::cout << "Error: " << ec.message() << std::endl;
            }
        }
    }

    return 0;
}
