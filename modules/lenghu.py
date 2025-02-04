#!/usr/bin/env python3

import threading
import os
import socket
import json
import syslog
import sys

import daemon
import pidlockfile
import lockfile


from astropy.coordinates import SkyCoord
import astropy.units as u

def uds_process():
    pass

class UDSServer(daemon.DaemomContext):
    def __init__(self, sock_path=sock_path, pid_file=pid_file, lock_file=lock_file):
        self.sock_path = sock_path
        self.pid_file = pid_file
        self.lock_file = lock_file
        super(UDSServer, self).__init__(pidfile=pidlockfile.PIDLockFile(pid_file))

    def run(self):
        with lockfile.FileLock(self.lock_file):
            try:
                if not os.path.exits(self.path):
                    os.unlink(self.path)
            except OSError:
                syslog.syslog(syslog.OG_ERR, 'Cannot unlink {:s}'.format(self.path))
                sys.exit(1)
            self.server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
            self.server.bind(self.path)
            self.listen(5)
            while True:
                connection, client_address = self.server.accept()
                thread = threading.Thread(target=uds_process, args=())


def main():

    if os.path.exits(pid_file):
        syslog.syslog(syslog.LOG_ERR, )
        sys.exit(1)
