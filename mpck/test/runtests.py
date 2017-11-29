#!/usr/bin/env python3

import subprocess
from tempfile import NamedTemporaryFile
import re
import inspect
import struct

class TestResult:
    def __init__(self, mpck_result, assertion):
        self.mpck_result = mpck_result
        self.assertion = assertion

    def __str__(self):
        if self.mpck_result.has_segfaulted():
            return "\033[91m segfault \033[0m"

        if self.assertion:
            return "\033[92m ok \033[0m"
        else:
            return "\033[91m fail \033[0m"

    def __invert__(self):
        return TestResult(self.mpck_result, not self.assertion)


class MpckResult:
    def __init__(self, mp3data, exec_result):
        self.result = self._parse_mpck_output(exec_result.stdout.decode('utf-8'))
        self.returncode = exec_result.returncode
        self.mp3data = mp3data

    def _parse_mpck_output(self, output):
        out_dict = {}
        for line in output.split("\n"):
            m = re.match("\s*(.*?)          *(.*)", line)
            if m:
                out_dict[m.group(1)] = m.group(2)
        return out_dict

    def returncode_is_ok(self):
        return self.returncode >= 0

    def has_id3v1_tag(self):
        return TestResult(self, self.result.get("ID3V1") == "yes")

    def has_apev2_tag(self):
        return TestResult(self, self.result.get("APEV2") == "yes")

    def has_segfaulted(self):
        return self.returncode == -11


def mpck(mp3data):
    with NamedTemporaryFile(suffix="mp3") as mp3file:
        mp3file.write(mp3data)
        mp3file.seek(0)
        result = subprocess.run(["../mpck", "-v", mp3file.name], stdout=subprocess.PIPE)
        return MpckResult(mp3data, result)


def get_test_name(assertion):
    previous_frame = inspect.currentframe().f_back.f_back
    (filename, line_number, function_name, lines, index) = inspect.getframeinfo(previous_frame)
    return "test at line %d: %s" % (line_number, str(assertion.mpck_result.mp3data)[2:18])


def test(assertion):
    name = get_test_name(assertion)
    print("%-40s %s" % (name, assertion))


if __name__ == "__main__":
    tag = b"TAG" + 125 * b"a"
    test(mpck(tag).has_id3v1_tag())
    test(mpck(b"T" + tag).has_id3v1_tag())
    test(mpck(b"TA" + tag).has_id3v1_tag())

    ape = b"APETAGEX" + struct.pack("<I", 2000) + struct.pack("<I", 0) + struct.pack("<I", 0) + b"\xa0\0\0\0"
    test(mpck(ape).has_apev2_tag())
    test(mpck(b"APE" + ape).has_apev2_tag())
    test(mpck(b"APETAG" + ape).has_apev2_tag())
    test(~mpck(b"APETAGXX" + ape[8:]).has_apev2_tag())

    not_ape = b"APETAGE*"
    for i in range(1, len(not_ape) + 1):
        test(~mpck(not_ape[:i] * 20).has_apev2_tag())
