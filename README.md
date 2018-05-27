# EXIF based photo importer

## Overview

Eximp moves photos, and other media from cameras and phones, into a media
library.  It stores the files named as a creation date in ISO format, and with
a sensible year/month directory structure.

Eximp will recurse down directory structures similar (or not) to the ones it
creates.  In this sense it will work on archives created by other importers.

If the photo is a JPEG file, eximp uses the EXIF metadata to discern the date
and also constructs a make-model string.  If it's not JPEG, it tries to use
libavformat to find the metadata.  For other formats with no metadata, eximp
discerns a date from the filename if possible.  This covers most cases where
photos come from camera memory or a Dropbox folder.

## Technical blurb

There are also readers for PNG and MP4 file formats.  In practice I never got
any joy from these.  PNG typically doesn't contain any useful metadata; that's
to say there are placeholders but the content creators don't use them.  MP4 for
sure has the data, but mp4v2 fails on the test cases I gave it.  Bento4 will
read the files that mp4v2 fails on, but it's GPL so I didn't take it further
given that avformat will read them.

--
[Phil Garner](http://www.idiap.ch/~pgarner)
Earth, early third millenuim.
