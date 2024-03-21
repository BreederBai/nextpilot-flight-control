#!/usr/bin/env python3
# /******************************************************************
#  *      _   __             __   ____   _  __        __
#  *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
#  *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
#  *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
#  *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
#  *
#  * Copyright All Reserved © 2015-2023 NextPilot Development Team
#  ******************************************************************/

#
# Serial firmware uploader for the NextPilot bootloader
#
# The NextPilot firmware file is a JSON-encoded Python object, containing
# metadata fields and a zlib-compressed base64-encoded firmware image.
#
# The uploader uses the following fields from the firmware file:
#
# image
#       The firmware that will be uploaded.
# image_size
#       The size of the firmware in bytes.
# board_id
#       The board for which the firmware is intended.
# board_revision
#       Currently only used for informational purposes.
#

# for python2.7 compatibility
from __future__ import print_function

import sys
import argparse
import binascii
import socket
import struct
import json
import zlib
import base64
import time
import array
import os

from sys import platform as _platform

try:
    import serial
except ImportError as e:
    print("Failed to import serial: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user pyserial")
    print("")
    sys.exit(1)

# Detect python version
if sys.version_info[0] < 3:
    runningPython3 = False
else:
    runningPython3 = True


class FirmwareNotSuitableException(Exception):
    def __init__(self, message):
        super(FirmwareNotSuitableException, self).__init__(message)


