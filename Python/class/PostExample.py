import API

x = API.Isense('106','key','TYLER')

print x.getDatasetFieldData('MLB Team Statistics 2013','Runs')

x.postDataset('Runs','MLB Team Statistics 2013',[5])

x.postDatasetHorizontal(['Runs','Hits'],'Test',[[5],[2]])

x.appendToDataset('TESTETSTES 2015-03-18 17:26:40',['Runs','Hits'],[[5],[2]])