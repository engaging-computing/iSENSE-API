import iSense

# Get a Project Object
my_project = iSense.Project(1200)
user_credentials = iSense.Credentials(username="myusername@email.com", password="my_password")
key_credentials = iSense.Credentials(contrib_key="MyProjectsKey", contrib_name="My Name")

# Print some info about the project
print("Project id:")
print(my_project.getId())
print("Project name:")
print(my_project.getName())

# Get all the fields assosiated with the project
field_array = my_project.getFields()

# Print info about the fields
print("\nType:")
print(field_array[0].getType())
print("Name:")
print(field_array[0].getName())
print("Restrictions:")
print(field_array[0].getRestrictions()) 
print("Units:")
print(field_array[0].getUnit())

print("\nSee if field is a number.")
print(field_array[0].getType() == iSense.NUMBER)

# Print all the names of the data sets
print("\nNames of data sets and their data:")
data_set_array = my_project.getDataSets()
for data_set in data_set_array:
	print(data_set.getName())
	print(data_set.getData())


# Prints the data from the first data set that is assosiated with the first field
data_set = data_set_array[0]
field = field_array[0]

print("\ndata sets field data")
print(data_set.getDataByField(field))

print("\nprojects field data")
print(my_project.getDataByField(field))


# Create our own Project
response = iSense.createProject("Test Project created in python.", user_credentials)
new_project = iSense.Project(response['id'])
print("\nNew Projects Name:")
print(new_project.getName())

# Add a field to the new Project
server_response = new_project.addField(user_credentials, "My Number Field", iSense.NUMBER, "feet")

# This will give us the field object
field_array = new_project.getFields()
new_field = field_array[0]
print("\nField we created:")
print(new_field.getName())

# Upload Data to our new project.
# Data must be in the format {"fieldId": [data,data2,data3[], "fieldID2": [data, data2, data3]}
print("\nData Set Count Before Upload:")
print(new_project.getDataSetCount())
data_to_be_uploaded = {str(new_field.getId()): [1, 2, 3] }
new_project.createDataSet("My new data set", data_to_be_uploaded, user_credentials)
print("\nData Set Count After Upload:")
print(new_project.getDataSetCount())