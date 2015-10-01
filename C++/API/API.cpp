#include "include/API.h"

iSENSE::iSENSE() {                  // Default constructor
  upload_URL = EMPTY;
  get_URL = EMPTY;
  get_UserURL = EMPTY;
  title = EMPTY;
  project_ID = EMPTY;
  dataset_ID = EMPTY;
  contributor_key = EMPTY;
  contributor_label = "label";
  email = EMPTY;
  password = EMPTY;

  // This should be called ONCE, and it setups libcurl.
  curl_global_init(CURL_GLOBAL_ALL);
}

// Constructor with parameters
iSENSE::iSENSE(std::string proj_ID, std::string proj_title,
               std::string label, std::string contr_key) {
  set_project_ID(proj_ID);
  set_project_title(proj_title);
  set_project_label(label);
  set_contributor_key(contr_key);

  // This should be called ONCE for the entire programs lifetime.
  // This is simply to setup libcurl.
  curl_global_init(CURL_GLOBAL_ALL);
}

// Override the constructor, we need to make sure we cleanup libcurl.
iSENSE::~iSENSE() {
  // Make sure to cleanup libcurl exactly ONCE.
  curl_global_cleanup();
}

// Similar to the constructor with parameters, but can be called at anytime
void iSENSE::set_project_all(std::string proj_ID, std::string proj_title,
                             std::string label, std::string contr_key) {
  set_project_ID(proj_ID);
  set_project_title(proj_title);
  set_project_label(label);
  set_contributor_key(contr_key);
}

// Set the Project ID, and the upload/get URLs as well.
void iSENSE::set_project_ID(std::string proj_ID) {
  project_ID = proj_ID;
  upload_URL = devURL + "/projects/" + project_ID + "/jsonDataUpload";
  get_URL = devURL + "/projects/" + project_ID;
  get_project_fields();
}

// The user should also set the project title
void iSENSE::set_project_title(std::string proj_title) {
  title = proj_title;
}

// This one is optional, by default the label will be "label".
void iSENSE::set_project_label(std::string proj_label) {
  contributor_label = proj_label;
}

// As well as the contributor key they will be using
void iSENSE::set_contributor_key(std::string proj_key) {
  contributor_key = proj_key;
}

// Users should not call this and should instead use the appendbyName methods.
void iSENSE::set_dataset_ID(std::string proj_dataset_ID) {
  dataset_ID = proj_dataset_ID;
}

// Sets both email & password at once. Checks for valid email / password.
bool iSENSE::set_email_password(std::string proj_email,
                                std::string proj_password) {
  email = proj_email;
  password = proj_password;

  if (!get_check_user()) {
    std::cerr << "\nError in: set_email_password()\n";
    std::cerr << "Your email and password are **not** valid.\n";
    std::cerr << "You also need to have created an account on iSENSE.\n";
    std::cerr << "See: http://rsense-dev.cs.uml.edu/users/new \n\n";
    return false;
  }

  return true;
}

// Function that the user can call to just generate an ISO 8601 timestamp
std::string iSENSE::generate_timestamp(void) {
  time_t time_stamp;
  time(&time_stamp);
  char buffer[sizeof "2011-10-08T07:07:09Z"];

  // Timestamp is in the form of Year - Month - Day -- Hour - Minute - Seconds
  strftime(buffer, sizeof buffer, "%Y-%m-%dT%H:%M:%SZ", gmtime(&time_stamp));

  std::string cplusplus_timestamp(buffer);  // Convert char array to C++ string

  return cplusplus_timestamp;
}

void iSENSE::clear_data(void) {     // Resets the object and clears the map.
  upload_URL = EMPTY;
  get_URL = EMPTY;
  get_UserURL = EMPTY;
  title = EMPTY;
  project_ID = EMPTY;               // Set these to default values
  contributor_key = EMPTY;
  contributor_label = "label";
  email = EMPTY;
  password = EMPTY;

  map_data.clear();   // Clear the map_data

  // Clear the picojson objects

  // Under the hood picojson::objects are STL maps and picojson::arrays are STL vectors.
  upload_data.clear();
  fields_data.clear();
  owner_info.clear();

  // Uses picojson's = operator to clear the get_data obj and the fields obj.
  // Should check and see if this is bad.
  value new_object;
  get_data = new_object;
  fields = new_object;

  // Clear the field array (STL vectors)
  fields_array.clear();
  media_objects.clear();
  data_sets.clear();
}

