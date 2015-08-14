(function() {
  var get = function(url, data) {
    var xhr;
    if (typeof data === 'object') {
      xhr = $.ajax({
        async: false,
        data: data,
        dataType: 'JSON',
        type: 'GET',
        url: url
      });
    } else {
      xhr = $.ajax({
        async: false,
        dataType: 'JSON',
        type: 'GET',
        url: url
      });
    }

    var parsedResponse;
    try {
      parsedResponse = JSON.parse(xhr.responseText);
    } catch (e) {
      parsedResponse = {};
    }

    return {
      response: parsedResponse,
      status: xhr.status
    };
  }

  var post = function(url, data) {
    var xhr;
    if (typeof data === 'object') {
      xhr = $.ajax({
        async: false,
        data: data,
        dataType: 'JSON',
        type: 'POST',
        url: url
      });
    } else {
      xhr = $.ajax({
        async: false,
        dataType: 'JSON',
        type: 'POST',
        url: url
      });
    }

    var parsedResponse;
    try {
      parsedResponse = JSON.parse(xhr.responseText);
    } catch (e) {
      parsedResponse = {};
    }

    return {
      response: parsedResponse,
      status: xhr.status
    };
  }

  var padInteger = function(num, size) {
    var numStr = num.toString();
    var zeros = size - numStr.length + 1;
    var padding = new Array(Math.max(0, zeros)).join('0');
    return padding + numStr;
  }

  var handleStatus = function(status, accept, reject) {
    if (accept.indexOf(status) > -1) {
      return;
    } else if (reject.indexOf(status) > -1) {
      switch (status) {
      case 401:
        throw ISenseException(ISenseErrors.Unauthorized);
      case 404:
        throw ISenseException(ISenseErrors.NotFound);
      case 409:
        throw ISenseException(ISenseErrors.Conflicts);
      case 422:
        throw ISenseException(ISenseErrors.BadInput);
      default:
        throw ISenseException(ISenseErrors.OtherError + String(status));
      }
    } else {
      throw ISenseException(ISenseErrors.OtherError + String(status));
    }
  }

  window.ISenseException = function(msg) {
    if (this instanceof ISenseException) {
      this.msg = msg;
    } else {
      return new ISenseException(msg);
    }
  };

  window.ISenseErrors = {
    AuthBad: 'Provided credentials are invalid',
    AuthUser: 'Must use username and password to add fields to a project',
    BadInput: 'Server couldn\'t parse request',
    Conflicts: 'Attempted to create an object that already exists',
    DSetType: 'Can\'t add non-data set type to a project\'s data sets',
    FieldType: 'Can\'t add non-field type to a project\'s fields',
    NeedsCredentials: 'Credentials object must be credentials object',
    NoID: 'This method requires the object to have a valid ID',
    NotFound: 'Couldn\'t find object',
    OtherError: 'Server returned status ',
    Unauthorized: 'Unauthorized to do this action with provided credentials'
  };

  window.ISenseAPI = {
    TIMESTAMP: 1,
    NUMBER: 2,
    TEXT: 3,
    LATITUDE: 4,
    LONGITUDE: 5,

    //URL: 'http://isenseproject.org/api/v1/',
    URL: 'http://rsense-dev.cs.uml.edu/api/v1/',
    //URL: 'http://localhost:3000/api/v1/',

    getProject: function(projectId, recur) {
      if (typeof recur === 'undefined') {
        recur = false;
      }
      var url = ISenseAPI.URL + 'projects/' + projectId + '?recur=' + recur;
      var res = get(url);

      handleStatus(res.status, [200], [404]);

      return res.response;
    },

    createProject: function(email, password, name) {
      var url = ISenseAPI.URL + 'projects';
      var res = post(url, {
        email: email,
        password: password,
        project_name: name
      });

      handleStatus(res.status, [201], [401, 422]);

      return res.response;
    },

    createContributorKey: function(email, password, projectId, name, key) {
      var url = ISenseAPI.URL + 'projects/' + projectId + '/add_key';
      var res = post(url, {
        email: email,
        password: password,
        contrib_key: {
          name: name,
          key: key
        }
      });

      handleStatus(res.status, [201], [401, 409, 422]);

      return res.response;
    },

    getField: function(fieldId) {
      var url = ISenseAPI.URL + 'fields/' + fieldId;
      var res = get(url);

      handleStatus(res.status, [200], [404]);

      return res.response;
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

      if (typeof other !== 'undefined' && other !== null) {
        if (type == ISense.TEXT) {
          data['field']['restrictions'] = other;
        } else if (type == ISense.NUMBER) {
          data['field']['unit'] = other;
        }
      }

      var res = post(url, data);

      handleStatus(res.status, [201], [401, 422]);

      return res.response;
    },

    getDataSet: function(dataSetId) {
      var url = ISenseAPI.URL + 'data_sets/' + dataSetId;
      var res = get(url);

      handleStatus(res.status, [200], [404]);

      return res.response;
    },

    uploadDataSet: function(email, password, projectId, title, data) {
      var url = ISenseAPI.URL + 'projects/' + projectId + '/jsonDataUpload';
      var res = post(url, {
        email: email,
        password: password,
        title: title,
        data: data
      });

      handleStatus(res.status, [200], [401, 422]);

      return res.response;
    },

    uploadDataSetWithKey: function(contribKey, contribName, projectId, title, data) {
      var url = ISenseAPI.URL + 'projects/' + projectId + '/jsonDataUpload';
      var res = post(url, {
        contribution_key: contribKey,
        contributor_name: contribName,
        title: title,
        data: data
      });

      handleStatus(res.status, [200], [401, 422]);

      return res.response;
    },

    editDataSet: function(email, password, dataSetId, data) {
      var url = ISenseAPI.URL + 'data_sets/' + dataSetId + '/edit';
      var data = {
        email: email,
        password: password,
        data: data
      };
      var res = get(url, data);

      handleStatus(res.status, [200], [401, 404, 422]);

      return res.response;
    },

    editDataSetWithKey: function(contribKey, dataSetId, data) {
      var url = ISenseAPI.URL + 'data_sets/' + dataSetId + '/edit';
      var data = {
        contribution_key: contribKey,
        data: data
      };
      var res = get(url, data);

      handleStatus(res.status, [200], [401, 404, 422]);

      return res.response;
    },

    appendDataSet: function(email, password, dataSetId, data) {
      var url = ISenseAPI.URL + 'data_sets/append';
      var res = post(url, {
        email: email,
        password: password,
        id: dataSetId,
        data: data
      });
      
      handleStatus(res.status, [200], [401, 404, 422]);

      return res.response;
    },

    appendDataSetWithKey: function(contribKey, dataSetId, data) {
      var url = ISenseAPI.URL + 'data_sets/append';
      var res = post(url, {
        contribution_key: contribKey,
        id: dataSetId,
        data: data
      });

      handleStatus(res.status, [200], [401, 404, 422]);

      return res.response;
    }
  };

  window.ISense = {
    TIMESTAMP: ISenseAPI.TIMESTAMP,
    NUMBER: ISenseAPI.NUMBER,
    TEXT: ISenseAPI.TEXT,
    LATITUDE: ISenseAPI.LATITUDE,
    LONGITUDE: ISenseAPI.LONGITUDE,

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
          throw ISenseException(ISenseErrors.AuthBad);
        }
      } else {
        return new ISense.Credentials(opts);
      }
    },

    createProject: function(name, creds) {
      if (!(creds instanceof ISense.Credentials)) {
        throw ISenseException(ISenseErrors.NeedsCredentials);
      } else if (creds.isUser()) {
        var r = ISenseAPI.createProject(creds._username, creds._password, name);
        return ISense.Project(r.id);
      } else if (creds.isKey()) {
        throw ISenseException(ISenseErrors.AuthUser);
      } else {
        throw ISenseException(ISenseErrors.AuthBad);
      }
    }
  }

  ISenseException.prototype.toString = function() {
    return 'iSense Error: ' + this.msg;
  }

  ISense.Project.prototype.refresh = function() {
    ISense.Project.call(this, this._id);
    return this;
  }

  ISense.Project.prototype.addField = function(name, type, other, creds) {
    if (!(creds instanceof ISense.Credentials)) {
      throw ISenseException(ISenseErrors.NeedsCredentials);
    } else if (!creds.isUser()) {
      throw ISenseException(ISenseErrors.AuthUser);
    } else {
      var r = ISenseAPI.createField(
        creds._username,
        creds._password,
        this._id,
        name,
        type,
        other);
      var field = ISense.Field(r);
      this.refresh();
      return field;
    }
  }

  ISense.Project.prototype.addDataSet = function(title, data, creds) {
    var r;
    var dateTime = new Date;
    var timeTitle = title + ' '
      + dateTime.getFullYear().toString() + '/'
      + padInteger(dateTime.getMonth() + 1, 2) + '/'
      + padInteger(dateTime.getDate(), 2) + ' '
      + padInteger(dateTime.getHours(), 2) + ':'
      + padInteger(dateTime.getMinutes(), 2) + ':'
      + padInteger(dateTime.getSeconds(), 2) + '.'
      + padInteger(dateTime.getMilliseconds(), 3);

    if (!(creds instanceof ISense.Credentials)) {
      throw ISenseException(ISenseErrors.NeedsCredentials);
    } else if (creds.isUser()) {
      r = ISenseAPI.uploadDataSet(
        creds._username,
        creds._password,
        this._id,
        timeTitle,
        data);
    } else if (creds.isKey()) {
      r = ISenseAPI.uploadDataSetWithKey(
        creds._contribKey,
        creds._contribName,
        this._id,
        timeTitle,
        data);
    } else {
      throw ISenseException(ISenseErrors.AuthBad);
    }

    this.refresh();

    var newDataSet = ISense.DataSet({id: r.id});
    return newDataSet;
  }

  ISense.Project.prototype.addContribKey = function(label, key, creds) {
    // email password projid name key
    var r;
    if (!(creds instanceof ISense.Credentials)) {
      throw ISenseException(ISenseErrors.NeedsCredentials);
    } else if (creds.isUser()) {
      r = ISenseAPI.createContributorKey(
        creds._username,
        creds._password,
        this._id,
        label,
        key);
    } else if (creds.isKey()) {
      throw ISenseException(ISenseErrors.AuthUser);
    } else {
      throw ISenseException(ISenseErrors.AuthBad);
    }

    return this;
  }

  ISense.Project.prototype.getID = function() {
    return this._id;
  }

  ISense.Project.prototype.getName = function() {
    return this._name;
  }

  ISense.Project.prototype.getURL = function() {
    return this._url;
  }

  ISense.Project.prototype.getPath = function() {
    return this._path;
  }

  ISense.Project.prototype.isHidden = function() {
    return this._hidden;
  }

  ISense.Project.prototype.isFeatured = function() {
    return this._featured;
  }

  ISense.Project.prototype.getLikeCount = function() {
    return this._likeCount;
  }

  ISense.Project.prototype.getContent = function() {
    return this._content;
  }

  ISense.Project.prototype.getTimeSinceCreation = function() {
    return this._timeAgo;
  }

  ISense.Project.prototype.getTimeOfCreation = function() {
    return this._created
  }

  ISense.Project.prototype.getOwnerName = function() {
    return this._ownerName;
  }

  ISense.Project.prototype.getOwnerURL = function() {
    return this._ownerUrl;
  }

  ISense.Project.prototype.getDataSetCount = function() {
    return this._dataCount;
  }

  ISense.Project.prototype.getDataSetIDs = function() {
    return this._dataSetIDs;
  }

  ISense.Project.prototype.getDataSetNames = function() {
    return this._dataSets.map(function(x) {
      return x._name;
    });
  }

  ISense.Project.prototype.getDataSets = function() {
    return this._dataSets;
  }

  ISense.Project.prototype.getDataSetByID = function(id) {
    return this._dataSets.filter(function(x) {
      return id === x._id;
    });
  }

  ISense.Project.prototype.getDataSetByName = function(name) {
    var nameLower = name.toLowerCase();
    return this._dataSets.filter(function(x) {
      return nameLower === x._name.toLowerCase();
    });
  }

  ISense.Project.prototype.getFieldCount = function() {
    return this._fieldCount;
  }

  ISense.Project.prototype.getFieldIDs = function() {
    return this._fields.map(function(x) {
      return x._id;
    });
  }

  ISense.Project.prototype.getFieldNames = function() {
    return this._fields.map(function(x) {
      return x._name;
    })
  }

  ISense.Project.prototype.getFields = function() {
    return this._fields;
  }

  ISense.Project.prototype.getFieldByID = function(name) {
    return this._fields.filter(function(x) {
      return id === x._id;
    });
  }

  ISense.Project.prototype.getFieldByName = function(name) {
    var nameLower = name.toLowerCase();
    return this._fields.filter(function(x) {
      return nameLower === x._name.toLowerCase();
    });
  }

  ISense.Project.prototype.getDataByField = function(field) {
    var dataObject = {};
    this._dataSets.map(function(x) {
      dataObject[x._id] = x.getDataByField(field);
    });
    return dataObject;
  }

  ISense.Project.prototype.getDataByFields = function(fieldList) {
    var dataObject = {};
    this._dataSets.map(function(x) {
      dataObject[x._id] = x.getDataByFields(fieldList);
    });
    return dataObject;
  }

  ISense.DataSet.prototype.refresh = function() {
    var r = ISenseAPI.getDataSet(this._id);
    ISense.DataSet.call(this, r);
  }

  ISense.DataSet.prototype.appendData = function(data, creds) {
    var r;
    if (!(creds instanceof ISense.Credentials)) {
      throw ISenseException(ISenseErrors.NeedsCredentials);
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
      throw ISenseException(ISenseErrors.AuthBad);
    }

    this.refresh();
    return this;
  }

  ISense.DataSet.prototype.editData = function(data, creds) {
    var r;
    if (!(creds instanceof ISense.Credentials)) {
      throw ISenseException(ISenseErrors.NeedsCredentials);
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
      throw ISenseException(ISenseErrors.AuthBad);
    }

    this.refresh();
    return this;
  }

  ISense.DataSet.prototype.getID = function() {
    return this._id;
  }

  ISense.DataSet.prototype.getName = function() {
    return this._name;
  }

  ISense.DataSet.prototype.getURL = function() {
    return this._url;
  }

  ISense.DataSet.prototype.getPath = function() {
    return this._path;
  }

  ISense.DataSet.prototype.getTimeOfCreation = function() {
    return this._created;
  }

  ISense.DataSet.prototype.getFieldCount = function() {
    return this._fieldCount;
  }

  ISense.DataSet.prototype.getDataPointCounts = function() {
    return this._rowCount;
  }

  ISense.DataSet.prototype.getDisplayURL = function() {
    return this._displayURL;
  }

  ISense.DataSet.prototype.getData = function() {
    return this._data;
  }

  ISense.DataSet.prototype.getDataByField = function(field) {
    return this._data.map(function(x) {
      return x[field._id.toString()];
    });
  }

  ISense.DataSet.prototype.getDataByFields = function(fieldList) {
    var fieldIDs = fieldList.map(function(x) {
      return x._id;
    });
    return this._data.map(function(x) {
      var dataObject = {};
      Object.keys(x).map(function(y) {
        if (fieldIDs.indexOf(Number(y)) > -1) {
          dataObject[y] = x[y];
        }
      });
      return dataObject;
    });
  }

  ISense.Field.prototype.refresh = function() {
    var r = ISenseAPI.getField(this._id);
    ISense.Field.call(this, r);
  }

  ISense.Field.prototype.getID = function() {
    return this._id;
  }

  ISense.Field.prototype.getName = function() {
    return this._name;
  }

  ISense.Field.prototype.getType = function() {
    return this._type;
  }

  ISense.Field.prototype.getRestrictions = function() {
    return this._restrictions;
  }

  ISense.Field.prototype.getUnits = function() {
    return this._units;
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
