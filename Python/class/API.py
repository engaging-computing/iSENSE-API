import requests,json,time,datetime

baseUrl = 'http://rsense-dev.cs.uml.edu/api/v1/';

class Isense:

    def __init__(self,projectID,contributorKey,contributorName):
        self.projectID = projectID
        self.contributorKey = contributorKey
        self.contributorName = contributorName

    def projectGetRequest(self):
        
        urlProject = baseUrl + 'projects/' + self.projectID+'?recur=true';

        data = requests.get(urlProject)

        return data

    def getDatasetLocation(self,datasetName,parsedResponseProject):

        for i in range(0,parsedResponseProject.json()['dataSetCount']):

            if parsedResponseProject.json()['dataSets'][i]['name'] == datasetName:
                datasetLocation = i
                datasetID = parsedResponseProject.json()['dataSets'][i]['id']
                return datasetLocation

        return None

    def getDatasetId(self,datasetName,parsedResponseProject):

        datasetID = None
        for i in range(0,parsedResponseProject.json()['dataSetCount']):

            if parsedResponseProject.json()['dataSets'][i]['name'] == datasetName:
                datasetLocation = i;

                datasetID = parsedResponseProject.json()['dataSets'][i]['id'];

        if datasetID == None:
            return None

        return datasetID;
  

    def getFieldID(self,fieldName,parsedResponseProject):

        datasetID = None

        for i in range(0,parsedResponseProject.json()['fieldCount']):

            if parsedResponseProject.json()['fields'][i]['name'] == fieldName:

                fieldID = parsedResponseProject.json()['fields'][i]['id']
                fieldID = str(fieldID)
                return fieldID;
       
        return None      
      
    def getDatasetFieldData(self,datasetName,fieldName):

        values = []

        parsedResponseProject = self.projectGetRequest()

        datasetLocation = self.getDatasetLocation(datasetName,parsedResponseProject)

        fieldID = self.getFieldID(fieldName,parsedResponseProject)

        if fieldID == None:
            return None

        for i in range(0,parsedResponseProject.json()['dataSets'][datasetLocation]['datapointCount']):
            values.append(int(parsedResponseProject.json()['dataSets'][datasetLocation]['data'][i][fieldID]))  

        return values

    def postDataset(self,fieldName,datasetName,fieldData):
        
        timestamp = time.time()
        timestamp_reformatted = datetime.datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d %H:%M:%S')

        parsedResponseProject = self.projectGetRequest()
        fieldID = self.getFieldID(fieldName,parsedResponseProject)

        if fieldID == None:
            return None
            
        url = baseUrl + 'projects/' + self.projectID+'/jsonDataUpload'

        payload = {
            'title': datasetName + ' ' + timestamp_reformatted,                                 
            'contribution_key': self.contributorKey,                    
            'contributor_name': self.contributorName,
            'data':
            {
                fieldID: fieldData
            }
        }
        headers = {'content-type': 'application/json'}

        r = requests.post(url, data=json.dumps(payload), headers=headers)

    def postDatasetHorizontal(self,fields,title,data):

        timestamp = time.time()
        timestamp_reformatted = datetime.datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d %H:%M:%S')
        parsedResponseProject = self.projectGetRequest()
        dataForPost = {}
        fieldID = []

        for i in range(0, len(fields)):

            fieldID.append(self.getFieldID(fields[i],parsedResponseProject))
            dataForPost[fieldID[i]] = data[i]

        url = baseUrl+ 'projects/' + self.projectID+'/jsonDataUpload'

        payload = {
            'title': title + ' ' + timestamp_reformatted,                                 
            'contribution_key': self.contributorKey,                    
            'contributor_name': self.contributorName,
            'data': dataForPost    
        }

        headers = {'content-type': 'application/json'}

        r = requests.post(url, data=json.dumps(payload), headers=headers)

    def appendToDataset(self,datasetName,fields,data):

        parsedResponseProject = self.projectGetRequest()
        datasetId = self.getDatasetId(datasetName,parsedResponseProject)
        dataForPost = {}
        fieldID = []

        for i in range(0, len(fields)):

            fieldID.append(self.getFieldID(fields[i],parsedResponseProject))
            dataForPost[fieldID[i]] = data[i]

        url = 'http://rsense-dev.cs.uml.edu/api/v1/data_sets/append'

        payload = {
            'contribution_key': self.contributorKey,                    
            'id': datasetId,
            'data': dataForPost
            
        }

        headers = {'content-type': 'application/json'}

        r = requests.post(url, data=json.dumps(payload), headers=headers)
