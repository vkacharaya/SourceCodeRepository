///////////////////////////////////////////////////////////////////////
// BrowseWindow.xaml.cs - GUI for Project3HelpWPF                      //
// ver 1.0                                                           //
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018 //
// Author: Vishnu Karthik Ravindran, CSE 687 - Object Oriented Design//
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package provides a WPF-based GUI for RepositoryWithWPF demo.  It's 
 * responsibilities are to:
 * - Event handler for exit button in the browse window
 * - Swowing file text and file meta data
 *   
 * Required Files:
 * ---------------
 * BrowseWindow.xaml, BrowseWindow.xaml.cs
 * Translater.dll
 * 
 * Maintenance History:
 * --------------------
 * ver 1.0 : 30 Mar 2018
 * - first release
 */

using System.Windows;

namespace RepositoryWPF
{
    /// <summary>
    /// Interaction logic for BrowseWindow.xaml
    /// </summary>
    public partial class BrowseWindow : Window
    {
        /// <summary>
        /// Constructor for browse window
        /// </summary>
        public BrowseWindow()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Event handler for exit button in the browse window
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        public void Exit_ButtonClick(object sender,RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