// Add one piece of data to the map of data.
void iSENSE::push_back(std::string field_name, std::string data) {
  map_data[field_name].push_back(data);
}

// Add a field name / vector of strings (data) to the map.
void iSENSE::push_vector(std::string field_name, std::vector<std::string> data) {
  // This will store a copy of the vector<string> in the map.
  // If you decide to add more data, you will need to use the push_back method.
  map_data[field_name] = data;
}

// Searches for projects with the search term.
std::vector<std::string> iSENSE::get_projects_search(std::string search_term) {
  std::string get_search = devURL + "/projects?utf8=true&search=" \
                         + search_term + "&sort=updated_at&order=DESC";
  std::vector<std::string> project_titles;    // Vector of project titles.

  curl = curl_easy_init();            // get curl handle
  MEMFILE* json_file = memfopen();    // Writing JSON to this file.

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, get_search.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memfwrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, json_file);

    // Perform the request, res will get the return code.
    res = curl_easy_perform(curl);

    // Get HTTP code for error checking.
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  }

  // Check for errors.
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
  }
  if (http_code != HTTP_AUTHORIZED) {   // Didn't get HTTP code 200.
    std::cerr << "\nError in: get_projects_search(string search_term) \n";
    std::cerr << "Project search failed.\n";
    std::cerr << "Something with either curl, your internet connection, \n";
    std::cerr << "iSENSE or something else failed.\n";
    memfclose(json_file);

    return project_titles;
  }

  std::string errors;
  value projects_json;

  // Parse the JSON file.
  parse(projects_json, json_file->data, json_file->data + json_file->size, &errors);

  // If we have errors, print them out and quit.
  if (errors.empty() != true) {
    std::cerr << "\nError in: get_projects_search(string search_term)";
    std::cerr << "Error parsing JSON file.\n";
    std::cerr << "Error was: " << errors << "\n";
    memfclose(json_file);

    return project_titles;  // Return an empty vector
  }

  // Convert the JSON array (projects_json) into a vector of project title strings
  array projects_array = projects_json.get<array>();
  array::iterator it, the_begin, the_end;

  the_begin = projects_array.begin();
  the_end = projects_array.end();

  // Check and see if the projects_title JSON array is empty
  if (the_begin == the_end) {
    std::cerr << "\nError in: get_projects_search(string search_term) \n";
    std::cerr << "Project title array is empty.\n";
    memfclose(json_file);

    return project_titles;  // Return an empty vector
  }

  for (it = projects_array.begin(); it != projects_array.end(); it++) {
    object obj = it->get<object>();       // Get the current object
    std::string name = obj["name"].get<std::string>();   // Grab the field name
    project_titles.push_back(name);       // Push the name back into the vector.
  }

  // Always cleanup
  curl_easy_cleanup(curl);
  memfclose(json_file);
  return project_titles;     // Return a vector of project titles
}

bool iSENSE::get_check_user() {
  if (email == EMPTY || email.empty()) {
    std::cerr << "\nPlease set an email for this project.\n";
    return false;
  } else if (password == EMPTY || password.empty()) {
    std::cerr << "\nPlease set a password for this project.\n";
    return false;
  }

  get_UserURL = devURL + "/users/myInfo?email=" + email + "&password=" + password;

  curl = curl_easy_init();            // get curl handle

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, get_UserURL.c_str());       // GET URL
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, suppress_output); // Suppress output to STDOUT
    curl_easy_perform(curl);                                        // Perform the request
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);    // get HTTP code.
  }

  curl_easy_cleanup(curl);    // Always cleanup.

  if (http_code == HTTP_AUTHORIZED) {
    return true;
  }

  return false;
}

