package api_test;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JFrame;

import java.util.ArrayList;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.SwingUtilities;
import javax.swing.SwingWorker;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import edu.uml.cs.isense.api.API;
import edu.uml.cs.isense.api.UploadInfo;
import edu.uml.cs.isense.objects.RPerson;
import edu.uml.cs.isense.objects.RProject;
import edu.uml.cs.isense.objects.RProjectField;

public class ApiTest {
	private JFrame frame;
	
	int FILEPICK = 0;
	int MEDIAPROJPICK = 1;
	int MEDIADATASET = 2;
	
	private JButton dev;
	private JButton live;
	
	API api;
	
	int projectId;
	JPanel buttons;
	JPanel results;


	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					ApiTest window = new ApiTest();
					window.frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the application.
	 */
	public ApiTest() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		//initialize api object
		api = API.getInstance();

		//text for status bar
		frame = new JFrame("API Tests");
		
		//get window size
		Toolkit tk = Toolkit.getDefaultToolkit();  
		int xSize = ((int) tk.getScreenSize().getWidth());  
		int ySize = ((int) tk.getScreenSize().getHeight());
		frame.setSize(xSize, ySize); 
		
		//what happens when user closes window
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		//Set up window layout as grid of 2 rows and 1 column
		BoxLayout myLayout = new BoxLayout(frame.getContentPane(), BoxLayout.Y_AXIS);
		frame.setLayout(myLayout);
		
		//buttons
		dev = new JButton("Test Dev");
		live = new JButton("Test Production");
		
		//Size of buttons
		dev.setSize(100,50);
		live.setSize(100,50);

		//JLabel status = new JLabel("Results will be displayed here.");

		//adding panels to main window
		buttons = new JPanel();
		buttons.add(dev);
		buttons.add(live);	
		buttons.setMinimumSize(new Dimension(300, 50));
		buttons.setMaximumSize(new Dimension(300, 50));


		//panel to show results of test
		results = new JPanel();
		results.setLayout(new BoxLayout(results, BoxLayout.Y_AXIS));
		