class firmware(object):
    """Loads a firmware file"""

    desc = {}
    image = bytes()
    crctab = array.array(
        "I",
        [
            0x00000000,
            0x77073096,
            0xEE0E612C,
            0x990951BA,
            0x076DC419,
            0x706AF48F,
            0xE963A535,
            0x9E6495A3,
            0x0EDB8832,
            0x79DCB8A4,
            0xE0D5E91E,
            0x97D2D988,
            0x09B64C2B,
            0x7EB17CBD,
            0xE7B82D07,
            0x90BF1D91,
            0x1DB71064,
            0x6AB020F2,
            0xF3B97148,
            0x84BE41DE,
            0x1ADAD47D,
            0x6DDDE4EB,
            0xF4D4B551,
            0x83D385C7,
            0x136C9856,
            0x646BA8C0,
            0xFD62F97A,
            0x8A65C9EC,
            0x14015C4F,
            0x63066CD9,
            0xFA0F3D63,
            0x8D080DF5,
            0x3B6E20C8,
            0x4C69105E,
            0xD56041E4,
            0xA2677172,
            0x3C03E4D1,
            0x4B04D447,
            0xD20D85FD,
            0xA50AB56B,
            0x35B5A8FA,
            0x42B2986C,
            0xDBBBC9D6,
            0xACBCF940,
            0x32D86CE3,
            0x45DF5C75,
            0xDCD60DCF,
            0xABD13D59,
            0x26D930AC,
            0x51DE003A,
            0xC8D75180,
            0xBFD06116,
            0x21B4F4B5,
            0x56B3C423,
            0xCFBA9599,
            0xB8BDA50F,
            0x2802B89E,
            0x5F058808,
            0xC60CD9B2,
            0xB10BE924,
            0x2F6F7C87,
            0x58684C11,
            0xC1611DAB,
            0xB6662D3D,
            0x76DC4190,
            0x01DB7106,
            0x98D220BC,
            0xEFD5102A,
            0x71B18589,
            0x06B6B51F,
            0x9FBFE4A5,
            0xE8B8D433,
            0x7807C9A2,
            0x0F00F934,
            0x9609A88E,
            0xE10E9818,
            0x7F6A0DBB,
            0x086D3D2D,
            0x91646C97,
            0xE6635C01,
            0x6B6B51F4,
            0x1C6C6162,
            0x856530D8,
            0xF262004E,
            0x6C0695ED,
            0x1B01A57B,
            0x8208F4C1,
            0xF50FC457,
            0x65B0D9C6,
            0x12B7E950,
            0x8BBEB8EA,
            0xFCB9887C,
            0x62DD1DDF,
            0x15DA2D49,
            0x8CD37CF3,
            0xFBD44C65,
            0x4DB26158,
            0x3AB551CE,
            0xA3BC0074,
            0xD4BB30E2,
            0x4ADFA541,
            0x3DD895D7,
            0xA4D1C46D,
            0xD3D6F4FB,
            0x4369E96A,
            0x346ED9FC,
            0xAD678846,
            0xDA60B8D0,
            0x44042D73,
            0x33031DE5,
            0xAA0A4C5F,
            0xDD0D7CC9,
            0x5005713C,
            0x270241AA,
            0xBE0B1010,
            0xC90C2086,
            0x5768B525,
            0x206F85B3,
            0xB966D409,
            0xCE61E49F,
            0x5EDEF90E,
            0x29D9C998,
            0xB0D09822,
            0xC7D7A8B4,
            0x59B33D17,
            0x2EB40D81,
            0xB7BD5C3B,
            0xC0BA6CAD,
            0xEDB88320,
            0x9ABFB3B6,
            0x03B6E20C,
            0x74B1D29A,
            0xEAD54739,
            0x9DD277AF,
            0x04DB2615,
            0x73DC1683,
            0xE3630B12,
            0x94643B84,
            0x0D6D6A3E,
            0x7A6A5AA8,
            0xE40ECF0B,
            0x9309FF9D,
            0x0A00AE27,
            0x7D079EB1,
            0xF00F9344,
            0x8708A3D2,
            0x1E01F268,
            0x6906C2FE,
            0xF762575D,
            0x806567CB,
            0x196C3671,
            0x6E6B06E7,
            0xFED41B76,
            0x89D32BE0,
            0x10DA7A5A,
            0x67DD4ACC,
            0xF9B9DF6F,
            0x8EBEEFF9,
            0x17B7BE43,
            0x60B08ED5,
            0xD6D6A3E8,
            0xA1D1937E,
            0x38D8C2C4,
            0x4FDFF252,
            0xD1BB67F1,
            0xA6BC5767,
            0x3FB506DD,
            0x48B2364B,
            0xD80D2BDA,
            0xAF0A1B4C,
            0x36034AF6,
            0x41047A60,
            0xDF60EFC3,
            0xA867DF55,
            0x316E8EEF,
            0x4669BE79,
            0xCB61B38C,
            0xBC66831A,
            0x256FD2A0,
            0x5268E236,
            0xCC0C7795,
            0xBB0B4703,
            0x220216B9,
            0x5505262F,
            0xC5BA3BBE,
            0xB2BD0B28,
            0x2BB45A92,
            0x5CB36A04,
            0xC2D7FFA7,
            0xB5D0CF31,
            0x2CD99E8B,
            0x5BDEAE1D,
            0x9B64C2B0,
            0xEC63F226,
            0x756AA39C,
            0x026D930A,
            0x9C0906A9,
            0xEB0E363F,
            0x72076785,
            0x05005713,
            0x95BF4A82,
            0xE2B87A14,
            0x7BB12BAE,
            0x0CB61B38,
            0x92D28E9B,
            0xE5D5BE0D,
            0x7CDCEFB7,
            0x0BDBDF21,
            0x86D3D2D4,
            0xF1D4E242,
            0x68DDB3F8,
            0x1FDA836E,
            0x81BE16CD,
            0xF6B9265B,
            0x6FB077E1,
            0x18B74777,
            0x88085AE6,
            0xFF0F6A70,
            0x66063BCA,
            0x11010B5C,
            0x8F659EFF,
            0xF862AE69,
            0x616BFFD3,
            0x166CCF45,
            0xA00AE278,
            0xD70DD2EE,
            0x4E048354,
            0x3903B3C2,
            0xA7672661,
            0xD06016F7,
            0x4969474D,
            0x3E6E77DB,
            0xAED16A4A,
            0xD9D65ADC,
            0x40DF0B66,
            0x37D83BF0,
            0xA9BCAE53,
            0xDEBB9EC5,
            0x47B2CF7F,
            0x30B5FFE9,
            0xBDBDF21C,
            0xCABAC28A,
            0x53B39330,
            0x24B4A3A6,
            0xBAD03605,
            0xCDD70693,
            0x54DE5729,
            0x23D967BF,
            0xB3667A2E,
            0xC4614AB8,
            0x5D681B02,
            0x2A6F2B94,
            0xB40BBE37,
            0xC30C8EA1,
            0x5A05DF1B,
            0x2D02EF8D,
        ],
    )
    crcpad = bytearray(b"\xff\xff\xff\xff")

    def __init__(self, path):
        # read the file
        f = open(path, "r")
        self.desc = json.load(f)
        f.close()

        self.image = bytearray(zlib.decompress(base64.b64decode(self.desc["image"])))

        # pad image to 4-byte length
        while (len(self.image) % 4) != 0:
            self.image.extend(b"\xff")

    def property(self, propname):
        return self.desc[propname]

    def __crc32(self, bytes, state):
        for byte in bytes:
            index = (state ^ byte) & 0xFF
            state = self.crctab[index] ^ (state >> 8)
        return state

    def crc(self, padlen):
        state = self.__crc32(self.image, int(0))
        for i in range(len(self.image), (padlen - 1), 4):
            state = self.__crc32(self.crcpad, state)
        return state


