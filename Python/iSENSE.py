import requests,json,time,datetime

# This is a wrapper for the iSENSE API
# Docs are at http://isenseproject.org/api/v1/docs

# All API calls go through the following url
BASE_URL = 'http://rsense-dev.cs.uml.edu/api/v1/'

# Field Types
TIMESTAMP = 1
NUMBER = 2
TEXT = 3
LOCATION = 4

# Gets info about a project, if recur is true it returns extra info
def getProject(projID, recur=False):
    url = BASE_URL + 'projects/' + str(projID) +'?recur=' + str(recur).lower()
    return requests.get(url)

# Creates a New Project
def createProject(email, password, name):
    url = BASE_URL + 'projects/'
    payload = {'email': email, 'password': password, 'project_name': name}
    r = requests.post(url, params=payload)

# Must be the owner of the project to create a key
def createContributorKey(email, password, projId, name, key):
    url = BASE_URL + 'projects/' + str(projId) + "/add_key/"
    payload = {'email': email, 'password': password, 'contrib_key': {'name': name, 'key': key} }
    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}
    return requests.post(url, data=data, headers=headers)

# Gets field info
def getField(fieldId):
    url = BASE_URL + 'fields/' + str(fieldId)
    return requests.get(url)

# Name is field name, type is field type (number, text, timestamp, location) 
# location will create lat and long
def createField(email, password, projectId, name, fieldType, other):
    url = BASE_URL + 'projects/'

    if other is None:
        payload = {'email': email, 'password': password, 'field': {'project_id': str(projectId), 'name': name, 'field_type': fieldType}} 
    if fieldType == NUMBER: # other will be units
        payload = {'email': email, 'password': password, 'field': {'project_id': str(projectId), 'name': name, 'field_type': fieldType, 'unit': other}}
    elif fieldType == TEXT: # other will be text restrictions
        payload = {'email': email, 'password': password, 'field': {'project_id': str(projectId), 'name': name, 'field_type': fieldType, 'restrictions': other}}
    else: # other will be ignored.
        payload = {'email': email, 'password': password, 'field': {'project_id': str(projectId), 'name': name, 'field_type': fieldType}}

    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}

    return requests.post(url, data=data, headers=headers)

# Gets a data set
def getDataSet(dataSetId):
    url = BASE_URL + 'data_sets/' + str(dataSetId)
    return requests.get(url)

# Uploads a new data set with an email and password
def uploadDataSet(email, password, projId, title, data):
    url = BASE_URL + 'projects/' + str(projId) + '/jsonDataUpload'
    payload = {'email': email, 'password': password, 'title': title, 'data': data}

    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}

    return requests.post(url, data=data, headers=headers)

# Uploads a new data set with a key
def uploadDataSetWithKey(contribKey, contribName, projId, title, data):
    url = BASE_URL + 'projects/' + str(projId) + '/jsonDataUpload'
    payload = {'contribution_key': contribKey, 'contributor_name': contribName, 'title': title, 'data': data}
    
    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}

    return requests.post(url, data=data, headers=headers)


# Replaces an existing data set's data with new data (You must be the owner)
def editDataSet(email, password, dataSetId, data):
    url = BASE_URL + 'data_sets/' + str(dataSetId) + '/edit'
    payload = {'email': email, 'password': password, 'data': data}
    
    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}
    
    return requests.post(url, data=data, headers=headers)

# Replaces an existing data set's data with new data (You must be the owner)
def editDataSetWithKey(contribKey, dataSetId, data):
    url = BASE_URL + 'data_sets/' + str(dataSetId) + '/edit'
    payload = {'contribution_key': contribKey, 'contributor_name': contribName, 'data': data}
    
    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}
    
    return requests.post(url, data=data, headers=headers)

# Appends to an existing data set (you must be the owner)
def appendDataSet(email, password, dataSetId, data):
    url = BASE_URL + 'data_sets/append'
    payload = {'email': email, 'password': password, 'id': str(dataSetId), 'data': data}
    
    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}
    
    return requests.post(url, data=data, headers=headers)

# Appends to an existing data set (it must have been uploaded with the same key)
def appendDataSetWithKey(contribKey, dataSetId, data):
    url = BASE_URL + 'data_sets/append'
    payload = {'contribution_key': contribKey, 'id': str(dataSetId), 'data': data}
    
    data = json.dumps(payload)
    headers = {'Content-Type': 'application/json'}
    
    return requests.post(url, data=data, headers=headers)