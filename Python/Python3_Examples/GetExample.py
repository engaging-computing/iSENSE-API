import iSENSE


# Gets Project 100
print("Get Project 100")
response = iSENSE.getProject(100)
print(response.text)

print("Get Project 100 with recur=True")
# Same get call but with recur=True
response = iSENSE.getProject(100, recur=True)
print(response.text)

print "Get Data Set 500"
# Get Data Set 500
response = iSENSE.getDateSet(500)
print(response.text)