class uploader(object):
    """Uploads a firmware file to the PX FMU bootloader"""

    # protocol bytes
    INSYNC = b"\x12"
    EOC = b"\x20"

    # reply bytes
    OK = b"\x10"
    FAILED = b"\x11"
    INVALID = b"\x13"  # rev3+
    BAD_SILICON_REV = b"\x14"  # rev5+

    # command bytes
    NOP = b"\x00"  # guaranteed to be discarded by the bootloader
    GET_SYNC = b"\x21"
    GET_DEVICE = b"\x22"
    CHIP_ERASE = b"\x23"
    CHIP_VERIFY = b"\x24"  # rev2 only
    PROG_MULTI = b"\x27"
    READ_MULTI = b"\x28"  # rev2 only
    GET_CRC = b"\x29"  # rev3+
    GET_OTP = b"\x2a"  # rev4+  , get a word from OTP area
    GET_SN = b"\x2b"  # rev4+  , get a word from SN area
    GET_CHIP = b"\x2c"  # rev5+  , get chip version
    SET_BOOT_DELAY = b"\x2d"  # rev5+  , set boot delay
    GET_CHIP_DES = b"\x2e"  # rev5+  , get chip description in ASCII
    MAX_DES_LENGTH = 20

    REBOOT = b"\x30"

    INFO_BL_REV = b"\x01"  # bootloader protocol revision
    BL_REV_MIN = 2  # minimum supported bootloader protocol
    BL_REV_MAX = 5  # maximum supported bootloader protocol
    INFO_BOARD_ID = b"\x02"  # board type
    INFO_BOARD_REV = b"\x03"  # board revision
    INFO_FLASH_SIZE = b"\x04"  # max firmware size in bytes

    PROG_MULTI_MAX = 252  # protocol max is 255, must be multiple of 4
    READ_MULTI_MAX = 252  # protocol max is 255

    NSH_INIT = bytearray(b"\x0d\x0d\x0d")
    NSH_REBOOT_BL = b"reboot -b\n"
    NSH_REBOOT = b"reboot\n"
    MAVLINK_REBOOT_ID1 = bytearray(
        b"\xfe\x21\x72\xff\x00\x4c\x00\x00\x40\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xf6\x00\x01\x00\x00\x53\x6b"
    )
    MAVLINK_REBOOT_ID0 = bytearray(
        b"\xfe\x21\x45\xff\x00\x4c\x00\x00\x40\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xf6\x00\x00\x00\x00\xcc\x37"
    )

    MAX_FLASH_PRGRAM_TIME = 0.001  # Time on an F7 to send SYNC, RESULT from last data in multi RXed

    def __init__(self, portname, baudrate_bootloader, baudrate_flightstack):
        # Open the port, keep the default timeout short so we can poll quickly.
        # On some systems writes can suddenly get stuck without having a
        # write_timeout > 0 set.
        # chartime 8n1 * bit rate is us
        self.chartime = 10 * (1.0 / baudrate_bootloader)

        # we use a window approche to SYNC,<result> gathring
        self.window = 0
        self.window_max = 256
        self.window_per = 2  # Sync,<result>
        self.ackWindowedMode = False  # Assume Non Widowed mode for all USB CDC
        self.port = serial.Serial(portname, baudrate_bootloader, timeout=0.5, write_timeout=0)
        self.otp = b""
        self.sn = b""
        self.baudrate_bootloader = baudrate_bootloader
        self.baudrate_flightstack = baudrate_flightstack
        self.baudrate_flightstack_idx = -1

    def close(self):
        if self.port is not None:
            self.port.close()

    def open(self):
        # upload timeout
        timeout = time.time() + 0.2

        # attempt to open the port while it exists and until timeout occurs
        while self.port is not None:
            portopen = True
            try:
                portopen = self.port.is_open
            except AttributeError:
                portopen = self.port.isOpen()

            if not portopen and time.time() < timeout:
                try:
                    self.port.open()
                except OSError:
                    # wait for the port to be ready
                    time.sleep(0.04)
                except serial.SerialException:
                    # if open fails, try again later
                    time.sleep(0.04)
            else:
                break

    # debugging code
    def __probe(self, state):
        # self.port.setRTS(state)
        return

    def __send(self, c):
        # print("send " + binascii.hexlify(c))
        self.port.write(c)

    def __recv(self, count=1):
        c = self.port.read(count)
        if len(c) < 1:
            raise RuntimeError("timeout waiting for data (%u bytes)" % count)
        # print("recv " + binascii.hexlify(c))
        return c

    def __recv_int(self):
        raw = self.__recv(4)
        val = struct.unpack("<I", raw)
        return val[0]

    def __getSync(self, doFlush=True):
        if doFlush:
            self.port.flush()
        c = bytes(self.__recv())
        if c != self.INSYNC:
            raise RuntimeError("unexpected %s instead of INSYNC" % c)
        c = self.__recv()
        if c == self.INVALID:
            raise RuntimeError("bootloader reports INVALID OPERATION")
        if c == self.FAILED:
            raise RuntimeError("bootloader reports OPERATION FAILED")
        if c != self.OK:
            raise RuntimeError("unexpected response 0x%x instead of OK" % ord(c))

    # The control flow for reciving Sync is on the order of 16 Ms per Sync
    # This will validate all the SYNC,<results> for a window of programing
    # in about 13.81 Ms for 256 blocks written
    def __ackSyncWindow(self, count):
        if count > 0:
            data = bytearray(bytes(self.__recv(count)))
            if len(data) != count:
                raise RuntimeError("Ack Window %i not %i " % (len(data), count))
            for i in range(0, len(data), 2):
                if bytes([data[i]]) != self.INSYNC:
                    raise RuntimeError("unexpected %s instead of INSYNC" % data[i])
                if bytes([data[i + 1]]) == self.INVALID:
                    raise RuntimeError("bootloader reports INVALID OPERATION")
                if bytes([data[i + 1]]) == self.FAILED:
                    raise RuntimeError("bootloader reports OPERATION FAILED")
                if bytes([data[i + 1]]) != self.OK:
                    raise RuntimeError("unexpected response 0x%x instead of OK" % ord(data[i + 1]))

    # attempt to get back into sync with the bootloader
    def __sync(self):
        # send a stream of ignored bytes longer than the longest possible conversation
        # that we might still have in progress
        # self.__send(uploader.NOP * (uploader.PROG_MULTI_MAX + 2))
        self.port.flushInput()
        self.__send(uploader.GET_SYNC + uploader.EOC)
        self.__getSync()

    def __trySync(self):
        try:
            self.port.flush()
            if self.__recv() != self.INSYNC:
                # print("unexpected 0x%x instead of INSYNC" % ord(c))
                return False
            c = self.__recv()
            if c == self.BAD_SILICON_REV:
                raise NotImplementedError()
            if c != self.OK:
                # print("unexpected 0x%x instead of OK" % ord(c))
                return False
            return True

        except NotImplementedError:
            raise RuntimeError(
                "Programing not supported for this version of silicon!\n"
                "See https://docs.px4.io/master/en/flight_controller/silicon_errata.html"
            )
        except RuntimeError:
            # timeout, no response yet
            return False

    # attempt to determins if the device is CDCACM or A FTDI
    def __determineInterface(self):
        self.port.flushInput()
        # Set a baudrate that can not work on a real serial port
        # in that it is 233% off.
        try:
            self.port.baudrate = self.baudrate_bootloader * 2.33
        except NotImplementedError as e:
            # This error can occur because pySerial on Windows does not support odd baudrates
            print(str(e) + " -> could not check for FTDI device, assuming USB connection")
            return

        self.__send(uploader.GET_SYNC + uploader.EOC)
        try:
            self.__getSync(False)
        except:
            # if it fails we are on a real serial port - only leave this enabled on Windows
            if sys.platform.startswith("win"):
                self.ackWindowedMode = True
        finally:
            self.port.baudrate = self.baudrate_bootloader

    # send the GET_DEVICE command and wait for an info parameter
    def __getInfo(self, param):
        self.__send(uploader.GET_DEVICE + param + uploader.EOC)
        value = self.__recv_int()
        self.__getSync()
        return value

    # send the GET_OTP command and wait for an info parameter
    def __getOTP(self, param):
        t = struct.pack("I", param)  # int param as 32bit ( 4 byte ) char array.
        self.__send(uploader.GET_OTP + t + uploader.EOC)
        value = self.__recv(4)
        self.__getSync()
        return value

    # send the GET_SN command and wait for an info parameter
    def __getSN(self, param):
        t = struct.pack("I", param)  # int param as 32bit ( 4 byte ) char array.
        self.__send(uploader.GET_SN + t + uploader.EOC)
        value = self.__recv(4)
        self.__getSync()
        return value

    # send the GET_CHIP command
    def __getCHIP(self):
        self.__send(uploader.GET_CHIP + uploader.EOC)
        value = self.__recv_int()
        self.__getSync()
        return value

    # send the GET_CHIP command
    def __getCHIPDes(self):
        self.__send(uploader.GET_CHIP_DES + uploader.EOC)
        length = self.__recv_int()
        value = self.__recv(length)
        self.__getSync()
        pieces = value.split(b",")
        return pieces

    def __drawProgressBar(self, label, progress, maxVal):
        if maxVal < progress:
            progress = maxVal

        percent = (float(progress) / float(maxVal)) * 100.0

        sys.stdout.write("\r%s: [%-20s] %.1f%%" % (label, "=" * int(percent / 5.0), percent))
        sys.stdout.flush()

    # send the CHIP_ERASE command and wait for the bootloader to become ready
    def __erase(self, label):
        print("Windowed mode: %s" % self.ackWindowedMode)
        print("\n", end="")
        self.__send(uploader.CHIP_ERASE + uploader.EOC)

        # erase is very slow, give it 30s
        deadline = time.time() + 30.0
        while time.time() < deadline:
            usualEraseDuration = 15.0
            estimatedTimeRemaining = deadline - time.time()
            if estimatedTimeRemaining >= usualEraseDuration:
                self.__drawProgressBar(label, 30.0 - estimatedTimeRemaining, usualEraseDuration)
            else:
                self.__drawProgressBar(label, 10.0, 10.0)
                sys.stdout.write(" (timeout: %d seconds) " % int(deadline - time.time()))
                sys.stdout.flush()

            if self.__trySync():
                self.__drawProgressBar(label, 10.0, 10.0)
                return

        raise RuntimeError("timed out waiting for erase")

    # send a PROG_MULTI command to write a collection of bytes
    def __program_multi(self, data, windowMode):
        length = len(data).to_bytes(1, byteorder="big")

        self.__send(uploader.PROG_MULTI)
        self.__send(length)
        self.__send(data)
        self.__send(uploader.EOC)
        if not windowMode:
            self.__getSync(False)
        else:
            # The following is done to have minimum delay on the transmission
            # of the ne fw. The per block cost of __getSync was about 16 mS per.
            # Passively wait on Sync and Result using board rates and
            # N.B. attempts to activly wait on InWating still carried 8 mS of overhead
            self.__probe(False)
            self.__probe(True)
            time.sleep((ord(length) * self.chartime) + uploader.MAX_FLASH_PRGRAM_TIME)
            self.__probe(False)

    # verify multiple bytes in flash
    def __verify_multi(self, data):
        length = len(data).to_bytes(1, byteorder="big")

        self.__send(uploader.READ_MULTI)
        self.__send(length)
        self.__send(uploader.EOC)
        self.port.flush()
        programmed = self.__recv(len(data))
        if programmed != data:
            print("got    " + binascii.hexlify(programmed))
            print("expect " + binascii.hexlify(data))
            return False
        self.__getSync()
        return True

    # send the reboot command
    def __reboot(self):
        self.__send(uploader.REBOOT + uploader.EOC)
        self.port.flush()

        # v3+ can report failure if the first word flash fails
        if self.bl_rev >= 3:
            self.__getSync()

    # split a sequence into a list of size-constrained pieces
    def __split_len(self, seq, length):
        return [seq[i : i + length] for i in range(0, len(seq), length)]

    # upload code
    def __program(self, label, fw):
        self.__probe(False)
        print("\n", end="")
        code = fw.image
        groups = self.__split_len(code, uploader.PROG_MULTI_MAX)
        # Give imedate feedback
        self.__drawProgressBar(label, 0, len(groups))
        uploadProgress = 0
        for bytes in groups:
            self.__program_multi(bytes, self.ackWindowedMode)
            # If in Window mode, extend the window size for the __ackSyncWindow
            if self.ackWindowedMode:
                self.window += self.window_per

            # Print upload progress (throttled, so it does not delay upload progress)
            uploadProgress += 1
            if uploadProgress % 256 == 0:
                self.__probe(True)
                self.__probe(False)
                self.__probe(True)
                self.__ackSyncWindow(self.window)
                self.__probe(False)
                self.window = 0
                self.__drawProgressBar(label, uploadProgress, len(groups))

        # Do any remaining fragment
        self.__ackSyncWindow(self.window)
        self.window = 0
        self.__drawProgressBar(label, 100, 100)

    # verify code
    def __verify_v2(self, label, fw):
        print("\n", end="")
        self.__send(uploader.CHIP_VERIFY + uploader.EOC)
        self.__getSync()
        code = fw.image
        groups = self.__split_len(code, uploader.READ_MULTI_MAX)
        verifyProgress = 0
        for bytes in groups:
            verifyProgress += 1
            if verifyProgress % 256 == 0:
                self.__drawProgressBar(label, verifyProgress, len(groups))
            if not self.__verify_multi(bytes):
                raise RuntimeError("Verification failed")
        self.__drawProgressBar(label, 100, 100)

    def __verify_v3(self, label, fw):
        print("\n", end="")
        self.__drawProgressBar(label, 1, 100)
        expect_crc = fw.crc(self.fw_maxsize)
        self.__send(uploader.GET_CRC + uploader.EOC)
        time.sleep(0.5)
        report_crc = self.__recv_int()
        self.__getSync()
        if report_crc != expect_crc:
            print("Expected 0x%x" % expect_crc)
            print("Got      0x%x" % report_crc)
            raise RuntimeError("Program CRC failed")
        self.__drawProgressBar(label, 100, 100)

    def __set_boot_delay(self, boot_delay):
        self.__send(uploader.SET_BOOT_DELAY + struct.pack("b", boot_delay) + uploader.EOC)
        self.__getSync()

    # get basic data about the board
    def identify(self):
        self.__determineInterface()
        # make sure we are in sync before starting
        self.__sync()

        # get the bootloader protocol ID first
        self.bl_rev = self.__getInfo(uploader.INFO_BL_REV)
        if (self.bl_rev < uploader.BL_REV_MIN) or (self.bl_rev > uploader.BL_REV_MAX):
            print("Unsupported bootloader protocol %d" % uploader.INFO_BL_REV)
            raise RuntimeError("Bootloader protocol mismatch")

        self.board_type = self.__getInfo(uploader.INFO_BOARD_ID)
        self.board_rev = self.__getInfo(uploader.INFO_BOARD_REV)
        self.fw_maxsize = self.__getInfo(uploader.INFO_FLASH_SIZE)

    # upload the firmware
    def upload(self, fw, force=False, boot_delay=None):
        # Make sure we are doing the right thing
        start = time.time()
        if self.board_type != fw.property("board_id"):
            msg = "Firmware not suitable for this board (Firmware board_type=%u board_id=%u)" % (
                self.board_type,
                fw.property("board_id"),
            )
            print("WARNING: %s" % msg)
            if force:
                print("FORCED WRITE, FLASHING ANYWAY!")
            else:
                raise FirmwareNotSuitableException(msg)

        # Prevent uploads where the image would overflow the flash
        if self.fw_maxsize < fw.property("image_size"):
            raise RuntimeError("Firmware image is too large for this board")

        # OTP added in v4:
        if self.bl_rev >= 4:
            for byte in range(0, 32 * 6, 4):
                x = self.__getOTP(byte)
                self.otp = self.otp + x
                # print(binascii.hexlify(x).decode('Latin-1') + ' ', end='')
            # see src/modules/systemlib/otp.h in px4 code:
            self.otp_id = self.otp[0:4]
            self.otp_idtype = self.otp[4:5]
            self.otp_vid = self.otp[8:4:-1]
            self.otp_pid = self.otp[12:8:-1]
            self.otp_coa = self.otp[32:160]
            # show user:
            try:
                print("sn: ", end="")
                for byte in range(0, 12, 4):
                    x = self.__getSN(byte)
                    x = x[::-1]  # reverse the bytes
                    self.sn = self.sn + x
                    print(binascii.hexlify(x).decode("Latin-1"), end="")  # show user
                print("")
                print("chip: %08x" % self.__getCHIP())

                otp_id = self.otp_id.decode("Latin-1")
                if "PX4" in otp_id:
                    print("OTP id: " + otp_id)
                    print("OTP idtype: " + binascii.b2a_qp(self.otp_idtype).decode("Latin-1"))
                    print("OTP vid: " + binascii.hexlify(self.otp_vid).decode("Latin-1"))
                    print("OTP pid: " + binascii.hexlify(self.otp_pid).decode("Latin-1"))
                    print("OTP coa: " + binascii.b2a_base64(self.otp_coa).decode("Latin-1"))

            except Exception:
                # ignore bad character encodings
                pass

        # Silicon errata check was added in v5
        if self.bl_rev >= 5:
            des = self.__getCHIPDes()
            if len(des) == 2:
                print("family: %s" % des[0])
                print("revision: %s" % des[1])
                print("flash: %d bytes" % self.fw_maxsize)

                # Prevent uploads where the maximum image size of the board config is smaller than the flash
                # of the board. This is a hint the user chose the wrong config and will lack features
                # for this particular board.

                # This check should also check if the revision is an unaffected revision
                # and thus can support the full flash, see
                # https://github.com/PX4/Firmware/blob/master/src/drivers/boards/common/stm32/board_mcu_version.c#L125-L144

                if self.fw_maxsize > fw.property("image_maxsize") and not force:
                    raise RuntimeError(
                        "Board can accept larger flash images (%u bytes) than board config (%u bytes). Please use the correct board configuration to avoid lacking critical functionality."
                        % (self.fw_maxsize, fw.property("image_maxsize"))
                    )
        else:
            # If we're still on bootloader v4 on a Pixhawk, we don't know if we
            # have the silicon errata and therefore need to flash px4_fmu-v2
            # with 1MB flash or if it supports px4_fmu-v3 with 2MB flash.
            if fw.property("board_id") == 9 and fw.property("image_size") > 1032192 and not force:
                raise RuntimeError(
                    "\nThe Board uses bootloader revision 4 and can therefore not determine\n"
                    "if flashing more than 1 MB (px4_fmu-v3_default) is safe, chances are\n"
                    "high that it is not safe! If unsure, use px4_fmu-v2_default.\n"
                    "\n"
                    "If you know you that the board does not have the silicon errata, use\n"
                    "this script with --force, or update the bootloader. If you are invoking\n"
                    "upload using make, you can use force-upload target to force the upload.\n"
                )
        self.__erase("Erase  ")
        self.__program("Program", fw)

        if self.bl_rev == 2:
            self.__verify_v2("Verify ", fw)
        else:
            self.__verify_v3("Verify ", fw)

        if boot_delay is not None:
            self.__set_boot_delay(boot_delay)

        print("\nRebooting.", end="")
        self.__reboot()
        self.port.close()
        print(" Elapsed Time %3.3f\n" % (time.time() - start))

    def __next_baud_flightstack(self):
        if self.baudrate_flightstack_idx + 1 >= len(self.baudrate_flightstack):
            return False
        try:
            self.port.baudrate = self.baudrate_flightstack[self.baudrate_flightstack_idx + 1]
            self.baudrate_flightstack_idx = self.baudrate_flightstack_idx + 1
        except serial.SerialException:
            # Sometimes _configure_port fails
            time.sleep(0.04)

        return True

    def send_reboot(self):
        if not self.__next_baud_flightstack():
            return False

        print("Attempting reboot on %s with baudrate=%d..." % (self.port.port, self.port.baudrate), file=sys.stderr)
        if "ttyS" in self.port.port:
            print("If the board does not respond, check the connection to the Flight Controller")
        else:
            print("If the board does not respond, unplug and re-plug the USB connector.", file=sys.stderr)

        try:
            # try MAVLINK command first
            self.port.flush()
            self.__send(uploader.MAVLINK_REBOOT_ID1)
            self.__send(uploader.MAVLINK_REBOOT_ID0)
            # then try reboot via NSH
            self.__send(uploader.NSH_INIT)
            self.__send(uploader.NSH_REBOOT_BL)
            self.__send(uploader.NSH_INIT)
            self.__send(uploader.NSH_REBOOT)
            self.port.flush()
            self.port.baudrate = self.baudrate_bootloader
        except Exception:
            try:
                self.port.flush()
                self.port.baudrate = self.baudrate_bootloader
            except Exception:
                pass

        return True


