#!/usr/bin/env python3

import subprocess
from tempfile import NamedTemporaryFile
import re
import inspect
import struct


class MpckResult:
    def __init__(self, mp3data, exec_result):
        self.stdout = exec_result.stdout.decode('utf-8')
        self.result = self._parse_mpck_output(self.stdout)
        self.returncode = exec_result.returncode
        self.mp3data = mp3data
        self.assertions = []

    def _parse_mpck_output(self, output):
        out_dict = {}
        for line in output.split("\n"):
            m = re.match("\s*(.*?)          *(.*)", line)
            if m:
                out_dict[m.group(1)] = m.group(2)
        return out_dict

    def _assert(self, name, result):
        self.assertions.append((name, result))

    def has_id3v1_tag(self):
        self._assert("ID3v1", self.result.get("ID3V1") == "yes")
        return self

    def has_apev2_tag(self, has=True):
        self._assert("APEv2", has == (self.result.get("APEV2") == "yes"))
        return self

    def _has_segfaulted(self):
        return self.returncode == -11

    def has_frames(self, n):
        self._assert("frames=%d" % n, self.result.get("frames") == str(n))
        return self

    def has_unidentified(self, n):
        self._assert("unidentified=%d" % n, self.result.get("unidentified").startswith(str(n) +" b"))
        return self

    def __str__(self):
        if self._has_segfaulted():
            return "\033[91m segfault \033[0m"

        if not self.assertions:
            return "\033[93m unknown \033[0m"

        if all([res for name, res in self.assertions]):
            return "\033[92m ok \033[0m"
        else:
            return "\033[91m fail \033[0m (on %s)" % ", ".join([name for name, res in self.assertions if not res])


def mpck(mp3data):
    with NamedTemporaryFile(suffix="mp3") as mp3file:
        mp3file.write(mp3data)
        mp3file.seek(0)
        result = subprocess.run(["../mpck", "-v", mp3file.name], stdout=subprocess.PIPE)
        return MpckResult(mp3data, result)


def get_test_name(assertion, extra_info):
    previous_frame = inspect.currentframe().f_back.f_back
    (filename, line_number, function_name, lines, index) = inspect.getframeinfo(previous_frame)
    extra_str = ", ".join([key + "=" + str(value) for key, value in extra_info.items()])
    return "test at line %3d: %s %s" % (line_number, str(assertion.mp3data)[2:18].rstrip("'"), extra_str)


def test(assertion, **kwargs):
    name = get_test_name(assertion, kwargs)
    print("%-40s %s" % (name, assertion))


if __name__ == "__main__":
    tag = b"TAG" + 125 * b"a"
    test(mpck(tag).has_id3v1_tag().has_unidentified(0))
    test(mpck(b"T" + tag).has_id3v1_tag().has_unidentified(1))
    test(mpck(b"TA" + tag).has_id3v1_tag().has_unidentified(2))

    ape = b"APETAGEX" + struct.pack("<I", 2000) + struct.pack("<I", 0) + struct.pack("<I", 0) + b"\xa0\0\0\0"
    test(mpck(ape).has_apev2_tag())
    test(mpck(b"APE" + ape).has_apev2_tag().has_unidentified(3))
    test(mpck(b"APETAG" + ape).has_apev2_tag().has_unidentified(6))
    test(mpck(b"APETAGXX" + ape[8:]).has_apev2_tag(False))

    not_ape = b"APETAGE*"
    for i in range(1, len(not_ape) + 1):
        test(mpck(not_ape[:i] * 20).has_apev2_tag(False))

    # MPEG 1 layer 3 frame, 128kbs, 44100 Hz, 417b
    frame = b"\xff\xfb\x90\0" + b"A" * 413
    test(mpck(frame).has_frames(1).has_unidentified(0))

    test(mpck(frame + frame).has_frames(2).has_unidentified(0))

    garbage = b"U"

    for i in range(10):
        test(mpck(frame + garbage * i).has_unidentified(i), i=i)

    for i in range(10):
        test(mpck(frame + garbage * i + frame).has_unidentified(i).has_frames(2), i=i)

    for i in range(1, 4):
        test(mpck(frame[:i]).has_frames(0))
