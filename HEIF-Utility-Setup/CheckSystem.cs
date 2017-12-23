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
                textBox1.Text += "Windows 7 或更新版本的操作系统 ---- 符合\r\n";
            else
                textBox1.Text += "Windows 7 或更新版本的操作系统 ---- 不符合\r\n";

            allok = allok && isx64();

            if (isx64())
                textBox1.Text += "64 位的操作系统 ---- 符合\r\n";
            else
                textBox1.Text += "64 位的操作系统 ---- 不符合\r\n";

            textBox1.Text += "\r\n";

            if (allok)
            {
                textBox1.Text += "操作系统符合要求，点击底部的“下一步”来继续。\r\n";
                OnNav(this, new NavArgs() { type = 3 });
                OnNav(this, new NavArgs() { type = 4 });
            }
            else
            {
                textBox1.Text += "操作系统不符合要求，HEIF 实用工具 无法在此计算机正常工作，请退出配置向导。\r\n";
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
