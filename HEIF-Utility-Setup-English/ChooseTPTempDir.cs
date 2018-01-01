using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace HEIF_Utility_Setup
{
    public partial class ChooseTPTempDir : Form
    {
        public string folder;
        public ChooseTPTempDir()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            var filepicker = new FolderBrowserDialog();
            filepicker.ShowNewFolderButton = true;
            filepicker.ShowDialog();
            if (string.IsNullOrEmpty(filepicker.SelectedPath))
                return;
            folder = filepicker.SelectedPath;
            label2.Text = folder;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(folder))
            {
                MessageBox.Show("You have not selected a directory yet", "");
                return;
            }
            this.Close();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            folder = "";
            this.Close();
        }
    }
}
