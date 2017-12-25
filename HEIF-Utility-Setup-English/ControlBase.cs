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
    public class NavArgs : EventArgs
    {
        public int type = 0;
        /*
        1 锁定下一个
        2 锁定前一个
        3 解锁下一个
        4 解锁前一个
        */
    }
}
