# content of test_content.py

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
    p = subprocess.Popen(["./bin/integratio", "./tests/content/content.json"])
    time.sleep(0.1)
    yield p
    p.kill();
    
@pytest.fixture(scope='function')
def runConnectorSeveral():
    p = subprocess.Popen(["./bin/integratio", "./tests/content/content_part_1.json", "./tests/content/content_part_2.json"])
    time.sleep(0.1)
    yield p
    p.kill();
    
    
@pytest.fixture(scope='function')
def runConnectorSeveralInvert():
    p = subprocess.Popen(["./bin/integratio", "./tests/content/content_part_2.json", "./tests/content/content_part_1.json"])
    time.sleep(0.1)
    yield p
    p.kill();
    
@pytest.fixture(scope='function')
def runConnectorSeveral5():
    p = subprocess.Popen(["./bin/integratio", "./tests/content/content_part_1.json", "./tests/content/content_part_2.json","./tests/content/content_part_3.json", "./tests/content/content_part_4.json","./tests/content/content_part_5.json"])
    time.sleep(0.1)
    yield p
    p.kill();

# Tests the normal HTTP response
def test_content_1(runConnector):
    server_address = ("192.168.178.89", 80)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(5)
    sock.connect(server_address)

    sock.send("GET /500 HTTP/1.1\r\n\r\n")
    resp500 = sock.recv(1024)
    print resp500;
    
    assert (resp500 == "HTTP/1.1 500 Internal Server error\r\nConnection: close\r\nDate: Sat, 27 Aug 2016 18:51:19 GMT\r\nServer: Apache/2.4.10 (Unix)\r\nContent-length: 26\r\n\r\nResponse for the 500 error")


# Tests the normal HTTP response, but another resource
def test_content_2(runConnector):
    server_address = ("192.168.178.89", 80)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(5)
    sock.connect(server_address)

    sock.send("GET /404 HTTP/1.1\r\n\r\n")
    resp404 = sock.recv(1024)
    print resp404;
    
    assert (resp404 == "HTTP/1.1 404 Resource Not Found\r\nConnection: close\r\nDate: Sat, 27 Aug 2016 18:51:19 GMT\r\nServer: Apache/2.4.10 (Unix)\r\nContent-length: 26\r\n\r\nResponse for the 404 error")


# 2 different resources loaded from different test conf files.
#  
# NOTE: the first loaded file will provide the resource to the 
# first client to connect
#
def test_content_3(runConnectorSeveral):
    server_address = ("192.168.178.89", 80)
    sock1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock1.settimeout(5)
    sock1.connect(server_address)

    sock2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock2.settimeout(5)
    sock2.connect(server_address)

    sock1.send("GET /part1 HTTP/1.1\r\n\r\n")
    part1 = sock1.recv(1024)
    print part1;
    
    sock2.send("GET /part2 HTTP/1.1\r\n\r\n")
    part2 = sock2.recv(1024)
    print part2;
    
    assert (part1 == "HTTP/1.1 200 OK\r\nConnection: close\r\nDate: Sat, 27 Aug 2016 18:51:19 GMT\r\nServer: Apache/2.4.10 (Unix)\r\nContent-length: 19\r\n\r\nResponse for PART 1")
    assert (part2 == "HTTP/1.1 200 OK\r\nConnection: close\r\nDate: Sat, 27 Aug 2016 18:51:19 GMT\r\nServer: Apache/2.4.10 (Unix)\r\nContent-length: 19\r\n\r\nResponse for PART 2")

