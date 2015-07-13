using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using FSUIPC;
using System.Collections;
using System.IO.Ports;

namespace VRFSConnect
{

    public partial class Form1 : Form
    {
        
        
        private Offset<short> nav1 = new Offset<short>(0x0350);
        private Offset<short> adf1 = new Offset<short>(0x034c);
        private Offset<short> adfext = new Offset<short>(0x0356);
        private Offset<short> com1 = new Offset<short>(0x034e);
        private Offset<short> com1standby = new Offset<short>(0x311a);
        private Offset<short> nav1standby = new Offset<short>(0x311e);

        private SerialPort sp;
        
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try {
                FSUIPCConnection.Open();
                timer1.Enabled = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            FSUIPCConnection.Close();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            FSUIPCConnection.Process();
            string serialText = ConstructSerialString();
            textBox1.Text = serialText;

            if(sp != null && sp.IsOpen)
            {
                sp.Write(serialText);
                ReadSerialData();
            }
        }

        private string ConstructSerialString()
        {
            string s = "";
            string com1s = com1.Value.ToString("X");
            s += "C1" + com1s.Substring(0, 2) + "." + com1s.Substring(2, 2) + "|";
            string com1sb = com1standby.Value.ToString("X");
            s += "1" + com1sb.Substring(0, 2) + "." + com1sb.Substring(2) + ";";

            string nav1s = nav1.Value.ToString("X");
            if(nav1s.Length == 3)
            {
                nav1s = "0" + nav1s;
            }
            s += "I1" + nav1s.Substring(0, 2) + "." + nav1s.Substring(2, 2) + "|";
            string nav1sb = nav1standby.Value.ToString("X");
            s += "1" + nav1sb.Substring(0, 2) + "." + nav1sb.Substring(2, 2) + ";";

            string adfa = adf1.Value.ToString("X");
            string adfb = adfext.Value.ToString("X");

            int t = 0;
            int f = 0;

            if (adfb != "0")
            {
                if(adfb.Length == 3)
                {
                    adfb = "0" + adfb;
                }
                t = int.Parse(adfb.Substring(0, 2));
                f = int.Parse(adfb.Substring(2, 2));

            }
            
            
            int fr = int.Parse(adfa);
            s += "A" + (t + fr) + "." + f + ";";

            return s;
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            string[] Ports = SerialPort.GetPortNames();
            for(int i = 0; i < Ports.Length; i++)
            {
                cbPorts.Items.Add(Ports[i]);
            }
            cbPorts.Text = "COM5";
            
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if(cbPorts.Text == "")
            {
                MessageBox.Show("Missing COM Port");
                return;
            }

            sp = new SerialPort(cbPorts.Text, 9600);
            sp.Open();
        }

