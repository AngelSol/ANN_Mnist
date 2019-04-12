using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

// This is the code for your desktop app.
// Press Ctrl+F5 (or go to Debug > Start Without Debugging) to run your app.

//make all variables global?


namespace HW5_Machine

{

    

    public partial class Form1 : Form
    {
        string filename;
        const int WIDTH = 28;
        const int HIGHT = 28;
        const int INPUTS = WIDTH * HIGHT;
        const int HIDDEN_NODES = 40;
        const int OUTPUTS = 10;

        public Form1()
        {
            InitializeComponent();
            
        }

        private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            // Click on the link below to continue learning how to build a desktop app using WinForms!
            System.Diagnostics.Process.Start("http://aka.ms/dotnet-get-started-desktop");

        }

        private void button1_Click(object sender, EventArgs e)
        {
           
            Bitmap myBitmap = new Bitmap(filename);   

            int maxloc = 0;
            double maxout = 0.0;

            double [,] w = new double[INPUTS,HIDDEN_NODES];
            double [,] u = new double[HIDDEN_NODES,OUTPUTS];
            double[] b = new double[HIDDEN_NODES];
            double[] c = new double[OUTPUTS];
            double[] s = new double[HIDDEN_NODES];
            double[] y = new double[HIDDEN_NODES];
            double[] r = new double[HIDDEN_NODES];
            double[] z = new double[OUTPUTS];

            double[] x = new double[INPUTS];
            //setup weights
            BinaryReader reader = new BinaryReader(File.Open("weights", FileMode.Open));

            for (int m = 0; m < HIDDEN_NODES; m++)
            {
                for (int l = 0; l < INPUTS; l++)
                {
                    w[l,m] = reader.ReadDouble();
                }
                b[m] = reader.ReadDouble();
            }

            //update u and c change limits
            for (int m = 0; m < OUTPUTS; m++)
            {
                for (int l = 0; l < HIDDEN_NODES; l++)
                {

                    u[l,m] = reader.ReadDouble();
                }

               c[m] = reader.ReadDouble();
            }

            reader.Close();


            for (int i = 0; i < HIGHT; i++) 
            {
                for (int j = 0; j < WIDTH; j++)
                {

                    x[i * HIGHT + j] = myBitmap.GetPixel(j, i).R;
                }
            }

            //read image pixles
            for(int i =0; i< HIDDEN_NODES; i++)
            {
                s[i] = 0;
                for (int j = 0; j < INPUTS; j++)
                {
                    s[i] += w[j,i] * x[j];
                }
                s[i] += b[i];

            }
            //old
            for (int j = 0; j < HIDDEN_NODES; j++)
            {
                y[j] = 1 / (1 + Math.Exp(-s[j]));
            }

            for (int j = 0; j < OUTPUTS; j++)
            {
                r[j] = 0;
                for (int l = 0; l < HIDDEN_NODES; l++)
                {
                    r[j] += u[l,j] * y[l];
                }
                r[j] += c[j];
            }

            for (int j = 0; j < OUTPUTS; j++)
            {
                z[j] = 1 / (1 + Math.Exp(-r[j]));

            }

            //output calculation
            maxloc = 0;
            maxout = z[0];
            for (int m = 0; m < OUTPUTS; m++)
            {
                if (z[m] > maxout)
                {
                    maxloc = m;
                    maxout = z[m];
                }
            }
            OutputBox.Text = maxloc.ToString();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
        private void OutputBox_TextChanged(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void folderBrowserDialog1_HelpRequest(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            //openFileDialog1.ShowDialog();
            OpenFileDialog select = new OpenFileDialog();
            select.Filter = "All Files (*.*)|*.*";
            if (select.ShowDialog() == DialogResult.OK)
            {
               filename = select.FileName;     
            }
        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {

        }
    }
}
