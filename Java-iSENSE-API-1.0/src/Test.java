import static org.junit.Assert.*;

import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Date;

import org.json.JSONArray;
import org.json.JSONObject;
import org.junit.BeforeClass;

import edu.uml.cs.isense.api.API;
import edu.uml.cs.isense.api.UploadInfo;
import edu.uml.cs.isense.objects.RDataSet;
import edu.uml.cs.isense.objects.RPerson;
import edu.uml.cs.isense.objects.RProject;
import edu.uml.cs.isense.objects.RProjectField;


public class Test {
	static API api;

	@BeforeClass
	public static void OneTimeSetup() {
		api = API.getInstance();
		try	{
			String ip = InetAddress.getLocalHost().getHostAddress();
			if(ip.equals("129.63.16.128"))
				api.useDev(false);
			else
				api.setBaseUrl("http://"+ip+":3000");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	@org.junit.Test
	public void getProjectsTest() {
		ArrayList<RProject> rps = api.getProjects(1, 10, true, API.CREATED_AT, "");
		RProject rp = api.getProject(rps.get(0).project_id);
		assertNotNull(rp.name);
	}

	@org.junit.Test
	public void loginTest() {
		RPerson rp = api.createSession("mobile.fake@example.com", "mobile");
		assertNotNull(rp);
	}

	@org.junit.Test
	public void createProjectWithDataTest() {
		//tests that modify the state of the database should not run on live iSENSE
		if(!api.isLive()) {
			api.createSession("mobile.fake@example.com", "mobile");
			RProjectField rpf = new RProjectField();
			rpf.name = "field";
			rpf.type = RProjectField.TYPE_NUMBER;
			ArrayList<RProjectField> rpfs = new ArrayList<RProjectField>();
			rpfs.add(rpf);
			int pid = api.createProject("Automated Test " + new Date(), rpfs);
			assertNotNull(api.getProject(pid));
			
			JSONObject data = new JSONObject();
			long fieldId = api.getProjectFields(pid).get(0).field_id;
			data.put(""+fieldId, new JSONArray().put(5).put(6).put(7));
			UploadInfo info = api.uploadDataSet(pid, data, "Test Dataset");
			//assertNotEquals(dsid, -1);
			assertNotNull(api.getDataSet(info.dataSetId));
		}
	}
	
	@org.junit.Test
	public void getDatasetTest() {
		ArrayList<RProject> rps = api.getProjects(1, 10, true, API.CREATED_AT, "");
		ArrayList<RDataSet> rdss = api.getDataSets(rps.get(0).project_id);
		if(rdss.size() != 0) {
			assertNotNull(rdss.get(0));
		}
	}

}