# 2 different resources loaded from different test conf files.
# Input files (command line arguments) are inverted
#
def test_content_4(runConnectorSeveralInvert):
    server_address = ("192.168.178.89", 80)
    sock1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock1.settimeout(1)
    sock1.connect(server_address)

    sock2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock2.settimeout(5)
    sock2.connect(server_address)

    sock1.send("GET /part2 HTTP/1.1\r\n\r\n")
    part1 = sock1.recv(1024)
    print part1;
    
    sock2.send("GET /part1 HTTP/1.1\r\n\r\n")
    part2 = sock2.recv(1024)
    print part2;
    
    assert (part1 == "HTTP/1.1 200 OK\r\nConnection: close\r\nDate: Sat, 27 Aug 2016 18:51:19 GMT\r\nServer: Apache/2.4.10 (Unix)\r\nContent-length: 19\r\n\r\nResponse for PART 2")
    assert (part2 == "HTTP/1.1 200 OK\r\nConnection: close\r\nDate: Sat, 27 Aug 2016 18:51:19 GMT\r\nServer: Apache/2.4.10 (Unix)\r\nContent-length: 19\r\n\r\nResponse for PART 1")

# 5 different resources loaded from different test conf files.
# Still the order should hold
#
def test_content_5(runConnectorSeveral5):
    server_address = ("192.168.178.89", 80)
    
    sock1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock3 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock4 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock5 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    sock1.settimeout(1)
    sock2.settimeout(1)
    sock3.settimeout(1)
    sock4.settimeout(1)
    sock5.settimeout(1)
    
    sock1.connect(server_address)
    sock2.connect(server_address)
    sock3.connect(server_address)
    sock4.connect(server_address)
    sock5.connect(server_address)

    sock1.send("GET /part1 HTTP/1.1\r\n\r\n")
    part1 = sock1.recv(1024)
    print part1;
    
    sock2.send("GET /part2 HTTP/1.1\r\n\r\n")
    part2 = sock2.recv(1024)
    print part2;
    
    sock3.send("GET /part3 HTTP/1.1\r\n\r\n")
    part3 = sock3.recv(1024)
    print part3;
    
    sock4.send("GET /part4 HTTP/1.1\r\n\r\n")
    part4 = sock4.recv(1024)
    print part4;
    
    sock5.send("GET /part5 HTTP/1.1\r\n\r\n")
    part5 = sock5.recv(1024)
    print part5;
    
    
    assert (part1 == "HTTP/1.1 200 OK\r\nConnection: close\r\nDate: Sat, 27 Aug 2016 18:51:19 GMT\r\nServer: Apache/2.4.10 (Unix)\r\nContent-length: 19\r\n\r\nResponse for PART 1")
    assert (part2 == "HTTP/1.1 200 OK\r\nConnection: close\r\nDate: Sat, 27 Aug 2016 18:51:19 GMT\r\nServer: Apache/2.4.10 (Unix)\r\nContent-length: 19\r\n\r\nResponse for PART 2")
    assert (part3 == "HTTP/1.1 200 OK\r\nConnection: close\r\nDate: Sat, 27 Aug 2016 18:51:19 GMT\r\nServer: Apache/2.4.10 (Unix)\r\nContent-length: 19\r\n\r\nResponse for PART 3")
    assert (part4 == "HTTP/1.1 200 OK\r\nConnection: close\r\nDate: Sat, 27 Aug 2016 18:51:19 GMT\r\nServer: Apache/2.4.10 (Unix)\r\nContent-length: 19\r\n\r\nResponse for PART 4")
    assert (part5 == "HTTP/1.1 200 OK\r\nConnection: close\r\nDate: Sat, 27 Aug 2016 18:51:19 GMT\r\nServer: Apache/2.4.10 (Unix)\r\nContent-length: 19\r\n\r\nResponse for PART 5")
            

# This test verify the delay is applied to a closing connection
#def test_delay_4(runConnectorCloseDelay):
#    server_address = ("192.168.178.89", 80)
#    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#    sock.setblocking(1)
#    sock.connect(server_address)

#    sock.send("GET /delay.html HTTP/1.1\r\n\r\n")
#    print sock.recv(1024)
#    
#    sock.shutdown(socket.SHUT_RDWR)

#    start = time.time()
#    while(getTCPState(sock) != "7"):
#        pass
#    end = time.time()

#    assert (end - start) > 2