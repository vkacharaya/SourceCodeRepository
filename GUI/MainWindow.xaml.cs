///////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - GUI for Project3HelpWPF                      //
// ver 1.0                                                           //
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018 //
// Author: Vishnu Karthik Ravindran, CSE 687 - Object Oriented Design//
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package provides a WPF-based GUI for RepositoryWithWPF demo.  It's 
 * responsibilities are to:
 * - Provide a display of directory contents of a remote ServerPrototype.
 * - It provides a subdirectory list and a filelist for the selected directory.
 * - You can navigate into subdirectories through the treeview similar to the
 * - windows view.
 *   
 * Required Files:
 * ---------------
 * Mainwindow.xaml, MainWindow.xaml.cs
 * Translater.dll
 * 
 * Maintenance History:
 * --------------------
 * ver 1.0 : 30 Mar 2018
 * - first release
 * - Several early prototypes were discussed in class. Those are all superceded
 *   by this package.
 * ver 2.0 : 04 April 2018
 * - Added functions for checkin file transfer, checkout file transfer, browsing,
 * - view file metadata, view file full text
 */

// Translater has to be statically linked with CommLibWrapper
// - loader can't find Translater.dll dependent CommLibWrapper.dll

#region namespace 

using MsgPassingCommunication;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using FormsNS = System.Windows.Forms;
using System.Windows.Media.Imaging;

#endregion namespace

/// <summary>
/// Package for creating classes needed for the Repository Client
/// </summary>
namespace RepositoryWPF
{
    /// <summary>
    /// Window class containing functions doing operations on
    /// Check-In file transfer, check-out file transfer, browsing.
    /// This class mainly holds the event handlers for all the above operations.
    /// It also contains separate thread for invoking functions using the command 
    /// received from the server.
    /// </summary>
    public partial class MainWindow : Window
    {
        /// <summary>
        /// Constructor for the MainWindow class.
        /// This binds the xaml file to the application.
        /// </summary>
        public MainWindow(string clientPath)
        {
            InitializeComponent();
            ClientSourcePath.Text = clientPath;
            NamespaceTxt.Text = Path.GetFileName(clientPath);
            localStorage = (Path.DirectorySeparatorChar + Path.GetFileName(Path.GetDirectoryName(clientPath)));
            dependentsWindow = new Window1(this);
        }

        #region private variables

        /// <summary>
        /// This dictionary is used to store the translater instances whenever
        /// a new listener request comes. Translater object in this dictionary 
        /// is used to interact the comm channel.
        /// </summary>
        private Dictionary<string, Translater> translater
            = new Dictionary<string, Translater>();

        /// <summary>
        /// Index of the translater instance for the specific endpoints
        /// </summary>
        private int translaterRefID;

        /// <summary>
        /// Local path for the project which is used for generating the path of file
        /// </summary>
        private string localStorage;

        /// <summary>
        /// Instance of the browser window containing view file text 
        /// and view file meta data
        /// </summary>
        private BrowseWindow br;

        /// <summary>
        /// This object contains the client hostname and port number. It
        /// is intialized on windows load and the values are fetched from the 
        /// Application Configuration.
        /// </summary>
        private CsEndPoint endPoint_;

        /// <summary>
        /// This thread collection is used for invoking the client functions 
        /// by server using the command sent by them.
        /// </summary>
        private List<Thread> rcvThrd = new List<Thread>();

        /// <summary>
        /// This object contains the server hostname and port number.It
        /// is intialized on windows load and the values are fetched from the 
        /// Application Configuration.
        /// </summary>
        private CsEndPoint serverEndPoint = new CsEndPoint();

        /// <summary>
        /// Header content of the check-in tab
        /// </summary>
        private const string checkInTab = "Check-In";

        /// <summary>
        /// Header content of the check out tab
        /// </summary>
        private const string checkOutTab = "Check-Out";

        /// <summary>
        /// Header content of the browse tab
        /// </summary>
        private const string browseTab = "Browse";

        /// <summary>
        /// This object contains the server hostname and port number.It
        /// is intialized on windows load and the values are fetched from the 
        /// Application Configuration.
        /// </summary>
        private string localPath;

        /// <summary>
        /// Dictionary containing the delegates for the command.
        /// Used by the thread running in the background away from the UI thread.
        /// </summary>
        private Dictionary<string, Action<CsMessage>> dispatcher_
            = new Dictionary<string, Action<CsMessage>>();

        /// <summary>
        /// Dictionary containing the category values for the file
        /// selected in the check-in list.
        /// </summary>
        private Dictionary<string, List<string>> categoryValues
            = new Dictionary<string, List<string>>();

        /// <summary>
        /// Dictionary containing the dependents value for the
        /// file selected in the check-in list
        /// </summary>
        private Dictionary<string, List<string>> dependentValues
            = new Dictionary<string, List<string>>();

        /// <summary>
        /// This is the file path of the save file folder 
        /// used in the communication channel by the client. This folder is used
        /// by both client and server in this project
        /// </summary>
        private string saveFilePath = ConfigurationManager.AppSettings["projectRoot"]
            + ConfigurationManager.AppSettings["saveFiles"];

        /// <summary>
        /// This is the file path of the Send File folder
        /// used in the communication channel by the client. 
        /// This folder is used by both client and server in this project
        /// </summary>
        private string sendFilePath = ConfigurationManager.AppSettings["projectRoot"]
            + ConfigurationManager.AppSettings["sendFiles"];

        /// <summary>
        /// Key Value of "To" attribute in the client message 
        /// Created this variable to avoid hardcoding.
        /// </summary>
        private const string toM = "to";

        /// <summary>
        /// Key Value of "from" attribute in the client message 
        /// Created this variable to avoid hardcoding.
        /// </summary>
        private const string fromM = "from";

        /// <summary>
        /// Key Value of "command" attribute in the client message 
        /// Created this variable to avoid hardcoding.
        /// </summary>
        private const string commandM = "command";

        /// <summary>
        /// Key Value of "file" attribute in the client message 
        /// Created this variable to avoid hardcoding.
        /// </summary>
        private const string fileM = "sendingFile";

        /// <summary>
        /// Key Value of "filter" attribute in the client message 
        /// Created this variable to avoid hardcoding.
        /// </summary>
        private const string filterM = "filter";

        /// <summary>
        /// Key Value of "path" attribute in the client message 
        /// Created this variable to avoid hardcoding.
        /// </summary>
        private const string pathM = "path";

        /// <summary>
        /// Key Value of "categ" attribute in the client message 
        /// Created this variable to avoid hardcoding.
        /// </summary>
        private const string categM = "categ";

        /// <summary>
        /// Key Value of "dependents" attribute in the client message 
        /// Created this variable to avoid hardcoding.
        /// </summary>
        private const string dependentsM = "dependents";

        /// <summary>
        /// Key Value of "description" attribute in the client message 
        /// Created this variable to avoid hardcoding.
        /// </summary>
        private const string descriptionM = "description";

        /// <summary>
        /// Key Value of "dir" attribute in the client message 
        /// Created this variable to avoid hardcoding.
        /// </summary>
        private const string dir = "dir";

        /// <summary>
        /// Key Value of "testMsg" attribute in the client message 
        /// Created this variable to avoid hardcoding.
        /// </summary>
        private const string testMsg = "testMsg";

        /// <summary>
        /// Key Value of "message" attribute in the client message 
        /// Created this variable to avoid hardcoding.
        /// </summary>
        private const string msgAttrib = "message";

        /// <summary>
        /// Flag indicating whether checkOut listbox has triggered the event 
        /// in the check-out tab.
        /// </summary>
        private bool isCheckOutSecondList = false;

        /// <summary>
        /// Temporaray variable for storing the list 
        /// selected in various tabs
        /// </summary>
        private string listName;

        /// <summary>
        /// Username of the logged in user
        /// </summary>
        private string userName = "User1";

        /// <summary>
        /// Key Value of "user" attribute in the client message 
        /// Created this variable to avoid hardcoding.
        /// </summary>
        private string userM = "user";

        /// <summary>
        /// Namespace of the project. Later generated using file path.
        /// </summary>
        private string nameSpace = "NoSqlDb";

        /// <summary>
        /// This window is the dependnents window used 
        /// for showing the dependents.
        /// </summary>
        Window1 dependentsWindow;

        #endregion private variables

        #region private functions

        /// <summary>
        /// This thread is responsible for running the delegates added 
        /// in the dispatcher through the reply received from the server.
        /// </summary>
        private void ProcessMessages()
        {
            translaterRefID = GetTranslaterRefID();
            ThreadStart thrdProc = () =>
            {
                while (true)
                {
                    try
                    {
                        CsMessage msg = translater.ElementAt(translaterRefID).Value.getMessage();
                        if (msg.attributes.Count == 0)
                            continue;
                        DisplayCases(msg.value(commandM));
                        msg.show("");
                        string msgId = msg.value(commandM);
                        if (dispatcher_.ContainsKey(msgId) && msg.value(testMsg) != "1")
                            dispatcher_[msgId].Invoke(msg);
                        isCheckOutSecondList = false;
                    }
                    catch (Exception)
                    {
                        continue;
                    }
                }
            };
            //Initialize thread and runs the thread in background
            rcvThrd.Add(new Thread(thrdProc)
            {
                IsBackground = true
            });
            //Starts the thread
            rcvThrd[translaterRefID].Start();
        }

