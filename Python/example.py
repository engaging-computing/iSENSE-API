import iSense

# Get a Project Object
my_project = iSense.Project(1200)
user_credentials = iSense.Credentials(username="your email", password="your password")

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
new_project = iSense.createProject("Test Project created in python.", user_credentials)
print("\nNew Projects Name:")
print(new_project.getName())

# Add a key to the project
new_project.addKey(user_credentials, "key", "key") # Change first key to MyKeyLabel once API gets fixed
key_credentials = iSense.Credentials(contrib_key="key", contrib_name="Python User")

# Add fields to project
print("\n Add some fields to project:")
# Add a some fields to the new Project
num_field = new_project.addField(user_credentials, "My Number Field", iSense.NUMBER, "feet")
lat_field = new_project.addField(user_credentials, None, iSense.LATITUDE, None)
lon_field = new_project.addField(user_credentials, None, iSense.LONGITUDE, None)
# "field_array = new_project.getFields()" would return an array containing the three fields at this point
print num_field.getName()
print lat_field.getName()
print lon_field.getName()


# Upload Data to our new project.
# Data must be in the format {"fieldId": [data,data2,data3[], "fieldID2": [data, data2, data3]}
print("\nData Set Count Before Upload:")
print(new_project.getDataSetCount())
data_to_be_uploaded = {str(num_field.getId()): [1, 2, 3, 4] }
new_ds = new_project.createDataSet("My new data set", data_to_be_uploaded, user_credentials)
print("\nData Set Count After Upload:")
print(new_project.getDataSetCount())
print("Data From new DS:")
print(new_ds.getData())

# Upload same data again but with key
ds_from_key = new_project.createDataSet("My new data set", data_to_be_uploaded, key_credentials)
print("\nData Set Count After Upload:")
print(new_project.getDataSetCount())
print("Data From DS uploaded with key:")
print(ds_from_key.getData())

# Append Data
# data_to_be_appended = {str(num_field.getId()): [4] }
# new_ds.appendData(data_to_be_appended, user_credentials)
# print("Data user uploaded after appending 4:")
# print(new_ds.getData())

# data_to_be_appended = {str(num_field.getId()): [4] }
# ds_from_key.appendData(data_to_be_appended, key_credentials)
# print("Data uploaded with key after appending 4:")
# print(ds_from_key.getData())

# Editing Data from 1, 2, 3, 4 -> 5, 6, 7, 8
data_to_be_edited = {str(num_field.getId()): [5, 6, 7, 8] }
new_ds.editData(data_to_be_edited, user_credentials)
print("Data user uploaded after editing to 5, 6, 7, 8:")
print(new_ds.getData())

# Editing the data set that was uploaded with a key
data_to_be_edited = {str(num_field.getId()): [5, 6, 7, 8] }
ds_from_key.editData(data_to_be_edited, key_credentials)
print("Data user uploaded after editing to 5, 6, 7, 8:")
print(new_ds.getData())