		JScrollPane scrollPane = new JScrollPane(results);
		scrollPane.getVerticalScrollBar().setUnitIncrement(16);

		
		//add button panel and results panel to window
		frame.add(buttons);
		frame.add(scrollPane);
		frame.revalidate();
		
		
		//Called dev button is clicked
				dev.addActionListener(new ActionListener() {
					@Override
						public void actionPerformed(ActionEvent e) {
							api.useDev(true);
							JLabel status = new JLabel();
							status.setText("Starting tests on rsense-dev...");
							status.setAlignmentX(results.CENTER_ALIGNMENT);
							results.add(status, 0);
							
							frame.revalidate();
							new LoginTask().execute();
						}
			        });      
						
				
				//Called when live button is clicked
				live.addActionListener(new ActionListener() {
					@Override
					public void actionPerformed(ActionEvent e) {
						api.useDev(false);
						JLabel status = new JLabel();
						status.setText("Starting tests on isenseproject...");
						status.setAlignmentX(results.CENTER_ALIGNMENT);
						results.add(status, 0);
						
						frame.revalidate();
						//new LoginTask().execute();
					}
		        });
	}
	
	 class LoginTask extends SwingWorker {
	        /**
	         * @throws Exception
	         */
	        protected Object doInBackground() throws Exception {				
	        	RPerson person = api.createSession("mobile.fake@example.com", "mobile");

    	    	boolean success = (person != null);
    	    	System.out.print(success);
    	    	
    	    	//Runs on ui thread
	        	SwingUtilities.invokeLater(new Runnable() {
	        	    public void run() {
    					JLabel status = new JLabel();
    					
	    				if(success) {
	    					status.setText("\nLogin successful.");
							status.setAlignmentX(results.CENTER_ALIGNMENT);
							status.setForeground(Color.green);
							results.add(status, 0);
							frame.revalidate();
	    					new CreateProjectTask().execute();
	    				} else {
	    					status.setText("\nLogin failed.");
							status.setAlignmentX(results.CENTER_ALIGNMENT);
							status.setForeground(Color.red);
							results.add(status, 0);
							frame.revalidate();
	    				}
	    			
	        	    }
	        	  });
				return success;
	    }
	 }
	        
        class LogoutTask extends SwingWorker {
	        /**
	         * @throws Exception
	         */
	        protected Object doInBackground() throws Exception {
				api.deleteSession();
				return null;
		    }
        }
        
        class ProjectsTask extends SwingWorker {
	        /**
	         * @throws Exception
	         */
	        protected Object doInBackground() throws Exception {
	    		ArrayList<ArrayList<RProjectField>> rpfs = new ArrayList<ArrayList<RProjectField>>();
	        	ArrayList<RProject> rps = api.getProjects(1, 2, true, API.CREATED_AT, "");
	        	
				for(RProject rp : rps) {
					rpfs.add(api.getProjectFields(rp.project_id));
				}	
				
				for(RProject p : rps) {
					JLabel status = new JLabel();
					status.setText(status.getText() + "\n" + p.name + "\n");
					
					if(rpfs.size() > 0) {
						for(RProjectField rp : rpfs.remove(0)) {
							status.setText(status.getText() + " - "+rp.name+"\n");
						}
					}
				}
				
				JLabel status = new JLabel();
				if(rps.size() <= 2) {
					status.setText("Get (at most) 2 Projects failed");
					status.setAlignmentX(results.CENTER_ALIGNMENT);
					status.setForeground(Color.red);

					results.add(status, 0);
					frame.revalidate();
				} else {
					status.setText("Get (at most) 2 Projects successful");
					status.setAlignmentX(results.CENTER_ALIGNMENT);
					status.setForeground(Color.green);

					results.add(status, 0);
					frame.revalidate();
				}
				return null;
	        }
        }
       
	 
	 class AppendTask extends SwingWorker {
	        /**
	         * @throws Exception
	         */
	        protected Object doInBackground() throws Exception {
	        	JSONObject toAppend = new JSONObject();
	        	
	        	//TODO 0 1 and 2 are not correct field id's
				try {
					toAppend.put("0", new JSONArray().put("2013/08/05 10:50:20"));
					toAppend.put("1", new JSONArray().put("119"));
					toAppend.put("2", new JSONArray().put("120"));
				} catch (JSONException e) {
					e.printStackTrace();
				}
				api.appendDataSetData(20, toAppend);
				
			//	new DeleteProjectTask().execute(); TODO
		        return null;
		    }
     }
	 
	 
	    
     class UploadTask extends SwingWorker {
	        /**
	         * @throws Exception
	         */
	        protected Object doInBackground() throws Exception {
	        	JSONObject j = new JSONObject();
	        	//TODO not 1 and 0
				try {
					j.put("1", new JSONArray().put("45"));
					j.put("0", new JSONArray().put("2013/08/02 09:50:01"));
				} catch (JSONException e) {
					e.printStackTrace();
					UploadInfo info = new UploadInfo();
					return info;
				}
				
				UploadInfo info = api.uploadDataSet(projectId, j, "mobile upload test");
				
				JLabel status = new JLabel();
				if(info.dataSetId == -1) {
					status.setText("Upload data set fail.");
					status.setAlignmentX(results.CENTER_ALIGNMENT);
					status.setForeground(Color.red);

					results.add(status, 0);
					frame.revalidate();
				} else {
					status.setText("Upload data set success.");
					status.setAlignmentX(results.CENTER_ALIGNMENT);
					status.setForeground(Color.green);

					results.add(status, 0);
					frame.revalidate();
				}

				//TODO
//				new DeleteProjectTask().execute(); 
	        	
	        	return null;
		    }
     }
     
     class ProjMediaTask extends SwingWorker {
	        /**
	         * @throws Exception
	         */
	        protected Object doInBackground() throws Exception {
				return null;
	        	//TODO fix
//				api.uploadMedia(7, new File(params[0]), API.TargetType.PROJECT);
				//new AppendTask().execute(); TODO

		    }
     }
		
     class DSMediaTask extends SwingWorker {
	        /**
	         * @throws Exception
	         */
	        protected Object doInBackground() throws Exception {
				return null;
	        	//TODO api upload media
//				api.uploadMedia(42, new File(params[0]), API.TargetType.DATA_SET);

		    }
     }
		
     
     class CreateProjectTask extends SwingWorker {
	        /**
	         * @throws Exception
	         */
	        protected Object doInBackground() throws Exception {
				ArrayList<RProjectField> fields = new ArrayList<RProjectField>();
				
				RProjectField time = new RProjectField();
				time.type = RProjectField.TYPE_TIMESTAMP;
				time.name = "Time";
				fields.add(time);
				
				
				RProjectField amount = new RProjectField();
				amount.type = RProjectField.TYPE_NUMBER;
				amount.name = "Amount";
				amount.unit = "units";
				fields.add(amount);
				
				projectId = api.createProject("Test Project", fields);
				JLabel status = new JLabel();

				if(projectId == -1) {
					status.setText("Create project fail.");
					status.setAlignmentX(results.CENTER_ALIGNMENT);
					status.setForeground(Color.red);

					results.add(status, 0);
					frame.revalidate();
				} else {
					status.setText("Create project Success.");
					status.setAlignmentX(results.CENTER_ALIGNMENT);
					status.setForeground(Color.green);

					results.add(status, 0);
					frame.revalidate();
					new UploadTask().execute();

				}
				
				return null;

	        }
  }
		//TODO delete project api call
//		class DeleteProjectTask extends SwingWorker {
//	        /**
//	         * @throws Exception
//	         */
//	        protected Object doInBackground() throws Exception {
//				
//				int proj = api.deleteProject(projectId);
//				
//				if(proj == -1) {
//					status.setText(status.getText() + "Delete project failed.");
//				} else {
//					status.setText(status.getText() + "Delete project successful.");
//				}
//				
//				return null;
//		    }
//		}
//		
}

	