        /// <summary>
        /// This method is used for showing the display text for
        /// various commands
        /// </summary>
        /// <param name="command">Operation to be carried on server</param>
        private void DisplayCases(string command)
        {
            switch (command)
            {
                case "checkIn":
                    DisplayMessage("Requirement 2b# Checked-in files/Versioning and creating new versions stored in ServerStorage/Query folder");
                    break;
                case "checkOut":
                    DisplayMessage("Requirement 2c# Checked-out/Versioning files and stored in LocalStorage/Query folder with dependents");
                    break;
                case "connect":
                    DisplayMessage("Requirement 2a# Connect to the server");
                    break;
                case "closeCheckIn":
                    DisplayMessage("Requirement 2d# Closing the file/dependency close/Closed Pending case/User Mismatch");
                    break;
                case "editMetaData":
                    DisplayMessage("Requirement 2e# Update Metadata");
                    break;
                case "GetMetaData":
                    DisplayMessage("Requirement 2f# Retrieve Metadata");
                    break;
                case "queryFiles":
                    DisplayMessage("Requirement 2g# Query Files - Without Parents filter and Other filters");
                    break;
                case "oneway":
                    DisplayMessage("Requirement 4# One way - Only request sent to the server");
                    break;
                default:
                    break;
            }
        }

        /// <summary>
        /// This function is used for updating the dependents from List into the
        /// temporary dependents variable. 
        /// </summary>
        /// <param name="key"></param>
        /// <param name="dependents"></param>
        public void UpdateDependents(string key, ItemCollection dependents)
        {
            if (key != null)
            {
                if (dependentValues.ContainsKey(key))
                {
                    dependentValues[key].Clear();

                }
                else
                {
                    dependentValues.Add(key, new List<string>());
                }
                foreach (string dep1 in dependents)
                {
                    dependentValues[key].Add(dep1);
                }
            }

        }

        /// <summary>
        /// This functions is used to remove the dependents value
        /// when the user removes it in dependents window.
        /// </summary>
        /// <param name="key">Selected file name</param>
        public void RemoveDependents(string key)
        {
            if (dependentValues.ContainsKey(key))
            {
                dependentValues.Remove(key);
            }
        }

        /// <summary>
        /// This method gets the reference ID needed for accesssing 
        /// threads running when multiple listening endpoints are
        /// available
        /// </summary>
        /// <returns>Key Value of the threadID</returns>
        private int GetTranslaterRefID()
        {
            int count = 0;
            foreach (var transObj in translater)
                if (!(transObj.Key == CsEndPoint.toString(endPoint_)))
                    count++;
                else
                    break;
            return count;
        }

        /// <summary>
        /// Clears the directory list. 
        /// It is used in many places through out the application.
        /// </summary>
        private void ClearDirs()
        {
            DirList.Items.Clear();
        }

        /// <summary>
        /// This method contains the lambda expression for checkin
        /// and also adds the function reference into the dispatcher
        /// </summary>
        private void DispatcherCheckIn()
        {
            Action<CsMessage> checkIn = (CsMessage msg) =>
            {
                var enumer = msg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    UpdateMataDataInCheckIn(enumer.Current.Key, enumer.Current.Value);
                }
                Action checkInStatus = () =>
                {
                    CheckInFiles.Foreground = Brushes.DarkMagenta;
                    StatusMessage.Content = msg.attributes[msgAttrib];
                    StatusMessage.Foreground = Brushes.DarkOliveGreen;

                };
                Dispatcher.Invoke(checkInStatus);
            };
            AddClientProc("checkIn", checkIn);
        }

