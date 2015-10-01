#ifndef API_h
#define API_h

// Windows likes the curl header this way.
#ifdef WIN32
#include <curl.h>

// Linux / other systems.
#else
#include <curl/curl.h>
#endif

#include "picojson/picojson.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <ctime>

// For picojson
using namespace picojson;

// Currently only rSENSE is supported. In the future, allow switching between
// dev and live.
const std::string dev_baseURL = "http://rsense-dev.cs.uml.edu";
const std::string live_baseURL = "http://isenseproject.org";
const std::string devURL = "http://rsense-dev.cs.uml.edu/api/v1";
const std::string liveURL = "http://isenseproject.org/api/v1";

// GET related constants
const int GET_NORMAL = 1;
const int GET_QUIET = 2;

// POST related constants
const int POST_KEY = 1;
const int APPEND_KEY = 2;
const int POST_EMAIL = 3;
const int APPEND_EMAIL = 4;

// HTTP Error codes
const int HTTP_AUTHORIZED = 200;
const int HTTP_UNAUTHORIZED = 401;
const int HTTP_NOT_FOUND = 404;
const int HTTP_CONFLICT = 409;
const int HTTP_UNPROC_ENTRY = 422;
const int CURL_ERROR = -1;

// Error checking constants
const std::string GET_ERROR = "ERROR";
const std::string EMPTY = "-----";

// This is required for libcurl to save the JSON from iSENSE in a C++ string.
size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up);

/*
 *    This is from the picojson example page
 *    I use it to save the JSON from iSENSE to memory (temporary)
 *    See the following URL for an example:
 *    https://github.com/kazuho/picojson/blob/master/examples/github-issues.cc
 */
typedef struct {
  char* data;         // response data from server
  size_t size;          // response size of data
} MEMFILE;

MEMFILE*  memfopen();
void memfclose(MEMFILE* mf);
size_t suppress_output(char* ptr, size_t size, size_t nmemb, void* stream);
size_t memfwrite(char* ptr, size_t size, size_t nmemb, void* stream);
char* memfstrdup(MEMFILE* mf);

class iSENSE {
public:
  // Constructors
  iSENSE();
  iSENSE(std::string proj_ID, std::string proj_title,
         std::string label, std::string contr_key);

  // Destructor for cleaning up stuff.
  ~iSENSE();

  // Similar to the constructor with parameters, but called after
  // the object is created. This way you can change the title/project ID/etc.
  void set_project_all(std::string proj_ID, std::string proj_title,
                       std::string label, std::string contr_key);

  // These can be used to manually project data. Be sure to set a contributor
  // key or email / password as well!
  void set_project_ID(std::string proj_ID);
  void set_project_title(std::string proj_title);
  void set_contributor_key(std::string proj_key);
  void set_project_label(std::string proj_label);

  // This should be used for setting the email / password for a project.
  // Returns true if the email / password are valid, or false if they are not.
  bool set_email_password(std::string proj_email, std::string proj_password);

  void clear_data();    // Resets the object and clears the map.
  void debug();         // For debugging, this method dumps all the data.

  /*  This function will push data back to the map.
   *  User must give the push_back function the following:
   *    1. Field name (as seen on iSENSE)
   *    2. Some data (in string format). For numbers, use std::to_string.
   *
   *  Note: this function merely pushes a single piece of data to the map.
   *  If you want to add more than one string of data, you should used either
   *  a loop or create a vector of strings and push that back to the object
   *  using the push_vector function below.                                      */
  void push_back(std::string field_name, std::string data);

  /* Add a field name / vector of strings (data) to the map.
   * See above notes. Behaves similar to the push_back function.
   * Also if you push a vector back, it makes a copy of the vector and saves it
   * in the map. You will need to use the push_back function to add more data!   */
  void push_vector(std::string field_name, std::vector<std::string> data);

  // Note: only returns the timestamp, does not add it to the map.
  std::string generate_timestamp(void);

  // iSENSE API functions
  bool get_check_user();      // Verifies email / password.
  bool get_project_fields();  // GETs fields off iSENSE

  // This function grabs fields, datasets, media objects and owner information
  // Saves these arrays into picojson arrays.
  bool get_datasets_and_mediaobjects();

