///////////////////////////////////////////////////////////////////////
// DepenedentsWindow.xaml.cs - GUI for Project3HelpWPF               //
// ver 1.0                                                           //
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
 * DepenedentsWindow.xaml, DepenedentsWindow.xaml.cs
 * Translater.dll
 * 
 * Maintenance History:
 * --------------------
 * ver 1.0 : 30 Mar 2018
 * - first release
 */

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media.Imaging;

namespace RepositoryWPF
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        /// <summary>
        /// This is the instance of the Main window 
        /// which will be used in the Dependents window
        /// </summary>
        private MainWindow mainWindow;

        /// <summary>
        /// This holds the selected file name in the CheckIn list box for 
        /// adding dependents
        /// </summary>
        private string fileName;

        /// <summary>
        /// Constructor used to intitialize the selected item values 
        /// in the main window to the dependents window
        /// </summary>
        /// <param name="bWIn"></param>
        public Window1(MainWindow bWIn)
        {
            if (bWIn.CheckInFiles.SelectedItem != null)
                fileName = ((ListBoxItem)bWIn.CheckInFiles.SelectedItem).Content.ToString();
            mainWindow = bWIn;

            InitializeComponent();
            foreach (string item in mainWindow.Dependents.Items)
            {
                getStackPanelList(item);
            }
            RemDirDep.AddHandler(TreeViewItem.ExpandedEvent, new RoutedEventHandler(bWIn.TreeView_ExpandRemote));
            RemDirDep.AddHandler(TreeViewItem.SelectedEvent, new RoutedEventHandler(bWIn.DirList_ClickRemote));
        }

        /// <summary>
        /// Event Handler for Add dependents button click. It sends the message back to the
        /// Main window
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            mainWindow.Dependents.Items.Clear();
            foreach (StackPanel item in DependentsManage.Items)
            {
                mainWindow.Dependents.Items.Add(((TextBlock)item.Children[0]).Text.ToString());
            }
            mainWindow.UpdateDependents(fileName, mainWindow.Dependents.Items);
            this.Close();
        }

        /// <summary>
        /// Used for adding the files to the dependents list on
        /// double click of the fileitem in the list
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void RemDirDepList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            StackPanel deleteItem = new StackPanel();
            string dependentList = ((ListBoxItem)RemDirDepList.SelectedItem).Content.ToString();
            foreach (StackPanel item in DependentsManage.Items)
            {
                if (dependentList.Contains(((TextBlock)item.Children[0]).Tag.ToString()))
                {
                    deleteItem = item;
                    break;
                }
            }
            DependentsManage.Items.Remove(deleteItem);
            getStackPanelList(((ListBoxItem)RemDirDepList.SelectedItem).Content.ToString());
        }

        /// <summary>
        /// This function is written for generating the stack Panel elements for
        /// various controls.
        /// </summary>
        private void getStackPanelList(string categoryTxt)
        {
            StackPanel sp = new StackPanel();
            BitmapImage bmp = new BitmapImage();
            TextBlock tb = new TextBlock();
            tb.Tag = categoryTxt.Substring(0, categoryTxt.LastIndexOf('.'));
            bmp.BeginInit();
            bmp.UriSource = new Uri(System.IO.Path.GetFullPath("../../../../GUI/images/delete.png"));
            bmp.EndInit();
            tb.Text = categoryTxt;
            tb.Width = 185;
            Image deleteImg = new Image() { Source = bmp, Height = 15.0 };
            deleteImg.MouseDown += new MouseButtonEventHandler(addDeletimgClick);
            sp.Orientation = Orientation.Horizontal;
            sp.Children.Add(tb);
            sp.Children.Add(deleteImg);
            DependentsManage.Items.Add(sp);
        }

        /// <summary>
        /// This function is written for delete image click in dependents list
        /// </summary>
        /// <param name="sender">Contains sender information</param>
        /// <param name="e">State information and event data</param>
        private void addDeletimgClick(object sender, MouseButtonEventArgs e)
        {
            StackPanel selectedCategItem = new StackPanel();
            selectedCategItem = (StackPanel)((Image)e.Source).Parent;
            DependentsManage.Items.Remove(selectedCategItem);
        }
    }
}
