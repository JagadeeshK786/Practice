using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace HEIF_Utility_Setup
{
    public partial class TP : UserControl
    {
        public delegate void NavHandler(object sender, NavArgs e);
        public event NavHandler OnNav;

        public TP()
        {
            InitializeComponent();
        }

        string removeslash(string str)
        {
            if (str.Length == 0)
                return str;
            while (str[str.Length - 1] == '\\' || str[str.Length - 1] == '/')
                str.Remove(str.Length - 1, 1);
            return str;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            var box = new ChooseTPTempDir();
            box.ShowDialog();
            if (string.IsNullOrEmpty(box.folder))
                return;

            var path = box.folder;

            try
            {
                removeslash(path);
                File.WriteAllText("conf/HUTPWriteDirectory", path, Encoding.ASCII);

                System.Diagnostics.Process p = new System.Diagnostics.Process();
                p.StartInfo.FileName = "RegSvr32";
                p.StartInfo.Arguments = "HEIF-Utility-Thumbnail-Provider.dll";
                p.StartInfo.UseShellExecute = false;    //是否使用操作系统shell启动
                p.StartInfo.RedirectStandardInput = true;//接受来自调用程序的输入信息
                p.StartInfo.RedirectStandardOutput = true;//由调用程序获取输出信息
                p.StartInfo.RedirectStandardError = true;//重定向标准错误输出
                p.StartInfo.CreateNoWindow = true;//不显示程序窗口
                p.Start();//启动程序
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "错误");
            }
        }

        private void label2_Click(object sender, EventArgs e)
        {
            try
            {
                System.Diagnostics.Process.Start("https://liuziangexit.com/HEIF-Utility/Help/No11.html");
            }
            catch (Exception)
            {
                var box = new ShowLinkCopyable();
                box.link.Text = "https://liuziangexit.com/HEIF-Utility/Help/No11.html";
                box.ShowDialog();
            }
        }
    }
}
