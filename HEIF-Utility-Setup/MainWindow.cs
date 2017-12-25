using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace HEIF_Utility_Setup
{
    public partial class MainWindow : Form
    {
        private List<string> title_list = new List<string> { "欢迎", "系统要求", "检查VS15", "ThumbnailProvider", "完成" };
        int now = 0;

        public MainWindow()
        {
            InitializeComponent();

            for (int i = 0; i < title_list.Count; i++)
            {
                treeView1.Nodes.Add(title_list[i], title_list[i]);
            }
            Title.Text = title_list[0];
            var box = new Welcome();
            box.OnNav += new Welcome.NavHandler(NavHandler);
            box.Dock = DockStyle.Fill;
            mainpanel.Controls.Add(box);
            box.BringToFront();
        }

        private void treeView1_Enter(object sender, EventArgs e)
        {
            label1.Focus();
        }

        private void NavHandler(object sender, NavArgs e)
        {
            switch (e.type)
            {
                case 1:
                    {
                        next.Visible = false;
                    }
                    break;
                case 2:
                    {
                        prev.Visible = false;
                    }
                    break;
                case 3:
                    {
                        next.Visible = true;
                    }
                    break;
                case 4:
                    {
                        prev.Visible = true;
                    }
                    break;
            }
            this.Refresh();
        }

        private void next_Click(object sender, EventArgs e)
        {
            Control new_ctrl = null;
            switch (now)
            {
                case 0:
                    {
                        var box = new CheckSystem();
                        box.OnNav += new CheckSystem.NavHandler(NavHandler);
                        new_ctrl = box;
                    }
                    break;
                case 1:
                    {
                        var box = new CheckVS15();
                        box.OnNav += new CheckVS15.NavHandler(NavHandler);
                        new_ctrl = box;
                    }
                    break;
                case 2:
                    {
                        var box = new TP();
                        box.OnNav += new TP.NavHandler(NavHandler);
                        new_ctrl = box;
                    }
                    break;
                case 3:
                    {
                        var box = new Finish();
                        box.OnNav += new Finish.NavHandler(NavHandler);
                        new_ctrl = box;
                        next.Text = "完成";
                    }
                    break;
                case 4:
                    {
                        Close();
                        Environment.Exit(0);
                    }
                    break;
                default:
                    {
                        return;
                    }
                    break;
            }

            mainpanel.Controls.Clear();
            Title.Text = title_list[now + 1];
            new_ctrl.Dock = DockStyle.Fill;
            mainpanel.Controls.Add(new_ctrl);
            new_ctrl.BringToFront();
            now++;
        }

        private void prev_Click(object sender, EventArgs e)
        {
            Control new_ctrl = null;
            switch (now)
            {
                case 1:
                    {
                        var box = new Welcome();
                        box.OnNav += new Welcome.NavHandler(NavHandler);
                        new_ctrl = box;                        
                    }
                    break;
                case 2:
                    {
                        var box = new CheckSystem();
                        box.OnNav += new CheckSystem.NavHandler(NavHandler);
                        new_ctrl = box;
                    }
                    break;
                case 3:
                    {
                        var box = new CheckVS15();
                        box.OnNav += new CheckVS15.NavHandler(NavHandler);
                        new_ctrl = box;
                    }
                    break;
                case 4:
                    {
                        var box = new TP();
                        box.OnNav += new TP.NavHandler(NavHandler);
                        new_ctrl = box;
                    }
                    break;
                default:
                    {
                        return;
                    }
                    break;
            }

            mainpanel.Controls.Clear();
            Title.Text = title_list[now - 1];
            new_ctrl.Dock = DockStyle.Fill;
            mainpanel.Controls.Add(new_ctrl);
            new_ctrl.BringToFront();
            now--;
        }
    }
}
