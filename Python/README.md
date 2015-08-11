Python
================================================

These examples use a variety of python libraries.

For HTTP GET/POST requests, there are two that seem to work:

1. urllib2 / json - working for basic GET requests
2. requests - works for GET and POST requests

************************************************ 

pip is a package management system used to install and manage software packages written in Python. We recomend you install the packages required using pip.

More info on how to install pip can be found using the link below. (Windows, Mac, and Linux)

https://pip.pypa.io/en/latest/installing.html

*************************************************

To install the packages needed, simply use the following commands:

Python 2
*************************************************
sudo apt-get install python-pip (Install pip linux only)
sudo easy_install pip (Install pip Mac only)
pip install requests
pip install json
*************************************************

Python 3
*************************************************
sudo apt-get install python3-pip (Install pip3 linux only)
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