def main():
    # Python2 is EOL
    if not runningPython3:
        raise RuntimeError("Python 2 is not supported. Please try again using Python 3.")

    # Parse commandline arguments
    parser = argparse.ArgumentParser(description="Firmware uploader for the NextPilot system.")
    parser.add_argument(
        "--port", action="store", required=True, help="Comma-separated list of serial port(s) to which the FMU may be attached"
    )
    parser.add_argument(
        "--baud-bootloader",
        action="store",
        type=int,
        default=115200,
        help="Baud rate of the serial port (default is 115200) when communicating with bootloader, only required for true serial ports.",
    )
    parser.add_argument(
        "--baud-flightstack",
        action="store",
        default="57600",
        help="Comma-separated list of baud rate of the serial port (default is 57600) when communicating with flight stack (Mavlink or NSH), only required for true serial ports.",
    )
    parser.add_argument(
        "--force",
        action="store_true",
        default=False,
        help="Override board type check, or silicon errata checks and continue loading",
    )
    parser.add_argument("--boot-delay", type=int, default=None, help="minimum boot delay to store in flash")
    parser.add_argument("firmware", action="store", help="Firmware file to be uploaded")
    args = parser.parse_args()

    # warn people about ModemManager which interferes badly with Pixhawk
    if os.path.exists("/usr/sbin/ModemManager"):
        print("==========================================================================================================")
        print("WARNING: You should uninstall ModemManager as it conflicts with any non-modem serial device (like Pixhawk)")
        print("==========================================================================================================")

    # Load the firmware file
    fw = firmware(args.firmware)

    percent = fw.property("image_size") / fw.property("image_maxsize")
    binary_size = float(fw.property("image_size"))
    binary_max_size = float(fw.property("image_maxsize"))
    percent = (binary_size / binary_max_size) * 100

    print(
        "Loaded firmware for board id: %s,%s size: %d bytes (%.2f%%), waiting for the bootloader..."
        % (fw.property("board_id"), fw.property("board_revision"), fw.property("image_size"), percent)
    )
    print()

    # tell any GCS that might be connected to the autopilot to give up
    # control of the serial port

    # send to localhost and default GCS port
    ipaddr = "127.0.0.1"
    portnum = 14550

    # COMMAND_LONG in MAVLink 1
    heartbeatpacket = bytearray.fromhex("fe097001010000000100020c5103033c8a")
    commandpacket = bytearray.fromhex("fe210101014c00000000000000000000000000000000000000000000803f00000000f6000000008459")

    # initialize an UDP socket
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # send heartbeat to initialize connection and command to free the link
    s.sendto(heartbeatpacket, (ipaddr, portnum))
    s.sendto(commandpacket, (ipaddr, portnum))

    # close the socket
    s.close()

    # Spin waiting for a device to show up
    try:
        while True:
            portlist = []
            patterns = args.port.split(",")
            # on unix-like platforms use glob to support wildcard ports. This allows
            # the use of /dev/serial/by-id/usb-3D_Robotics on Linux, which prevents the upload from
            # causing modem hangups etc
            if "linux" in _platform or "darwin" in _platform or "cygwin" in _platform:
                import glob

                for pattern in patterns:
                    portlist += glob.glob(pattern)
            else:
                portlist = patterns

            baud_flightstack = [int(x) for x in args.baud_flightstack.split(",")]

            successful = False
            unsuitable_board = False
            for port in portlist:
                # print("Trying %s" % port)

                # create an uploader attached to the port
                try:
                    if "linux" in _platform:
                        # Linux, don't open Mac OS and Win ports
                        if "COM" not in port and "tty.usb" not in port:
                            up = uploader(port, args.baud_bootloader, baud_flightstack)
                    elif "darwin" in _platform:
                        # OS X, don't open Windows and Linux ports
                        if "COM" not in port and "ACM" not in port:
                            up = uploader(port, args.baud_bootloader, baud_flightstack)
                    elif "cygwin" in _platform:
                        # Cygwin, don't open native Windows COM and Linux ports
                        if "COM" not in port and "ACM" not in port:
                            up = uploader(port, args.baud_bootloader, baud_flightstack)
                    elif "win" in _platform:
                        # Windows, don't open POSIX ports
                        if "/" not in port:
                            up = uploader(port, args.baud_bootloader, baud_flightstack)
                except Exception:
                    # open failed, rate-limit our attempts
                    time.sleep(0.05)

                    # and loop to the next port
                    continue

                found_bootloader = False
                while True:
                    up.open()

                    # port is open, try talking to it
                    try:
                        # identify the bootloader
                        up.identify()
                        found_bootloader = True
                        print()
                        print(
                            "Found board id: %s,%s bootloader version: %s on %s"
                            % (up.board_type, up.board_rev, up.bl_rev, port)
                        )
                        break

                    except Exception:
                        if not up.send_reboot():
                            break

                        # wait for the reboot, without we might run into Serial I/O Error 5
                        time.sleep(0.25)

                        # always close the port
                        up.close()

                        # wait for the close, without we might run into Serial I/O Error 6
                        time.sleep(0.3)

                if not found_bootloader:
                    # Go to the next port
                    continue

                try:
                    # ok, we have a bootloader, try flashing it
                    up.upload(fw, force=args.force, boot_delay=args.boot_delay)

                    # if we made this far without raising exceptions, the upload was successful
                    successful = True

                except RuntimeError as ex:
                    # print the error
                    print("\nERROR: %s" % ex.args)

                except FirmwareNotSuitableException:
                    unsuitable_board = True
                    up.close()
                    continue

                except IOError:
                    up.close()
                    continue

                finally:
                    # always close the port
                    up.close()

                # we could loop here if we wanted to wait for more boards...
                if successful:
                    sys.exit(0)
                else:
                    sys.exit(1)

            if unsuitable_board:
                # If we land here, we went through all ports, did not flash any
                # board and found at least one unsuitable board.
                # Exit with 2, so a caller can distinguish from other errors
                sys.exit(2)

            # Delay retries to < 20 Hz to prevent spin-lock from hogging the CPU
            time.sleep(0.05)

    # CTRL+C aborts the upload/spin-lock by interrupt mechanics
    except KeyboardInterrupt:
        print("\n Upload aborted by user.")
        sys.exit(0)


if __name__ == "__main__":
    main()

# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
