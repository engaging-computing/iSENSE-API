Python
================================================

These examples use a variety of python libraries.

For HTTP GET/POST requests, there are two that seem to work:

1. urllib2 / json - working for basic GET requests
2. requests - works for GET and POST requests

To install these, simply use the following command (Linux. Windows How-To coming soon):

Python 2
*************************************************
sudo apt-get install python-pip (linux only)
sudo easy_install pip (Mac only)
pip install requests
pip install json
*************************************************

Python 3
*************************************************
sudo apt-get install python3-pip
pip3 install requests
pip3 install json
*************************************************


About each file
*************************************************
iSenseAPI.py is a low level API wrapper. 
iSense.py is a more abstract an easy to use wrapper around iSenseAPI.py
example.py shows how to use iSense.py

All of these can be run with python or python3

More information about the Requests library can be found here:
[Python Request Quickstart Guide](http://docs.python-requests.org/en/latest/user/quickstart/)