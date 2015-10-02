#include "include/API.h"

// For picojson
using namespace picojson;

/* This file requires Boost. Please make sure to run the following command
 * before compiling:
 * sudo apt-get install libboost-test-dev
 * The above command will install Boost into your development environment.
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>

/* This file will run a series of tests on the C++ API
 * The following methods are tested:
 *
 * get_check_user()
 * get_project_fields()
 * get_datasets_and_mediaobjects()
 * get_projects_search()
 * get_dataset()
 * get_dataset_ID()
 * get_field_ID()
 * post_json_email()
 * post_json_key()
 * append_email_byID()
 * append_email_byName()
 * append_key_byID()
 * append_key_byName()
 *
 */

// Constants for running C++ Tests
const std::string test_project_ID = "1406";
const std::string test_project_name = "C++ Testing ";
const std::string test_dataset_name = "Testing ";
const std::string test_dataset_ID_email = "11366";
const std::string test_dataset_ID_key = "11367";
const std::string test_email = "j@j.j";
const std::string test_password = "j";
const std::string test_project_key = "123";
const std::string test_project_label = "Boost";
const std::string test_search_true = "test";
const std::string test_search_empty = "abcdefghig";


/*
 * This is a derived class to quickly test the append_byID functions.
 * It is derived from iSENSE, and by doing this I can create a public function
 * which can access the two append_byID functions publicly.
 * Users would have to derive the class to do the same - simply using iSENSE,
 * the append_byID functions will be protected and unaccessable (since they
 * should be using the "append_byName, using dataset names rather than IDs)
 */
class Test: public iSENSE {
 public:
  void check_set_dataset_ID(std::string proj_dataset_ID) {
    set_dataset_ID(proj_dataset_ID);
  }
  bool check_append_key_byID(std::string dataset_ID) {
    return append_key_byID(dataset_ID);
  }
  bool check_append_email_byID(std::string dataset_ID) {
    return append_email_byID(dataset_ID);
  }
};


// Test the Check User method.
BOOST_AUTO_TEST_CASE(get_check_user) {
  iSENSE test;

  // Add test project
  test.set_project_ID(test_project_ID);
  test.set_project_title(test_dataset_name + test.generate_timestamp());

  // This is a valid email / password - so it should return true.
  BOOST_REQUIRE(test.set_email_password(test_email, test_password) == true);
}


// Test the GET project fields method.
BOOST_AUTO_TEST_CASE(get_project_fields) {

  // This iSENSE object should work.
  iSENSE test_true(test_project_ID, test_project_name, test_project_label, test_project_key);
  BOOST_REQUIRE(test_true.get_project_fields() == true);

  // This one should fail.
  iSENSE test_false;
  BOOST_REQUIRE(test_false.get_project_fields() == false);
}


// Test the get_datasets / media objects method
BOOST_AUTO_TEST_CASE(get_datasets_and_mediaobjects) {
  iSENSE test_true(test_project_ID, test_project_name, test_project_label, test_project_key);

  // Make the title unique
  test_true.set_project_title(test_dataset_name + test_true.generate_timestamp());

  // Test the GET method, should work for this project ID
  BOOST_REQUIRE(test_true.get_datasets_and_mediaobjects() == true);

  iSENSE test_false;

  // This should fail.
  BOOST_REQUIRE(test_false.get_datasets_and_mediaobjects() == false);
}


// Test the search project method
BOOST_AUTO_TEST_CASE(get_projects_search) {
  iSENSE test_true(test_project_ID, test_project_name, test_project_label, test_project_key);

  std::vector<std::string> project_titles;
  project_titles = test_true.get_projects_search(test_search_true);

  // This should not be empty.
  BOOST_REQUIRE(project_titles.empty() == false);

  iSENSE test_false;

  project_titles.clear();
  project_titles = test_false.get_projects_search(test_search_empty);

  // This should be empty, since its a blank search term.
  BOOST_REQUIRE(project_titles.empty() == true);
}


// Test the get dataset method
BOOST_AUTO_TEST_CASE(get_dataset) {
  iSENSE test_true("106", "test", "BOOST Test", "123");

  std::vector<std::string> baseball_hits;
  baseball_hits = test_true.get_dataset("MLB Team Statistics 2013", "Runs");

  BOOST_REQUIRE(baseball_hits.empty() == false);

  iSENSE test_false;

  baseball_hits.clear();
  baseball_hits = test_false.get_dataset("MLB Team Statistics 2013", "Runs");

  BOOST_REQUIRE(baseball_hits.empty() == true);
}


// Test the get_dataset_ID method
BOOST_AUTO_TEST_CASE(get_dataset_ID) {
  iSENSE test_true("106", "test", "BOOST Test", "123");

  // Pull down datasets
  test_true.get_datasets_and_mediaobjects();

  // Try and get dataset ID for the given dataset name.
  std::string datasetID = test_true.get_dataset_ID("MLB Team Statistics 2013");

  BOOST_REQUIRE(datasetID == "2801");

  iSENSE test_false;

  // Not pulling down datasets (or even setting a project ID!)
  // So this should be equal to GET_ERROR
  datasetID = test_false.get_dataset_ID("MLB Team Statistics 2013");

  BOOST_REQUIRE(datasetID == GET_ERROR);
}


