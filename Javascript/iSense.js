var setRecur = function(recur) {
  if (typeof recur === 'undefined') {
    return false;
  } else {
    return recur;
  }
};

var iSense = {
  TIMESTAMP: 1,
  NUMBER: 2,
  TEXT: 3,
  LATITUDE: 4,
  LONGITUDE: 5,

  //URL: 'http://isenseproject.org/api/v1/',
  URL: 'http://rsense-dev.cs.uml.edu/api/v1/',

  getProject: function(projectId, recur) {
    recur = setRecur(recur);
    var url = iSense.URL + 'projects/' + projectId + '?recur=' + recur;
    var response = $.ajax({
      type: 'GET',
      url: url,
      async: false,
      dataType: 'JSON'
    }).responseText;
    return JSON.parse(response);
  },

  createProject: function(email, password, name) {
    var url = iSense.URL + 'projects';
    response = $.ajax({
      async: false,
      data: {
        email: email,
        password: password,
        project_name: name
      },
      dataType: 'JSON',
      type: 'POST',
      url: url
    }).responseText;
    return JSON.parse(response);
  },

  createContributorKey: function(email, password, projectId, name, key) {
    var url = iSense.URL + 'projects/' + projectId + '/add_key';
    response = $.ajax({
      async: false,
      data: {
        email: email,
        password: password,
        contrib_key: {
          name: name,
          key: key
        }
      },
      dataType: 'JSON',
      type: 'POST',
      url: url
    }).responseText;
    return JSON.parse(response);
  },

  getField: function(fieldId) {
    var url = iSense.URL + 'fields/' + fieldId;
    var response = $.ajax({
      type: 'GET',
      url: url,
      async: false,
      dataType: 'JSON'
    }).responseText;
    return JSON.parse(response);
  },

  createField: function(email, password, projectId, name, type, other) {
    var url = iSense.URL + 'fields';
    var data = {
      email: email,
      password: password,
      field: {
        project_id: projectId,
        field_type: type,
        name: name
      }
    };

    if (typeof other !== 'undefined') {
      if (type == iSense.TEXT) {
        data['field']['restrictions'] = other;
      } else if (type == iSense.NUMBER) {
        data['field']['unit'] = other;
      }
    }

    response = $.ajax({
      async: false,
      data: data,
      dataType: 'JSON',
      type: 'POST',
      url: url
    }).responseText;
    return JSON.parse(response);
  },

  getDataSet: function(dataSetId) {
    var url = iSense.URL + 'data_sets/' + dataSetId;
    var response = $.ajax({
      type: 'GET',
      url: url,
      async: false,
      dataType: 'JSON'
    }).responseText;
    return JSON.parse(response);
  },

  uploadDataSet: function(email, password, projectId, title, data) {
    var url = iSense.URL + 'projects/' + projectId + '/jsonDataUpload';
    response = $.ajax({
      async: false,
      data: {
        email: email,
        password: password,
        title: title,
        data: data
      },
      dataType: 'JSON',
      type: 'POST',
      url: url
    }).responseText;
    return JSON.parse(response);
  },

  uploadDataSetWithKey: function(contribKey, contribName, projectId, title, data) {
    var url = iSense.URL + 'projects/' + projectId + '/jsonDataUpload';
    response = $.ajax({
      async: false,
      data: {
        contribution_key: contribKey,
        contributor_name: contribName,
        title: title,
        data: data
      },
      dataType: 'JSON',
      type: 'POST',
      url: url
    }).responseText;
    return JSON.parse(response);
  },

  editDataSet: function(email, password, dataSetId, data) {
    var url = iSense.URL + 'data_sets/' + dataSetId + '/edit';
    response = $.ajax({
      async: false,
      data: {
        email: email,
        password: password,
        data: data
      },
      dataType: 'JSON',
      type: 'GET',
      url: url
    }).responseText;
    return JSON.parse(response);
  },

  editDataSetWithKey: function(contribKey, dataSetId, data) {
    var url = iSense.URL + 'data_sets/' + dataSetId + '/edit';
    response = $.ajax({
      async: false,
      data: {
        contribution_key: contribKey,
        data: data
      },
      dataType: 'JSON',
      type: 'GET',
      url: url
    }).responseText;
    return JSON.parse(response);
  },

  appendDataSet: function(email, password, dataSetId, data) {
    var url = iSense.URL + 'data_sets/append';
    response = $.ajax({
      async: false,
      data: {
        email: email,
        password: password,
        id: dataSetId,
        data: data
      },
      dataType: 'JSON',
      type: 'POST',
      url: url
    }).responseText;
    return JSON.parse(response);
  },

  appendDataSetWithKey: function(contribKey, dataSetId, data) {
    var url = iSense.URL + 'data_sets/append';
    response = $.ajax({
      async: false,
      data: {
        contribution_key: contribKey,
        id: dataSetId,
        data: data
      },
      dataType: 'JSON',
      type: 'POST',
      url: url
    }).responseText;
    return JSON.parse(response);
  }
};

if (typeof module !== 'undefined') {
  module.exports = iSense;
}