  // Search for projects with the search term
  std::vector<std::string> get_projects_search(std::string search_term);

  // Return a vector of data given a field name
  std::vector<std::string> get_dataset(std::string dataset_name, std::string field_name);

  // Future: return a map of media objects
  // map<std::string, vector<std::string>> get_media_objects();

  bool post_json_email();          // Post using a email / password
  bool post_json_key();            // Post using contributor key

  /*  Notes about the append & edit functions
   *
   *  A Contributor key can append to datasets that YOUR KEY uploaded.
   *  Email & password can append to datasets YOU uploaded
   *  OR any datasets in projects YOU created.
   *
   *  You also do not need call get_datasets_and_mediaobjects() before using
   *  any of the editing and appending methods, as the append methods
   *  automatically call this function.
   */

  // Appends to a dataset using a dataset name and a key or email/password
  bool append_key_byName(std::string dataset_name);
  bool append_email_byName(std::string dataset_name);

  //****************************************************************************
  // Functions below this line should be ignored by users of the API, unless
  // an error occurs in one of them. Users should instead use the API functions
  // listed above.

  std::string get_dataset_ID(std::string dataset_name);
  std::string get_field_ID(std::string field_name);

  // Error methods - makes error checking simple.
  bool empty_project_check(int type, std::string method);
  bool check_http_code(int http_code, std::string method);

  // This formats the upload string
  void format_upload_string(int post_type);

  // This formats one FIELD ID : DATA pair
  void format_data(std::vector<std::string> *vect, array::iterator it, std::string field_ID);

  // This function makes a GET request via libcurl
  int get_data_funct(int get_type);

  // This function makes a POST request via libcurl
  int post_data_function(int post_type);

protected:

  /*  Users do not need to worry about dataset IDs. They only need to pass the
   *  append function a valid dataset name - the name as it appears on iSENSE.
   *  This method is marked as protected to prevent users from accessing it.     */
  void set_dataset_ID(std::string proj_dataset_ID);
  bool append_key_byID(std::string dataset_ID);
  bool append_email_byID(std::string dataset_ID);

private:
  /*  These two picojson objects will be used to upload to iSENSE.
   *  The upload_data object contains the entire upload string, in JSON format.
   *  Picojson can output this to a string and then we can pass that string
   *  to libcurl.
   *  The fields_data object is the object that the 'data' part
   *  contains in the upload_data object.
   *  Basically it is a bunch of key:values, with the key being the field ID
   *  and the value being an array of data (numbers/text/GPS coordinates/etc.    */
  object upload_data, fields_data, owner_info;

  /*  These three objects are the data that is pulled off iSENSE.
   *  The get_data object contains all the data we can pull off of iSENSE
   *  (what you find on /api/v1/projects/DATASET_ID_HERE).
   *  The fields object then contains just the field information, and the
   *  fields_array has that same data in an array form for iterating through it. */
  value get_data, fields;
  array fields_array, data_sets, media_objects;

  /*  Data to be uploaded to iSENSE. The string is the field name,
   *  the vector of strings contains all the data for that field name.           */
  std::map<std::string, std::vector<std::string> > map_data;

  //bool usingDev;            // Whether the user wants iSENSE or rSENSE
                              // (currently not implemented, future idea)

  // Data needed for processing the upload request
  std::string get_UserURL;        // URL to test credentials
  std::string get_URL;            // URL to get JSON from
  std::string upload_URL;         // URL to upload JSON to
  std::string title;              // title for the dataset
  std::string project_ID;         // project ID of the project
  std::string dataset_ID;         // dataset ID for appending
  std::string contributor_key;    // contributor key for the project
  std::string contributor_label;  // Label for the key, by default "cURL"
  std::string email;              // Email to be used to upload the data
  std::string password;           // Password to be used with an email address

  // libcurl objects / variables. Users should ignore this.
  // Defined once as the libcurl tutorial says to do:
  // http://curl.haxx.se/libcurl/c/libcurl-tutorial.html
  CURL *curl;             // curl handle
  CURLcode res;           // curl response code
  long http_code;         // HTTP status code
};

#endif
