using System;
using System.Windows.Forms;

namespace TooltipsTestApp
{
    public partial class Main : Form
    {
        public Main()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            notifyIcon1.Visible = true;
        }

        private void button8_Click(object sender, EventArgs e)
        {
            notifyIcon2.Visible = true;
        }

        private void button12_Click(object sender, EventArgs e)
        {
            notifyIcon3.Visible = true;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            notifyIcon1.Visible = false;
        }

        private void button7_Click(object sender, EventArgs e)
        {
            notifyIcon2.Visible = false;
        }

        private void button11_Click(object sender, EventArgs e)
        {
            notifyIcon3.Visible = false;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            notifyIcon1.ShowBalloonTip(5000);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            notifyIcon2.ShowBalloonTip(5000);
        }

        private void button10_Click(object sender, EventArgs e)
        {
            notifyIcon3.ShowBalloonTip(5000);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            notifyIcon1.ShowBalloonTip(5000, "Alt Title balloon 1", "Alt Text balloon 1", ToolTipIcon.None);
        }

        private void button5_Click(object sender, EventArgs e)
        {
            notifyIcon2.ShowBalloonTip(5000, "Alt Title balloon 2", "Alt Text balloon 2", ToolTipIcon.Error);
        }

        private void button9_Click(object sender, EventArgs e)
        {
            notifyIcon3.ShowBalloonTip(5000, "Alt Title balloon 3", "Alt Text balloon 3", ToolTipIcon.Info);
        }
    }
}
