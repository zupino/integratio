# content of test_delay.py

import requests
import pytest
import io
import json
import time
import socket
import errno
# Used to call the integratio
# binary as from command line
import subprocess

p = ''


def connect( time = 3, server_address = ("192.168.178.89", 80) ):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(time)
    sock.connect(server_address)

# Same as connect(), just return an error code instead of 
# raising an exception if the connection fails
def connect_ex( time = 3, server_address = ("192.168.178.89", 80) ):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(time)
    return sock.connect_ex(server_address)

# Define scope='function' to run the teardown
# code after each test method.
# Teardown code is anything after yield statement
@pytest.fixture(scope='function')
def runConnector():
    p = subprocess.Popen(["./bin/integratio"])
    time.sleep(0.1)
    yield p
    p.kill();


# TCZ will wait 3 seconds before reply to SYN.
# connect has a timeout of 1 in this test case.
# TEST PASS if the connect(1) raise the Timeout
# exception.

# NOTE: Not clear why but here the connect_ex return 
#       errno 11 (EAGAIN), but the exception name raised
#       by connect() is timeout. This is not clear but
#       I'll mark the test case to cover the timeout.
    
def test_delay_1(runConnector):
    with pytest.raises(socket.timeout):
        connect(0.5)
  
# In this case, the connection should happen 
# correctly
def test_delay_2(runConnector):
    assert connect_ex(2) == 0

# This test verify the delay in sending an HTTP response
def test_delay_3(runConnector):
    server_address = ("192.168.178.89", 80)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(5)
    sock.connect(server_address)
    sock.send("get /delay.html")
    start = time.time()
    print sock.recv(1024)
    end = time.time()

    assert (end - start) > 1
