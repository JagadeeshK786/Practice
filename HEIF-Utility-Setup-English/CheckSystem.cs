using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace HEIF_Utility_Setup
{
    public partial class CheckSystem : UserControl
    {
        public delegate void NavHandler(object sender, NavArgs e);
        public event NavHandler OnNav;
        
        public CheckSystem()
        {
            InitializeComponent();
        }

        private void CheckSystem_Load(object sender, EventArgs e)
        {
            OnNav(this, new NavArgs() { type = 1 });
            OnNav(this, new NavArgs() { type = 4 });
            //OnNav(this, new NavArgs() { type = 2 });
        }

        private bool checksystem()
        {
            Version sysver = System.Environment.OSVersion.Version;
            if (sysver.Major < 6)
                return false;
            if (sysver.Minor < 1)
                return false;
            return true;
        }

        private bool isx64()
        {
            return Environment.Is64BitOperatingSystem;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            button1.Enabled = false;

            textBox1.Text += "\r\n\r\n";

            bool allok = true;

            allok = allok && checksystem();

            if (checksystem())
                textBox1.Text += "Windows 7 or later ---- YES\r\n";
            else
                textBox1.Text += "Windows 7 or later ---- NO\r\n";

            allok = allok && isx64();

            if (isx64())
                textBox1.Text += "64-bit OS ---- YES\r\n";
            else
                textBox1.Text += "64-bit OS ---- NO\r\n";

            textBox1.Text += "\r\n";

            if (allok)
            {
                textBox1.Text += "OK, Please click \"Next\".\r\n";
                OnNav(this, new NavArgs() { type = 3 });
                OnNav(this, new NavArgs() { type = 4 });
            }
            else
            {
                textBox1.Text += "Not OK, Please quit.\r\n";
            }
                        
        }

        private void label2_Click(object sender, EventArgs e)
        {
            try
            {
                System.Diagnostics.Process.Start("https://liuziangexit.com/HEIF-Utility/Help/No12.html");
            }
            catch (Exception)
            {
                var box = new ShowLinkCopyable();
                box.link.Text = "https://liuziangexit.com/HEIF-Utility/Help/No12.html";
                box.ShowDialog();
            }
        }
    }
}