        private void ReadSerialData()
        {
            String ReceivedData;
            ReceivedData = sp.ReadExisting().Trim();
            if ( ReceivedData != "")
            {
                if(ReceivedData.StartsWith("$"))
                {
                    ReceivedData = ReceivedData.Replace("$", "");
                    if (ReceivedData.Length == 0) return;
                    if(ReceivedData[ReceivedData.Length - 1] != ';')
                    {
                        return;
                    }

                    String[] parts = ReceivedData.Split(';');
                    for( int i = 0; i < parts.Length; i++)
                    {

                        if(parts[i].Trim().Length == 0)
                        {
                            continue;
                        }
                        parts[i] = parts[i].Replace("-","").Trim();

                        int pipePos = parts[i].IndexOf("|");
                        


                        switch (parts[i][0])
                        {
                            case 'C':
                                
                                if (pipePos == -1 ) continue;
                                string com1a = parts[i].Substring(1, pipePos-1);
                                string com1s = parts[i].Substring(pipePos + 1);
                                int a = Convert.ToInt32(com1a.Substring(1, 2) + com1a.Substring(4, 2), 16);
                                int s = Convert.ToInt32(com1s.Substring(1, 2) + com1s.Substring(4, 2), 16);
                                
                                com1.Value = (short)a;
                                com1standby.Value = (short)s;
                                FSUIPCConnection.Process();
                                break;
                            case 'I':

                                if (pipePos == -1) continue;
                                
                                
                                string nava = parts[i].Substring(1, pipePos - 1);
                                string navs = parts[i].Substring(pipePos + 1);
                                int na = Convert.ToInt32(nava.Substring(1, 2) + nava.Substring(4, 2), 16);
                                int ns = Convert.ToInt32(navs.Substring(1, 2) + navs.Substring(4, 2), 16);
                                
                                nav1.Value = (short)na;
                                nav1standby.Value = (short)ns;
                                FSUIPCConnection.Process();
                                break;
                        }


                    }
                    


                }
                while (ReceivedData.Length != 0)
                {
                    if (ReceivedData.StartsWith("!")) // mcdu
                    {
                        ReceivedData = ReceivedData.Substring(1);
                        if(ReceivedData.Length == 0)
                        {
                            return;
                        }
                        char r = ReceivedData[0];
                        if (r != 0)
                        {
                            Offset<short> b = new Offset<short>(0x78EC, true);
                            if (r >= 49 && r <= 63)
                            {
                                b.Value = (short)(r - 49 + 28);
                            }
                            if (r >= 64 && r <= 90)
                            {
                                b.Value = (short)(r - 64);

                            }

                            if (r >= 18 && r < 30) // LSK1-6
                            {
                                b.Value = (short)(r + 20);
                            }
                            

                            switch ((int)r)
                            {
                                case 17:
                                    b.Value = 50; //CLR
                                    break;
                                case 48:
                                    b.Value = 27; // 0
                                    break;
                                case 16:
                                    b.Value = 51; //OVFY
                                    break;
                                case 1:
                                    b.Value = 56; //DIR
                                    break;
                                case 2:
                                    b.Value = 57; //PROG
                                    break;
                                case 3:
                                    b.Value = 58; //PERF
                                    break;
                                case 4:
                                    b.Value = 59; //INIT
                                    break;
                                case 5:
                                    b.Value = 60; //DATA
                                    break;
                                case 6:
                                    b.Value = 61; //FPLN
                                    break;
                                case 7:
                                    b.Value = 62; //RADNAV
                                    break;
                                case 8:
                                    b.Value = 63; //FUELPRED
                                    break;
                                case 9:
                                    b.Value = 64; //SECFLPN
                                    break;
                                case 11:
                                    b.Value = 65; //MCDUMENU
                                    break;
                                case 12:
                                    b.Value = 66; //AIRPORT
                                    break;
                                case 30:
                                    b.Value = 69; //UP
                                    break;
                                case 14:
                                    b.Value = 70; //DOWN
                                    break;
                                case 15:
                                    b.Value = 68; //RIGHT/NEXTPAGE
                                    break;
                                case 46:
                                    b.Value = 55; // POINT
                                    break;
                                case 47:
                                    b.Value = 53; // SLASH
                                    break;
                                case 43:
                                    b.Value = 54; // PLUS
                                    break;
                                case 45:
                                    b.Value = 52; // SPACE
                                    break;


















                            }
                            AddText(b.Value.ToString());
                            FSUIPCConnection.Process();
                        }
                    } else
                    {
                        ReceivedData = ReceivedData.Substring(1);
                    }
                }
                AddText(ReceivedData);
            
            }
        }
        public void AddText(String text)
        {
            txtSerialOut.Text += text;
            txtSerialOut.SelectionStart = txtSerialOut.TextLength;
            txtSerialOut.ScrollToCaret();
        }
        public static byte[] ToBcd(int value)
        {
            if (value < 0 || value > 99999999)
                throw new ArgumentOutOfRangeException("value");
            byte[] ret = new byte[2];
            for (int i = 0; i < 2; i++)
            {
                ret[i] = (byte)(value % 10);
                value /= 10;
                ret[i] |= (byte)((value % 10) << 4);
                value /= 10;
            }
            return ret;
        }
    }
}
