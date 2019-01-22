
///////////////////////////////////////////////////////////////////////
// App.xaml.cs - GUI for Project3HelpWPF                             //
// ver 1.0                                                           //
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018 //
// Author: Vishnu Karthik Ravindran, CSE 687 - Object Oriented Design//
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package provides a WPF-based GUI for RepositoryWithWPF demo.  It's 
 * responsibilities are to:
 * - Loads when the application is started. It creates two clients 
 *   listening on different ports
 *   
 * Required Files:
 * ---------------
 * App.xaml, App.xaml.cs
 * Translater.dll
 * 
 * Maintenance History:
 * --------------------
 * ver 1.0 : 30 Mar 2018
 * - first release
 */

using RepositoryWPF;
using System;
using System.Configuration;
using System.Windows;
using System.IO;

namespace WpfApp1
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        /// <summary>
        /// Loads when the application is started. It creates two clients listening on different ports
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void App_Startup(object sender, StartupEventArgs e)
        {
            //Window listening on 8082
            string path = Path.GetFullPath(ConfigurationManager.AppSettings["projectRoot"]
                + ConfigurationManager.AppSettings["clientfolder"].Substring(1) + "/NoSqlDb");
            MainWindow window_8082 = new MainWindow(path);
            window_8082.ConnectionIP1.Text = ConfigurationManager.AppSettings[0];
            window_8082.ConnectionPort1.Text = ConfigurationManager.AppSettings[1];
            window_8082.Title = "Version Control - Listening on port #" + ConfigurationManager.AppSettings[1];
            window_8082.Show();
        }
    }
}