// Test the get_field_ID method
BOOST_AUTO_TEST_CASE(get_field_ID) {
  iSENSE test_true("106", "test", "BOOST Test", "123");

  // Pull down fields
  test_true.get_project_fields();

  // Try and get a field ID for the given field.
  std::string fieldID = test_true.get_field_ID("Wins");

  BOOST_REQUIRE(fieldID == "637");

  iSENSE test_false;

  // Not pullin down fields, or setting a project ID, so this should be
  // equal to GET_ERROR.
  fieldID = test_false.get_field_ID("Wins");

  BOOST_REQUIRE(fieldID == GET_ERROR);
}


// Test POST with Contributor keys
BOOST_AUTO_TEST_CASE(post_JSON_withKey) {
  std::string project_title = "POST Test with Key ";
  iSENSE test(test_project_ID, project_title, test_project_label, test_project_key);

  // Make the title unique
  test.set_project_title(project_title  + test.generate_timestamp());

  // Push some data back
  test.push_back("Timestamp", test.generate_timestamp());
  test.push_back("Number", "123");
  test.push_back("Text", "ABC");

  BOOST_REQUIRE(test.post_json_key() == true);
}


// Test POST with Email/Password
BOOST_AUTO_TEST_CASE(post_JSON_withEmail) {
  std::string project_title = "POST Test with Email ";
  iSENSE test;

  // Make the title unique + set email/password
  test.set_project_ID(test_project_ID);
  test.set_project_title(project_title  + test.generate_timestamp());
  test.set_email_password(test_email, test_password);

  // Push some data back
  test.push_back("Timestamp", test.generate_timestamp());
  test.push_back("Number", "123");
  test.push_back("Text", "ABC");

  BOOST_REQUIRE(test.post_json_email() == true);
}


// Test Appending with Dataset IDs
// (Email / Password)
BOOST_AUTO_TEST_CASE(append_withDatasetID_byEmail) {
  Test test;
  std::string test_dataset_name_email = "C++ Dataset Append Test Email";

  // Add project info / dataset info to the object
  test.set_project_ID(test_project_ID);
  test.set_project_title(test_dataset_name_email);
  test.check_set_dataset_ID(test_dataset_ID_email);
  test.set_email_password(test_email, test_password);

  test.push_back("Number", "123456789");
  test.push_back("Text", "Datset ID Test - Email");
  test.push_back("Timestamp", test.generate_timestamp());

  BOOST_REQUIRE(test.check_append_email_byID(test_dataset_ID_email) == true);
}


// Test Appending with Dataset names
// (Email / Password)
BOOST_AUTO_TEST_CASE(append_withDatasetName_byEmail) {
  iSENSE test;
  std::string test_dataset_name_email = "C++ Dataset Append Test Email";

  // Add project info / dataset info to the object
  test.set_project_ID(test_project_ID);
  test.set_project_title(test_dataset_name_email);
  test.set_email_password(test_email, test_password);

  // Push data back to the object.
  test.push_back("Number", "123456789");
  test.push_back("Text", "Dataset Name Test - Email");
  test.push_back("Timestamp", test.generate_timestamp());

  BOOST_REQUIRE(test.append_email_byName(test_dataset_name_email) == true);
}


// Test Appending with Dataset IDs
// (Contributor keys)
BOOST_AUTO_TEST_CASE(append_withDatasetID_byKey) {
  Test test;
  std::string test_dataset_name_key = "key test";

  // Add project info / dataset info to the object
  test.set_project_ID(test_project_ID);
  test.set_project_title(test_project_name);
  test.check_set_dataset_ID(test_dataset_ID_key);
  test.set_project_label(test_project_label);
  test.set_contributor_key(test_project_key);

  // Push data back to the object.
  test.push_back("Number", "123456789");
  test.push_back("Text", "DatasetID Test --- Key");
  test.push_back("Timestamp", test.generate_timestamp());

  BOOST_REQUIRE(test.check_append_key_byID(test_dataset_ID_key) == true);
}


// Test Appending with Dataset names
// (Contributor keys)
BOOST_AUTO_TEST_CASE(append_withDatasetName_byKey) {
  iSENSE test;
  std::string test_dataset_name_key = "key test";

  // Add project info / dataset info to the object
  test.set_project_ID(test_project_ID);
  test.set_project_title(test_project_name);
  test.set_project_label(test_project_key);
  test.set_contributor_key(test_project_key);

  // Push data back to the object.
  test.push_back("Number", "123456789");
  test.push_back("Text", "Dataset Name Test -- Key");
  test.push_back("Timestamp", test.generate_timestamp());

  BOOST_REQUIRE(test.append_key_byName(test_dataset_name_key) == true);
}