bool iSENSE::get_project_fields() {
  if (project_ID == EMPTY || project_ID.empty()) {
    std::cerr << "Error - project ID not set!\n";
    return false;
  }

  get_URL = devURL + "/projects/" + project_ID;

  curl = curl_easy_init();            // get curl handle
  MEMFILE* json_file = memfopen();    // Writing JSON to this file.

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, get_URL.c_str());         // GET URL
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memfwrite);     // Write function.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, json_file);         // JSON saved here.
    curl_easy_perform(curl);                                      // peform the GET request
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);  // Store HTTP Code.
  }
  curl_easy_cleanup(curl);              // Clean up cURL

  if (http_code != HTTP_AUTHORIZED) {   // Check for valid HTTP code.
    std::cerr << "Error in method: get_project_fields()\n";
    std::cerr << "GET project fields failed. Project ID may be invalid.\n";
    memfclose(json_file);   // Close the memfile

    return false;
  }
  std::string errors;

  // Parse the JSON file.
  parse(get_data, json_file->data, json_file->data + json_file->size, &errors);
  memfclose(json_file);   // Close the memfile

  if (!errors.empty()) {    // If we have errors, print them out and quit.
    std::cerr << "\nError parsing JSON file in method: get_project_fields()\n";
    std::cerr << "Error was: " << errors;

    return false;
  }
  fields = get_data.get("fields");      // Save the fields to the field array
  fields_array = fields.get<array>();

  return true;
}

bool iSENSE::get_datasets_and_mediaobjects() {
  // Check that the project ID is set properly.
  // When the ID is set, the fields are also pulled down as well.
  if (project_ID == EMPTY || project_ID.empty()) {
    std::cerr << "\nError in method: get_datasets_and_mediaobjects()\n";
    std::cerr << "\nPlease set a project ID!\n";
    return false;
  }

  // The "?recur=true" will make iSENSE return:
  // ALL datasets in that project and ALL media objects in that project
  get_URL = devURL + "/projects/" + project_ID + "?recur=true";

  curl = curl_easy_init();            // get curl handle
  MEMFILE* json_file = memfopen();    // Writing JSON to this file.

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, get_URL.c_str());         // GET URL
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memfwrite);     // Write function
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, json_file);         // JSON saved here
    curl_easy_perform(curl);                                      // Perform the request
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);  // Get HTTP Code
  }
  curl_easy_cleanup(curl);    // Clean up cURL

  if (http_code != HTTP_AUTHORIZED) {
    std::cerr << "\nError in: get_datasets_and_mediaobjects().\n";
    std::cerr << "GET project fields failed.\n";
    std::cerr << "Is the project ID you entered valid?\n";
    memfclose(json_file);   // Close the memfile

    return false;
  }

  std::string errors;

  // Parse the JSON file.
  parse(get_data, json_file->data, json_file->data + json_file->size, &errors);
  memfclose(json_file);   // close the memfile

  if (errors.empty() != true) {   // If we have errors, print them out and quit.
    std::cerr << "\nError in method: get_datasets_and_mediaobjects()\n";
    std::cerr << "Parsing JSON file failed. Error was: " << errors;
    return false;
  }

  fields = get_data.get("fields");        // Save the fields to the field array
  fields_array = fields.get<array>();

  value temp = get_data.get("dataSets");  // Save the datasets to the datasets array
  data_sets = temp.get<array>();

  temp = get_data.get("mediaObjects");    // Save the media objs to the media objs array
  media_objects = temp.get<array>();

  temp = get_data.get("owner");           // Save the owner info.
  owner_info = temp.get<object>();

  return true;
}

