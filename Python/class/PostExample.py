import API




x = API.Isense('106','key','TYLER')

#print x.getDatasetFieldData('MLB Team Statistics 2013','Runs')

#x.postDataset('Runs','MLB Team Statistics 2013',[5])

#x.postDatasetHorizontal(['Runs','Hits'],'TESTETSTES',[[5],[2]])

x.appendToDataset('MLB Team Statistics 2013',['Runs','Hits'],[[5],[2]])