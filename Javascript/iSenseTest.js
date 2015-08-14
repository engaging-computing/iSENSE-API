(function() {

  var assertException = function(cb, msg) {
    var failed = false;
    try {
      cb();
    } catch (e) {
      failed = true;
    }

    if (!failed) {
      throw 'Assertion failed: ' + msg;
    }
  }

  var assertNoException = function(cb, msg) {
    try {
      cb();
    } catch (e) {
      console.log(e.toString());
      throw 'Assertion failed: ' + msg;
    }
  }

  var assertEquals = function(val, cb, msg) {
    var comp;
    try {
      comp = cb();
    } catch (e) {
      throw 'Assertion failed (with exception' + e.toString() + '): ' + msg;
    }

    if (val !== comp) {
      throw 'Assertion failed: ' + msg;
    }
  }

  var credUser = ISense.Credentials({
    username: 'a@a.a',
    password: 'a'
  });

  var credContrib = ISense.Credentials({
    contribName: 'contributor1',
    contribKey: 'a'
  });

  var credBoth = ISense.Credentials({
    username: 'a@a.a',
    password: 'a',
    contribName: 'contributor2',
    contribKey: 'a'
  });

  var name = 'JS Test Project #' + Math.floor(Math.random() * 1000);
  var dataName1 = 'JS Data #' + Math.floor(Math.random() * 10000);
  var dataName2 = 'JS Data #' + Math.floor(Math.random() * 10000);
  var dataName3 = 'JS Data #' + Math.floor(Math.random() * 10000);
  var dataName4 = 'JS Data #' + Math.floor(Math.random() * 10000);

  //////////////////////////////////////////////////////////////////////////////
  // Test creating projects under different conditions
  //////////////////////////////////////////////////////////////////////////////

  var proj;
  assertNoException(function() {
    proj = ISense.createProject(name, credUser);
  }, 'Should have been able to correctly create project');

  assertEquals(name, function() {
    return proj._name;
  }, 'Name wasn\'t set correctly');

  assertException(function() {
    ISense.createProject('Bad #1', credContrib);
  }, 'Shouldn\'t have been able to create project with contrib key');

  assertException(function() {
    ISense.createProject('Bad #2');
  }, 'Shouldn\'t have been able to create project without credentials');

  assertException(function() {
    ISense.createproject('Bad #3', credBoth);
  }, 'Shouldn\'t have been able to create project with both credentials');

  //////////////////////////////////////////////////////////////////////////////
  // Test getting projects
  //////////////////////////////////////////////////////////////////////////////


  assertNoException(function() {
    var getProj = ISense.Project(proj._id);
  }, 'Should have been able to get project');

  assertException(function() {
    ISense.Project(-1);
  }, 'Shouldn\'t have been able to get project');

  //////////////////////////////////////////////////////////////////////////////
  // Test adding some fields
  //////////////////////////////////////////////////////////////////////////////

  var fields = [];
  assertNoException(function() {
    fields[0] = proj.addField('Field 1', ISense.TIMESTAMP, null, credUser);
    fields[1] = proj.addField('Field 2', ISense.NUMBER, null, credUser);
    fields[2] = proj.addField('Field 3', ISense.TEXT, null, credUser);
    fields[3] = proj.addField('Field 4', ISense.LATITUDE, null, credUser);
    fields[4] = proj.addField('Field 5', ISense.LONGITUDE, null, credUser);
  }, 'Should have been able to create fields');

  assertException(function() {
    proj.addField('Field 1', ISense.NUMBER, null, credUser);
  }, 'Shouldn\'t have been able to add duplicate field name');

  //assertException(function() {
  //  proj.addField('Field 6', ISense.TIMESTAMP, null, credUser);
  //}, 'Shouldn\'t have been able to add second timestamp field');

  assertException(function() {
    proj.addField('Field 7', ISense.NUMBER, null, credContrib);
  }, 'Shouldn\'t have been able to add field with contrib key');

  //////////////////////////////////////////////////////////////////////////////
  // Test adding contribution keys
  //////////////////////////////////////////////////////////////////////////////

  assertNoException(function() {
    proj.addContribKey('b', 'a', credUser);
  }, 'Should have been able to add contribution key');

  assertException(function() {
    proj.addContribKey('c', 'a', credUser);
  }, 'Shouldn\'t have been able to add duplicate contribution key');

  assertException(function() {
    proj.addContribKey('c', 'b', credContrib);
  }, 'Shouldn\'t have been able to add contribution key with contrib key');

  //////////////////////////////////////////////////////////////////////////////
  // Test adding data to projects with both types of credentials
  //////////////////////////////////////////////////////////////////////////////

  var data = {};
  data[fields[0]._id] = ['1990', '1991', '1992', '1993', '1994'];
  data[fields[1]._id] = [1, 2, 3, 4, 5];
  data[fields[2]._id] = ['A', 'B', 'C', 'D', 'E'];
  data[fields[3]._id] = [50, 51, 52, 53, 54];
  data[fields[4]._id] = [50, 51, 52, 53, 54];

  var dsetUser;
  assertNoException(function() {
    dsetUser = proj.addDataSet(dataName1, data, credUser);
    proj.addDataSet(dataName2, data, credUser);
    proj.addDataSet(dataName3, data, credUser);
    proj.addDataSet(dataName4, data, credUser);
  }, 'Should have been able to add data to project');

  var badData = {}
  data[fields[0]._id] = ['a', '1991', '1992', '1993', '1994'];
  data[fields[1]._id] = [1, 2, 3, 4, 5];
  data[fields[2]._id] = ['A', 'B', 'C', 'D', 'E'];
  data[fields[3]._id] = [50, 51, 52, 53, 54];
  data[fields[4]._id] = [50, 51, 52, 53, 54];

  assertException(function() {
    proj.addDataSet('Bad #4', badData, credUser);
  }, 'Shouldn\'t have been able to add data to project');

  var dsetContrib;
  assertNoException(function() {
    dsetContrib = proj.addDataSet(dataName1, data, credContrib);
    proj.addDataSet(dataName2, data, credContrib);
    proj.addDataSet(dataName3, data, credContrib);
    proj.addDataSet(dataName4, data, credContrib);
  }, 'Should have been able to add data with contribution key');

  //////////////////////////////////////////////////////////////////////////////
  // Test editing and appending data to data sets
  //////////////////////////////////////////////////////////////////////////////

  assertNoException(function() {
    dsetUser.editData(data, credUser);
    dsetContrib.editData(data, credUser);
  }, 'Should have been able to edit data set data as user');

  //assertNoException(function() {
  //  dsetContrib.editData(data, credContrib);
  //}, 'Should have been able to edit data set data as contributor');

  assertNoException(function() {
    dsetUser.appendData(data, credUser);
    dsetContrib.appendData(data, credUser);
  }, 'a');

  console.log(proj.getDataByField(fields[0]));

  console.log(proj.getDataByFields([fields[0]]));

  console.log('.: FIN :.');
})();