std::vector<std::string> iSENSE::get_dataset(std::string dataset_name,
                                             std::string field_name) {
  std::vector<std::string> vector_data;

  // Make sure a valid project ID has been set
  if (project_ID == EMPTY || project_ID.empty()) {
    std::cerr << "\n\nError in method: get_dataset(string, string)\n";
    std::cerr << "Please set a project ID!\n";
    return vector_data;
  }

  // First call get_datasets_and_mediaobjects() and see if that is sucessful.
  if (!get_datasets_and_mediaobjects()) {
    std::cerr << "\n\nError in method: get_dataset(string, string)\n";
    std::cerr << "Failed to get datasets.\n";
    return vector_data;
  }

  array::iterator it, the_begin, the_end;
  the_begin = data_sets.begin();
  the_end = data_sets.end();

  if (the_begin == the_end) {   // Check and see if the data_sets array is empty
    std::cerr << "\n\nError in method: get_dataset(string, string)\n";
    std::cerr << "Datasets array is empty.\n";
    return vector_data;  // this is an empty vector
  }

  std::string dataset_ID = get_dataset_ID(dataset_name);
  std::string field_ID = get_field_ID(field_name);

  // If either dataset ID or field ID threw an error, quit.
  if (dataset_ID == GET_ERROR || field_ID == GET_ERROR) {
    std::cerr << "\n\nUnable to return a vector of data.\n";
    std::cerr << "Either the dataset / field names are incorrect, \n";
    std::cerr << "Or the project ID is wrong.\n";

    return vector_data;   // this is an empty vector
  }

  // This outer for loop is for going through all datasets in the project
  for (it = data_sets.begin(); it != data_sets.end(); it++) {
    object obj = it->get<object>();         // Get the current object
    std::string id = obj["id"].to_str();    // Get that object's dataset ID

    // If we have the right object, we can now start looking for the data array
    if (id == dataset_ID) {
      // obtain a const reference to the map (as seen on picoJSON github page)
      const object& cur_obj = it->get<object>();

      // This basically lets us iterate through the current object
      for (object::const_iterator i = cur_obj.begin(); i != cur_obj.end(); i++) {
        if (i->first == "data") {   // When we get here, we've found the data array! WOO HOO!
          array dataset_list = i->second.get<array>();

          // Go through the array and push_back data points for the given field name
          for (array::iterator iter = dataset_list.begin(); iter != dataset_list.end(); iter++) {
            object tmp_obj = iter->get<object>();               // Temp obj to
            std::string data_pt = tmp_obj[field_ID].to_str();   // get data point.
            vector_data.push_back(data_pt);                     // finally push_back.
          }
          return vector_data;   // Return the vector of data for the given field name.
        }
      }
    }
  }

  std::cerr << "\n\nError in method: get_dataset(string, string)\n";
  std::cerr << "Failed to get dataset. \n";
  std::cerr << "Check the following & make sure they are correct:\n";
  std::cerr << "field name, dataset name, project ID\n";

  return vector_data;     // This should be empty, or may not contain all the data.
}



bool iSENSE::post_json_key() {
  if(!empty_project_check(POST_KEY, "post_json_key()")) {
    return false;
  }

  upload_URL = devURL + "/projects/" + project_ID + "/jsonDataUpload";
  http_code = post_data_function(POST_KEY);

  if(!check_http_code(http_code, "post_json_key()")) {
    return false;
  }

  return true;
}

bool iSENSE::post_json_email() {
  if(!empty_project_check(POST_EMAIL, "post_json_email()")) {
    return false;
  }

  upload_URL = devURL + "/projects/" + project_ID + "/jsonDataUpload";
  http_code = post_data_function(POST_EMAIL);

  if(!check_http_code(http_code, "post_json_email()")) {
    return false;
  }

  return true;
}

// Append using a contributor key
bool iSENSE::append_key_byID(std::string dataset_ID) {
  if(!empty_project_check(APPEND_KEY, "append_key_byID")) {
    return false;
  }

  set_dataset_ID(dataset_ID);                       // Set the dataset_ID
  upload_URL = devURL + "/data_sets/append";        // Set the append API URL
  http_code = post_data_function(APPEND_KEY);       // Call helper function.

  if(!check_http_code(http_code, "append_key_byID")) {
    return false;
  }

  return true;
}

// Appends to a dataset using its dataset name. Calls append_key_byID
bool iSENSE::append_key_byName(std::string dataset_name) {
  if(!empty_project_check(APPEND_KEY, "append_key_byName")) {
    return false;
  }

  get_datasets_and_mediaobjects();    // Make sure we've got all the datasets.
  std::string dataset_ID = get_dataset_ID(dataset_name);  // Get the dataset ID

  if (dataset_ID != GET_ERROR) {
    return append_key_byID(dataset_ID);     // Call append byID function.
  }

  // If we got here, we failed to find that dataset name in the current project.
  std::cerr << "\nError in method: append_key_byName()\n";
  std::cerr << "Failed to find the dataset name in project # " << project_ID;
  return false;
}

// Append using email and password
bool iSENSE::append_email_byID(std::string dataset_ID) {
  if(!empty_project_check(APPEND_EMAIL,"append_email_byID")) {
    return false;
  }

  set_dataset_ID(dataset_ID);                           // Set the dataset_ID
  upload_URL = devURL + "/data_sets/append";            // Set the API URL
  http_code = post_data_function(APPEND_EMAIL);         // Call helper function.

  if(!check_http_code(http_code, "append_email_byID()")) {
    return false;
  }

  return true;
}