        /// <summary>
        /// This method contains the lambda expression for checkin
        /// and also adds the function reference into the dispatcher
        /// </summary>
        private void DispatcherCloseCheckIn()
        {
            Action<CsMessage> closeCheckInDisp = (CsMessage msg) =>
            {

                var enumer = msg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    UpdateMataDataInCheckIn(enumer.Current.Key, enumer.Current.Value);
                    Action closecheckInStatus = () =>
                    {
                        CheckInFiles.Foreground = Brushes.Black;
                        StatusMessage.Content = msg.attributes[msgAttrib];
                        StatusMessage.Foreground = Brushes.DarkOliveGreen;
                    };
                    Dispatcher.Invoke(closecheckInStatus);
                }
            };
            AddClientProc("closeCheckIn", closeCheckInDisp);
        }


        /// <summary>
        /// This method contains the lambda expression for checkin
        /// and also adds the function reference into the dispatcher
        /// </summary>
        private void DispatcherEditMetaData()
        {
            Action<CsMessage> editMetaData = (CsMessage msg) =>
            {
                Action edit = () =>
                {
                    var enumer = msg.attributes.GetEnumerator();
                    while (enumer.MoveNext())
                    {
                        UpdateMataDataInCheckIn(enumer.Current.Key, enumer.Current.Value);
                    }
                    CheckInFiles.Foreground = Brushes.DarkMagenta;
                    StatusMessage.Content = msg.attributes[msgAttrib];
                    StatusMessage.Foreground = Brushes.DarkOliveGreen;
                };
                Dispatcher.Invoke(edit);
            };
            AddClientProc("editMetaData", editMetaData);
        }

        /// <summary>
        /// This method contains the lambda expression for filtering query operation
        /// and also adds the function reference into the dispatcher
        /// </summary>
        private void DispatcherQUery()
        {
            Action<CsMessage> editMetaData = (CsMessage msg) =>
            {
                Action edit = () =>
                {
                    BrowseFileList1.Items.Clear();
                    var enumer = msg.attributes.GetEnumerator();
                    while (enumer.MoveNext())
                    {
                        if (enumer.Current.Key.Contains("file"))
                        {
                            Action<string> doFileQuery = (string file) =>
                            {
                                if (file.Contains("DbCore"))
                                {
                                    ListBoxItem item = new ListBoxItem();
                                    item.Tag = ConfigurationManager.AppSettings["projectRoot"] + "DbCore/" + file;
                                    item.Content = file;
                                    BrowseFileList1.Items.Add(item);
                                }
                                else
                                {
                                    ListBoxItem item = new ListBoxItem();
                                    item.Tag = ConfigurationManager.AppSettings["projectRoot"] + "Query/" + file;
                                    item.Content = file;
                                    BrowseFileList1.Items.Add(item);
                                }
                            };
                            Dispatcher.Invoke(doFileQuery, new Object[] { enumer.Current.Value });
                        }
                    }
                    SortDescriptor();
                };
                Dispatcher.Invoke(edit);
            };
            AddClientProc("queryFiles", editMetaData);
        }

        /// <summary>
        /// This function sorts the various lists present in various tabs
        /// </summary>
        private void SortDescriptor()
        {
            BrowseFileList1.Items.SortDescriptions.Add(new System.ComponentModel.SortDescription("Content", System.ComponentModel.ListSortDirection.Ascending));
            CheckOutFileList.Items.SortDescriptions.Add(new System.ComponentModel.SortDescription("Content", System.ComponentModel.ListSortDirection.Ascending));
            dependentsWindow.RemDirDepList.Items.SortDescriptions.Add(new System.ComponentModel.SortDescription("Content", System.ComponentModel.ListSortDirection.Ascending));

        }

        /// <summary>
        /// This method contains the lambda expression for browse
        /// and also adds the function reference into the dispatcher
        /// </summary>
        private void DispatcherBrowse()
        {
            Action<CsMessage> browse = (CsMessage message) =>
            {
                Action browseWindowUpdate = () =>
                {
                    try
                    {
                        string fileContent = File.ReadAllText(saveFilePath + "\\" + message.value(fileM));
                        br.BrowseWindowPara.Inlines.Clear();
                        br.BrowseWindowPara.Inlines.Add(new Run(fileContent));
                        br.Topmost = true;
                        br.CategoriesVw.Items.Clear();
                        br.DependentsVw.Items.Clear();
                        br.Title = br.Title + "-" + message.value(fileM);
                        var enumer = message.attributes.GetEnumerator();
                        while (enumer.MoveNext())
                        {
                            string key = enumer.Current.Key;
                            if (key.Contains(categM))
                                br.CategoriesVw.Items.Add(enumer.Current.Value);
                            if (key.Contains(dependentsM))
                                br.DependentsVw.Items.Add(enumer.Current.Value);
                            if (key.Contains(descriptionM))
                                br.DescriptionVw.Text = enumer.Current.Value;
                            if (key.Contains("status"))
                                br.FileStatus.Content = enumer.Current.Value;
                        }
                    }
                    catch
                    {
                        StatusMessage.Content = "Error!!!! Please check the message sent to the server";
                        StatusMessage.Foreground = Brushes.Firebrick;
                    }
                };
                Dispatcher.Invoke(browseWindowUpdate);
            };
            AddClientProc("Browse & viewMetaData", browse);
        }

        /// <summary>
        /// Adds the directory into the treeview structure
        /// based on the reference of the source directory list 
        /// </summary>
        private void AddDir(string dir, TreeViewItem src)
        {
            TreeViewItem item = new TreeViewItem
            {
                Header = dir
            };
            item.Tag = src.Tag + @"\" + dir;
            item.Items.Add(string.Empty);
            src.Items.Add(item);
        }

        /// <summary>
        /// Clears the file items in the list based on the reference
        /// retrieved from the selected tab 
        /// </summary>
        private void ClearFiles()
        {
            ListBox list = GetListReference();
            if (list != null)
            {
                list.Items.Clear();
            }
        }

        /// <summary>
        /// This function adds the file to the FileList Item collection. 
        /// ListBoxItem containing Content and Tag is inserted into the FileList.
        /// </summary>
        /// <param name="file">File Name which was retrieved from the directory selected in the UI</param>
        /// <param name="path">path of the directory selected in the UI</param>
        private void AddFile(string file, string path, ListBox list)
        {
            ListBoxItem item = new ListBoxItem()
            {
                Content = file,
                Tag = path + "/" + file
            };
            list.Items.Add(item);
        }

        /// <summary>
        /// This method adds the delegate into the dispatcher which is invoked 
        /// by the commands from the Server
        /// </summary>
        /// <param name="key">unique name for the delegated functions</param>
        /// <param name="clientProc">function reference to be executed on server request</param>
        private void AddClientProc(string key, Action<CsMessage> clientProc)
        {
            dispatcher_[key] = clientProc;
        }

        /// <summary>
        /// This method contains the lambda expression for browsing metadata
        /// and also adds the function reference into the dispatcher
        /// </summary>
        private void DispatcherLoadGetFiles()
        {
            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                Action clrFiles = () =>
                {
                    ClearFiles();
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("file"))
                    {
                        Action<string> doFileLoad = (string file) =>
                        {
                            ListBox list = GetListReference();
                            AddFile(file, rcvMsg.getPath(), list);
                            SortDescriptor();
                        };
                        Dispatcher.Invoke(doFileLoad, new Object[] { enumer.Current.Value });
                    }
                }

                listName = null;
            };
            AddClientProc("Browsing Pkg: getFiles", getFiles);
        }


        /// <summary>
        /// This method contains the lambda expression for browsing metadata
        /// and also adds the function reference into the dispatcher
        /// </summary>
        private void DispatcherGetStatus()
        {
            Action<CsMessage> getStatus = (CsMessage rcvMsg) =>
            {
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("open"))
                    {
                        Action<string> doFileGet = (string file) =>
                        {
                            foreach (ListBoxItem item in CheckInFiles.Items)
                            {
                                if ((string)item.Content == file)
                                {
                                    item.Foreground = Brushes.DarkMagenta;
                                }
                            }
                        };
                        Dispatcher.Invoke(doFileGet, new Object[] { enumer.Current.Value });
                    }
                    NewStatusDispatcher(key, enumer.Current.Value);
                }
            };
            AddClientProc("getStatus", getStatus);
        }

        /// <summary>
        /// This method is an exetension to the getStatus method.
        /// Updates the status in the meta data section
        /// </summary>
        /// <param name="key">File Name</param>
        /// <param name="value">Status of the file</param>
        private void NewStatusDispatcher(string key, string value)
        {
            if (key.Contains("new"))
            {
                Action<string> doFileNS = (string file) =>
                {
                    foreach (ListBoxItem item in CheckInFiles.Items)
                    {
                        if ((string)item.Content == file)
                        {
                            item.Foreground = Brushes.Brown;
                            item.ToolTip = "New File";
                        }
                    }
                };
                Dispatcher.Invoke(doFileNS, new Object[] { value });
            }

        }

        /// <summary>
        /// Function used to load all the lambda created for each command
        /// into the dispatcher which will be invoked later in the thread
        /// </summary>
        private void LoadDispatcher()
        {
            DispatcherLoadGetFiles();
            DispatcherQUery();
            DispatcherCheckIn();
            DispatcherCloseCheckIn();
            DispatcherCheckOut();
            DispatcherConnect();
            DispatcherBrowse();
            DispatcherGetMetaData();
            DispatcherEditMetaData();
            DispatcherGetStatus();
        }

        /// <summary>
        /// Event Handler for check in button click. It constructs check-in
        /// message and passes to the server for execution.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void CheckIn_Click(object sender, RoutedEventArgs e)
        {
            if (DescriptionTxt.Text == string.Empty)
            {
                StatusMessage.Content = "Please enter Description";
                StatusMessage.Foreground = Brushes.Firebrick;
                return;
            }
            if (CheckInFiles.Items.Count != 0)
            {
                foreach (ListBoxItem file in CheckInFiles.Items)
                {
                    CheckInFile(file, "checkIn");
                }
            }
            else
            {
                TabLoader();
            }
            categoryValues.Clear();
        }

        /// <summary>
        /// Event Handler for check out button click. It constructs check-out
        /// message and passes to the server for execution.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void CheckOut_click(object sender, RoutedEventArgs e)
        {
            ListBoxItem checkOutItem = (ListBoxItem)CheckOutFileList.SelectedItem;
            if (checkOutItem != null)
            {
                CheckOutFileM(checkOutItem);
            }
            else
            {
                StatusMessage.Content = "Select an Item in the CheckOut List";
                StatusMessage.Foreground = Brushes.Firebrick;
            }
        }

        /// <summary>
        /// Method for adding the checkout lambda function into the dispatcher.
        /// Contains events related to updation of UI controls
        /// </summary>
        private void DispatcherCheckOut()
        {
            Action<CsMessage> checkOut = (CsMessage msg) =>
            {
                string parentDir = Directory.GetParent(Directory.GetCurrentDirectory()).Parent.Parent.Parent.FullName;
                foreach (var attrib in msg.attributes.Where(c => c.Key.Contains("path")))
                {
                    string filepath = msg.value(attrib.Key).Replace(".." + ConfigurationManager.AppSettings[7],
                        localStorage);
                    int value;
                    string fileValue;
                    Int32.TryParse(System.Text.RegularExpressions.Regex.Match(attrib.Key, @"\d+").Value, out value);
                    fileValue = "sendingFile";
                    if (value != 0)
                    {
                        fileValue = fileValue + value.ToString();
                    }
                    //Used for copying from client savefilepath to client local directory
                    if (msg.attributes["recieveFile"] == msg.attributes[fileValue])
                    {
                        File.Copy(saveFilePath + "/" + msg.value(fileValue), parentDir + filepath, true);
                    }
                }
                Action status = () =>
                {
                    StatusMessage.Content = msg.attributes[msgAttrib];//+ ". Available in " + filepath;
                    StatusMessage.Foreground = Brushes.DarkOliveGreen;
                };
                Dispatcher.Invoke(status);
            };
            AddClientProc("checkOut", checkOut);
        }

        /// <summary>
        /// This method contains the lambda expression for getMetaData
        /// and also adds the function reference into the dispatcher
        /// </summary>
        private void DispatcherGetMetaData()
        {
            Action<CsMessage> getMetaData = (CsMessage rcvMsg) =>
            {
                Action clearFiles = () =>
                {
                    Dependents.Items.Clear();
                };
                Dispatcher.Invoke(clearFiles);
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    UpdateControls(key, enumer.Current.Value, rcvMsg.attributes[filterM]);
                };
                Action depFIles = () =>
                {
                    if (dependentValues.ContainsKey(rcvMsg.attributes[filterM]))
                        foreach (string fileN in dependentValues[rcvMsg.attributes[filterM]])
                            Dependents.Items.Add(fileN);
                    UpdateDependents(rcvMsg.attributes[filterM], Dependents.Items);
                };
                Dispatcher.Invoke(depFIles);
            };
            AddClientProc("GetMetaData", getMetaData);
        }

        /// <summary>
        /// This method contains the lambda expression for getMetaData in CheckOut tab.
        /// and also adds the function reference into the dispatcher
        /// </summary>
        private void DispatcherGetMetaDataForCheckOut()
        {
            Action<CsMessage> getMetaData = (CsMessage rcvMsg) =>
            {
                Action clearFiles = () =>
                {
                    DependentsCheckout.Items.Clear();
                };
                Dispatcher.Invoke(clearFiles);
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("dependents"))
                    {
                        Action<string> doFileDp = (string file) =>
                        {
                            DependentsCheckout.Items.Add(file);
                        };
                        Dispatcher.Invoke(doFileDp, new Object[] { enumer.Current.Value });
                    }
                };
            };
            AddClientProc("GetMetaData", getMetaData);
        }

        /// <summary>
        /// Updates the Metadata controls in the CheckIn tab for the value passed from
        /// enumerator.
        /// </summary>
        /// <param name="key">Message Attribute Name</param>
        /// <param name="value">Value of the attribute</param>
        /// <param name="fileName">File Name Selected in CheckIn</param>
        private void UpdateControls(string key, string value, string fileName)
        {
            if (key.Contains("dependents"))
            {
                Action<string> doFileDep = (string file) =>
                {
                    if (!dependentValues.ContainsKey(fileName))
                        Dependents.Items.Add(file);
                };
                Dispatcher.Invoke(doFileDep, new Object[] { value });
            }
            UpdateCategories(key, value);
            UpdateMataDataInCheckIn(key, value);
        }

        /// <summary>
        /// Updates the Category controls in the CheckIn tab for the value passed from
        /// enumerator.
        /// </summary>
        /// <param name="key">Message Attribute Name</param>
        /// <param name="value">Value of the attribute</param>
        private void UpdateCategories(string key, string value)
        {
            if (key.Contains("categories"))
            {
                Action<string> doCateg = (string file) =>
                {
                    if (!categoryValues.ContainsKey((string)((ListBoxItem)CheckInFiles.SelectedItem).Tag))
                    {
                        GetStackPanelList(file);
                    }
                    else
                    {
                        foreach (string item in categoryValues[(string)((ListBoxItem)CheckInFiles.SelectedItem).Tag])
                        {
                            if (item != file)
                            {
                                GetStackPanelList(file);
                            }
                        }
                    }
                    List<string> categoryValuesLst = new List<string>();
                    foreach (StackPanel item in CategoryList.Items)
                    {
                        TextBlock categItems = (TextBlock)item.Children[0];
                        categoryValuesLst.Add(categItems.Text);
                    }
                };
                Dispatcher.Invoke(doCateg, new Object[] { value });
            }
        }

        /// <summary>
        /// Updates the MetaData controls in the CheckIn tab for the value passed from
        /// enumerator.
        /// </summary>
        /// <param name="key">Message Attribute Name</param>
        /// <param name="value">Value of the attribute</param>
        private void UpdateMataDataInCheckIn(string key, string value)
        {
            if (key.Contains("description"))
            {
                Action<string> doFileDesc = (string file) => { DescriptionTxt.Text = file; };
                Dispatcher.Invoke(doFileDesc, new Object[] { value });
            }
            if (key.Contains("user"))
            {
                Action<string> doFileUser = (string file) => { UpdatedBy.Content = file; };
                Dispatcher.Invoke(doFileUser, new Object[] { value });
            }
            UpdateMataDataInCheckInDT(key, value);
        }


        /// <summary>
        /// Method for adding the checkout lambda function into the dispatcher
        /// Contains events related to updation of UI controls
        /// </summary>
        private void DispatcherConnect()
        {
            Action<CsMessage> connect = (CsMessage msg) =>
            {
                Action status = () =>
                {
                    StatusMessage.Content = msg.attributes[msgAttrib];
                    StatusMessage.Foreground = Brushes.DarkOliveGreen;
                    ConnectionStatus.Content = "Online";
                    ConnectionStatus.Background = (SolidColorBrush)(new BrushConverter().ConvertFrom("#FFADE80E"));
                    DisConnect.IsEnabled = true;
                    Connect.IsEnabled = false;
                    ConnectionIP.IsEnabled = false;
                    ConnectionPort.IsEnabled = false;
                    UserName.IsEnabled = false;
                };
                Dispatcher.Invoke(status);
            };
            AddClientProc("connect", connect);
        }

        /// <summary>
        /// Event Handler for file list double click. 
        /// It adds item to the check-in list
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void FileList_DblClick(object sender, MouseButtonEventArgs e)
        {
            ListBoxItem selectedItem = (ListBoxItem)FileList.SelectedItem;
            if (selectedItem == null)
                return;
            List<string> checkInList = CheckInFiles.Items.Cast<ListBoxItem>().Select(c => c.Content.ToString()).ToList();
            if (!checkInList.Contains(selectedItem.Content))
            {
                ListBoxItem list = new ListBoxItem
                {
                    Content = selectedItem.Content,
                    Tag = selectedItem.Tag,
                    ToolTip = "Closed/Closed Pending File"
                };
                CheckInFiles.Items.Add(list);
                if (CheckInFiles.SelectedItem == null)
                {
                    CheckInFiles.SelectedIndex = 0;
                }
                GetFileStatus();
            }
        }

        /// <summary>
        /// Updates the MetaData controls in the CheckIn tab for the value passed from
        /// enumerator.
        /// </summary>
        /// <param name="key">Message Attribute Name</param>
        /// <param name="value">Value of the attribute</param>
        private void UpdateMataDataInCheckInDT(string key, string value)
        {
            if (key.Contains("dateTime"))
            {
                Action<string> doFileDT = (string file) => { DateTimeLabel.Content = file; };
                Dispatcher.Invoke(doFileDT, new Object[] { value });
            }
            if (key.Contains("status"))
            {
                Action<string> doFileST = (string file) => { FileStatus.Content = file; };
                Dispatcher.Invoke(doFileST, new Object[] { value });
            }
        }

        /// <summary>
        /// Generates the message needed for the fileStatus command
        /// </summary>
        private void GetFileStatus()
        {
            CsMessage msg = new CsMessage();
            msg.add(toM, CsEndPoint.toString(serverEndPoint));
            msg.add(fromM, CsEndPoint.toString(endPoint_));
            msg.add(userM, UserName.Text ?? "");
            int count = 0;
            foreach (ListBoxItem item in CheckInFiles.Items)
            {
                msg.add("file" + count++, (string)item.Content);
            }
            msg.add(commandM, "getStatus");
            msg.add(testMsg, "0");
            msg.add("nameSP", nameSpace);
            translater.ElementAt(translaterRefID).Value.postMessage(msg);
        }

        /// <summary>
        /// Event Handler for file list single click in the checkout folder.
        /// It adds item to the check-out list
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void FileList_SingleClick(object sender, SelectionChangedEventArgs e)
        {
            ListBoxItem item = (ListBoxItem)LocalFileList_CheckOut.SelectedItem;
            if (item != null)
            {
                CsMessage msg = new CsMessage();
                msg.add(toM, CsEndPoint.toString(serverEndPoint));
                msg.add(fromM, CsEndPoint.toString(endPoint_));
                msg.add(commandM, "Browsing Pkg: getFiles");
                string filepath = Path.GetDirectoryName(item.Tag.ToString()
                    .Split(new string[] { localStorage }, StringSplitOptions.None)[1]);
                msg.add(pathM, filepath);
                msg.add(filterM, (string)item.Content);
                isCheckOutSecondList = true;
                DependentsCheckout.Items.Clear();
                translater.ElementAt(translaterRefID).Value.postMessage(msg);
            }
            else
            {
                CheckOutFileList.Items.Clear();
            }
        }

        /// <summary>
        /// Method called on expnding the tree view in the 
        /// browsing tab. It populates with directory contents.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        public void TreeView_ExpandRemote(object sender, RoutedEventArgs e)
        {
            TreeViewItem item = (TreeViewItem)e.Source;
            item.Items.Clear();
            DirectoryBrowse(item);
        }

        /// <summary>
        /// Method called on check in list double click.
        /// This removes the item from the list.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void CheckInList_DblClick(object sender, MouseButtonEventArgs e)
        {
            ListBoxItem selectedItem = (ListBoxItem)((ListBox)e.Source).SelectedItem;
            if (selectedItem != null)
            {
                this.RemoveDependents((string)selectedItem.Content);
                CheckInFiles.Items.Remove(selectedItem);
            }
        }

        /// <summary>
        /// Invoked on tree view item expansion in the check-in tab.
        /// Enumerates the local file directories for check-in.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void TreeView_Expand(object sender, RoutedEventArgs e)
        {
            TreeViewItem item = (TreeViewItem)e.Source;
            item.Items.Clear();
            var enumer = Directory.EnumerateDirectories((string)item.Tag);
            foreach (var dir in enumer)
            {
                AddDir(System.IO.Path.GetFileName(dir), item);
            }
        }

        /// <summary>
        /// Invoked on listen button click in the setting tab
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void Listen_click(object sender, RoutedEventArgs e)
        {
            endPoint_ = new CsEndPoint
            {
                machineAddress = ConnectionIP1.Text,
                port = Convert.ToInt32(ConnectionPort1.Text)
            };
            if (!translater.ContainsKey(CsEndPoint.toString(endPoint_)))
            {
                translater.Add(CsEndPoint.toString(endPoint_), new Translater());
                translater[CsEndPoint.toString(endPoint_)].listen(endPoint_);
                ProcessMessages();
            }
        }

        /// <summary>
        /// Called on the click of connect in the setting tab.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void Connect_click(object sender, RoutedEventArgs e)
        {
            ConnectServer();
        }

        /// <summary>
        /// Called on click of disconnect button click in the setting tab
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void DisConnect_click(object sender, RoutedEventArgs e)
        {
            translater.ElementAt(translaterRefID).Value.disconnect();
            StatusMessage.Content = "Connection disconnected with Server. Connect again to go Online.";
            StatusMessage.Foreground = Brushes.Firebrick;
            ConnectionStatus.Content = "Offline";
            ConnectionStatus.Background = (SolidColorBrush)(new BrushConverter().ConvertFrom("#FF7A7272"));
            DisConnect.IsEnabled = false;
            Connect.IsEnabled = true;
            ConnectionIP.IsEnabled = true;
            ConnectionPort.IsEnabled = true;
            UserName.IsEnabled = true;
            userName = UserName.Text;
        }

        /// <summary>
        /// Windows load function. This loads all the elements needed for the controls 
        /// on windows load. It also runs the test cases in this function.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            ConnectionIP.Text = ConfigurationManager.AppSettings[2];
            ConnectionPort.Text = ConfigurationManager.AppSettings[3];
            UserName.Text = userName;

            ConnectionIP.IsEnabled = false;
            ConnectionPort.IsEnabled = false;
            Connect.IsEnabled = false;
            UserName.IsEnabled = false;

            endPoint_ = new CsEndPoint
            {
                machineAddress = ConnectionIP1.Text,
                port = Convert.ToInt32(ConnectionPort1.Text)
            };

            translater.Add(CsEndPoint.toString(endPoint_), new Translater());
            //Creates comm channel for the client
            string fullSendPath = Path.GetFullPath(sendFilePath);
            string fullSavePath = Path.GetFullPath(saveFilePath);
            translater[CsEndPoint.toString(endPoint_)].listen(endPoint_);
            translater[CsEndPoint.toString(endPoint_)].setPathContext(fullSendPath, fullSavePath);

            // start processing messages
            ProcessMessages();

            // load dispatcher
            LoadDispatcher();

            serverEndPoint.machineAddress = ConfigurationManager.AppSettings[2];
            serverEndPoint.port = Convert.ToInt32(ConfigurationManager.AppSettings[3]);

            FilterList.Items.Add("Category");
            FilterList.Items.Add("File Names");
            FilterList.Items.Add("Children");
            FilterList.Items.Add("Versions");
            ChkOrphan.IsChecked = true;
            localPath = ClientSourcePath.Text;
            CreateDirectoryinTabs();
            AboutSection();
        }

        /// <summary>
        /// About section loaded from About.txt file
        /// </summary>
        private void AboutSection()
        {
            string fileContent = File.ReadAllText(Path.GetFullPath(ConfigurationManager.AppSettings["projectRoot"] + "About.txt"));
            BrowseWindowPara.Inlines.Clear();
            BrowseWindowPara.Inlines.Add(new Run(fileContent));
        }

        /// <summary>
        /// This method creates items needed for the controls in the 
        /// check-in, check-out and browse on default.
        /// </summary>
        private void CreateDirectoryinTabs()
        {
            TreeViewItem checkInDirItem = new TreeViewItem
            {
                Tag = localPath
            };
            checkInDirItem.Header = System.IO.Path.GetFileName((string)checkInDirItem.Tag);
            checkInDirItem.Items.Add(string.Empty);
            checkInDirItem.IsSelected = true;
            DirList.Items.Add(checkInDirItem);
            TreeViewItem checkOutDirItem = new TreeViewItem
            {
                Tag = localPath
            };
            checkOutDirItem.Header = System.IO.Path.GetFileName((string)checkOutDirItem.Tag);
            checkOutDirItem.Items.Add(string.Empty);
            checkOutDirItem.IsSelected = true;
            DirListCheckOut.Items.Add(checkOutDirItem);
            GetFilesForDir((string)checkInDirItem.Tag);
            Tabs.SelectedIndex = 1;
            GetFilesForDir((string)checkOutDirItem.Tag);
            localPath = ConfigurationManager.AppSettings[7] + "/" + NamespaceTxt.Text;
            TreeViewItem remoteDirItem = new TreeViewItem
            {
                Tag = localPath
            };
            remoteDirItem.Header = System.IO.Path.GetFileName((string)remoteDirItem.Tag);
            remoteDirItem.Items.Add(string.Empty);
            RemoteDirTree.Items.Add(remoteDirItem);
            Tabs.SelectedIndex = 0;
            TestCases();
        }

        /// <summary>
        /// This will update the status message on tab selection change
        /// </summary>
        private void TabLoader()
        {
            switch (Tabs.SelectedIndex)
            {
                case 0:
                    StatusMessage.Content = "Double Click on Files list to add into CheckIn ListBox.";
                    StatusMessage.Foreground = Brushes.Firebrick;
                    break;
                case 1:
                    StatusMessage.Content = "View all versions of a specific file on double click in Local Files List.";
                    StatusMessage.Foreground = Brushes.Firebrick;
                    break;
                case 2:
                    StatusMessage.Content = "";
                    break;
                case 4:
                    StatusMessage.Content = "Explore the tool by reading the About section.";
                    StatusMessage.Foreground = Brushes.Firebrick;
                    break;
                default:
                    StatusMessage.Content = "";
                    break;
            }

        }

        /// <summary>
        /// This adds the files for the selected directory in the appliction.
        /// </summary>
        /// <param name="tagSource">Contains the tag name of the control</param>
        private void GetFilesForDir(string tagSource)
        {
            ListBox list = GetListReference();
            list.Items.Clear();
            var enumer = Directory.EnumerateFiles(tagSource);
            foreach (var file in enumer)
                AddFile(System.IO.Path.GetFileName(file), tagSource, list);
        }

        /// <summary>
        /// Returns the reference of the selected list box in the application
        /// </summary>
        /// <returns>Selected List box reference</returns>
        private ListBox GetListReference()
        {
            TabItem selectedTab = (TabItem)Tabs.SelectedItem;
            ListBox list = null;
            if (listName == "RemDirDep")
            {
                return dependentsWindow.RemDirDepList;
            }
            switch (selectedTab.Header)
            {
                case checkInTab:
                    return FileList;
                case checkOutTab:
                    if (isCheckOutSecondList)
                        return CheckOutFileList;
                    return LocalFileList_CheckOut;
                case browseTab:
                    return BrowseFileList1;
                default:
                    return list;
            };
        }

        /// <summary>
        /// Returns the reference of the selected directory box in the application
        /// </summary>
        /// <returns>Selected directory box reference</returns>
        private TreeView GetDirReference()
        {
            TabItem selectedTab = (TabItem)Tabs.SelectedItem;
            TreeView dirTreeView = null;
            switch (selectedTab.Header)
            {
                case checkInTab:
                    return DirList;
                case checkOutTab:
                    return DirListCheckOut;
                default:
                    return dirTreeView;
            };
        }

        /// <summary>
        /// Triggered on click of directory list control in the selected tabs
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        public void DirList_Click(object sender, RoutedEventArgs e)
        {
            TreeView directoryRef = GetDirReference();
            TreeViewItem selectedDir = (TreeViewItem)directoryRef.SelectedItem;
            GetFilesForDir((string)selectedDir.Tag);
        }

        /// <summary>
        /// Called on tab change and then updates the status message
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void Tab_selectionChanged(object sender, SelectionChangedEventArgs e)
        {
            TabLoader();
        }

        /// <summary>
        /// Event handler for Browse button click. Opens the file content in a new window with metadata
        /// on browse button click
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void Browse_ButtonClick(Object sender, RoutedEventArgs e)
        {
            if (BrowseFileList1.SelectedItem != null)
            {
                ListBoxItem browseItem = (ListBoxItem)BrowseFileList1.SelectedItem;
                BrowseFileGetter(browseItem);
            }
            else
            {
                StatusMessage.Content = "Click on Browse/Double click an item in the Remote file window to browse.";
                StatusMessage.Foreground = Brushes.Firebrick;
            }
        }

        /// <summary>
        /// This function is used to open the file in a new browse 
        /// window along with metadata.
        /// </summary>
        /// <param name="browseItem">Selected Item for browsing in applciation</param>
        /// <param name="testID">Test flag for checking the test case message</param>
        private void BrowseFileGetter(ListBoxItem browseItem, int testID = 0)
        {
            br = new BrowseWindow();
            br.Show();
            CsMessage msg = new CsMessage();
            msg.add(toM, CsEndPoint.toString(serverEndPoint));
            msg.add(fromM, CsEndPoint.toString(endPoint_));
            msg.add("nameSP", nameSpace);
            msg.add(commandM, "Browse & viewMetaData");
            msg.add(testMsg, testID.ToString());
            msg.add(filterM, (string)browseItem.Content);
            msg.add(pathM, (string)browseItem.Tag);
            translater.ElementAt(translaterRefID).Value.postMessage(msg);
        }

        /// <summary>
        /// Event handler for loading the remote file list on click of the directory 
        /// in the remote directory
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        public void DirList_ClickRemote(object sender, RoutedEventArgs e)
        {
            TreeViewItem tvItem = (TreeViewItem)e.Source;
            listName = ((TreeView)sender).Name;
            RemoteFileRetriever(tvItem, e);
        }

        /// <summary>
        /// Loads the remote file list on click of the directory 
        /// in the remote directory
        /// </summary>
        private void RemoteFileRetriever(TreeViewItem tvItem, RoutedEventArgs e)
        {
            if (tvItem != null)
            {
                BrowseFile(tvItem);
            }
            else
            {
                BrowseFileList1.Items.Clear();
            }
        }
        /// <summary>
        /// Constructs the message for browsing to be sent to the
        /// server.
        /// </summary>
        /// <param name="tvItem">Selected directory item</param>
        /// <param name="testID">Test flag for checking the test case message</param>
        private void BrowseFile(TreeViewItem tvItem, int testID = 0)
        {
            CsMessage msg = new CsMessage();
            msg.add(toM, CsEndPoint.toString(serverEndPoint));
            msg.add(fromM, CsEndPoint.toString(endPoint_));
            msg.add(commandM, "Browsing Pkg: getFiles");
            msg.add(testMsg, testID.ToString());
            string filepath = tvItem.Tag.ToString()
                .Split(new string[] { ConfigurationManager.AppSettings[7] }, StringSplitOptions.None)[1];
            filepath = filepath == string.Empty ? "\\" : filepath + "";
            msg.add(pathM, filepath);
            translater.ElementAt(translaterRefID).Value.postMessage(msg);
        }



        /// <summary>
        /// Displays the message in the console for the test cases
        /// </summary>
        /// <param name="displayTxt">Contents to be displayed</param>
        private void DisplayMessage(string displayTxt)
        {
            Console.WriteLine();
            Console.WriteLine("********************************************************************************************");
            Console.WriteLine(displayTxt);
        }

        /// <summary>
        /// Used to browse the directory in the remote directory of browse tab.
        /// </summary>
        /// <param name="item"></param>
        /// <param name="testID">Test flag for checking the test case message</param>
        private void DirectoryBrowse(TreeViewItem item, int testID = 0)
        {
            CsMessage msg = new CsMessage();
            msg.add(toM, CsEndPoint.toString(serverEndPoint));
            msg.add(fromM, CsEndPoint.toString(endPoint_));
            msg.add(commandM, "Browsing Pkg: getDirs");
            msg.add(testMsg, testID.ToString());
            string filepath = item.Tag.ToString()
                .Split(new string[] { ConfigurationManager.AppSettings[7] }, StringSplitOptions.None)[1];
            filepath = filepath == string.Empty ? "\\" : filepath + "";
            msg.add(pathM, filepath);
            if (testID == 0)
            {
                Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
                {
                    var enumer = rcvMsg.attributes.GetEnumerator();
                    while (enumer.MoveNext())
                    {
                        string key = enumer.Current.Key;
                        if (key.Contains(dir))
                        {
                            Action<string> doDir = (string dir) =>
                            {
                                AddDir(dir, item);
                            };
                            Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
                        }
                    }
                };
                AddClientProc("Browsing Pkg: getDirs", getDirs);
            }
            translater.ElementAt(translaterRefID).Value.postMessage(msg);
        }

        /// <summary>
        /// Used to connect the application listening on a port to server
        /// </summary>
        /// <param name="testID">Test flag for checking the test case message</param>
        private void ConnectServer(int testID = 0)
        {
            serverEndPoint.machineAddress = ConnectionIP.Text;
            serverEndPoint.port = Convert.ToInt32(ConnectionPort.Text);
            translater.ElementAt(translaterRefID).Value.connect();
            CsMessage msg = new CsMessage();
            msg.add(toM, CsEndPoint.toString(serverEndPoint));
            msg.add(testMsg, testID.ToString());
            msg.add(fromM, CsEndPoint.toString(endPoint_));
            msg.add(commandM, "connect");
            translater.ElementAt(translaterRefID).Value.postMessage(msg);
        }

        /// <summary>
        /// Used to check in files selected through the application 
        /// from local folder to server folder
        /// </summary>
        /// <param name="file"></param>
        /// <param name="testID">Test flag for checking the test case message</param>
        private void CheckInFile(ListBoxItem file, string command, int testID = 0, int verbose = 0)
        {
            CsMessage msg = new CsMessage();
            msg.add(toM, CsEndPoint.toString(serverEndPoint));
            msg.add(userM, UserName.Text ?? "");
            msg.add(fromM, CsEndPoint.toString(endPoint_));
            msg.add(commandM, command);
            if (verbose == 1)
            {
                msg.add("verboseBytes", "");
            }
            msg.add(testMsg, testID.ToString());
            msg.add(fileM, (string)file.Content);
            msg.add("desc", DescriptionTxt.Text);
            msg.add("nameSP", nameSpace);
            int count = 0;
            if (ApplyForAllChk.IsChecked ?? false)
            {
                foreach (StackPanel item in CategoryList.Items)
                {
                    msg.add("category" + count++, ((TextBlock)item.Children[0]).Text);
                }
            }
            else
            {
                if (categoryValues.ContainsKey((string)file.Tag))
                {
                    foreach (var item in categoryValues[(string)file.Tag])
                    {
                        msg.add("category" + count++, item);
                    }
                }
            }
            count = 0;
            if (dependentValues.ContainsKey((string)file.Content))
            {
                foreach (string item in dependentValues[(string)file.Content])
                {
                    msg.add("dependents" + count++, item);
                }
            }
            string filepaths = file.Tag.ToString()
                .Split(new string[] { localStorage }, StringSplitOptions.None)[1];
            msg.add(pathM, filepaths);
            string filePath = Directory.GetParent(Directory.GetCurrentDirectory()).Parent
                .Parent.Parent.FullName + "/" + ConfigurationManager.AppSettings[5]
                + "/" + System.IO.Path.GetFileName((string)file.Tag);
            File.Copy(Path.GetFullPath((string)file.Tag), filePath, true);
            translater.ElementAt(translaterRefID).Value.postMessage(msg);
        }

        /// <summary>
        /// Used to check out the files passed through the application 
        /// from server folder
        /// </summary>
        /// <param name="file">File item selected in the check out list</param>
        /// <param name="testID">Test flag for checking the test case message</param>
        private void CheckOutFileM(ListBoxItem file, int testID = 0)
        {
            CsMessage msg = new CsMessage();
            msg.add(toM, CsEndPoint.toString(serverEndPoint));
            msg.add(fromM, CsEndPoint.toString(endPoint_));
            msg.add(commandM, "checkOut");
            msg.add(testMsg, testID.ToString());
            if (LatestVersionChk.IsChecked ?? false)
            {
                var selectedFile = (ListBoxItem)LocalFileList_CheckOut.SelectedItem;
                msg.add(filterM, (string)selectedFile.Content);
            }
            else
            {
                msg.add(filterM, (string)file.Content);
            }
            msg.add("nameSP", nameSpace);
            msg.add(pathM, (string)file.Tag);
            msg.add("Dependents", (DependentsCheckOutChk.IsChecked ?? false) ? "1" : "0");
            msg.add("IsLatest", (LatestVersionChk.IsChecked ?? false) ? "1" : "0");
            msg.add(userM, userName);
            translater.ElementAt(translaterRefID).Value.postMessage(msg);

        }

        /// <summary>
        /// This function is written for browse button click in browse tab.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void BrowseButton_Click(object sender, RoutedEventArgs e)
        {
            using (var fbd = new FormsNS.FolderBrowserDialog())
            {
                fbd.ShowNewFolderButton = false;
                fbd.RootFolder = Environment.SpecialFolder.MyComputer;
                FormsNS.DialogResult result = fbd.ShowDialog();
                if (result == FormsNS.DialogResult.OK && !string.IsNullOrWhiteSpace(fbd.SelectedPath))
                {
                    ClientSourcePath.Text = fbd.SelectedPath;
                    NamespaceTxt.Text = Path.GetFileName(ClientSourcePath.Text);
                }
            }
        }

        /// <summary>
        /// This function updates the path in the Settings section.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void UpdatePath_Click(object sender, RoutedEventArgs e)
        {
            userName = UserName.Text;
            nameSpace = NamespaceTxt.Text;
            localPath = ClientSourcePath.Text;
            MainWindow window_8082 = new MainWindow(ClientSourcePath.Text);
            window_8082.ConnectionIP1.Text = this.ConnectionIP1.Text;
            window_8082.ConnectionPort1.Text = this.ConnectionPort1.Text;
            Application.Current.MainWindow = window_8082;
            window_8082.Show();
            this.Close();
        }

        /// <summary>
        /// This function is written for close checkin button click in check-in tab.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void CloseCheckIn_Click(object sender, RoutedEventArgs e)
        {
            if (CheckInFiles.Items.Count != 0)
            {
                foreach (ListBoxItem file in CheckInFiles.Items)
                {
                    CloseCheckIn(file, "0");
                }
            }
            else
            {
                TabLoader();
            }
        }

        /// <summary>
        /// This function generates CsMessage for the closeCheckIn command
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void CloseCheckIn(ListBoxItem file, string testMsgStr)
        {
            CsMessage msg = new CsMessage();
            msg.add(toM, CsEndPoint.toString(serverEndPoint));
            msg.add(fromM, CsEndPoint.toString(endPoint_));
            msg.add(userM, UserName.Text ?? "");
            msg.add(commandM, "closeCheckIn");
            msg.add(testMsg, testMsgStr);
            msg.add(filterM, (string)file.Content);
            msg.add("nameSP", nameSpace);
            string filepath2 = file.Tag.ToString().Split(new string[] { localStorage }, StringSplitOptions.None)[1];
            msg.add(pathM, filepath2);
            translater.ElementAt(translaterRefID).Value.postMessage(msg);
        }

        /// <summary>
        /// This function is written for Add Category button click in browse tab.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void AddCategory_Click(object sender, RoutedEventArgs e)
        {
            if (CategoryTxt.Text.Trim() == "")
            {
                StatusMessage.Content = "Enter Category Value";
                StatusMessage.Foreground = Brushes.Firebrick;
                return;
            }

            ListBoxItem selectedCheckInItem = (ListBoxItem)CheckInFiles.SelectedItem;
            if (selectedCheckInItem != null)
            {
                StatusMessage.Content = "";
                bool flagValue = false;
                if (categoryValues.ContainsKey((string)selectedCheckInItem.Tag))
                {
                    if (categoryValues[(string)selectedCheckInItem.Tag].Count != 0)
                    {
                        flagValue = categoryValues[(string)selectedCheckInItem.Tag].Contains(CategoryTxt.Text);
                    }
                }
                if (CategoryTxt.Text.Trim() != string.Empty && !flagValue)
                {
                    GetStackPanelList(CategoryTxt.Text);
                    List<string> categoryValuesLst = new List<string>();
                    foreach (StackPanel item in CategoryList.Items)
                    {
                        TextBlock categItems = (TextBlock)item.Children[0];
                        categoryValuesLst.Add(categItems.Text);
                    }
                    if (!categoryValues.ContainsKey((string)selectedCheckInItem.Tag))
                        categoryValues.Add((string)selectedCheckInItem.Tag, categoryValuesLst);
                    else
                        categoryValues[(string)selectedCheckInItem.Tag] = categoryValuesLst;
                }
            }
            else
            {
                StatusMessage.Content = "Select an Item in the Check-In List";
                StatusMessage.Foreground = Brushes.Firebrick;
            }
        }

        /// <summary>
        /// This function is written for generating the stack Panel elements for
        /// various controls.
        /// </summary>
        private void GetStackPanelList(string categoryTxt)
        {
            StackPanel sp = new StackPanel();
            BitmapImage bmp = new BitmapImage();
            TextBlock tb = new TextBlock();
            bmp.BeginInit();
            bmp.UriSource = new Uri(Path.GetFullPath("../../../../GUI/images/delete.png"));
            bmp.EndInit();
            tb.Text = categoryTxt;
            tb.Width = 185;
            Image deleteImg = new Image() { Source = bmp, Height = 15.0 };
            deleteImg.MouseDown += new MouseButtonEventHandler(AddDeletimgClicks);
            sp.Orientation = Orientation.Horizontal;
            sp.Children.Add(tb);
            sp.Children.Add(deleteImg);
            CategoryList.Items.Add(sp);
        }

        /// <summary>
        /// This function is written for delete image click in various tab.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void AddDeletimgClicks(object sender, MouseButtonEventArgs e)
        {
            ListBoxItem selectedCheckInItem = (ListBoxItem)CheckInFiles.SelectedItem;
            if (selectedCheckInItem != null)
            {
                StackPanel selectedCategItem = new StackPanel();
                selectedCategItem = (StackPanel)((Image)e.Source).Parent;
                CategoryList.Items.Remove(selectedCategItem);
                List<string> categoryValuesLst = new List<string>();
                foreach (StackPanel itemCateg in CategoryList.Items)
                {
                    TextBlock categItems = (TextBlock)itemCateg.Children[0];
                    categoryValuesLst.Add(categItems.Text);
                }
                categoryValues[(string)selectedCheckInItem.Tag] = categoryValuesLst;
            }
        }

        /// <summary>
        /// This function is written for check-in button click in browse tab.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        public void CheckInFiles_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ListBoxItem selectedItem = (ListBoxItem)CheckInFiles.SelectedItem;
            if (selectedItem != null)
            {
                CategoryList.Items.Clear();
                if (categoryValues.ContainsKey((string)selectedItem.Tag))
                {
                    foreach (var itemTxt in categoryValues[(string)selectedItem.Tag])
                    {
                        GetStackPanelList(itemTxt);
                    }
                }
                GetMetaDataMsg((string)selectedItem.Content);
                DispatcherGetMetaData();
            }
        }

        /// <summary>
        /// This function is written for check-in button click in browse tab.
        /// </summary>
        private void GetMetaDataMsg(string fileName)
        {
            CsMessage msg = new CsMessage();
            msg.add(toM, CsEndPoint.toString(serverEndPoint));
            msg.add(fromM, CsEndPoint.toString(endPoint_));
            msg.add(userM, UserName.Text ?? "");
            msg.add(commandM, "GetMetaData");
            msg.add(testMsg, "0");
            msg.add("nameSP", nameSpace);
            msg.add("executedBy", "metaDataCheckin");
            msg.add(filterM, (string)fileName);
            translater.ElementAt(translaterRefID).Value.postMessage(msg);
        }



        /// <summary>
        /// This function is written for Add Dependents button click in browse tab.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void AddDependent_Click(object sender, RoutedEventArgs e)
        {
            if (!dependentsWindow.IsVisible)
            {
                dependentsWindow = new Window1(this);
                localPath = ConfigurationManager.AppSettings[7] + "/" + NamespaceTxt.Text;
                TreeViewItem remoteDirItem = new TreeViewItem
                {
                    Tag = localPath
                };
                remoteDirItem.Header = System.IO.Path.GetFileName((string)remoteDirItem.Tag);
                remoteDirItem.Items.Add(string.Empty);
                dependentsWindow.RemDirDep.Items.Add(remoteDirItem);
            }
            dependentsWindow.Topmost = true;

            dependentsWindow.Show();
        }

        /// <summary>
        /// This function is written for UpdateMetaData button click in CheckIn tab.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (DescriptionTxt.Text == string.Empty)
            {
                StatusMessage.Content = "Please enter Description";
                StatusMessage.Foreground = Brushes.Firebrick;
                return;
            }
            if (CheckInFiles.Items.Count != 0)
            {
                foreach (ListBoxItem file in CheckInFiles.Items)
                {
                    CheckInFile(file, "editMetaData");
                }
            }
            else
            {
                TabLoader();
            }
            categoryValues.Clear();
        }

        /// <summary>
        /// This function is written for check-out file list double click
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void CheckOutFileList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (CheckOutFileList.SelectedItem == null)
            {
                return;
            }
            CsMessage msgChk = new CsMessage();
            msgChk.add(toM, CsEndPoint.toString(serverEndPoint));
            msgChk.add(fromM, CsEndPoint.toString(endPoint_));
            msgChk.add(userM, UserName.Text ?? "");
            msgChk.add(commandM, "GetMetaData");
            msgChk.add(testMsg, "0");
            msgChk.add("executedBy", "metaDataCheckout");
            msgChk.add("nameSP", nameSpace);
            msgChk.add(filterM, ((ListBoxItem)CheckOutFileList.SelectedItem).Content.ToString());
            translater.ElementAt(translaterRefID).Value.postMessage(msgChk);

            DispatcherGetMetaDataForCheckOut();
        }

        /// <summary>
        /// This function is written for Add filter button click in browse tab.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void Button_Click_AddFilter(object sender, RoutedEventArgs e)
        {
            if (FilterList.SelectedItem != null)
            {
                if (FilterText.Text == "")
                {
                    StatusMessage.Content = "Please fill a value in the \"Filter By\" box";
                    StatusMessage.Foreground = Brushes.Firebrick;
                    return;
                }
                foreach (StackPanel list in FilterListBox.Items)
                {
                    if (((TextBlock)list.Children[0]).Tag == FilterList.SelectedItem)
                    {
                        FilterListBox.Items.Remove(list);
                        break;
                    }
                }

                string selectedValue = "ON: \"" + FilterList.SelectedItem.ToString() + "\" - BY: \"" + FilterText.Text + "\"";
                GetStackPanelFileList(selectedValue, FilterList.SelectedItem.ToString(), FilterText.Text);
            }
            else
            {
                StatusMessage.Content = "Select a value in the Filter by combo-box";
                StatusMessage.Foreground = Brushes.Firebrick;
            }
        }

        /// <summary>
        /// This function is written for generating stackpanel elements in browse tab.
        /// </summary>
        private void GetStackPanelFileList(string categoryTxt, string category, string filter)
        {
            StackPanel sp = new StackPanel();
            BitmapImage bmp = new BitmapImage();
            TextBlock tb = new TextBlock();
            bmp.BeginInit();
            bmp.UriSource = new Uri(Path.GetFullPath("../../../../GUI/images/delete.png"));
            bmp.EndInit();
            tb.Text = categoryTxt;
            tb.Tag = category;
            tb.Width = 225;
            Image deleteImg = new Image() { Source = bmp, Height = 15.0 };
            deleteImg.Tag = filter;
            deleteImg.MouseDown += new MouseButtonEventHandler(AddDeleteFIlter);
            sp.Orientation = Orientation.Horizontal;
            sp.Children.Add(tb);
            sp.Children.Add(deleteImg);
            FilterListBox.Items.Add(sp);
        }

        /// <summary>
        /// This function is written for delete button click in browse tab.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void AddDeleteFIlter(object sender, MouseButtonEventArgs e)
        {
            var selectedCategItem = (StackPanel)((Image)e.Source).Parent;
            if (selectedCategItem != null)
            {
                FilterListBox.Items.Remove(selectedCategItem);
            }
        }

        /// <summary>
        /// This function is written for filter button click in browse tab.
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void Query_Click(object sender, RoutedEventArgs e)
        {
            if (FilterListBox.Items.Count == 0)
            {
                return;
            }

            int count = 0;
            CsMessage msgChk = QueryMsgConstruct("0");
            foreach (StackPanel list in FilterListBox.Items)
            {
                msgChk.add(filterM + count, ((TextBlock)list.Children[0]).Tag + "-" +
                    ((Image)list.Children[1]).Tag);
            }
            translater.ElementAt(translaterRefID).Value.postMessage(msgChk);

        }

        /// Constructs query messages
        private CsMessage QueryMsgConstruct(string testMsgSTr)
        {
            CsMessage msgChk = new CsMessage();
            msgChk.add(toM, CsEndPoint.toString(serverEndPoint));
            msgChk.add(fromM, CsEndPoint.toString(endPoint_));
            msgChk.add(userM, UserName.Text ?? "");
            msgChk.add(commandM, "queryFiles");
            msgChk.add(testMsg, testMsgSTr);
            msgChk.add("IsOrphan", (ChkOrphan.IsChecked ?? false) ? "1" : "0");
            msgChk.add("nameSP", nameSpace);
            return msgChk;
        }
        #endregion private functions

        #region Test Cases

        /// <summary>
        /// Resets the value of the files for the test cases.
        /// </summary>
        private void ResetValues()
        {
            List<string> listFiles = new List<string> { "Query.cpp.0", "Query.h.0", "DbCore.cpp.0", "DbCore.h.0" };
            foreach (var files in Directory.GetFiles(ConfigurationManager.AppSettings["projectRoot"] + "ServerStorage/NoSqlDb/DbCore/"))
            {
                if (!listFiles.Contains(Path.GetFileName(files)))
                {
                    File.Delete(files);
                }
            }
            foreach (var files in Directory.GetFiles(ConfigurationManager.AppSettings["projectRoot"] + "ServerStorage/NoSqlDb/Query/"))
            {
                if (!listFiles.Contains(Path.GetFileName(files)))
                {
                    File.Delete(files);
                }
            }
        }
        /// <summary>
        /// Test case executor. This function is called on windows load and 
        /// executes all the basic functions.
        /// </summary>
        private void TestCases()
        {
            ResetValues();
            DisplayMessage("Requirement 1#,7# requires Visual Examination. Using C# code to invoke this display function. \nRequirement 6# is shown in the last message.");
            //Test case showing the connection between the client and server. Request and response messages are shown in both client and server console. 
            ConnectServer(1);
            if (endPoint_.port.ToString() == ConfigurationManager.AppSettings[1])
            {
                localStorage = "/LocalStorage";
                ListBoxItem checkinFIle = new ListBoxItem
                {
                    Tag = ConfigurationManager.AppSettings["projectRoot"] + "LocalStorage\\NoSqlDb\\Query\\Query.h",
                    Content = "Query.h"
                };
                DescriptionTxt.Text = "Test Description";
                CheckInFile(checkinFIle, "checkIn", 1);
                DescriptionTxt.Text = "";
                localStorage = "\\LocalStorage";
            }
            if (endPoint_.port.ToString() == ConfigurationManager.AppSettings[1])
            {
                localStorage = "/LocalStorage";
                ListBoxItem checkinFIle = new ListBoxItem
                {
                    Tag = ConfigurationManager.AppSettings["projectRoot"] + "LocalStorage\\NoSqlDb\\DbCore\\DbCore.cpp",
                    Content = "DbCore.cpp"
                };
                dependentValues.Add("DbCore.cpp", new List<String> { "Query.h.0" });
                DescriptionTxt.Text = "Test Description";
                CheckInFile(checkinFIle, "checkIn", 1); DescriptionTxt.Text = "";
                localStorage = "\\LocalStorage";
                dependentValues.Clear();
            }
            TestCaseCloseCheckInCases();
            ListBoxItem checkOutItem = new ListBoxItem
            {
                Tag = "..\\ServerStorage\\NoSqlDb\\Query\\Query.cpp.0",
                Content = "Query.cpp.0"
            };
            CheckOutFileM(checkOutItem, 1); DependentsCheckOutChk.IsChecked = true;
            ListBoxItem checkOutItemDep = new ListBoxItem
            {
                Tag = "..\\ServerStorage\\NoSqlDb\\Query\\DbCore.cpp.1",
                Content = "DbCore.cpp.1"
            };
            CheckOutFileM(checkOutItemDep, 1);
            DependentsCheckOutChk.IsChecked = false;

            TestCaseBrowse();
        }

        /// <summary>
        /// Test case executor. This function is called on windows load and 
        /// executes all the basic close checkin functions.
        /// </summary>
        private void TestCaseCloseCheckInCases()
        {
            if (endPoint_.port.ToString() == ConfigurationManager.AppSettings[1])
            {
                ListBoxItem closeItem = new ListBoxItem
                {
                    Tag = "..\\LocalStorage/NoSqlDb/DbCore/DbCore.cpp",
                    Content = "DbCore.cpp"
                };
                CloseCheckIn(closeItem, "1");
                ListBoxItem closeItemQuery = new ListBoxItem
                {
                    Tag = "..\\LocalStorage/NoSqlDb/Query/Query.h",
                    Content = "Query.h"
                };
                CloseCheckIn(closeItemQuery, "1");
                ListBoxItem closeItemPendign = new ListBoxItem
                {
                    Tag = "..\\LocalStorage/NoSqlDb/Query/DbCore.h",
                    Content = "DbCore.h"
                };
                CloseCheckIn(closeItemPendign, "1");
                UserName.Text = "User2";
                ListBoxItem closeItemUserDIff = new ListBoxItem
                {
                    Tag = "..\\LocalStorage/NoSqlDb/DbCore/DbCore.h",
                    Content = "DbCore.h"
                };
                CloseCheckIn(closeItemUserDIff, "1");
                TestCaseForUpdateMetaData();
            }

        }

        /// <summary>
        /// Test case executor. This function is called on windows load and 
        /// executes all the update metadata functions.
        /// </summary>
        private void TestCaseForUpdateMetaData()
        {
            UserName.Text = "User1"; GetMetaDataMsg("DbCore.h");
            dependentValues.Add("DbCore.h", new List<String> { "DbCore.h.0" });
            categoryValues.Add("DbCore.h", new List<String> { "Test" });
            DescriptionTxt.Text = "Updated Description";
            ListBoxItem checkinFIle = new ListBoxItem
            {
                Tag = ConfigurationManager.AppSettings["projectRoot"] + "LocalStorage\\NoSqlDb\\DbCore\\DbCore.h",
                Content = "DbCore.h"
            };
            localStorage = "/LocalStorage";
            CheckInFile(checkinFIle, "editMetaData");
            DescriptionTxt.Text = "";
            localStorage = "\\LocalStorage"; dependentValues.Clear(); categoryValues.Clear();

            CsMessage msgChk = QueryMsgConstruct("1");
            msgChk.add(filterM + 0, "File Names" + "-" + "Query(.*)");
            msgChk.add(filterM + 1, "Versions" + "-" + "0");
            translater.ElementAt(translaterRefID).Value.postMessage(msgChk);

            ChkOrphan.IsChecked = true;
            CsMessage msgChkOrphan = QueryMsgConstruct("1");
            msgChkOrphan.add(filterM + 0, "File Names" + "-" + "DbCore(.*)");
            translater.ElementAt(translaterRefID).Value.postMessage(msgChkOrphan);
            ChkOrphan.IsChecked = false;
            CsMessage onewayMsg = new CsMessage();
            onewayMsg.add(toM, CsEndPoint.toString(serverEndPoint));
            onewayMsg.add(fromM, CsEndPoint.toString(endPoint_));
            onewayMsg.add(commandM, "oneway");
            translater.ElementAt(translaterRefID).Value.postMessage(onewayMsg);

        }

        /// <summary>
        /// Displays the message in the console for the test cases
        /// </summary>
        private void bytesSenderFn()
        {
            localStorage = "/LocalStorage";
            ListBoxItem checkinFIle = new ListBoxItem
            {
                Tag = ConfigurationManager.AppSettings["projectRoot"] + "LocalStorage\\NoSqlDb\\DbCore\\DbCore.cpp",
                Content = "DbCore.cpp"
            };
            DescriptionTxt.Text = "Test Description";
            CheckInFile(checkinFIle, "checkIn", 1, 1);
            DescriptionTxt.Text = "";
            localStorage = "\\LocalStorage";
        }
        /// <summary>
        /// Test case executor. This function is called on windows load and 
        /// executes all the basic browsing functions.
        /// </summary>
        private void TestCaseBrowse()
        {
            if (endPoint_.port.ToString() == ConfigurationManager.AppSettings[1])
            {
                ListBoxItem browseItem = new ListBoxItem
                {
                    Tag = "../ServerStorage/NoSqlDb/Query/Query.cpp.0",
                    Content = "Query.cpp.0"
                };
                BrowseFileGetter(browseItem);
            }
            if (endPoint_.port.ToString() == ConfigurationManager.AppSettings[1])
            {
                TreeViewItem browsefileItem = new TreeViewItem
                {
                    Tag = "/ServerStorage",
                };
                BrowseFile(browsefileItem, 1);
            }
            if (endPoint_.port.ToString() == ConfigurationManager.AppSettings[1])
            {
                TreeViewItem dirTreeView = new TreeViewItem
                {
                    Tag = "/ServerStorage/NoSqlDb"
                };
                DirectoryBrowse(dirTreeView, 1);

                bytesSenderFn();
            }
        }
        #endregion Test Cases
    }
}
