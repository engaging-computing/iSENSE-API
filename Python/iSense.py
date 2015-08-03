import iSenseAPI

# Field Types
TIMESTAMP = iSenseAPI.TIMESTAMP
NUMBER = iSenseAPI.NUMBER
TEXT = iSenseAPI.TEXT
LOCATION = iSenseAPI.LOCATION

INVALID_CREDENTIALS_MESSAGE = "Credentials object is invalid. It must be initialized with either (username and password) OR (contrib_key and contrib_name)"

class Project(object):

    def __init__(self, project_id): 

        dictionary = iSenseAPI.getProject(project_id, recur=True)

        # __ makes the variables private. That way they can only be accessed within this class
        self.__id = project_id
        self.__name = dictionary['name']
        self.__url = dictionary['url']
        self.__path = dictionary['path']
        self.__hidden = dictionary['hidden']
        self.__featured = dictionary['featured']
        self.__like_count = dictionary['likeCount']
        self.__content = dictionary['content']
        self.__time_ago_in_words = dictionary['timeAgoInWords']
        self.__created_at = dictionary['createdAt']
        self.__owner_name = dictionary['ownerName']
        self.__owner_url = dictionary['ownerUrl']
        self.__data_set_count = dictionary['dataSetCount']
        self.__data_set_ids = dictionary['dataSetIDs']
        self.__field_count = dictionary['fieldCount']

        # array of data set objects
        self.__data_sets = []
        for data_set in dictionary['dataSets']:
            data_set_object = DataSet(data_set)
            self.__data_sets.append(data_set_object)

        self.__fields = []
        self.__field_ids = []
        for field in dictionary['fields']:
            field_object = Field(field['id'], field['name'], field['type'], 
                                unit=field['unit'], restrictions=field['restrictions'])
            self.__field_ids.append(field['id'])
            self.__fields.append(field_object)

    def refresh(self):
        self.__init__(self.__id)

    def getId(self):
        return self.__id

    def getDataSets(self):
        return self.__data_sets

    def getDataSetById(self, data_id):
        for d in self.__data_sets:
            if d.getName() == data_id:
                return d 

        # None of the data sets match
        return None

    def getDataSetByName(self, name):
        for d in self.__data_sets:
            if d.getName() == name:
                return d

        # None of the data sets match
        return None

    def getFields(self):
        return self.__fields

    def getFieldIds(self):
        return self.__field_ids

    # Other can be None, units if field is a number, or an array of text restrictions if the field is a text field
    def addField(self, credentials, name, fieldType, other):
        # Only the owner of the project can add fields
        if credentials.isUser():
            response = iSenseAPI.createField(credentials.getUsername(), credentials.getPassword(), self.__id, name, fieldType, other)
            self.refresh()
            return response

        return INVALID_CREDENTIALS_MESSAGE


    #   createDataSet takes a dictionary like this
    #   {
    #       "FIELD_ID" => [1,2,3,4,5], # This would be like a column in a spread sheet (the field would be the header)
    #       "FIELD_ID_2" => ["blue","red",,,"green"] # This would be another column in a spread sheet. (the empty spots will show up as blank entries for those rows)
    #   }

    def createDataSet(self, title, data, credentials):
        # post request too post data
        if credentials.isUser():
            result =  iSenseAPI.uploadDataSet(credentials.getUsername(), credentials.getPassword(), self.__id, title, data)
        elif credentials.isKey():
            result =  iSenseAPI.uploadDataSetWithKey(credentials.getContribKey(), credentials.getContribName(), self.id, title, data)
        else:
            result = INVALID_CREDENTIALS_MESSAGE

        # Updates this object to reflect the new data sets.
        self.refresh()

        # This result is a dictionary of the server response.
        return result

    def getName(self):
        return self.__name

    def getUrl(self):
        return self.__url

    def getPath(self):
        return self.__path

    def getHidden(self):
        return self.__hidden

    def getFeatured(self):
        return self.__featured

    def getLikeCount(self):
        return self.__like_count

    def getContent(self):
        return self.__content

    def getCreationTimeInWords(self):
        return self.__time_ago_in_words

    def getCreationTime(self):
        return self.__created_at

    def ownerName(self):
        return self.__owner

    def ownerUrl(self):
        return self.__owner_url

    def getDataSetCount(self):
        return self.__data_set_count

    def getDataSetIds(self):
        return self.__data_set_ids

    def getFieldCount(self):
        return self.__field_count

    def getDataByField(self, field):
        data = []
        for data_set in self.__data_sets:
            data.extend(data_set.getDataByField(field))

        return data



