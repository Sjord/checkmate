# Checkmate MP3 Checker

A program to check MP3 files for errors. Scans MP3 files to verify frame
placement, frame header correctness, and header consistency throughout the
file. Since MP3 is a relatively lenient file format, this is a best-effort
tool: it reports whether a file is likely good or bad, not a definitive
bit-exact validation.

The project was started about 21 years ago and was already old at that time.
The Windows version was originally built with Visual C++ 6.

## Repository layout

- `mpck/` -- the command-line checker (the core of the project)
- `mp3checker/windows/` -- Windows Explorer-style GUI, uses the same core
- `vc++/` -- old Visual C++ 6 project files (`.dsw`, `.dsp`, `.plg`)
- `libgnugetopt-1.2/` -- bundled getopt implementation for platforms that
  lack it
- `debian/` -- Debian packaging
- `mpck.1` -- man page for mpck
- `autogen.sh`, `configure.ac`, `Makefile.am` -- autotools build system

## Build

autotools-based. If `configure` is absent, run `./autogen.sh` first.

```
./configure
make
make install
```

The Windows GUI is not built by autotools; it has its own Visual Studio
project files under `vc++/mp3checker/` (and `vc++/mpck/` for the CLI on
Windows).

## mpck source files (`mpck/`)

| File | Role |
|------|------|
| `main.c` | entry point, option parsing |
| `options.c/h` | global command-line options (macro-generated getters/setters) |
| `mpck.h` | shared constants, types, platform compat (`TRUE`/`FALSE`, `errno_t`, `DIRSEP`) |
| `bufio.c/h` | buffered I/O abstraction (`CFILE`); uses `mmap` on Unix, `ReadFile` on Windows |
| `bitfile.c/h` | bit-level reader over `CFILE` (used for layer 2 CRC) |
| `file.c/h` | per-file state: frame counts, bitrates, tag detection, error flags |
| `frame.c/h` | per-frame state: header fields parsed from the 4-byte sync header |
| `checkfile.c` | top-level per-file check: opens file, finds last frame, scans forward |
| `checkframe.c` | core: parses a frame header, validates fields, checks CRC16, checks consistency vs. previous frame |
| `checkarguments.c/h` | walks argv, recurses directories, dispatches to `checkfile` |
| `metatag.c/h` | detects and skips ID3v1, ID3v2, APEv1/v2 tags |
| `xing.c/h` | reads the Xing/Info and LAME header from the first frame |
| `crc.c/h` | CRC16 implementation (polynomial `0x8005`), supports bit-aligned reads |
| `layer2.c/h` | CRC data-length calculation for MPEG layer 2 |
| `matrices.h` | lookup tables: bitrate, samplerate, samples-per-frame by version/layer |
| `filename.c/h` | filename sanity checks (length, strange characters) |
| `total.c/h` | aggregate stats across all scanned files |
| `print.c/h` | all output: per-file summary, totals, errors, XML, version, usage |
| `mp3error.h` | error-code table (flag, short code, long description) |

## How checking works

1. Open the file, determine total size.
2. Search backwards from the end (up to 16 KB) to find the last valid frame.
   This lets the tool handle leading junk (ID3v2 tags, etc.).
3. Scan forward from the start. For each byte:
   - `0xFF` + two bytes with top 3 bits set: possible MP3 frame header.
     Parse and validate it.
   - `TAG` / `ID3` / `APETAGEX`: skip the corresponding metadata tag.
   - Anything else: count as "alien" (unidentified) bytes.
4. For each candidate frame header:
   - **Validity**: version, layer, bitrate, samplerate must be in-range.
   - **CRC16**: if the protection bit is set, compute CRC over the specified
     number of bits and compare.
   - **Consistency**: MPEG version and layer must match the previous valid
     frame (bitrate may vary -- VBR is allowed).
   - **Xing/LAME header**: on the first valid frame, read the Xing or Info
     header and the LAME extension. Its frame count, byte count and
     MusicLength are compared with the actual frames and total frame length.
     The LAME MusicCRC is checked against the music data after the tag frame.
5. Report per-file summary and aggregate totals.

## Error model

Error flags are bitmask values in `mpck.h` (`ERR_NONE` through
`ERR_BADFNAME`). A file is "bad" if any flag is set. Human-readable
descriptions live in `mp3errors.h`.

## Exit status

| Code | Meaning |
|------|---------|
| 0 | All files good |
| 1 | At least one file bad |
| 2 (ENOENT) | File/directory not found |
| 4 (EINTR) | SIGINT received |
| 5 (EIO) | Read error |
| 12 (ENOMEM) | Malloc failed |
| 21 (EISDIR) | Argument is a directory (non-recursive mode) |
| 22 (EINVAL) | No filename supplied / invalid argument |
| 36 (ENAMETOOLONG) | Extension or filename too long |

## Code conventions

- C89/C99 style, no C++ constructs in the `mpck/` or `mp3checker/` code.
- Tab indentation (not spaces).
- Every source and header file begins with a GPL v2+ copyright notice block
  followed by a one-line description comment.
- Include guards use `__NAME_H__` (double underscore) for most files,
  `_MPCK_H` for `mpck.h`.
- `TRUE`/`FALSE` are defined as `1`/`0` in `mpck.h` (not `bool`).
- Platform differences are handled with `#ifdef _WIN32` blocks in `mpck.h`
  and `bufio.c`, not in separate platform-specific source files (except the
  Windows GUI).
- `options.c/h` uses a macro (`OPTION`) to auto-generate
  `options_set_*`/`options_get_*` pairs; to add a new option, add one line
  to `options.h`.
- There is a `// FIXME` comment in `checkframe.c` about freeform bitrate
  being ignored in validity checks, and another about returning the number
  of uninterpreted bytes from `findframe`.

## Testing

The test suite is run with:

```
make check
```

It generates its MP2/MP3 fixtures at test time (nothing is committed) and
checks mpck's output on known good and known bad files. The generated files
live in `mpck/test/data/` (gitignored) and can be cached between CI runs;
`mpck/test/run-tests.sh --rebuild` regenerates them.

- `mpck/test/gentestdata.sh` generates the fixtures: valid MP2/MP3 files with
  `twolame` and `lame` (using `sox` for the PCM source) for every bitrate and
  samplerate, both with and without CRC protection, plus ID3- and APE-tagged
  variants and the known-broken files. Unsupported encoder combinations are
  skipped.
- `mpck/test/addapetags.py` writes APEv1 and APEv2 tags to a file using
  mutagen.
- `mpck/test/run-tests.sh` runs mpck against the fixtures and checks the
  output.
- `mpck/test/testlib.sh` is a small POSIX shell test framework.

The tests require `lame`, `twolame` and `sox`; `configure` only hooks them
into `make check` when all three are present. `python3-mutagen` is used for
the APE tag fixtures; without it those specific tests are skipped.

## License

GNU GPL v2 or later (see `COPYING`).

## Key maintainer

Sjoerd Langkemper <sjoerd-mpck@linuxonly.nl>