// Appends to a dataset using its dataset name. Calls append_email_byID
bool iSENSE::append_email_byName(std::string dataset_name) {
  if(!empty_project_check(APPEND_EMAIL,"append_email_byName")) {
    return false;
  }

  get_datasets_and_mediaobjects();      // Make sure we've got all the datasets
  std::string dataset_ID = get_dataset_ID(dataset_name);  // Get the dataset ID

  if (dataset_ID != GET_ERROR) {
    return append_email_byID(dataset_ID);   // Call append byID function.
  }

  // If we got here, we failed to find that dataset name in the current project.
  std::cerr << "\nError in method: append_email_byName()\n";
  std::cerr << "Failed to find the dataset name in project # " << project_ID;
  return false;
}

//******************************************************************************
// Below this point are helper functions. Users should only call functions
// above this point, as these are all called by the API functions.

// Convert field name to field ID
std::string iSENSE::get_field_ID(std::string field_name) {
  array::iterator it;

  // Check and see if the fields object is empty
  if (fields.is<picojson::null>() == true) {
    std::cerr << "\nError in method: get_field_ID()\n";
    std::cerr << "Field array wasn't set up.";
    std::cerr << "Have you pulled the fields off iSENSE?\n";
    return GET_ERROR;
  }

  // We made an iterator above, that will let us run through the fields
  for (it = fields_array.begin(); it != fields_array.end(); it++) {
    object obj = it->get<object>();                    // Get the current object
    std::string field_ID = obj["id"].to_str();         // Grab the field ID
    std::string name = obj["name"].get<std::string>(); // Grab the field name

    if (name == field_name) {   // Found the given field name
      return field_ID;          // So return the field ID
    }
  }

  std::cerr << "\nError in method: get_field_ID()\n";
  std::cerr << "Unable to find the field ID for the given field name.\n";
  return GET_ERROR;
}

// Convert dataset name to dataset ID
std::string iSENSE::get_dataset_ID(std::string dataset_name) {
  array::iterator it;

  // Check and see if the datasets object is empty
  // Need to find out how to do this using picojson arrays!

  // This is similar to the get_field_ID function loop.
  for (it = data_sets.begin(); it != data_sets.end(); it++) {
    object obj = it->get<object>();                    // Get the current object
    std::string dataset_ID = obj["id"].to_str();         // Grab the dataset ID
    std::string name = obj["name"].get<std::string>(); // Grab the dataset name

    if (name == dataset_name) {   // We found the dataset name
      return dataset_ID;          // So return the dataset ID
    }
  }

  std::cerr << "\nError in method: get_dataset_ID()\n";
  std::cerr << "Unable to find the dataset ID for the given dataset name.\n";
  return GET_ERROR;
}

// Format JSON Upload strings.
void iSENSE::format_upload_string(int post_type) {
  upload_data["title"] = value(title);          // Add the title

  switch (post_type) {
    case POST_KEY:
      upload_data["contribution_key"] = value(contributor_key);
      upload_data["contributor_name"] = value(contributor_label);
      break;

    case APPEND_KEY:
      upload_data["contribution_key"] = value(contributor_key);
      upload_data["contributor_name"] = value(contributor_label);
      upload_data["id"] = value(dataset_ID);
      break;

    case POST_EMAIL:
      upload_data["email"] = value(email);
      upload_data["password"] = value(password);
      break;

    case APPEND_EMAIL:
      upload_data["email"] = value(email);
      upload_data["password"] = value(password);
      upload_data["id"] = value(dataset_ID);
      break;
  }

  array::iterator it;               // Grab all the fields using an iterator.
  std::vector<std::string> *vect;   // Pointer to one of the vectors in the map

  // Check and see if the fields object is empty
  if (fields.is<picojson::null>() == true) {
    std::cerr << "\nError in method: format_upload_string()\n";
    std::cerr << "Field array wasn't set up.\n";
    std::cerr << "Have you pulled the fields off iSENSE?\n";
    return;
  }

  // We made an iterator above, that will let us run through the fields
  for (it = fields_array.begin(); it != fields_array.end(); it++) {
    object obj = it->get<object>();                     // Get the current object
    std::string field_ID = obj["id"].to_str();          // Grab the field ID
    std::string name = obj["name"].get<std::string>();  // Grab the field name

    // Add the data in that field's vector to the fields_array object.
    vect = &map_data[name];
    format_data(vect, it, field_ID);
  }

  // Add the field_data obj to the upload_data obj
  upload_data["data"] = value(fields_data);
}

