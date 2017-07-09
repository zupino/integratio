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
import struct

def getTCPState(s):
    fmt = "B"*7+"I"*24
    x = struct.unpack(fmt, s.getsockopt(socket.IPPROTO_TCP, socket.TCP_INFO, 104))
    return str(x).replace("(", "").split(",")[0]

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
    p = subprocess.Popen(["./bin/integratio", "./tests/time/time.json"])
    time.sleep(0.1)
    yield p
    p.kill();

@pytest.fixture(scope='function')
def runConnectorSend():
    p = subprocess.Popen(["./bin/integratio", "./tests/time/time_send_1.json", "./tests/time/time_send_2.json"])
    time.sleep(0.1)
    yield p
    p.kill();

@pytest.fixture(scope='function')
def runConnectorCloseDelay():
    p = subprocess.Popen(["./bin/integratio", "./tests/time/time_close.json"])
    time.sleep(0.1)
    yield p
    p.kill();

@pytest.fixture(scope='function')
def runConnectorParallel():
    p = subprocess.Popen(["./bin/integratio", "./tests/time/time_parallel_1.json", "./tests/time_parallel_2.json"])
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
def test_delay_3(runConnectorSend):
    server_address = ("192.168.178.89", 80)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(5)
    sock.connect(server_address)
    sock.send("delay.html")
    start = time.time()
    print sock.recv(1024)
    end = time.time()

    assert (end - start) > 1

# This test verify the delay is applied to a closing connection
def test_delay_4(runConnectorCloseDelay):
    server_address = ("192.168.178.89", 80)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setblocking(1)
    sock.connect(server_address)

    sock.send("get /delay.html")
    print sock.recv(1024)
    
    sock.shutdown(socket.SHUT_RDWR)

    start = time.time()
    while(getTCPState(sock) != "7"):
        pass
    end = time.time()

    assert (end - start) > 2

# This test verify that closing the connection is NOT
# delay if the TIME test case aply to another State
# Note fixture is runConnector, not runConnectorClose
def test_delay_5(runConnector):
    server_address = ("192.168.178.89", 80)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setblocking(1)
    sock.connect(server_address)

    sock.send("get /delay.html")
    print sock.recv(1024)

    sock.shutdown(socket.SHUT_RDWR)

    start = time.time()
    while(getTCPState(sock) != "7"):
        pass
    end = time.time()

    assert (end - start) < 1

# This test verifies a different delay for
# two different TCP streams

def test_delay_6(runConnectorParallel):
    server_address = ("192.168.178.89", 80)
    sock1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    sock1.settimeout(15)
    sock2.settimeout(15)
    start1 = time.time()
    sock1.connect(server_address)
    end1 = time.time()
    sock1.shutdown(socket.SHUT_RDWR);
    sock1.close()

    start2 = time.time()
    sock2.connect(server_address)
    end2 = time.time()
    sock2.shutdown(socket.SHUT_RDWR)
    sock2.close()

    assert (end1 - start1) > 4
    assert (end2 - start2) > 2

