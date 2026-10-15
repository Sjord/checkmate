#!/usr/bin/env python3
"""Add APE tags to a test file.

This generates fixtures for mpck's APE tag detection.  Mutagen only supports
writing APEv2 tags, so an APEv1 tag is assembled here from mutagen's item
serialization plus a footer-only tag header.

Usage:
    addapetags.py [--apev1] [--apev2] INPUT OUTPUT

At least one of --apev1/--apev2 must be given; when neither is, both are
added.  INPUT is copied to OUTPUT, which is then tagged in place.
"""

import argparse
import shutil
import struct
import sys

try:
    from mutagen.apev2 import APETextValue, APEv2
except ImportError:
    sys.stderr.write("addapetags.py: the mutagen module is required\n")
    sys.exit(2)

APE_PREAMBLE = b"APETAGEX"
APE_VERSION_V1 = 1000
APE_FOOTER_SIZE = 32


def _ape_item(key, value):
    """Serialize a single text item the way mutagen does."""
    data = APETextValue(value, 0)._write()
    return struct.pack("<2I", len(data), 0) + key.encode("utf-8") + b"\0" + data


def write_apev2(path):
    """Write an APEv2 tag using mutagen's own writer (header + footer)."""
    tag = APEv2()
    tag["Title"] = "mpck test"
    tag["Artist"] = "mpck"
    tag["Comment"] = "APEv2 test fixture"
    tag.save(path)


def write_apev1(path):
    """Write a footer-only APEv1 tag.

    The last item value deliberately ends in characters that are neither
    'A', 'T' nor 'I': mpck scans for tags byte by byte and would otherwise
    mistake an earlier 'A' for the start of the "APETAGEX" preamble.
    """
    items = b"".join([
        _ape_item("Title", "mpck test"),
        _ape_item("Artist", "mpck"),
        _ape_item("Comment", "apev1 fixture ends here"),
    ])
    footer = bytearray(APE_PREAMBLE)
    footer += struct.pack("<4I", APE_VERSION_V1, len(items) + APE_FOOTER_SIZE,
                          3, 0)
    footer += b"\0" * 8

    with open(path, "ab") as tagfile:
        tagfile.write(items)
        tagfile.write(footer)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--apev1", action="store_true",
                        help="add an APEv1 tag")
    parser.add_argument("--apev2", action="store_true",
                        help="add an APEv2 tag")
    parser.add_argument("input", help="source MP3 file")
    parser.add_argument("output", help="file to write the tagged copy to")
    args = parser.parse_args()

    if not args.apev1 and not args.apev2:
        args.apev1 = args.apev2 = True

    shutil.copyfile(args.input, args.output)
    if args.apev2:
        write_apev2(args.output)
    if args.apev1:
        write_apev1(args.output)


if __name__ == "__main__":
    main()
