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
    public partial class Finish : UserControl
    {
        public delegate void NavHandler(object sender, NavArgs e);
        public event NavHandler OnNav;

        public Finish()
        {
            InitializeComponent();
        }

        private void Finish_Load(object sender, EventArgs e)
        {
            OnNav(this, new NavArgs() { type = 2 });
        }
    }
}
