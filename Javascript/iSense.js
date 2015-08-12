(function() {
  var get = function(url) {
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open("GET", url, false);
    xmlHttp.send(null);

    var responseObject;
    if (xmlHttp.responseType === "json") {
      responseObject = JSON.parse(responseObject);
    } else {
      responseObject = null;
    }

    return {
      status: xmlHttp.status,
      response: responseObject
    };
  };

  var post = function(url, obj) {

  };

  window.ISenseAPI = {
    TIMESTAMP: 1,
    NUMBER: 2,
    TEXT: 3,
    LATITUDE: 4,
    LONGITUDE: 5,

    //URL: 'http://isenseproject.org/api/v1/',
    URL: 'http://rsense-dev.cs.uml.edu/api/v1/',

    getProject: function(projectId, recur) {
      if (typeof recur === 'undefined') {
        recur = false;
      }
      var url = ISenseAPI.URL + 'projects/' + projectId + '?recur=' + recur;
      var response = get(url);
      // var response = $.ajax({
      //   type: 'GET',
      //   url: url,
      //   async: false,
      //   dataType: 'JSON'
      // }).responseText;
      // return JSON.parse(response);
      return response;
    },

    createProject: function(email, password, name) {
      var url = ISenseAPI.URL + 'projects';
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
      var url = ISenseAPI.URL + 'projects/' + projectId + '/add_key';
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
      var url = ISenseAPI.URL + 'fields/' + fieldId;
      var response = $.ajax({
        type: 'GET',
        url: url,
        async: false,
        dataType: 'JSON'
      }).responseText;
      return JSON.parse(response);
    },

    createField: function(email, password, projectId, name, type, other) {
      var url = ISenseAPI.URL + 'fields';
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
        if (type == ISense.TEXT) {
          data['field']['restrictions'] = other;
        } else if (type == ISense.NUMBER) {
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
      var url = ISenseAPI.URL + 'data_sets/' + dataSetId;
      var response = $.ajax({
        type: 'GET',
        url: url,
        async: false,
        dataType: 'JSON'
      }).responseText;
      return JSON.parse(response);
    },

    uploadDataSet: function(email, password, projectId, title, data) {
      var url = ISenseAPI.URL + 'projects/' + projectId + '/jsonDataUpload';
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
      var url = ISenseAPI.URL + 'projects/' + projectId + '/jsonDataUpload';
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
      var url = ISenseAPI.URL + 'data_sets/' + dataSetId + '/edit';
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
      var url = ISenseAPI.URL + 'data_sets/' + dataSetId + '/edit';
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
      var url = ISenseAPI.URL + 'data_sets/append';
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
      var url = ISenseAPI.URL + 'data_sets/append';
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

  window.ISense = {
    TIMESTAMP: ISenseAPI.TIMESTAMP,
    NUMBER: ISenseAPI.NUMBER,
    TEXT: ISenseAPI.TEXT,
    LATITUDE: ISenseAPI.LATITUDE,
    LONGITUDE: ISenseAPI.LONGITUDE,

    Errors: {
      AuthUser: 'Must use username and password to add fields to a project',
      AuthBad: 'Provided credentials are invalid',
      DSetType: 'Can\'t add non-data set type to a project\'s data sets',
      FieldType: 'Can\'t add non-field type to a project\'s fields',
      NoID: 'This method requires the object to have a valid ID',
      NeedsCredentials: 'Credentials object must be credentials object'
    },

    Exception: function(msg) {
      if (this instanceof ISense.Exception) {
        this.msg = msg;
      } else {
        return new ISense.Exception(msg);
      }
    },

    Project: function(project_id) {
      if (this instanceof ISense.Project) {
        var r = ISenseAPI.getProject(project_id, true);
        this._id = r.id;
        this._name = r.name;
        this._url = r.url;
        this._path = r.path;
        this._hidden = r.hidden;
        this._featured = r.featured;
        this._likeCount = r.likeCount;
        this._content = r.content;
        this._timeAgo = r.timeAgoInWords;
        this._created = r.createdAt;
        this._ownerName = r.ownerName;
        this._ownerUrl = r.ownerUrl;
        this._dataCount = r.dataSetCount;
        this._dataSetIDs = r.dataSetIDs;
        this._fieldCount = r.fieldCount;

        this._dataSets = r.dataSets.map(function(dataSet) {
          return ISense.DataSet(dataSet);
        });

        this._fields = r.fields.map(function(field) {
          return ISense.Field(field);
        });
      } else {
        return new ISense.Project(project_id);
      }
    },

    DataSet: function(opts) {
      if (this instanceof ISense.DataSet) {
        this._id = opts.id;
        this._name = opts.name;
        this._url = opts.url;
        this._path = opts.path;
        this._created = opts.createdAt;
        this._fieldCount = opts.fieldCount;
        this._rowCount = opts.datapointCount;
        this._displayURL = opts.displayURL;
        this._data = opts.data;
      } else {
        return new ISense.DataSet(opts);
      }
    },

    Field: function(opts) {
      if (this instanceof ISense.Field) {
        this._id = opts.id;
        this._name = opts.name;
        this._type = opts.type;
        this._restrictions = opts.restrictions;
        this._units = opts.units;
      } else {
        return new ISense.Field(opts);
      }
    },

    Credentials: function(opts) {
      if (this instanceof ISense.Credentials) {
        if ((opts.contribName !== undefined && opts.contribName !== null) &&
            (opts.contribKey !== undefined && opts.contribKey !== null)) {
          this._contribName = opts.contribName;
          this._contribKey = opts.contribKey;
        } else if ((opts.username !== undefined && opts.username !== null) &&
                   (opts.password !== undefined && opts.password !== null)) {
          this._username = opts.username;
          this._password = opts.password;
        } else {
          throw ISense.Exception(ISense.Errors.AuthBad);
        }
      } else {
        return new ISense.Credentials(opts);
      }
    },

    createProject: function(name, creds) {
      if (!(creds instanceof ISense.Credentials)) {
        throw ISense.Exception(ISense.Errors.NeedsCredentials);
      } else if (creds.isUser()) {
        var r = ISenseAPI.createProject(creds._username, creds._password, name);
        return ISense.Project(r.id);
      } else if (creds.isKey()) {
        throw ISense.Exception(ISense.Errors.AuthUser);
      } else {
        throw ISense.Exception(ISense.Errors.AuthBad);
      }
    }
  }

  ISense.Project.prototype.refresh = function() {
    ISense.Project.call(this, this._id);
    return this;
  }

  ISense.Project.prototype.addField = function(field, creds) {
    if (!(field instanceof ISense.Field)) {
      throw ISense.Exception(ISense.Errors.FieldType);
    } else if (!(creds instanceof ISense.Credentials)) {
      throw ISense.Exception(ISense.Errors.NeedsCredentials);
    } else if (!creds.isUser()) {
      throw ISense.Exception(ISense.Errors.AuthUser);
    } else {
      var r = ISenseAPI.createField(
        creds._username,
        creds._password,
        self._id,
        field._name,
        field._type,
        field._other);
      field._id = r.id;
      this.refresh();
      field.refresh();
      return field;
    }
  }

  ISense.Project.prototype.addDataSet = function(title, data, creds) {
    var r;

    if (!(creds instanceof ISense.Credentials)) {
      throw ISense.Exception(ISense.Errors.NeedsCredentials);
    } else if (creds.isUser()) {
      r = ISenseAPI.uploadDataSet(
        creds._username,
        creds._password,
        this._id,
        title,
        data);
    } else if (creds.isKey()) {
      r = ISenseAPI.uploadDataSetWithKey(
        creds._contribKey,
        creds._contribName,
        this._id,
        title,
        data);
    } else {
      throw ISense.Exception(ISense.Errors.AuthBad);
    }

    this.refresh();

    var newDataSet = ISense.DataSet({id: r.id});
    return newDataSet;
  }

  ISense.DataSet.prototype.refresh = function() {
    var r = ISenseAPI.getDataSet(this._id);
    ISense.DataSet.call(this, r);
  }

  ISense.DataSet.prototype.appendData = function(data, creds) {
    var r;
    if (!(creds instanceof ISense.Credentials)) {
      throw ISense.Exception(ISense.Errors.NeedsCredentials);
    } else if (creds.isKey()) {
      r = ISenseAPI.appendDataSetWithKey(
        creds._contribKey,
        this._id,
        data);
    } else if (creds.isUser()) {
      r = ISenseAPI.appendDataSet(
        creds._username,
        creds._password,
        this._id,
        data);
    } else {
      throw ISense.Exception(ISense.Errors.AuthBad);
    }

    this.refresh();
    return this;
  }

  ISense.DataSet.prototype.editData = function(data, creds) {
    var r;
    if (!(creds instanceof ISense.Credentials)) {
      throw ISense.Exception(ISense.Errors.NeedsCredentials);
    } else if (creds.isKey()) {
      r = ISenseAPI.editDataSetWithKey(
        creds._contribKey,
        this._id,
        data);
    } else if (creds.isUser()) {
      r = ISenseAPI.editDataSet(
        creds._username,
        creds._password,
        this._id,
        data);
    } else {
      throw ISense.Exception(ISense.Errors.AuthBad);
    }

    this.refresh();
    return this;
  }

  ISense.Credentials.prototype.isUser = function() {
    return (this._username !== undefined && this._username !== null) &&
           (this._password !== undefined && this._password !== null);
  }

  ISense.Credentials.prototype.isKey = function() {
    return (this._contribName !== undefined && this._contribName !== null) &&
           (this._contribKey !== undefined && this._contribKey !== null);
  }
})();