class DataSet(object):
    # fields is an array. ex. [time, x, y]
    # data is an array of arrays (each inner array represents the data for a field)

    def __init__(self, dictionary):
        self.__id = dictionary['id']
        self.__name = dictionary['name']
        self.__url = dictionary['url']
        self.__path = dictionary['path']
        self.__created_at = dictionary['createdAt']
        self.__field_count = dictionary['fieldCount']
        self.__data_point_count = dictionary['datapointCount']
        self.__display_url = dictionary['displayURL']
        self.__data = dictionary['data']


    # Refresh pulls down the data sets latest data from isense
    def refresh(self):
        self.__init__(self.data)
        return

    def getId(self):
        return self.__id

    def getName(self):
        return self.__name

    def getPath(self):
        return self.__path

    def getCreationTime(self):
        return self.__created_at

    def getFieldCount(self):
        return self.__field_count

    def getDataPointCound(self):
        return self.__data_point_count

    def getDisplayUrl(self):
        return self.__display_url

    def getDataByField(self, field):
        # return all data related to a field
        data = []

        for point in self.__data:
            data.append(point[str(field.getId())])

        return data

    def getData(self):
        return self.__data

    # given another data object it will append it to this data set
    def appendData(self, data_set_id, data_dictionary, credentials):

        # Append the new data to the data set on iSENSE
        if credentials.isUser():
            response = iSenseAPI().appendDataSet(credentials.getUsername(), credentials.getPassword(),
                data_set_id, data_dictionary)
        elif credentials.isKey():
            response = iSenseAPI().appendDataSet(credentials.contrib_key(),
                data_set_id, data_dictionary)
        else:
            response = INVALID_CREDENTIALS 

        # After the data is uploaded refresh this data set object so it has the latest data 
        refresh()

        # Response is a dictionary. It can be used to check if the post request was successful.
        # It also will contain any errors the server may have returned.
        return response

   


class Field(object):
    def __init__(self, field_id, name, field_type, unit="", restrictions=[]):
        self.__id = field_id
        self.__name = name
        self.__type = field_type
        self.__restrictions = restrictions
        self.__unit = unit

    def getType(self):
        return self.__type

    def getName(self):
        return self.__name

    def getId(self):
        return self.__id

    def getRestrictions(self):
        return self.__restrictions

    def getUnit(self):
        return self.__unit

# Credentials requires either (username and password) OR (key and contrib_name)
class Credentials(object):
    def __init__(self, username=None, password=None, 
                 contrib_key=None, contrib_name=None):

        self.__user = False
        self.__key = False

        if username and password:
            self.__user = True
            self.__username = username
            self.__password = password
        elif contrib_key and contrib_name:
            key = True
            self.__contrib_key = contrib_key
            self.__contrib_name = contrib_name


    def getUsername(self):
        return self.__username

    def getPassword(self):
        return self.__password

    def getContribKey(self):
        return self.__contrib_key

    def getContribName(self):
        return self.__contrib_name

    # Check if the credentials are for a user
    def isUser(self):
        return self.__user

    # Check if the credentials are a key
    def isKey(self):
        return self.__key


def createProject(name, credentials):
    if credentials.isUser():
        username = credentials.getUsername()
        password = credentials.getPassword()
        response = iSenseAPI.createProject(username, password, name)
    else:
        return "Credentials must be for a User"

    # If project was created the id will be in the response. Response is a dictionary.
    return response 