// This makes format_upload_string() much shorter.
void iSENSE::format_data(std::vector<std::string> *vect,
                         array::iterator it, std::string field_ID) {
  std::vector<std::string>::iterator x;
  value::array data;        // picojson::value::array, represents a JSON array.

  for (x = vect -> begin(); x < vect -> end(); x++) {
    data.push_back(value(*x));    // Push all the vector data into a JSON array.
  }

  fields_data[field_ID] = value(data); // Push the JSON array to the upload_data obj.
}


// This function is called by all of the POST functions.
int iSENSE::post_data_function(int post_type) {
  // Upload_URL must have already been set. Otherwise the request will fail.
  if (upload_URL == EMPTY || upload_URL.empty()) {
    std::cerr << "\nError in method: post_data_function()\n";
    std::cerr << "Please set a valid upload URL.\n";
    return CURL_ERROR;
  }

  format_upload_string(post_type);        // format the upload string

  CURL *curl = curl_easy_init();          // cURL object
  long http_code = 0;                     // HTTP status code

  struct curl_slist *headers = NULL;      // Headers for uploading via JSON
  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Accept-Charset: utf-8");
  headers = curl_slist_append(headers, "charsets: utf-8");
  headers = curl_slist_append(headers, "Content-Type: application/json");

  if (curl) {
    // Get the upload JSON as a std::string
    std::string upload_str = (value(upload_data).serialize());

    // POST data
    curl_easy_setopt(curl, CURLOPT_URL, upload_URL.c_str());        // URL
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, upload_str.c_str()); // JSON data
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);            // JSON Headers

    // Verbose debug output - turn this on if you are having problems uploading.
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    std::cout << "\nrSENSE response: \n";

    curl_easy_perform(curl);// Perform the request, res will get the return code
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);          // Clean up curl.

    return http_code;                 // Return the HTTP code we get from curl.
  }

  return CURL_ERROR;                  // If curl fails, return CURL_ERROR (-1).
}

// Checks to see if the given project has been properly setup.
// Shouldn't be any empty values, such as project ID, contributor key, etc.
bool iSENSE::empty_project_check(int type, std::string method) {
  // Check email based values, such as email & password.
  if (type == POST_EMAIL || type == APPEND_EMAIL) {
    if (email == EMPTY || email.empty()) {
      std::cerr << "\nError in method: " << method << "\n";
      std::cerr << "Please set an email address!\n";
      return false;
    }
    if (password == EMPTY || password.empty()) {
      std::cerr << "\nError in method: " << method << "\n";
      std::cerr << "Please set a password!\n";
      return false;
    }
  }

  // Check key based values, such as contributor key and label.
  if (type == POST_KEY || type == APPEND_KEY) {
    if (contributor_key == EMPTY || contributor_key.empty()) {
      std::cerr << "\nError in method: " << method << "\n";
      std::cerr << "Please set a contributor key!\n";
      return false;
    }
    if (contributor_label == "label" || contributor_label.empty()) {
      // If a label wasn't set, automatically set it to "cURL"
      contributor_label = "cURL";
    }
  }

  // The rest are general checks that should not be empty, since the calling
  // method depends on them being set properly.
  if (project_ID == EMPTY || project_ID.empty()) {
    std::cerr << "\nError in method: " << method << "\n";
    std::cerr << "Please set a project ID!\n";
    return false;
  }
  if (title == EMPTY || title.empty()) {
    std::cerr << "\nError in method: " << method << "\n";
    std::cerr << "Please set a project title!\n";
    return false;
  }
  if (map_data.empty()) {
    std::cerr << "\nError in method: " << method << "\n";
    std::cerr << "Map of keys/data is empty.\n";
    std::cerr << "You should push some data back to this object.\n";
    return false;
  }

  return true;
}

// Checks a given HTTP code for errors.
bool iSENSE::check_http_code(int http_code, std::string method) {
  if(http_code == HTTP_AUTHORIZED) {
    return true;    // Print nothing for OK codes.
  }

  // Print out error messages.
  std::cerr << "\nError in method: " << method << "\n";
  std::cerr << "Request **failed**\n";
  std::cerr << "HTTP Response Code was: " << http_code << "\n";

  if (http_code == HTTP_UNAUTHORIZED) {
    std::cerr << "Try checking to make sure your contributor key is valid\n";
    std::cerr << "for the project you are trying to contribute to.\n";
  }
  else if (http_code == HTTP_NOT_FOUND) {
    std::cerr << "Unable to find that project ID.\n";
  }
  else if (http_code == HTTP_UNPROC_ENTRY) {
    std::cerr << "Something went wrong with your formatting.\n";
    std::cerr << "Try formatting your data differently, using a contributor \n";
    std::cerr << "key instead of an email, or asking for help from others. \n";
    std::cerr << "You can also try running the the program with the debug \n";
    std::cerr << "method enabled, by typing: object_name.debug()\n";
    std::cerr << "This will output a ton of data to the console and \n";
    std::cerr << "may help you in debugging your program.\n";
  }
  else if (http_code == CURL_ERROR) {
    std::cerr << "Curl failed for some unknown reason.\n";
    std::cerr << "Make sure you've installed curl / libcurl, and have the \n";
    std::cerr << "picojson header file as well.\n";
  }
  return false;
}

