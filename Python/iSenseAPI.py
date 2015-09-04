import requests,json,time,datetime

# This is a wrapper for the iSENSE API
# Docs are at https://isenseproject.org/api/v1/docs

# All API calls go through the following url
BASE_URL = 'https://isenseproject.org/api/v1/'
#BASE_URL = 'http://rsense-dev.cs.uml.edu/api/v1/'


# Field Types
TIMESTAMP = 1
NUMBER = 2
TEXT = 3
LATITUDE = 4
LONGITUDE = 5

# Gets info about a project, if recur is true it returns extra info
def getProject(projID, recur=False):
    url = BASE_URL + 'projects/' + str(projID) +'?recur=' + str(recur).lower()
    request = requests.get(url)
    if request.status_code != 200:
        raise ISenseApiException(request.status_code)
    dictionary = json.loads(request.text)
    return dictionary

# Creates a New Project
def createProject(email, password, name):
    url = BASE_URL + 'projects/'
    payload = {'email': email, 'password': password, 'project_name': name}
    request = requests.post(url, params=payload)
    if request.status_code != 201:
        raise ISenseApiException(request.status_code)
    dictionary = json.loads(request.text)
    return dictionary

# Must be the owner of the project to create a key
def createContributorKey(email, password, projId, name, key):
    url = BASE_URL + 'projects/' + str(projId) + "/add_key/"
    payload = {'email': email, 'password': password, 'contrib_key': {'name': name, 'key': key} }
    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}
    request = requests.post(url, data=data, headers=headers)
    if request.status_code != 201:
        raise ISenseApiException(request.status_code)
    dictionary = json.loads(request.text)
    return dictionary

# Gets field info
def getField(fieldId):
    url = BASE_URL + 'fields/' + str(fieldId)
    request = requests.get(url)
    if request.status_code != 200:
        raise ISenseApiException(request.status_code)
    dictionary = json.loads(request.text)
    return dictionary

# Name is field name, type is field type (number, text, timestamp, location) 
# location will create lat and long
def createField(email, password, projectId, name, fieldType, other):
    url = BASE_URL + 'fields/'

    if other is None:
        payload = {'email': email, 'password': password, 'field': {'project_id': str(projectId), 'name': name, 'field_type': fieldType}} 
    elif fieldType == NUMBER: # other will be units
        payload = {'email': email, 'password': password, 'field': {'project_id': str(projectId), 'name': name, 'field_type': fieldType, 'unit': other}}
    elif fieldType == TEXT: # other will be text restrictions
        payload = {'email': email, 'password': password, 'field': {'project_id': str(projectId), 'name': name, 'field_type': fieldType, 'restrictions': other}}
    else: # other will be ignored.
        payload = {'email': email, 'password': password, 'field': {'project_id': str(projectId), 'name': name, 'field_type': fieldType}}

    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}

    request = requests.post(url, data=data, headers=headers)
    if request.status_code != 201:
        raise ISenseApiException(request.status_code)
    dictionary = json.loads(request.text)
    return dictionary

# Gets a data set
def getDataSet(dataSetId):
    url = BASE_URL + 'data_sets/' + str(dataSetId)
    request = requests.get(url)
    if request.status_code != 200:
        raise ISenseApiException(request.status_code)
    dictionary = json.loads(request.text)
    return dictionary

# Uploads a new data set with an email and password
def uploadDataSet(email, password, projId, title, data):
    url = BASE_URL + 'projects/' + str(projId) + '/jsonDataUpload'

    # append timestamp
    timestamp = datetime.datetime.now().time()
    title = title + " " + str(timestamp)

    payload = {'email': email, 'password': password, 'title': title, 'data': data}

    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}

    request = requests.post(url, data=data, headers=headers)
    if request.status_code != 200:
        raise ISenseApiException(request.status_code)
    dictionary = json.loads(request.text)
    return dictionary

# Uploads a new data set with a key
def uploadDataSetWithKey(contribKey, contribName, projId, title, data):
    url = BASE_URL + 'projects/' + str(projId) + '/jsonDataUpload'

    # append timestamp
    timestamp = datetime.datetime.now().time()
    title = title + " " + str(timestamp)

    payload = {'contribution_key': contribKey, 'contributor_name': contribName, 'title': title, 'data': data}
    
    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}

    request = requests.post(url, data=data, headers=headers)
    if request.status_code != 200:
        raise ISenseApiException(request.status_code)
    dictionary = json.loads(request.text)
    return dictionary


# Replaces an existing data set's data with new data (You must be the owner)
def editDataSet(email, password, dataSetId, data):
    url = BASE_URL + 'data_sets/' + str(dataSetId) + '/edit'
    payload = {'email': email, 'password': password, 'data': data}
    
    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}
    
    request = requests.get(url, data=data, headers=headers)
    if request.status_code != 200:
        raise ISenseApiException(request.status_code)
    dictionary = json.loads(request.text)
    return dictionary

# Replaces an existing data set's data with new data (You must be the owner)
def editDataSetWithKey(contribKey, dataSetId, data):
    url = BASE_URL + 'data_sets/' + str(dataSetId) + '/edit'
    payload = {'contribution_key': contribKey, 'data': data}
    
    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}
    
    request = requests.get(url, data=data, headers=headers)
    if request.status_code != 200:
        raise ISenseApiException(request.status_code)
    dictionary = json.loads(request.text)
    return dictionary

# Appends to an existing data set (you must be the owner)
def appendDataSet(email, password, dataSetId, data):
    url = BASE_URL + 'data_sets/append'
    payload = {'email': email, 'password': password, 'id': str(dataSetId), 'data': data}
    
    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}
    
    request = requests.post(url, data=data, headers=headers)
    if request.status_code != 200:
        raise ISenseApiException(request.status_code)
    dictionary = json.loads(request.text)
    return dictionary

# Appends to an existing data set (it must have been uploaded with the same key)
def appendDataSetWithKey(contribKey, dataSetId, data):
    url = BASE_URL + 'data_sets/append'
    payload = {'contribution_key': contribKey, 'id': str(dataSetId), 'data': data}
    
    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}
    
    request = requests.post(url, data=data, headers=headers)
    if request.status_code != 200:
        raise ISenseApiException(request.status_code)
    dictionary = json.loads(request.text)
    return dictionary

class ISenseApiException(Exception):
   def __init__(self, status):
        status_codes =  {       404 : 'Not found. Check to see if the id you provided is valid.',
                500 : 'iSENSE request failed due to an internal server error.',
                401 : 'The credentials you provided are invalid or do not have permission to make the request.',
                422 : 'The server was unable to parse request.'
        }

        self.error_msg = status_codes.get(status, "There was a problem with a request made to iSENSE. Status code " + str(status))
        print self.error_msg