// Call this function to dump all the data in the given object.
void iSENSE::debug() {
  std::cout << "\nProject Title: " << title << "\n";
  std::cout << "Project ID: " << project_ID << "\n";
  std::cout << "Dataset ID: " << dataset_ID << "\n";
  std::cout << "Contributor Key: " << contributor_key << "\n";
  std::cout << "Contributor Label: " << contributor_label << "\n";
  std::cout << "Email Address: " << email << "\n";
  std::cout << "Password: " << password << "\n";
  std::cout << "Upload URL: " << upload_URL << "\n";
  std::cout << "GET URL: " << get_URL << "\n";
  std::cout << "GET User URL: " << get_UserURL << "\n\n";

  std::cout << "Upload string (picojson object): \n";
  std::cout << value(upload_data).serialize() << "\n\n";

  std::cout << "Upload Fields Data (picojson object) \n";
  std::cout << value(fields_data).serialize() << "\n\n";

  std::cout << "GET Data (picojson value): \n";
  std::cout << get_data.serialize() << "\n\n";

  std::cout << "GET Field Data (picojson value): \n";
  std::cout << fields.serialize() << "\n\n";

  std::cout << "GET Fields array (picojson array): \n";
  std::cout << value(fields_array).serialize() << "\n\n";

  // This part may get huge depending on the project!
  // May want a confirm if the user actually wants to display these.
  std::cout << "Datasets (picojson array): ";
  std::cout << value(data_sets).serialize() << "\n\n";

  std::cout << "Media objects (picojson array): ";
  std::cout << value(media_objects).serialize() << "\n\n";

  std::cout << "Owner info (picojson object): ";
  std::cout << value(owner_info).serialize() << "\n\n";

  std::cout << "Map data: \n\n";

  // These for loops will dump all the data in the map.
  // Good for debugging.
  std::map<std::string, std::vector<std::string>>::iterator it;
  std::vector<std::string>::iterator vect;
  for (it = map_data.begin(); it != map_data.end(); it++) {
        std::cout << it->first << " ";

        for (vect = (it->second).begin(); vect != (it->second).end(); vect++) {
              std::cout << *vect << " ";
        }
        std::cout << "\n";
  }
}


//******************************************************************************
// These are needed for picojson & libcURL.
MEMFILE*  memfopen() {
  MEMFILE* mf = (MEMFILE*) malloc(sizeof(MEMFILE));
  mf->data = NULL;
  mf->size = 0;
  return mf;
}

void memfclose(MEMFILE* mf) {
  if (mf == NULL) {                 // Double check to make sure that mf exists.
    return;
  }
  if (mf != NULL && mf->data) {     // OK to free the char array
    free(mf->data);
  }
  free(mf);                         // And OK to free the structure
}

// Simple function only used by the get_check_user function to
// suppress curl's output to the screen.
size_t suppress_output(char* ptr, size_t size, size_t nmemb, void* stream) {
  return size * nmemb;
}

size_t memfwrite(char* ptr, size_t size, size_t nmemb, void* stream) {
  MEMFILE* mf = (MEMFILE*) stream;
  int block = size * nmemb;

  if (!mf->data) {
    mf->data = (char*) malloc(block);
  } else {
    mf->data = (char*) realloc(mf->data, mf->size + block);
  }
  if (mf->data) {
    memcpy(mf->data + mf->size, ptr, block);
    mf->size += block;
  }

  return block;
}

char* memfstrdup(MEMFILE* mf) {
  char* buf = (char*)malloc(mf->size + 1);
  memcpy(buf, mf->data, mf->size);
  buf[mf->size] = 0;

  return buf;
}
